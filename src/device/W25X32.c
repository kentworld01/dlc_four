//=========================================================================================
//#define MODULE_DEVICE_W25X 1
#include "Device.h"
#include "OS.h"
#include "device/w25x32.h"




#if (MODULE_DEVICE_W25X == 0)
#else

//===========================================================================================
// W25X Status register
//
#define SR_SRP			(1 << 7)
#define SR_TB			(1 << 5)
#define SR_WEL			(1 << 1)
#define SR_BUSY			(1 << 0)

BYTE W25X_ReadStatus(void)
{
	BYTE st;

	_flash_select();

	W25X_SPIWrite(W25X_READ_STATUS);
	st = W25X_SPIRead();

	_flash_deselect();

	return st;
}
BYTE W25X_WaitForReady()
{
	BYTE sr;
	DWORD n;

	for(n = 0; (W25X_ReadStatus() & SR_BUSY) && (n < 0x80000000); n++)
		WDT_CLEAR();

//	TRACE("WaitCount = %d\n", n);

	return sr;
}

// W25X32, 0x00ffef15
//
DWORD W25X_ReadChipInfo(void)
{
	DWORD Info = 0;

	_flash_select();

	W25X_SPIWrite(W25X_READ_CHIPINFO);
	W25X_SPIRead();										// Dummy byte
	W25X_SPIRead();										// Dummy byte
	Info |= ((DWORD)W25X_SPIRead() << 16);
	Info |= ((DWORD)W25X_SPIRead() << 8);
	Info |= ((DWORD)W25X_SPIRead() << 0);

	_flash_deselect();

	return Info;
}

DWORD W25X_ReadJEDECID(void)
{
	DWORD Info = 0;

	_flash_select();

	W25X_SPIWrite(W25X_READ_JEDECID);
	Info |= ((DWORD)W25X_SPIRead() << 16);
	Info |= ((DWORD)W25X_SPIRead() << 8);
	Info |= ((DWORD)W25X_SPIRead() << 0);

	_flash_deselect();

	return Info;
}

void W25X_EnableWrite(void)
{
	_flash_select();

	W25X_SPIWrite(W25X_WRITE_ENABLE);

	_flash_deselect();
}

void W25X_DisableWrite(void)
{
	_flash_select();

	W25X_SPIWrite(W25X_WRITE_DISABLE);

	_flash_deselect();
}

void W25X_ReadData(DWORD address, BYTE *data, DWORD count)
{
	_flash_select();

	W25X_SPIWrite(W25X_READ_DATA);
	W25X_SPIWrite((BYTE)(address >> 16));
	W25X_SPIWrite((BYTE)(address >> 8));
	W25X_SPIWrite((BYTE)(address >> 0));

	for(; count; count--)
		*data++ = W25X_SPIRead();							// Receive data

	_flash_deselect();
}

void W25X_ReadFast(DWORD address, BYTE *data, DWORD count)
{
	_flash_select();

	W25X_SPIWrite(W25X_READ_FAST);
	W25X_SPIWrite((BYTE)(address >> 16));
	W25X_SPIWrite((BYTE)(address >> 8));
	W25X_SPIWrite((BYTE)(address >> 0));

	W25X_SPIRead();										// Dummy byte

	for(; count; count--)
		*data++ = W25X_SPIRead();							// Receive data

	_flash_deselect();
}

// 调用此函数前，必须执行页编程边界检查
//
void W25X_PageProgram(DWORD address, BYTE *data, DWORD count)
{
	W25X_EnableWrite();

	_flash_select();

	W25X_SPIWrite(W25X_PAGE_PROGRAM);
	W25X_SPIWrite((BYTE)(address >> 16));
	W25X_SPIWrite((BYTE)(address >> 8));
	W25X_SPIWrite((BYTE)(address >> 0));

	for(; count; count--)
		W25X_SPIWrite(*data++);					// Send data


	W25X_WaitForReady();
	_flash_deselect();
}

void W25X_ChipErase(void)
{

	W25X_EnableWrite();
	_flash_select();
	W25X_SPIWrite(W25X_ERASE_CHIP);
	_flash_deselect();

	W25X_WaitForReady();
}

void W25X_BlockErase(DWORD address)
{

	W25X_EnableWrite();
	_flash_select();
	W25X_SPIWrite(W25X_ERASE_BLOCK);
	W25X_SPIWrite((BYTE)(address >> 16));
	W25X_SPIWrite((BYTE)(address >> 8));
	W25X_SPIWrite((BYTE)(address >> 0));
	_flash_deselect();

	W25X_WaitForReady();
}

void W25X_SectorErase(DWORD address)
{

	W25X_EnableWrite();
	_flash_select();
	W25X_SPIWrite(W25X_ERASE_SECTOR);
	W25X_SPIWrite((BYTE)(address >> 16));
	W25X_SPIWrite((BYTE)(address >> 8));
	W25X_SPIWrite((BYTE)(address >> 0));
	_flash_deselect();

	W25X_WaitForReady();
}

//===========================================================================================
// 转换层接口，用于封装w25x特性，向统一接口层提供过渡转换接口
//
void w25x_program(DWORD address, BYTE *data, DWORD count)
{
	DWORD offset_in_page, bytes_in_page, write_bytes;

	for(; count; address += write_bytes, data += write_bytes, count -= write_bytes)
	{
		offset_in_page = (address & (W25X32_PAGESIZE - 1));
		bytes_in_page = W25X32_PAGESIZE - offset_in_page;
		write_bytes = min(bytes_in_page, count);

		W25X_PageProgram(address, data, write_bytes);
	}
}

void w25x_copy(BYTE target_bank, DWORD target_address, BYTE source_bank, DWORD source_address, DWORD count)
{
	DWORD offset_in_page, bytes_in_page, copy_bytes;
	BYTE buffer[W25X32_PAGESIZE];

	for(; count; source_address += copy_bytes, target_address += copy_bytes, count -= copy_bytes)
	{
		offset_in_page = (target_address & (W25X32_PAGESIZE - 1));
		bytes_in_page = W25X32_PAGESIZE - offset_in_page;
		copy_bytes = min(bytes_in_page, count);

		_flash_set_bank_selection(source_bank);
		W25X_ReadFast(source_address, buffer, copy_bytes);

		_flash_set_bank_selection(target_bank);
		W25X_PageProgram(target_address, buffer, copy_bytes);
	}
}

// 调用此函数前，必须执行扇区边界检查
//
bool w25x_sector_is_need_erase(DWORD address, BYTE *data, DWORD count)
{
	DWORD compare_bytes, i;
	BYTE buffer[W25X_BUFFER_SIZE];
	BYTE c1, c2;

	for(; count; address += compare_bytes, data += compare_bytes, count -= compare_bytes)
	{
		compare_bytes = min(W25X_BUFFER_SIZE, count);

		W25X_ReadFast(address, buffer, compare_bytes);

		for(i = 0; i < compare_bytes; i++)
		{
			c1 = buffer[i];
			c2 = data[i];
			if ((c1 & c2) != c2)					// Compare data
				return TRUE;
		}
	}

	return FALSE;
}

DWORD w25x_sector_mask_copy(BYTE target_bank, DWORD target_address, BYTE source_bank, DWORD source_address, WORD data_offset, BYTE *data, WORD count)
{
	DWORD copy_bytes;

	assert_param((data_offset + count) <= W25X32_SECTORSIZE);

	_flash_set_bank_selection(target_bank);
	W25X_EnableWrite();
	W25X_SectorErase(target_address);

	copy_bytes = data_offset;
	if (copy_bytes)
	{
		w25x_copy(target_bank, target_address, source_bank, source_address, copy_bytes);
		source_address += copy_bytes;
		target_address += copy_bytes;
	}

	if (count)
	{
		_flash_set_bank_selection(target_bank);
		w25x_program(target_address, data, count);
		source_address += count;
		target_address += count;
	}

	copy_bytes = W25X32_SECTORSIZE - (data_offset + count);
	if (copy_bytes)
		w25x_copy(target_bank, target_address, source_bank, source_address, copy_bytes);

	return TRUE;
}

//===========================================================================================
// flash uni interface
//
DWORD W25X_Read(DWORD address, BYTE* data, DWORD count)
{
	WDT_CLEAR();
	W25X_ReadFast(address, data, count);

	return count;
}

DWORD W25X_Write(DWORD address, BYTE *data, DWORD count)
{
	DWORD offset_in_sector, bytes_in_sector, write_bytes, remain_bytes;

	for(remain_bytes = count; remain_bytes; address += write_bytes, data += write_bytes, remain_bytes -= write_bytes)
	{
		offset_in_sector = (address & (W25X32_SECTORSIZE - 1));
		bytes_in_sector = W25X32_SECTORSIZE - offset_in_sector;
		write_bytes = min(bytes_in_sector, remain_bytes);

		WDT_CLEAR();

		if (w25x_sector_is_need_erase(address, data, write_bytes))
		{
			W25X_EnableWrite();
			W25X_SectorErase(address);
		}

		w25x_program(address, data, write_bytes);
	}

	return count;
}

DWORD W25X_Edit(DWORD address, BYTE *data, DWORD count)
{
	DWORD align_address, offset_in_sector, bytes_in_sector, write_bytes, remain_bytes;
	BYTE bank_target, bank_swap;

	bank_target = _flash_get_bank_selection();
	bank_swap = _flash_get_bank_of_swap();

	for(remain_bytes = count; remain_bytes; address += write_bytes, data += write_bytes, remain_bytes -= write_bytes)
	{
		align_address = address & ~(W25X32_SECTORSIZE - 1);
		offset_in_sector = (address & (W25X32_SECTORSIZE - 1));
		bytes_in_sector = W25X32_SECTORSIZE - offset_in_sector;
		write_bytes = min(bytes_in_sector, remain_bytes);

		WDT_CLEAR();

		if (w25x_sector_is_need_erase(address, data, write_bytes))
		{
			w25x_sector_mask_copy(bank_swap, FLASH_SWAP_ADDRESS, bank_target, align_address, offset_in_sector, data, write_bytes);
			w25x_sector_mask_copy(bank_target, align_address, bank_swap, FLASH_SWAP_ADDRESS, 0, 0, 0);
		}
		else
			w25x_program(address, data, write_bytes);
	}

	return count;
}

bool W25X_Erase(DWORD address, DWORD count)
{
	DWORD i, align_address = address & ~(W25X32_SECTORSIZE - 1);
	DWORD sector_begin = address >> 12;
	DWORD sector_end = (address + count - 1) >> 12;

	for(i = sector_begin; i <= sector_end; i++, align_address += W25X32_SECTORSIZE)
	{
		WDT_CLEAR();

		W25X_EnableWrite();
		W25X_SectorErase(align_address);
	}

	return TRUE;
}





int _flash_get_bank_selection()
{
	return 0;
}

int _flash_get_bank_of_swap()
{
	return 0;
}
int _flash_set_bank_selection( int bank)
{
	return 0;
}


int WDT_CLEAR()
{
	return 0;
}
int WDT_Clear()
{
	return 0;
}

int _flash_select()
{
	return spi_ss(2);
}

unsigned char W25X_SPIWrite( unsigned char cmd )
{
	return spi_send( cmd );
}

int W25X_SPIRead()
{
	return spi_send( 0xff );
	//return spi_send( 0x00 );
}

int _flash_deselect()
{
	return spi_ss(3);
}

int _min( int m1, int m2 )
{
	if( m1 < m2 )
		return m1;
	return m2;
}


#endif
