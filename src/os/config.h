#ifndef CONFIG_A0106_H
#define CONFIG_A0106_H

#define PLATFORM_MSMTECH_A0106 1
#if (PLATFORM_MSMTECH_A0106 == 1)

#include "typedef.h"

//===================================================================================
//
// Debug
//
#define DEBUG_VERSION					(1)						// 使能调试版本
#define DEBUG_SLIENCE					(0)						// 调试版本静音，家中需要保持安静时使用；对发布版本无效
#define DEBUG_PORT						(1)						// 通过UART0或UART1输出调试信息
#define DEBUG_SINGLESTEP				(0)
#define DEBUG_FONT						(0)
#define DEBUG_SD						(0)

//===================================================================================
//
#define PLATFORM_HARDWARE_GS05		(0)						// 硬件平台型号定义
#define KEY_FOR_UPGRADE_MPLOADER		(PKT_ALL_TRANSMIT_V2)	// Key system

//===================================================================================
// Device
//
#define MODULE_DEVICE_RTC				(1)
#define MODULE_DEVICE_PCF8563			(0)
#define RTC_CLOCK_32768KHZ			(1)
#define RTC_CLOCK_FPCLK				(0)

#define DEVICE_UART0_BAUD				(115200)
#define DEVICE_UART1_BAUD				(115200)

#define MODULE_DEVICE_RTC				(1)
#define MODULE_DEVICE_ADC				(1)
#define MODULE_DEVICE_PWM				(0)
#define MODULE_DEVICE_SPI0			(1)
#define MODULE_DEVICE_SPI1			(1)
#define MODULE_DEVICE_IIC0			(1)
#define MODULE_DEVICE_AT45			(1)
#define MODULE_DEVICE_W25X			(1)
#define MODULE_DEVICE_K9F				(0)
#define MODULE_DEVICE_RC500			(1)
#define MODULE_DEVICE_W5100			(0)
#define MODULE_DEVICE_DM9000			(1)
#define MODULE_DEVICE_BC7281			(0)
#define MODULE_DEVICE_LED				(0)
#define MODULE_DEVICE_KEY				(1)
#define MODULE_DEVICE_BUZZER			(1)
#define MODULE_DEVICE_SD				(1)
#define MODULE_DEVICE_S1				(0)
#define MODULE_DEVICE_PM				(1)
#define MODULE_DEVICE_TAS				(0)
#define MODULE_DEVICE_FM1702Q			(0)
#define MODULE_DEVICE_FM1702SL		(1)
#define MODULE_DEVICE_RC500			(1)

#define MODULE_DEVICE_LCD				(1)
#define MODULE_DEVICE_LCD_COUNT		(1)
#define LCD_INTERFACE_8080			(1)
#define LCD_INTERFACE_6800			(0)
#define LCD_VENDOR_TPV					(1)
#define LCD_VENDOR_ET					(0)

#if (MODULE_DEVICE_FM1702SL == 0)
#else
	#define _d_fm1702sl_modules
#endif

//===================================================================================
// Memory
//
#define _d_use_flash_manager_version 		2

#define FLASH_BANK_COUNT				(1)
#define FLASH_SWAP_SECTOR				(0x00003000 / W25X32_SECTORSIZE)
#define FLASH_SWAP_ADDRESS			(FLASH_SWAP_SECTOR * W25X32_SECTORSIZE)
#define FLASH_START					((DWORD)0x00000000)
#define FLASH_SIZE						(flash_get_size())

#define ROM_START						((DWORD)0x08000000)
#define ROM_SIZE						((DWORD)0x00040000)
#define RAM_START						((DWORD)0x20000000)
#define RAM_SIZE						((DWORD)0x00004000)

#define IS_IN_ROM(adress)				(((DWORD)(adress) >= ROM_START) && ((DWORD)(adress) < (ROM_START + ROM_SIZE)))
#define IS_IN_RAM(adress)				(((DWORD)(adress) >= RAM_START) && ((DWORD)(adress) < (RAM_START + RAM_SIZE)))
#define IS_IN_FLASH(adress)			(((DWORD)(adress) >= FLASH_START) && ((DWORD)(adress) < (FLASH_START + FLASH_SIZE)))

//===================================================================================
// OS
//
#define MODULE_OS_FLASH_CACHE			(0)		// 尚未考虑周全，勿开启此功能
#define MODULE_OS_FLASH_MGR			(1)
#define MODULE_OS_FAT					(1)
#define MODULE_OS_TCPIP				(1)

// GUI
#define MODULE_GUI_SYSTEM				(1)
#define MODULE_GUI_INVERT_SCREEN		(1)
// GUI objects
#define MODULE_GUI_SYSTEM_FONT		(1)
#define MODULE_GUI_SYSTEM_BITMAP		(1)
#define MODULE_GUI_SYSTEM_TEXTTOOLS	(1)
#define MODULE_GUI_SYSTEM_GRAPHTOOLS	(1)
#define MODULE_GUI_SYSTEM_FORM		(1)
#define MODULE_GUI_SYSTEM_DIALOGBOX	(1)
#define MODULE_GUI_SYSTEM_LABEL		(1)
#define MODULE_GUI_SYSTEM_LIST		(1)
#define MODULE_GUI_SYSTEM_EDITOR		(1)
#define MODULE_GUI_SYSTEM_MENU		(1)

// APP
#define MODULE_APP_SALE				(0)
#define MODULE_APP_DES					(1)


//#define _d_dir_file_modules
#define _d_basic_modules
#if (MODULE_OS_TCPIP == 0)
#else
	#define _d_net_modules
#endif


#endif



// modules define

#define _d_sd 0
#define _d_uffs 1


#endif
