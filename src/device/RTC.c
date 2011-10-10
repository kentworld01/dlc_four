//===============================================================================



#include "hw_types.h"
#include "hw_hibernate.h"
#include "hibernate.h"



#include "Device.h"
#include "OS.h"



#if (MODULE_DEVICE_RTC == 0)
#else

//===============================================================================
static BOOL RTC_32K_GOOD = TRUE;



void RTC_Set( unsigned long val )
{
	//HibernateRTCSet( val );
    	HWREG(HIB_RTCLD) = val;
}

unsigned long RTC_Get()
{
	return HibernateRTCGet();
}

void RTC_SelectSubClock(void)
{
}

void RTC_SelectFPClock(void)
{
}

void rtc_check_clock(void)
{
#if 0
	DWORD t1, t2, dt;

	t1 = CTC;
	__Delay(10);
	t2 = CTC;
	dt = (t1 > t2) ? (t1 - t2) : (t2 - t1);
	TRACE("dt = 0x%08x\n", dt);

	//if ((dt > 0x180) && (2165 != YEAR))// 检测32768Hz时钟及RTC是否正常

	if (dt > 0x180)								// 检测32768Hz时钟及RTC是否正常
		RTC_32K_GOOD = TRUE;
	else
		RTC_32K_GOOD = FALSE;

	TRACE("RTC_32K_Good = %d\n", RTC_32K_GOOD);
#endif
}

void rtc_enable_write_access(void)
{
	/* Enable PWR and BKP clocks */
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	//PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	//BKP_DeInit();
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
BOOL rtc_configuration(void)
{
#if 0
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
#if 1
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
#else
	for(n = 0x00080000; n && (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); n--);
	if (0 == n)
	{
		TRACEs("FAIL");
		return FALSE;
	}
#endif

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	/* Wait until last write operation on RTC registers has finished */
	//RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	/* Wait until last write operation on RTC registers has finished */
	//RTC_WaitForLastTask();
#endif
	return TRUE;
}

/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void rtc_set_default_time(void)
{
	DATETIME dt;

	TRACE("\n RTC configured....\n");

	dt.year = 2010;
	dt.month = 11;
	dt.day = 8;
	dt.hour = 17;
	dt.minute = 12;
	dt.second = 0;

	_rtc_set(&dt);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void RTC_NVIC_Configuration(void)
{
#if 0
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
#endif
}

BOOL rtc_diag(void)
{
#if 0
	DWORD n = 0;

	/* Clear RSF flag */
	RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;

	/* Loop until RSF flag is set */
	while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET)
	{
		if (++n >= 20000)
		{
			TRACEs("RTC FAILURE!");
			return FALSE;
		}
	}
#endif
	return TRUE;
}

void RTC_Init(void)
{
#if 0
	RTC_NVIC_Configuration();

	if (0xA5A5 != BKP_ReadBackupRegister(BKP_DR1))
	{
		/* Backup data register value is not correct or not yet programmed (when
		the first time the program is executed) */
		TRACE("\n\n RTC not yet configured....");

		rtc_enable_write_access();
		if (!rtc_configuration())
			return;
		rtc_set_default_time();

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			TRACE("\n\n Power On Reset occurred....");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			TRACE("\n\n External Reset occurred....");
		}

		TRACE("\n No need to configure RTC....\n");

		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();
	}

#if 0		// Enable the RTC Second interrupt
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	/* Wait until last write operation on RTC registers has finished */
	//RTC_WaitForLastTask();
#endif

#if 0		// #ifdef RTCClockOutput_Enable
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Disable the Tamper Pin */
	BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	          	               functionality must be disabled */

	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

	//RTC_32K_GOOD = rtc_diag();

#if (RTC_CLOCK_32768KHZ == 1)
	RTC_SelectSubClock();
	rtc_check_clock();
#endif

#if (RTC_CLOCK_FPCLK == 1)
	if (!RTC_32K_GOOD)							// 32K失效或未启动
		RTC_SelectFPClock();
#endif

#endif
}

BOOL RTC_32K_Good(void)
{
	return RTC_32K_GOOD;
}

void RTC_GetTime(void *DateTime)
{
	DATETIME *dt;
	DWORD rt;

	dt = (DATETIME *)DateTime;
	if (!dt)
		return;

	rt = RTC_Get();
	RelatvieToDateTime(dt, rt);
}

void _rtc_set(void *date_time)
{
	DATETIME *dt = (DATETIME *)date_time;
	DWORD rt;

	/* Wait until last write operation on RTC registers has finished */
	//RTC_WaitForLastTask();

	rt = DateTimeToRelatvie(dt);
	RTC_Set(rt);

	/* Wait until last write operation on RTC registers has finished */
	//RTC_WaitForLastTask();
}

void RTC_SetTime(void *DateTime)
{
	rtc_enable_write_access();
	if (!rtc_configuration())
		return;
	_rtc_set(DateTime);

	//BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);	// FIXME BKP_DR1 = ?
}

void rtc_display_time(void)
{
	DATETIME dt;

	RTC_GetTime(&dt);

	TRACE("Time: %04u-%02u-%02u %02u:%02u:%02u\n",
		dt.year, dt.month, dt.day,
		dt.hour, dt.minute, dt.second);
}

void rtc_init(void)
{
	HibernateEnableExpClk( SysCtlClockGet() );
	HibernateRTCEnable();
	//rtc_set_default_time();
}

#endif
