//===========================================================================================
#include "Device.h"
#include "OS.h"


//===========================================================================================
//
#define MPL_HEADER_BLOCK_SIZE				(0x0080)

const char OS_BUILDDATE[] = "OS_BUILD:\t"__DATE__;   

DWORD OSEnterCnt = 0;
static MPL_HEADER g_mpl_header;

#if 0
int (*idle_call_back)() = 0;

DWORD GetLpc2134checkSum(DWORD nStartAddr, DWORD nEndAddr)
{
	DWORD nCheckSum, i;
	BYTE *p;

	nCheckSum = 0;

	if ((nEndAddr < nStartAddr) || (nEndAddr - nStartAddr < 0x1000))
		return nCheckSum;

	for(p = 0, i = nStartAddr; i < nEndAddr; i++)
	{	
		if (i == 0x14 || i == 0x134)
			i = i + 3;
		else
			nCheckSum = nCheckSum + p[i];
	}

	return nCheckSum;	
}
#endif

//===========================================================================================
//
DWORD vmm_memory_read(BOOL *result, DWORD address, BYTE *data, DWORD length)
{
	BYTE *p = data;

	*result = FALSE;

#if 0
#if (DEBUG_VERSION == 1)
	// !!!!!! ****** 因代码安全问题，发布版本严禁内存访问 ****** !!!!!!
	//
	if (address < 0x00000040)
	{
		// 需要将0x0的中断向量表重映射到FLASH，才可访问到FLASH的正确内容
		//
		CPU_Enter_Critical();
		MEMMAP = 0x01;					// Vector Map: 00 - Bootloader; 01 - FLASH; 02 - RAM;
	}

	*result = TRUE;
	STREAM_AddBinary(p, address, length);
	p += length;

	if (address < 0x00000040)
	{
		MEMMAP = 0x02;					// Vector Map: 00 - Bootloader; 01 - FLASH; 02 - RAM;
		CPU_Exit_Critical();
	}
#endif
#else						// 伪读操作，仅用于数据传输特性测试
	p += length;

	*result = TRUE;
#endif

	return STREAM_GetLength(data, p);
}

DWORD vmm_memory_write(BOOL *result, DWORD address, BYTE *data, DWORD length)
{
	BYTE *p = data;

	*result = FALSE;

#if 0
#if (DEBUG_VERSION == 1)
	// !!!!!! ****** 因代码安全问题，发布版本严禁内存访问 ****** !!!!!!
	//
	*result = TRUE;
	STREAM_GetBinary(address, p, length);
#endif
#else						// 伪写操作，仅用于数据传输特性测试
	if (address == 0)
		TRACE("\n");
	TRACEh(address);

	*result = TRUE;
#endif

	return STREAM_GetLength(data, p);
}

//===========================================================================================
//
DWORD vmm_flash_read(BOOL *result, DWORD address, BYTE *data, DWORD length)
{
	BYTE *p = data;
	DWORD real_bytes;

	real_bytes = Flash_Read(address, (void *)p, length);
	p += length;

	*result = (real_bytes == length) ? TRUE : FALSE;

	return STREAM_GetLength(data, p);
}

DWORD vmm_flash_write(BOOL *result, DWORD address, BYTE *data, DWORD length)
{
	BYTE *p = data;
	DWORD real_bytes;

	real_bytes = Flash_Write(address, (void *)p, length);
	p += length;

	*result = (real_bytes == length) ? TRUE : FALSE;

	return 0;
}

//===========================================================================================
//
MPL_HEADER *mploader_find_mpl_header(DWORD *buffer, DWORD length)
{
	MPL_HEADER *mpl_header = 0;
	DWORD i;

	for(i = 0; i < length / sizeof(DWORD); i++)
	{
		//TRACEh(buffer[i]);

		if (MPL_MAGICNUMBER == buffer[i])
		{
			mpl_header = (MPL_HEADER *)&buffer[i];
			break;
		}
	}

	return mpl_header;
}

void mploader_load_mpl_header(void)
{
	DWORD address;
	MPL_HEADER *header = 0;

	memset((void *)&g_mpl_header, 0x00, sizeof(MPL_HEADER));

	for(address = 0x0000; (!header) && (address < 0x0200); address += MPL_HEADER_BLOCK_SIZE)
	{
		header = mploader_find_mpl_header((DWORD *)address, MPL_HEADER_BLOCK_SIZE);
		if (header)
		{
			memcpy((void *)&g_mpl_header, (void *)header, sizeof(MPL_HEADER));

			//TRACEx(&g_mpl_header, sizeof(MPL_HEADER));
		}
	}
}

// MPLoader for DLC320SK-V1.03-B0603 --- 该版本未曾发行应用
// MPLoader_for_DLC320SK-V1.04-B0610
// MPLoader_for_DLC320SK-V1.05-B0707
// MPLoader_for_DLC320SK-V1.06-B0924
// MPLoader_for_DLC320SK-V1.07-B0223
// MPLoader_for_DLC320SK-V1.08-B0608
//
// 字符串操作，注意避免出现缓冲区溢出漏洞
//
void mploader_get_full_version_info(MPL_HEADER *mpl_header, char *version_info)
{
	char product[64];
	char version[32], build[32];

	if (version_info)
		version_info[0] = 0;

	strcpy(product, "unknown");
	strcpy(version, "unknown");
	strcpy(build, "unknown");

	if ('M' == mpl_header->ProductName[0])
	{
		memcpy((void *)product, mpl_header->ProductName, 64);
		product[63] = 0;
	}
	
	if ('V' == mpl_header->Version[0])
	{
		memcpy((void *)version, mpl_header->Version, 32);
		version[31] = 0;
	}
	
	if ('B' == mpl_header->BuildNumber[0])
	{
		memcpy((void *)build, mpl_header->BuildNumber, 32);
		build[31] = 0;
	}

	if (version_info)
		s_sprintf(version_info, "%s-%s-%s", product, version, build);
}

void GetMPLHeader(MPL_HEADER *mpl_header)
{
	if (MPL_MAGICNUMBER != g_mpl_header.MagicNumber)
		mploader_load_mpl_header();

	if (MPL_MAGICNUMBER == g_mpl_header.MagicNumber)
		memcpy((void *)mpl_header, (void *)&g_mpl_header, sizeof(MPL_HEADER));
	else
		memset((void *)mpl_header, 0x00, sizeof(MPL_HEADER));
}

DWORD GetProductSN(void)
{
	if (MPL_MAGICNUMBER != g_mpl_header.MagicNumber)
		mploader_load_mpl_header();

	return g_mpl_header.SN;
}

//===============================================================================
//
void app_event_loop(DWORD exit_event_type)
{
	EVENT ev;

	wdt_start();
	SYS_SyncRTC();

	do									// 消息处理循环
	{
		SYS_AutoSyncRTC();

		PeekEvent();					// 生成系统事件

		if (GetEvent(&ev))				// 提取系统事件
		{
			//if( ev.Type != 17 && ev.Type != 22 )
				//_d_int( ev.Type );

			ProcessEvent(&ev);			// 响应系统事件

			if (exit_event_type
					&& (exit_event_type == ev.Type))
				break;
		}
		else
		{
#if 0
			// here is idle
			if( idle_call_back )
			{
				idle_call_back();
			}
#endif
		}

		WDT_Clear();
		cpu_sleep();
	}
	while (1);
}

void App_Run(void)
{
	app_event_loop(EV_APPEXIT);
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	TRACE("ASSERT_FAILED, File: %s, Line: %u\n", file, line);

	//cpu_halt();
}

//===========================================================================================
//
void OS_Init(void)
{
	//TRACE(OS_BUILDDATE); TRACE("\n");

	Timer_Init();
	Event_Init();
	FlashMgr_Init();
	GUI_Init();
//	dir_file_init();
}

int main(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	 	this is done through SystemInit() function which is called from startup
	 	file (startup_stm32f10x_xx.s) before to branch to application main.
	 	To reconfigure the default setting of SystemInit() function, refer to
	 	system_stm32f10x.c file
	*/
	OSEnterCnt = 0;
	CPU_Enter_Critical();
	trace_init();
	Device_Init();
	OS_Init();
	CPU_Exit_Critical();

	//Test_Entry();
	//test_system();
	AppMain(0, 0);

	return 0;
}
