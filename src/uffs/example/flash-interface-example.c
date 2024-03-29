#define _d_w25x_block_erase 0
#define _d_w25x_sector_erase 1

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
  the GNU General Public License v2.my_application_main_entry
 
  This exception does not invalidate any other reasons why a work based
  on this file might be covered by the GNU General Public License.
*/
/**
 * \file flash-interface-example.c
 * \brief example for using flash driver and multiple partitions, with static memory allocator.
 * \author Ricky Zheng, created at 27 Nov, 2007
 */
  
#include <string.h>

#include "uffs/uffs_device.h"
#include "uffs/uffs_flash.h"
#include "uffs/uffs_mtb.h"
#include "uffs/uffs_fs.h"

#define PFX "nand-drv:"



#if _d_w25x_block_erase
#define _d_w25x_page_size 2048
#define _d_w25x_block_size (64*1024)
#define TOTAL_BLOCKS    (3*1024/64)	// use header 3 m space.
#endif
#if _d_w25x_sector_erase
#define _d_w25x_page_size 256
#define _d_w25x_block_size (4*1024)
#define TOTAL_BLOCKS    (3*1024/4)	// use header 3 m space.
#endif

#define PAGE_SPARE_SIZE 16
#define PAGE_DATA_SIZE  ( _d_w25x_page_size - PAGE_SPARE_SIZE)
#define PAGES_PER_BLOCK ( _d_w25x_block_size / _d_w25x_page_size )
#define PAGE_SIZE	(PAGE_DATA_SIZE + PAGE_SPARE_SIZE)
#define BLOCK_DATA_SIZE (PAGE_DATA_SIZE * PAGES_PER_BLOCK)
#define NR_PARTITION	1								/* total partitions */
#define PAR_1_BLOCKS	(TOTAL_BLOCKS)	/* partition 2 */


struct my_nand_chip {
	void *IOR_ADDR;
	void *IOW_ADDR;
	UBOOL inited;
	// ... 
};

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_RESET		0xff



int write_data_addr( uffs_Device *dev, u32 block, u32 page, u32 addr )
{
}
int write_erase_addr( uffs_Device *dev, u32 block )
{
}
int write_data( uffs_Device *dev, u8* data, u32 len )
{
}
int read_data( uffs_Device *dev, u8* data, u32 len )
{
}

/* impelent the following functions for your NAND flash */
#define CHIP_SET_CLE(chip) {}
#define CHIP_CLR_CLE(chip) {}
#define CHIP_SET_ALE(chip) {}
#define CHIP_CLR_ALE(chip) {}
#define CHIP_SET_NCS(chip) {}
#define CHIP_CLR_NCS(chip) {}
#define CHIP_BUSY(chip) {}
#define CHIP_READY(chip) {}
#define WRITE_COMMAND(chip, cmd) {}
#define WRITE_DATA_ADDR(chip, block, page, offset) {}
#define WRITE_ERASE_ADDR(chip, block) {}
#define WRITE_DATA(chip, data, len) {}
#define READ_DATA(chip, data, len) {}

#define PARSE_STATUS(v) (UFFS_FLASH_NO_ERR)	// parse status to UFFS_FLASH_NO_ERR or UFFS_FLASH_BAD_BLK



#if CONFIG_USE_STATIC_MEMORY_ALLOCATOR == 0
int main()
{
	uffs_Perror(UFFS_ERR_NORMAL, "This example need CONFIG_USE_STATIC_MEMORY_ALLOCATOR = 1");
	return 0;
}
#else


static int nand_read_page(uffs_Device *dev, u32 block, u32 page, u8 *data, int data_len, u8 *ecc,
						u8 *spare, int spare_len)
{
	u8 val = 0;
	int ret = UFFS_FLASH_NO_ERR;
	struct my_nand_chip *chip = (struct my_nand_chip *) dev->attr->_private;

	CHIP_CLR_NCS(chip);
	if (data && data_len > 0) {
#if 1
		W25X_ReadData( block* _d_w25x_block_size +page* _d_w25x_page_size , data, data_len );
#else
		CHIP_SET_CLE(chip);
		WRITE_COMMAND(chip, NAND_CMD_READ0);
		CHIP_CLR_CLE(chip);
		CHIP_SET_ALE(chip);
		WRITE_DATA_ADDR(chip, block, page, 0);
		CHIP_CLR_ALE(chip);
		READ_DATA(chip, data, data_len);

		// for now, we return all 0xFF to pass UFFS mount, you should remove this at your driver
		memset(data, 0xFF, data_len);
#endif
	}

	if (spare && spare_len > 0) {
#if 1
		W25X_ReadData( block* _d_w25x_block_size +page* _d_w25x_page_size  + dev->attr->page_data_size, spare, spare_len );
#else
		CHIP_SET_CLE(chip);
		WRITE_COMMAND(chip, NAND_CMD_READOOB);
		CHIP_CLR_CLE(chip);
		CHIP_SET_ALE(chip);
		WRITE_DATA_ADDR(chip, block, page, dev->attr->page_data_size);
		CHIP_CLR_ALE(chip);
		READ_DATA(chip, spare, spare_len);

		// for now, we return all 0xFF to pass UFFS mount, you should remove this at your driver
		memset(spare, 0xFF, spare_len);
#endif
	}

	if (data == NULL && spare == NULL) {
#if 1
		W25X_ReadData( block* _d_w25x_block_size +page* _d_w25x_page_size  + dev->attr->page_data_size + dev->attr->block_status_offs, &val, 1 );
		ret = (val == 0xFF ? UFFS_FLASH_NO_ERR : UFFS_FLASH_BAD_BLK);
#else
		// read bad block mark
		CHIP_SET_CLE(chip);
		WRITE_COMMAND(chip, NAND_CMD_READOOB);
		CHIP_CLR_CLE(chip);
		CHIP_SET_ALE(chip);
		WRITE_DATA_ADDR(chip, block, page, dev->attr->page_data_size + dev->attr->block_status_offs);
		CHIP_CLR_ALE(chip);
		READ_DATA(chip, &val, 1);
		ret = (val == 0xFF ? UFFS_FLASH_NO_ERR : UFFS_FLASH_BAD_BLK);

		// for now, we return UFFS_FLASH_NO_ERR to pass UFFS mount, you should remove this at your driver
		ret = UFFS_FLASH_NO_ERR;
#endif
	}

	CHIP_SET_NCS(chip);

	return ret;
}

static int nand_write_page(uffs_Device *dev, u32 block, u32 page,
							const u8 *data, int data_len, const u8 *spare, int spare_len)
{
	u8 val = 0;
	int ret = UFFS_FLASH_NO_ERR;
	UBOOL fall_through = FALSE;
	struct my_nand_chip *chip = (struct my_nand_chip *) dev->attr->_private;

	CHIP_CLR_NCS(chip);

	if (data && data_len > 0) {
#if 1
		w25x_program( block* _d_w25x_block_size +page* _d_w25x_page_size  + 0, data, data_len );
		//W25X_PageProgram( block* _d_w25x_block_size +page* _d_w25x_page_size  + 0, data, data_len );
#else
		CHIP_SET_CLE(chip);
		WRITE_COMMAND(chip, NAND_CMD_READ0);
		WRITE_COMMAND(chip, NAND_CMD_SEQIN);
		CHIP_CLR_CLE(chip);
		CHIP_SET_ALE(chip);
		WRITE_DATA_ADDR(chip, block, page, 0);
		CHIP_CLR_ALE(chip);
		CHIP_BUSY(chip);
		WRITE_DATA(chip, data, data_len);
#endif
		if (data_len == dev->attr->page_data_size)
			fall_through = U_TRUE;
		else {
#if 1
			ret = PARSE_STATUS(val);
#else
			CHIP_SET_CLE(chip);
			WRITE_COMMAND(chip, NAND_CMD_PAGEPROG);
			WRITE_COMMAND(chip, NAND_CMD_STATUS);
			CHIP_CLR_CLE(chip);
			CHIP_READY(chip);
			READ_DATA(chip, &val, 1);
			ret = PARSE_STATUS(val);
#endif
		}
	}

	if (ret != UFFS_FLASH_NO_ERR)
		goto ext;

	if (spare && spare_len > 0) {
#if 1
		W25X_PageProgram( block* _d_w25x_block_size +page* _d_w25x_page_size  + dev->attr->page_data_size, spare, spare_len );
		ret = PARSE_STATUS(val);
#else
		if (!fall_through) {
			CHIP_SET_CLE(chip);
			WRITE_COMMAND(chip, NAND_CMD_READOOB);
			WRITE_COMMAND(chip, NAND_CMD_SEQIN);
			CHIP_CLR_CLE(chip);
			CHIP_SET_ALE(chip);
			WRITE_DATA_ADDR(chip, block, page, dev->attr->page_data_size);
			CHIP_CLR_ALE(chip);
			CHIP_BUSY(chip);
		}
		WRITE_DATA(chip, spare, spare_len);
		CHIP_SET_CLE(chip);
		WRITE_COMMAND(chip, NAND_CMD_PAGEPROG);
		WRITE_COMMAND(chip, NAND_CMD_STATUS);
		CHIP_CLR_CLE(chip);
		CHIP_READY(chip);
		READ_DATA(chip, &val, 1);
		ret = PARSE_STATUS(val);
#endif
	}

	if (data == NULL && spare == NULL) {
#if 1
		val = 0;
		W25X_PageProgram( block* _d_w25x_block_size +page* _d_w25x_page_size  + dev->attr->page_data_size+ dev->attr->block_status_offs, &val, 1 );
		ret = PARSE_STATUS(val);
#else
		// mark bad block
		CHIP_SET_CLE(chip);
		WRITE_COMMAND(chip, NAND_CMD_READOOB);
		WRITE_COMMAND(chip, NAND_CMD_SEQIN);
		CHIP_CLR_CLE(chip);
		CHIP_SET_ALE(chip);
		WRITE_DATA_ADDR(chip, block, page, dev->attr->page_data_size + dev->attr->block_status_offs);
		CHIP_CLR_ALE(chip);
		CHIP_BUSY(chip);
		val = 0;
		WRITE_DATA(chip, &val, 1);
		CHIP_SET_CLE(chip);
		WRITE_COMMAND(chip, NAND_CMD_PAGEPROG);
		WRITE_COMMAND(chip, NAND_CMD_STATUS);
		CHIP_CLR_CLE(chip);
		CHIP_READY(chip);
		READ_DATA(chip, &val, 1);
		ret = PARSE_STATUS(val);
#endif
	}

ext:
	CHIP_SET_NCS(chip);

	return ret;
}

static int nand_erase_block(uffs_Device *dev, u32 block)
{
	u8 val;
	struct my_nand_chip *chip = (struct my_nand_chip *) dev->attr->_private;

	CHIP_CLR_NCS(chip);

#if 1
#if _d_w25x_block_erase
	W25X_BlockErase( block* _d_w25x_block_size  );
#endif
#if _d_w25x_sector_erase
	W25X_SectorErase( block* _d_w25x_block_size  );
#endif
#else
	CHIP_SET_CLE(chip);
	WRITE_COMMAND(chip, NAND_CMD_ERASE1);
	CHIP_CLR_CLE(chip);
	CHIP_SET_ALE(chip);
	WRITE_ERASE_ADDR(chip, blcok);
	CHIP_CLR_ALE(chip);
	CHIP_SET_CLE(chip);
	WRITE_COMMAND(chip, NAND_CMD_ERASE2);
	WRITE_COMMAND(chip, NAND_CMD_STATUS);
	CHIP_CLR_CLE(chip);
	CHIP_READY(chip);
	READ_DATA(chip, &val, 1);
#endif

	CHIP_SET_NCS(chip);

	return PARSE_STATUS(val);
}


static int nand_init_flash(uffs_Device *dev)
{
	// initialize your hardware here ...
	struct my_nand_chip *chip = (struct my_nand_chip *) dev->attr->_private;

	if (!chip->inited) {
		// setup chip I/O address, setup NAND flash controller ... etc.
		// chip->IOR_ADDR = 0xF0000000
		// chip->IOW_ADDR = 0xF0000000
		chip->inited = U_TRUE;
	}
	return 0;
}

static int nand_release_flash(uffs_Device *dev)
{
	// release your hardware here
	struct my_nand_chip *chip = (struct my_nand_chip *) dev->attr->_private;

	return 0;
}

static uffs_FlashOps g_my_nand_ops = {
	nand_init_flash,	// InitFlash()
	nand_release_flash,	// ReleaseFlash()
	nand_read_page,		// ReadPage()
	NULL,				// ReadPageWithLayout
	nand_write_page,	// WritePage()
	NULL,				// WirtePageWithLayout
	NULL,				// IsBadBlock(), let UFFS take care of it.
	NULL,				// MarkBadBlock(), let UFFS take care of it.
	nand_erase_block,	// EraseBlock()
};

/////////////////////////////////////////////////////////////////////////////////

// change these parameters to fit your nand flash specification

#if 0
#define TOTAL_BLOCKS    1024
#define PAGE_DATA_SIZE  512
#define PAGE_SPARE_SIZE 16
#define PAGES_PER_BLOCK 32
#define PAGE_SIZE		(PAGE_DATA_SIZE + PAGE_SPARE_SIZE)
#define BLOCK_DATA_SIZE (PAGE_DATA_SIZE * PAGES_PER_BLOCK)
#define NR_PARTITION	2								/* total partitions */
#define PAR_1_BLOCKS	100								/* partition 1 */
#define PAR_2_BLOCKS	(TOTAL_BLOCKS - PAR_1_BLOCKS)	/* partition 2 */
struct my_nand_chip g_nand_chip = {0};
static struct uffs_StorageAttrSt g_my_flash_storage = {0};

/* define mount table */
static uffs_Device demo_device_1 = {0};
static uffs_Device demo_device_2 = {0};

static uffs_MountTable demo_mount_table[] = {
	{ &demo_device_1,  0, PAR_1_BLOCKS - 1, "/data/" },
	{ &demo_device_2,  PAR_1_BLOCKS, PAR_1_BLOCKS + PAR_2_BLOCKS - 1, "/" },
	{ NULL, 0, 0, NULL }
};

/* static alloc the memory for each partition */
static int static_buffer_par1[UFFS_STATIC_BUFF_SIZE(PAGES_PER_BLOCK, PAGE_SIZE, PAR_1_BLOCKS) / sizeof(int)];
static int static_buffer_par2[UFFS_STATIC_BUFF_SIZE(PAGES_PER_BLOCK, PAGE_SIZE, PAR_2_BLOCKS) / sizeof(int)];;
#else

struct my_nand_chip g_nand_chip = {0};
static struct uffs_StorageAttrSt g_my_flash_storage = {0};

/* define mount table */
static uffs_Device demo_device_1 = {0};
static uffs_Device demo_device_2 = {0};

static uffs_MountTable demo_mount_table[] = {
	{ &demo_device_1,  0, PAR_1_BLOCKS - 1, "/" },
	{ NULL, 0, 0, NULL }
};

/* static alloc the memory for each partition */
static int static_buffer_par1[UFFS_STATIC_BUFF_SIZE(PAGES_PER_BLOCK, PAGE_SIZE, PAR_1_BLOCKS) / sizeof(int)];
#endif



static void init_nand_chip(struct my_nand_chip *chip)
{
	// init chip IO address, etc.
}

static void setup_flash_storage(struct uffs_StorageAttrSt *attr)
{
	memset(attr, 0, sizeof(struct uffs_StorageAttrSt));
	
	attr->total_blocks = TOTAL_BLOCKS;			/* total blocks */
	attr->page_data_size = PAGE_DATA_SIZE;		/* page data size */
	attr->pages_per_block = PAGES_PER_BLOCK;	/* pages per block */
	attr->spare_size = PAGE_SPARE_SIZE;		  	/* page spare size */
	attr->block_status_offs = 4;				/* block status offset is 5th byte in spare */
	attr->ecc_opt = 0;              /* ecc option */
	//attr->ecc_opt = UFFS_ECC_SOFT;              /* ecc option */
	attr->layout_opt = UFFS_LAYOUT_UFFS;        /* let UFFS do the spare layout */    
}

static URET my_InitDevice(uffs_Device *dev)
{
	dev->attr = &g_my_flash_storage;
	dev->attr->_private = (void *) &g_nand_chip;	// hook nand_chip data structure to attr->_private
	dev->ops = &g_my_nand_ops;

    
	return U_SUCC;
}

static URET my_ReleaseDevice(uffs_Device *dev)
{
	return U_SUCC;
}


static int my_init_filesystem(void)
{
	uffs_MountTable *mtbl = &(demo_mount_table[0]);

	/* setup nand storage attributes */
	setup_flash_storage(&g_my_flash_storage);

	/* setup memory allocator */
	uffs_MemSetupStaticAllocator(&demo_device_1.mem, static_buffer_par1, sizeof(static_buffer_par1));
	//uffs_MemSetupStaticAllocator(&demo_device_2.mem, static_buffer_par2, sizeof(static_buffer_par2));

	/* register mount table */
	while(mtbl->dev) {
		// setup device init/release entry
		mtbl->dev->Init = my_InitDevice;
		mtbl->dev->Release = my_ReleaseDevice;
		uffs_RegisterMountTable(mtbl);
		mtbl++;
	}
	
	return uffs_InitMountTable() == U_SUCC ? 0 : -1;
}

/* application entry */
int uffs_flash_interface_main()
{
	my_init_filesystem();

	// ... my application codes ....
	// read/write/create/delete files ...

	uffs_ReleaseMountTable();

	return 0;
}

int uffs_flash_interface_init()
{
	my_init_filesystem();
	return 0;
}

#endif


/////////////////////////////////////////////////////////////////////////////////
