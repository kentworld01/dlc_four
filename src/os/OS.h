#ifndef OS_H
#define OS_H

//===========================================================================================
//
// OS
#include "config.h"

#include "Trace.h"
#include "CPU.h"
#include "mp_misc.h"

#include "gui/gui.h"
#include "device/lcd.h"
#include "device/rtc.h"
#include "device/w25x32.h"

#include "DataTran.h"

#include "uffs/inc/uffs/uffs_config.h"
#include "uffs/inc/uffs/uffs_fs.h"
#include "uffs/inc/uffs/uffs_fd.h"
#include "uffs/inc/uffs/uffs_utils.h"


#include "gui4/gui_if.h"
#include "os/vfs/txt_file_index.h"
#include "os/vfs/rand_file.h"

#include "property.h"


//#include "Queue.h"
//#include "Event.h"
//#include "ExtTimer.h"
//#include "FlashMgr.h"
//#include "CommunMgr.h"
//#include "API.h"
//#include "protect_code.h"
//#include "DataTran.h"

// TCP/IP
//#include "tcpip/Socket.h"
//#include "tcpip/tcp.h"
//#include "tcpip/net.h"

// GUI
//#include "GUI.h"

// FAT
//#include "diskio.h"
//#include "ff.h"			/* FatFs configurations and declarations */

// Audio
//#include "wave_file.h"

//#include "DES.h"
//#include "CRC16.h"

//===========================================================================================
//
extern const char OS_BUILDDATE[];

//===========================================================================================
//
typedef struct
{
	DWORD MagicNumber;
	DWORD SizeofHeader;
	DWORD SN;						// 0609100001 ~ 1299999999, mmyyAnnnnn, mm: 月份, yy: 年份, A: 区域, nnnnn: 批号，共10位数字
#if (0)								// DLC320SK-V230
	WORD log_address;
	WORD log_count;
#else								// DLC320SK-V231
	DWORD upgrade_to_k2_time;
#endif
	DWORD Dummy2;
	char *ProductName;
	char *Version;
	char *BuildNumber;
} MPL_HEADER;

typedef struct
{
	DWORD Jump;
	DWORD MagicNumber;
	DWORD SizeofHeader;
	DWORD SizeofImage;
	DWORD Checksum;
	DWORD Offset_ProductName;
	DWORD Offset_Version;
	DWORD Offset_BuildNumber;
} MPS_HEADER;

//===========================================================================================
//
extern DWORD OSEnterCnt;
extern WORD __Vectors[];

//===========================================================================================
// Init
void OS_Init(void);
void Event_Init(void);

DWORD  GetLpc2134checkSum(DWORD nStartAddr,DWORD nEndAddr);
void GetMPLHeader(MPL_HEADER *MPLHeader);
DWORD GetProductSN(void);
bool product_sn_write(DWORD new_sn);
DWORD mploader_get_time_of_upgrade_to_k2(void);

MPL_HEADER *mploader_find_mpl_header(DWORD *buffer, DWORD length);
void mploader_load_mpl_header(void);
void mploader_get_full_version_info(MPL_HEADER *mpl_header, char *version_info);

DWORD vmm_memory_read(bool *result, DWORD address, BYTE *data, DWORD length);
DWORD vmm_memory_write(bool *result, DWORD address, BYTE *data, DWORD length);

DWORD vmm_flash_read(bool *result, DWORD address, BYTE *data, DWORD length);
DWORD vmm_flash_write(bool *result, DWORD address, BYTE *data, DWORD length);

//===========================================================================================
extern void Test_Entry(void);
extern int AppMain(int a, int b);

#endif
