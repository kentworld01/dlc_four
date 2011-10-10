#ifndef W25X_H
#define W25X_H


#if (MODULE_DEVICE_W25X == 0)

#else

//===========================================================================================
// W25X32基本属性
//
#define W25X_MANUFACTURERID		((DWORD)0x00ef0000)

#define W25Q16_JEDECID				((DWORD)0x00ef4015)
#define W25X16_JEDECID				((DWORD)0x00ef3015)
#define W25X16_CHIPSIZE			((DWORD)0x00200000)	// 芯片容量
#define W25X16_PAGESIZE			((DWORD)0x0100)		// 页面大小
#define W25X16_SECTORSIZE			((DWORD)0x1000)

#define W25Q32_JEDECID				((DWORD)0x00ef4016)
#define W25X32_JEDECID				((DWORD)0x00ef3016)
#define W25X32_CHIPSIZE			((DWORD)0x00400000)	// 芯片容量
#define W25X32_PAGESIZE			((DWORD)0x0100)		// 页面大小
#define W25X32_SECTORSIZE			((DWORD)0x1000)

#define W25Q64_JEDECID				((DWORD)0x00ef4017)
#define W25X64_JEDECID				((DWORD)0x00ef3017)
#define W25X64_CHIPSIZE			((DWORD)0x00800000)	// 芯片容量
#define W25X64_PAGESIZE			((DWORD)0x0100)		// 页面大小
#define W25X64_SECTORSIZE			((DWORD)0x1000)

#define W25X_BUFFER_SIZE			((DWORD)0x0100)		// 缓冲页面大小

//===========================================================================================
BYTE W25X_ReadStatus(void);

DWORD W25X_ReadChipInfo(void);
DWORD W25X_ReadJEDECID(void);
void W25X_EnableWrite(void);
void W25X_DisableWrite(void);

void W25X_ReadData(DWORD address, BYTE *data, DWORD count);
void W25X_ReadFast(DWORD address, BYTE *data, DWORD count);
void W25X_PageProgram(DWORD address, BYTE *data, DWORD count);

void W25X_ChipErase(void);
void W25X_BlockErase(DWORD address);
void W25X_SectorErase(DWORD address);

//flash uni interface
DWORD W25X_Read(DWORD address, BYTE* data, DWORD count);
DWORD W25X_Write(DWORD address, BYTE *data, DWORD count);
DWORD W25X_Edit(DWORD address, BYTE *data, DWORD count);
bool W25X_Erase(DWORD RealAddress, DWORD count);

unsigned char W25X_SPIWrite( unsigned char cmd );





typedef enum
{
	W25X_WRITE_STATUS = 0x01,
	W25X_PAGE_PROGRAM = 0x02,
	W25X_READ_DATA = 0x03,
	W25X_WRITE_DISABLE = 0x04,
	W25X_READ_STATUS = 0x05,
	W25X_WRITE_ENABLE = 0x06,
	W25X_READ_FAST = 0x0b,
	W25X_ERASE_SECTOR = 0x20,
	W25X_READ_FASTDUALOUT = 0x3b,
	W25X_READ_CHIPINFO = 0x90,
	W25X_READ_JEDECID = 0x9f,
	W25X_POWER_ON = 0xab,
	W25X_ERASE_CHIP = 0xc7,
	W25X_ERASE_BLOCK = 0xd8,
	W25X_POWER_DOWN = 0xb9
} W25X_CMD;
#define SR_SRP			(1 << 7)
#define SR_TB			(1 << 5)
#define SR_WEL			(1 << 1)
#define SR_BUSY			(1 << 0)
BYTE W25X_ReadStatus(void);
BYTE W25X_WaitForReady(void);
DWORD W25X_ReadChipInfo(void);
DWORD W25X_ReadJEDECID(void);
void W25X_EnableWrite(void);
void W25X_DisableWrite(void);
void W25X_ReadData(DWORD address, BYTE *data, DWORD count);
void W25X_ReadFast(DWORD address, BYTE *data, DWORD count);
void W25X_PageProgram(DWORD address, BYTE *data, DWORD count);
void W25X_ChipErase(void);
void W25X_BlockErase(DWORD address);
void W25X_SectorErase(DWORD address);
void w25x_program(DWORD address, BYTE *data, DWORD count);
void w25x_copy(BYTE target_bank, DWORD target_address, BYTE source_bank, DWORD source_address, DWORD count);
bool w25x_sector_is_need_erase(DWORD address, BYTE *data, DWORD count);
DWORD w25x_sector_mask_copy(BYTE target_bank, DWORD target_address, BYTE source_bank, DWORD source_address, WORD data_offset, BYTE *data, WORD count);
DWORD W25X_Read(DWORD address, BYTE* data, DWORD count);
DWORD W25X_Write(DWORD address, BYTE *data, DWORD count);
DWORD W25X_Edit(DWORD address, BYTE *data, DWORD count);
bool W25X_Erase(DWORD address, DWORD count);
int _flash_get_bank_selection(void);
int _flash_get_bank_of_swap(void);
int _flash_set_bank_selection( int bank);
int WDT_CLEAR(void);
int WDT_Clear(void);
int _flash_select(void);
unsigned char W25X_SPIWrite( unsigned char cmd );
int W25X_SPIRead(void);
int _flash_deselect(void);
int min( int m1, int m2 );


#endif



















#endif
