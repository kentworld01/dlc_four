/*
  This file is part of UFFS, the Ultra-low-cost Flash File System.
  
  Copyright (C) 2005-2009 Ricky Zheng <ricky_gz_zheng@yahoo.co.nz>

  UFFS is free software; you can redistribute it and/or modify it under
  the GNU Library General Public License as published by the Free Software 
  Foundation; either version 2 of the License, or (at your option) any
  later version.

  UFFS is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  or GNU Library General Public License, as applicable, for more details.
 
  You should have received a copy of the GNU General Public License
  and GNU Library General Public License along with UFFS; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA  02110-1301, USA.

  As a special exception, if other files instantiate templates or use
  macros or inline functions from this file, or you compile this file
  and link it with other works to produce a work based on this file,
  this file does not by itself cause the resulting work to be covered
  by the GNU General Public License. However the source code for this
  file must still be made available in accordance with section (3) of
  the GNU General Public License v2.
 
  This exception does not invalidate any other reasons why a work based
  on this file might be covered by the GNU General Public License.
*/

/** 
 * \file uffs_flash.c
 * \brief UFFS flash interface
 * \author Ricky Zheng, created 17th July, 2009
 */
#include "uffs/uffs_config.h"
#include "uffs/uffs_public.h"
#include "uffs/uffs_ecc.h"
#include "uffs/uffs_flash.h"
#include "uffs/uffs_device.h"
#include "uffs/uffs_badblock.h"
#include <string.h>

#define PFX "Flash: "

#define SPOOL(dev) &((dev)->mem.spare_pool)
#define HEADER(buf) ((struct uffs_MiniHeaderSt *)(buf)->header)

#define ECC_SIZE(dev)	((dev)->attr->ecc_size)
#define NOMINAL_ECC_SIZE(dev) (3 * ((((dev)->attr->page_data_size - 1) / 256) + 1))

#define TAG_STORE_SIZE	(sizeof(struct uffs_TagStoreSt))


static void TagMakeEcc(struct uffs_TagStoreSt *ts)
{
	ts->tag_ecc = 0xFFF;
	ts->tag_ecc = uffs_EccMake8(ts, sizeof(struct uffs_TagStoreSt));
}

static int TagEccCorrect(struct uffs_TagStoreSt *ts)
{
	u16 ecc_store, ecc_read;
	int ret;

	ecc_store = ts->tag_ecc;
	ts->tag_ecc = 0xFFF;
	ecc_read = uffs_EccMake8(ts, sizeof(struct uffs_TagStoreSt));
	ret = uffs_EccCorrect8(ts, ecc_read, ecc_store, sizeof(struct uffs_TagStoreSt));
	ts->tag_ecc = ecc_store;	// restore tag ecc

	return ret;

}

/** setup UFFS spare data & ecc layout */
static void InitSpareLayout(uffs_Device *dev)
{
	u8 s; // status byte offset
	u8 *p;

	s = dev->attr->block_status_offs;

	if (s < TAG_STORE_SIZE) {	/* status byte is within 0 ~ TAG_STORE_SIZE-1 */

		/* spare data layout */
		p = dev->attr->_uffs_data_layout;
		if (s > 0) {
			*p++ = 0;
			*p++ = s;
		}
		*p++ = s + 1;
		*p++ = TAG_STORE_SIZE - s;
		*p++ = 0xFF;
		*p++ = 0;

		/* spare ecc layout */
		p = dev->attr->_uffs_ecc_layout;
		if (dev->attr->ecc_opt != UFFS_ECC_NONE) {
			*p++ = TAG_STORE_SIZE + 1;
			*p++ = ECC_SIZE(dev);
		}
		*p++ = 0xFF;
		*p++ = 0;
	}
	else {	/* status byte > TAG_STORE_SIZE-1 */

		/* spare data layout */
		p = dev->attr->_uffs_data_layout;
		*p++ = 0;
		*p++ = TAG_STORE_SIZE;
		*p++ = 0xFF;
		*p++ = 0;

		/* spare ecc layout */
		p = dev->attr->_uffs_ecc_layout;
		if (dev->attr->ecc_opt != UFFS_ECC_NONE) {
			if (s < TAG_STORE_SIZE + ECC_SIZE(dev)) {
				if (s > TAG_STORE_SIZE) {
					*p++ = TAG_STORE_SIZE;
					*p++ = s - TAG_STORE_SIZE;
				}
				*p++ = s + 1;
				*p++ = TAG_STORE_SIZE + ECC_SIZE(dev) - s;
			}
			else {
				*p++ = TAG_STORE_SIZE;
				*p++ = ECC_SIZE(dev);
			}
		}
		*p++ = 0xFF;
		*p++ = 0;
	}

	dev->attr->data_layout = dev->attr->_uffs_data_layout;
	dev->attr->ecc_layout = dev->attr->_uffs_ecc_layout;
}

static int CalculateSpareDataSize(uffs_Device *dev)
{
	const u8 *p;
	int ecc_last = 0, tag_last = 0;
	int ecc_size, tag_size;
	int n;

	ecc_size = (dev->attr->ecc_opt == UFFS_ECC_NONE ? 0 : ECC_SIZE(dev));
	
	p = dev->attr->ecc_layout;
	if (p) {
		while (*p != 0xFF && ecc_size > 0) {
			n = (p[1] > ecc_size ? ecc_size : p[1]);
			ecc_last = p[0] + n;
			ecc_size -= n;
			p += 2;
		}
	}

	tag_size = TAG_STORE_SIZE;
	p = dev->attr->data_layout;
	if (p) {
		while (*p != 0xFF && tag_size > 0) {
			n = (p[1] > tag_size ? tag_size : p[1]);
			tag_last = p[0] + n;
			tag_size -= n;
			p += 2;
		}
	}

	n = (ecc_last > tag_last ? ecc_last : tag_last);
	n = (n > dev->attr->block_status_offs + 1 ?
			n : dev->attr->block_status_offs + 1);

	return n;
}


/**
 * Initialize UFFS flash interface
 */
URET uffs_FlashInterfaceInit(uffs_Device *dev)
{
	URET ret = U_FAIL;
	struct uffs_StorageAttrSt *attr = dev->attr;
	uffs_Pool *pool = SPOOL(dev);

	if (dev->mem.spare_pool_size == 0) {
		if (dev->mem.malloc) {
			dev->mem.spare_pool_buf = dev->mem.malloc(dev, UFFS_SPARE_BUFFER_SIZE);
			if (dev->mem.spare_pool_buf)
				dev->mem.spare_pool_size = UFFS_SPARE_BUFFER_SIZE;
		}
	}

	if (UFFS_SPARE_BUFFER_SIZE > dev->mem.spare_pool_size) {
		uffs_Perror(UFFS_ERR_DEAD,
					"Spare buffer require %d but only %d available.",
					UFFS_SPARE_BUFFER_SIZE, dev->mem.spare_pool_size);
		memset(pool, 0, sizeof(uffs_Pool));
		goto ext;
	}

	uffs_Perror(UFFS_ERR_NOISY,
					"alloc spare buffers %d bytes.",
					UFFS_SPARE_BUFFER_SIZE);
	uffs_PoolInit(pool, dev->mem.spare_pool_buf,
					dev->mem.spare_pool_size,
					UFFS_MAX_SPARE_SIZE, MAX_SPARE_BUFFERS);

	// init flash driver
	if (dev->ops->InitFlash) {
		if (dev->ops->InitFlash(dev) < 0)
			goto ext;
	}

	if (dev->ops->WritePage == NULL && dev->ops->WritePageWithLayout == NULL) {
		uffs_Perror(UFFS_ERR_SERIOUS, "Flash driver must provide 'WritePage' or 'WritePageWithLayout' function!");
		goto ext;
	}

	if (dev->ops->ReadPage == NULL && dev->ops->ReadPageWithLayout == NULL) {
		uffs_Perror(UFFS_ERR_SERIOUS, "Flash driver must provide 'ReadPage' or 'ReadPageWithLayout' function!");
		goto ext;
	}

	if (dev->attr->layout_opt == UFFS_LAYOUT_UFFS) {
		/* sanity check */

		if (dev->attr->ecc_size == 0 && dev->attr->ecc_opt != UFFS_ECC_NONE) {
			dev->attr->ecc_size = NOMINAL_ECC_SIZE(dev);
		}

		uffs_Perror(UFFS_ERR_NORMAL, "ECC size %d", dev->attr->ecc_size);

		if ((dev->attr->data_layout && !dev->attr->ecc_layout) ||
			(!dev->attr->data_layout && dev->attr->ecc_layout)) {
			uffs_Perror(UFFS_ERR_SERIOUS,
						"Please setup data_layout and ecc_layout, "
						"or leave them all NULL !");
			goto ext;
		}

		if (!attr->data_layout && !attr->ecc_layout)
			InitSpareLayout(dev);
	}
	else if (dev->attr->layout_opt == UFFS_LAYOUT_FLASH) {
		if (dev->ops->WritePageWithLayout == NULL || dev->ops->ReadPageWithLayout == NULL) {
			uffs_Perror(UFFS_ERR_SERIOUS, "When using UFFS_LAYOUT_FLASH option, "
				"flash driver must provide 'WritePageWithLayout' and 'ReadPageWithLayout' function!");
			goto ext;
		}
	}
	else {
		uffs_Perror(UFFS_ERR_SERIOUS, "Invalid layout_opt: %d", dev->attr->layout_opt);
		goto ext;
	}

	if (dev->ops->EraseBlock == NULL) {
		uffs_Perror(UFFS_ERR_SERIOUS, "Flash driver MUST implement 'EraseBlock()' function !");
		goto ext;
	}

	dev->mem.spare_data_size = CalculateSpareDataSize(dev);
	uffs_Perror(UFFS_ERR_NORMAL, "UFFS consume spare data size %d", dev->mem.spare_data_size);
	ret = U_SUCC;
ext:
	return ret;
}

/**
 * Release UFFS flash interface
 */
URET uffs_FlashInterfaceRelease(uffs_Device *dev)
{
	uffs_Pool *pool;

	pool = SPOOL(dev);
	if (pool->mem && dev->mem.free) {
		dev->mem.free(dev, pool->mem);
		pool->mem = NULL;
		dev->mem.spare_pool_size = 0;
	}
	uffs_PoolRelease(pool);
	memset(pool, 0, sizeof(uffs_Pool));

	// release flash driver
	if (dev->ops->ReleaseFlash) {
		if (dev->ops->ReleaseFlash(dev) < 0)
			return U_FAIL;
	}

	return U_SUCC;
}

/**
 * unload spare to tag and ecc.
 */
void uffs_FlashUnloadSpare(uffs_Device *dev,
						const u8 *spare, struct uffs_TagStoreSt *ts, u8 *ecc)
{
	u8 *p_tag = (u8 *)ts;
	int tag_size = TAG_STORE_SIZE;
	int ecc_size = dev->attr->ecc_size;
	int n;
	const u8 *p;

	// unload ecc
	p = dev->attr->ecc_layout;
	if (p && ecc) {
		while (*p != 0xFF && ecc_size > 0) {
			n = (p[1] > ecc_size ? ecc_size : p[1]);
			memcpy(ecc, spare + p[0], n);
			ecc_size -= n;
			ecc += n;
			p += 2;
		}
	}

	// unload tag
	if (ts) {
		p = dev->attr->data_layout;
		while (*p != 0xFF && tag_size > 0) {
			n = (p[1] > tag_size ? tag_size : p[1]);
			memcpy(p_tag, spare + p[0], n);
			tag_size -= n;
			p_tag += n;
			p += 2;
		}
	}
}

/**
 * Read tag from page spare
 *
 * \param[in] dev uffs device
 * \param[in] block flash block num
 * \param[in] page flash page num
 * \param[out] tag tag to be filled
 *
 * \return	#UFFS_FLASH_NO_ERR: success and/or has no flip bits.
 *			#UFFS_FLASH_IO_ERR: I/O error, expect retry ?
 *			#UFFS_FLASH_ECC_FAIL: spare data has flip bits and ecc correct failed.
 *			#UFFS_FLASH_ECC_OK: spare data has flip bits and corrected by ecc.
*/
int uffs_FlashReadPageTag(uffs_Device *dev,
							int block, int page, uffs_Tags *tag)
{
	uffs_FlashOps *ops = dev->ops;
	struct uffs_StorageAttrSt *attr = dev->attr;
	u8 * spare_buf;
	int ret = UFFS_FLASH_UNKNOWN_ERR;
	UBOOL is_bad = U_FALSE;

	spare_buf = (u8 *) uffs_PoolGet(SPOOL(dev));
	if (spare_buf == NULL)
		goto ext;

	if (ops->ReadPageWithLayout) {
		ret = ops->ReadPageWithLayout(dev, block, page, NULL, 0, NULL, tag ? &tag->s : NULL, NULL);
	}
	else {
		ret = ops->ReadPage(dev, block, page, NULL, 0, NULL,
									spare_buf, dev->mem.spare_data_size);

		if (!UFFS_FLASH_HAVE_ERR(ret))
			uffs_FlashUnloadSpare(dev, spare_buf, &tag->s, NULL);
	}

	if (UFFS_FLASH_IS_BAD_BLOCK(ret))
		is_bad = U_TRUE;

	// copy some raw data
	if (tag) {
		tag->_dirty = tag->s.dirty;
		tag->_valid = tag->s.valid;
	}

	if (UFFS_FLASH_HAVE_ERR(ret))
		goto ext;

	if (tag) {
		if (tag->_valid == 1) //it's not a valid page ? don't need go further
			goto ext;

		// do tag ecc correction
		if (dev->attr->ecc_opt != UFFS_ECC_NONE) {
			ret = TagEccCorrect(&tag->s);
			ret = (ret < 0 ? UFFS_FLASH_ECC_FAIL :
					(ret > 0 ? UFFS_FLASH_ECC_OK : UFFS_FLASH_NO_ERR));

			if (UFFS_FLASH_IS_BAD_BLOCK(ret))
				is_bad = U_TRUE;

			if (UFFS_FLASH_HAVE_ERR(ret))
				goto ext;
		}
	}

ext:
	if (is_bad) {
		uffs_BadBlockAdd(dev, block);
		uffs_Perror(UFFS_ERR_NORMAL,
					"A new bad block (%d) is detected.", block);
	}

	if (spare_buf)
		uffs_PoolPut(SPOOL(dev), spare_buf);

	return ret;
}

/**
 * Read page data to buf (do ECC error correction if needed)
 * \param[in] dev uffs device
 * \param[in] block flash block num
 * \param[in] page flash page num of the block
 * \param[out] buf holding the read out data
 *
 * \return	#UFFS_FLASH_NO_ERR: success and/or has no flip bits.
 *			#UFFS_FLASH_IO_ERR: I/O error, expect retry ?
 *			#UFFS_FLASH_ECC_FAIL: spare data has flip bits and ecc correct failed.
 *			#UFFS_FLASH_ECC_OK: spare data has flip bits and corrected by ecc.
 *			#UFFS_FLASH_UNKNOWN_ERR:
 */
int uffs_FlashReadPage(uffs_Device *dev, int block, int page, uffs_Buf *buf)
{
	uffs_FlashOps *ops = dev->ops;
	struct uffs_StorageAttrSt *attr = dev->attr;
	int size = dev->com.pg_size;
	u8 ecc_buf[UFFS_MAX_ECC_SIZE];
	u8 ecc_store[UFFS_MAX_ECC_SIZE];
	UBOOL is_bad = U_FALSE;
	struct uffs_TagStoreSt ts;

	u8 * spare;
	int ret = UFFS_FLASH_UNKNOWN_ERR;

	spare = (u8 *) uffs_PoolGet(SPOOL(dev));
	if (spare == NULL)
		goto ext;

	if (ops->ReadPageWithLayout) {
		ret = ops->ReadPageWithLayout(dev, block, page, buf->header, size, ecc_buf, &ts, ecc_store);
	}
	else {
		ret = ops->ReadPage(dev, block, page, buf->header, size, ecc_buf, spare, dev->mem.spare_data_size);
	}
	if (UFFS_FLASH_IS_BAD_BLOCK(ret))
		is_bad = U_TRUE;

	if (UFFS_FLASH_HAVE_ERR(ret))
		goto ext;
	
	// make ECC for UFFS_ECC_SOFT
	if (attr->ecc_opt == UFFS_ECC_SOFT)
		uffs_EccMake(buf->header, size, ecc_buf);

	// unload tag and ecc_store if driver doesn't do the layout
	if (ops->ReadPageWithLayout == NULL) {
		if (attr->ecc_opt == UFFS_ECC_SOFT || attr->ecc_opt == UFFS_ECC_HW)
			uffs_FlashUnloadSpare(dev, spare, &ts, ecc_store);
		else
			uffs_FlashUnloadSpare(dev, spare, &ts, NULL); // skip ecc_store for UFFS_ECC_NONE or UFFS_ECC_HW_AUTO
	}

	// check tag ecc. if ECC enabled, UFFS always use soft ecc for tag.
	if (attr->ecc_opt != UFFS_ECC_NONE) {
		ret = TagEccCorrect(&ts);
		ret = (ret < 0 ? UFFS_FLASH_ECC_FAIL :
				(ret > 0 ? UFFS_FLASH_ECC_OK : UFFS_FLASH_NO_ERR));

		if (UFFS_FLASH_IS_BAD_BLOCK(ret))
			is_bad = U_TRUE;

		if (UFFS_FLASH_HAVE_ERR(ret))
			goto ext;
	}

	// check page data ecc
	if (dev->attr->ecc_opt == UFFS_ECC_SOFT || dev->attr->ecc_opt == UFFS_ECC_HW) {

		ret = uffs_EccCorrect(buf->header, size, ecc_store, ecc_buf);
		ret = (ret < 0 ? UFFS_FLASH_ECC_FAIL :
				(ret > 0 ? UFFS_FLASH_ECC_OK : UFFS_FLASH_NO_ERR));

		if (UFFS_FLASH_IS_BAD_BLOCK(ret))
			is_bad = U_TRUE;

		if (UFFS_FLASH_HAVE_ERR(ret))
			goto ext;
	}

ext:
	switch(ret) {
		case UFFS_FLASH_IO_ERR:
			uffs_Perror(UFFS_ERR_NORMAL, "Read block %d page %d I/O error", block, page);
			break;
		case UFFS_FLASH_ECC_FAIL:
			uffs_Perror(UFFS_ERR_NORMAL, "Read block %d page %d ECC failed", block, page);
			ret = UFFS_FLASH_BAD_BLK;	// treat ECC FAIL as BAD BLOCK
			is_bad = U_TRUE;
			break;
		case UFFS_FLASH_ECC_OK:
			uffs_Perror(UFFS_ERR_NORMAL, "Read block %d page %d bit flip corrected by ECC", block, page);
			break;
		case UFFS_FLASH_BAD_BLK:
			uffs_Perror(UFFS_ERR_NORMAL, "Read block %d page %d BAD BLOCK found", block, page);
			break;
		case UFFS_FLASH_UNKNOWN_ERR:
			uffs_Perror(UFFS_ERR_NORMAL, "Read block %d page %d UNKNOWN error!", block, page);
			break;
		default:
			break;
	}

	if (is_bad)
		uffs_BadBlockAdd(dev, block);

	if (spare)
		uffs_PoolPut(SPOOL(dev), spare);


	return ret;
}

/**
 * make spare from tag and ecc
 *
 * \param[in] dev uffs dev
 * \param[in] ts uffs tag store, NULL if don't pack tag store
 * \param[in] ecc ecc of data, NULL if don't pack ecc
 * \param[out] spare output buffer
 * \note spare buffer size: dev->mem.spare_data_size,
 *		 all unpacked bytes will be inited 0xFF
 */
void uffs_FlashMakeSpare(uffs_Device *dev,
						 const uffs_TagStore *ts, const u8 *ecc, u8* spare)
{
	u8 *p_ts = (u8 *)ts;
	int ts_size = TAG_STORE_SIZE;
	int ecc_size = ECC_SIZE(dev);
	int n;
	const u8 *p;

	memset(spare, 0xFF, dev->mem.spare_data_size);	// initialize as 0xFF.

	// load ecc
	p = dev->attr->ecc_layout;
	if (p && ecc) {
		while (*p != 0xFF && ecc_size > 0) {
			n = (p[1] > ecc_size ? ecc_size : p[1]);
			memcpy(spare + p[0], ecc, n);
			ecc_size -= n;
			ecc += n;
			p += 2;
		}
	}

	p = dev->attr->data_layout;
	while (*p != 0xFF && ts_size > 0) {
		n = (p[1] > ts_size ? ts_size : p[1]);
		memcpy(spare + p[0], p_ts, n);
		ts_size -= n;
		p_ts += n;
		p += 2;
	}
}

/**
 * write the whole page, include data and tag
 *
 * \param[in] dev uffs device
 * \param[in] block
 * \param[in] page
 * \param[in] buf contains data to be wrote
 * \param[in] tag tag to be wrote
 *
 * \return	#UFFS_FLASH_NO_ERR: success.
 *			#UFFS_FLASH_IO_ERR: I/O error, expect retry ?
 *			#UFFS_FLASH_BAD_BLK: a new bad block detected.
 */
int uffs_FlashWritePageCombine(uffs_Device *dev,
							   int block, int page,
							   uffs_Buf *buf, uffs_Tags *tag)
{
	uffs_FlashOps *ops = dev->ops;
	int size = dev->com.pg_size;
	u8 ecc_buf[UFFS_MAX_ECC_SIZE];
	u8 *ecc = NULL;
	u8 *spare;
	struct uffs_MiniHeaderSt *header;
	int ret = UFFS_FLASH_UNKNOWN_ERR;
	UBOOL is_bad = U_FALSE;

	uffs_Buf *verify_buf;

	spare = (u8 *) uffs_PoolGet(SPOOL(dev));
	if (spare == NULL)
		goto ext;

	// setup header
	header = HEADER(buf);
	memset(header, 0xFF, sizeof(struct uffs_MiniHeaderSt));
	header->status = 0;

	// setup tag
	tag->s.dirty = TAG_DIRTY;		//!< set dirty bit
	tag->s.valid = TAG_VALID;		//!< set valid bit
	if (dev->attr->ecc_opt != UFFS_ECC_NONE)
		TagMakeEcc(&tag->s);
	else
		tag->s.tag_ecc = TAG_ECC_DEFAULT;
	
	if (dev->attr->ecc_opt == UFFS_ECC_SOFT) {
		uffs_EccMake(buf->header, size, ecc_buf);
		ecc = ecc_buf;
	}
	else if (dev->attr->ecc_opt == UFFS_ECC_HW) {
		ecc = ecc_buf;
	}

	if (ops->WritePageWithLayout) {
		ret = ops->WritePageWithLayout(dev, block, page,
							buf->header, size, ecc, &tag->s);
	}
	else {

		uffs_Assert(!(dev->attr->layout_opt == UFFS_LAYOUT_FLASH ||
					dev->attr->ecc_opt == UFFS_ECC_HW ||
					dev->attr->ecc_opt == UFFS_ECC_HW_AUTO), "WritePageWithLayout() not implemented ?");

		uffs_FlashMakeSpare(dev, &tag->s, ecc, spare);

		ret = ops->WritePage(dev, block, page, buf->header, size, spare, dev->mem.spare_data_size);

	}
	
	if (UFFS_FLASH_IS_BAD_BLOCK(ret))
		is_bad = U_TRUE;

	if (UFFS_FLASH_HAVE_ERR(ret))
		goto ext;

#ifdef CONFIG_PAGE_WRITE_VERIFY
	verify_buf = uffs_BufClone(dev, NULL);
	if (verify_buf) {
		ret = uffs_FlashReadPage(dev, block, page, verify_buf);
		if (!UFFS_FLASH_HAVE_ERR(ret)) {
			if (memcmp(buf->header, verify_buf->header, size) != 0) {
				uffs_Perror(UFFS_ERR_NORMAL,
							"Page write verify fail (block %d page %d)",
							block, page);
				ret = UFFS_FLASH_BAD_BLK;
			}
		}

		if (UFFS_FLASH_IS_BAD_BLOCK(ret))
			is_bad = U_TRUE;

		uffs_BufFreeClone(dev, verify_buf);
	}
#endif
ext:
	if (is_bad)
		uffs_BadBlockAdd(dev, block);

	if (spare)
		uffs_PoolPut(SPOOL(dev), spare);

	return ret;
}

/**
 * mark a clean page as 'dirty' (and 'invalid')
 *
 * \param[in] dev uffs device
 * \param[in] block
 * \param[in] page
 *
 * \return	#UFFS_FLASH_NO_ERR: success.
 *			#UFFS_FLASH_IO_ERR: I/O error, expect retry ?
 *			#UFFS_FLASH_BAD_BLK: a new bad block detected.
 */
int uffs_FlashMarkDirtyPage(uffs_Device *dev, int block, int page)
{
	u8 *spare;
	struct uffs_TagStoreSt ts;
	uffs_FlashOps *ops = dev->ops;
	UBOOL is_bad = U_FALSE;
	int ret = UFFS_FLASH_UNKNOWN_ERR;

	spare = (u8 *) uffs_PoolGet(SPOOL(dev));
	if (spare == NULL)
		goto ext;

	memset(&ts, 0xFF, sizeof(ts));
	ts.dirty = TAG_DIRTY;  // set only 'dirty' bit
	
	if (dev->attr->ecc_opt != UFFS_ECC_NONE)
		TagMakeEcc(&ts);

	if (ops->WritePageWithLayout) {
		ret = ops->WritePageWithLayout(dev, block, page, NULL, 0, NULL, &ts);
	}
	else {
		uffs_FlashMakeSpare(dev, &ts, NULL, spare);
		ret = ops->WritePage(dev, block, page, NULL, 0, spare, dev->mem.spare_data_size);
	}

	if (UFFS_FLASH_IS_BAD_BLOCK(ret))
		is_bad = U_TRUE;

ext:
	if (is_bad)
		uffs_BadBlockAdd(dev, block);

	if (spare)
		uffs_PoolPut(SPOOL(dev), spare);

	return ret;
}

/** Mark this block as bad block */
URET uffs_FlashMarkBadBlock(uffs_Device *dev, int block)
{
	int ret;

	uffs_Perror(UFFS_ERR_NORMAL, "Mark bad block: %d", block);

	if (dev->ops->MarkBadBlock)
		return dev->ops->MarkBadBlock(dev, block) == 0 ? U_SUCC : U_FAIL;

#ifdef CONFIG_ERASE_BLOCK_BEFORE_MARK_BAD
	ret = dev->ops->EraseBlock(dev, block);
	if (ret != UFFS_FLASH_IO_ERR) {
		// note: even EraseBlock return UFFS_FLASH_BAD_BLK,
		//			we still process it ... not recommended for most NAND flash.
#endif
	if (dev->ops->WritePageWithLayout)
		ret = dev->ops->WritePageWithLayout(dev, block, 0, NULL, 0, NULL, NULL);
	else
		ret = dev->ops->WritePage(dev, block, 0, NULL, 0, NULL, 0);

#ifdef CONFIG_ERASE_BLOCK_BEFORE_MARK_BAD
	}
#endif

	return ret == UFFS_FLASH_NO_ERR ? U_SUCC : U_FAIL;
}

/** Is this block a bad block ? */
UBOOL uffs_FlashIsBadBlock(uffs_Device *dev, int block)
{
	u8 status = 0xFF;
	struct uffs_FlashOpsSt *ops = dev->ops;
	u8 *spare;
	UBOOL ret = U_FALSE;

	if (ops->IsBadBlock) {
		/* if flash driver provide 'IsBadBlock' function, call it */
		ret = (ops->IsBadBlock(dev, block) == 0 ? U_FALSE : U_TRUE);
	}
	else {
		/* otherwise we call ReadPage[WithLayout]() to get bad block status byte */
		/* check the first page */
		if (ops->ReadPageWithLayout) {
			ret = (ops->ReadPageWithLayout(dev, block, 0, NULL, 0, NULL, NULL, NULL)
												== UFFS_FLASH_BAD_BLK ? U_TRUE : U_FALSE);
		}
		else {
			ret = (ops->ReadPage(dev, block, 0, NULL, 0, NULL, NULL, 0)
												== UFFS_FLASH_BAD_BLK ? U_TRUE : U_FALSE);
		}

		if (ret == U_FALSE) {
			/* check the second page */
			if (ops->ReadPageWithLayout) {
				ret = (ops->ReadPageWithLayout(dev, block, 0, NULL, 0, NULL, NULL, NULL) 
													== UFFS_FLASH_BAD_BLK ? U_TRUE : U_FALSE);
			}
			else {
				ret = (ops->ReadPage(dev, block, 0, NULL, 0, NULL, NULL, 0)
													== UFFS_FLASH_BAD_BLK ? U_TRUE : U_FALSE);
			}
		}
	}

	//uffs_Perror(UFFS_ERR_NOISY, "Block %d is %s", block, ret ? "BAD" : "GOOD");

	return ret;
}

/** Erase flash block */
URET uffs_FlashEraseBlock(uffs_Device *dev, int block)
{
	int ret;

	ret = dev->ops->EraseBlock(dev, block);

	if (UFFS_FLASH_IS_BAD_BLOCK(ret))
		uffs_BadBlockAdd(dev, block);

	return UFFS_FLASH_HAVE_ERR(ret) ? U_FAIL : U_SUCC;
}

