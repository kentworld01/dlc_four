#ifndef DEVICE_H
#define DEVICE_H

//===============================================================================
//
// Device
#include "config.h"

#if 0
#include "__IRQ_Rountine.h"
#include "IO.h"
#include "VIC.h"
#include "Timer.h"
#include "ADC.h"
#include "PWM.h"
#include "RTC.h"
#include "IAP.h"
#include "LCD.h"
#include "AT45D16.h"
#include "W25X32.h"
#include "MX25Lxx.h"
#include "audio_dac.h"
#include "LED.h"
#include "Key.h"
#include "KeyMap.h"
#include "Buzzer.h"
#include "ADC.h"
#include "W5100.h"
//#include "FM1702Q.h"
#include "RC500.h"
#include "PM.h"
#include "PCF8563.h"

#include "sdconfig.h"
#include "sdhal.h"
#include "sddriver.h"
#include "sdcmd.h"
#include "sdcrc.h"

#include "TAS.h"

// My type
#include "DataTran.h"
#include "KeyMap.h"
#else
//#include "stm32f10x_it.h"
//#include "VIC.h"
//#include "Event.h"
//#include "IO.h"
//#include "Uart.h"
//#include "Timer.h"
//#include "RTC.h"
//#include "LCD.h"
//#include "Key.h"
//#include "KeyMap.h"
//#include "Buzzer.h"
//#include "indicator.h"
//#include "output_port.h"
//#include "AT45D16.h"
//#include "MX25Lxx.h"
#include "W25X32.h"
//#include "rfid/fm1702sl.h"
//#include "rfid/RC500.h"
//#include "dm9000x.h"
//#include "em4100_reader.h"
//#include "audio_dac.h"


#endif

//===============================================================================
extern DWORD gModuleDisable, gModuleFail;

typedef enum
{
	MB_NONE						= 0,
	MB_LCD						= (1 << 0),
	MB_ETHERNET				= (1 << 1),
	MB_RC500					= (1 << 2),
	MB_SDCARD					= (1 << 3),
	MB_NORFLASH				= (1 << 4),
	MB_VOICE					= (1 << 5),
	MB_RTC						= (1 << 6),
	MB_PCF8563					= (1 << 7),
	MB_UART0					= (1 << 8),
	MB_UART1					= (1 << 9),
	MB_SLAVEMCU				= (1 << 10),
	MB_KEYPAD					= (1 << 11),
	MB_POLL_IDCARD				= (1 << 12),
	MB_BUZZER					= (1 << 13),
	MB_LOCK						= (1 << 14),
	MB_IOCTRL					= (1 << 15),
	MB_LED						= (1 << 16),
	MB_POLL_ICCARD				= (1 << 17),
	MB_DCIN_STATE				= (1 << 18),
	MB_DCJACK_STATE			= (1 << 19),
	MB_CHG_STATE				= (1 << 20),
	MB_SDWP_STATE				= (1 << 21),
	MB_END
} MODULEDEFINE;

typedef enum
{
	ME_NULL						= 0,
	ME_KEYPAD					= (1 << 0),
	ME_POLLIDCARD				= (1 << 1),
	ME_BUZZER					= (1 << 2),
	ME_LOCK						= (1 << 3),
	ME_IOCTRL					= (1 << 4)
} SMPU_MODULE_ENABLE;

//===============================================================================
//
#define SYSTEMINFO_BASE               ((uint32_t)0x1FFF0000)    /*!< System infomation base address */
#define SI_CPU_U_ID					((unsigned char *)(SYSTEMINFO_BASE + 0xf7e8))
#define SI_FLASH_SIZE					(*(uint16_t *)(SYSTEMINFO_BASE + 0xf7e0))

//===============================================================================
//

//===============================================================================
//
void Device_Init(void);
void Device_HardReset(void);
void Device_SoftReset(void);

bool device_is_fail(DWORD Module);
void device_set_fail(DWORD Module);

bool device_is_disable(DWORD Module);
void device_set_enable(DWORD Module);
void device_set_disable(DWORD Module);

#endif
