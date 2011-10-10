//===================================================================================
#include "Device.h"
#include "OS.h"


//===================================================================================
//
DWORD gModuleDisable, gModuleFail;

//===================================================================================
bool GSC_Cmd51_ModuleEnable(BYTE ModuleEnable);

void Device_HardReset(void)
{
#if 0
#if PLATFORM_MSMTECH_A0020 == 1
	IO_Write(IOI_NRST3V, IOS_LOW);			// 硬件复位所有外设
	__Delay(30);
	IO_Write(IOI_NRST3V, IOS_HIGH);
	__Delay(30);
	//Pause();
#endif
#if PLATFORM_MSMTECH_A0106 == 1
	IO_Write(IOI_NRST3V, IOS_LOW);			// 硬件复位所有外设
	IO_Write(IOI_RST3V, IOS_HIGH);
	__Delay(30);
	IO_Write(IOI_NRST3V, IOS_HIGH);
	IO_Write(IOI_RST3V, IOS_LOW);			// 硬件复位所有外设
	__Delay(30);
#endif
#endif
}

void Device_SoftReset(void)
{
#if 0
	LCD_SoftReset();
	RC500_SoftReset();
	W5100_SoftReset();
	__Delay(25);							// 外设软复位
#endif
}

bool device_is_fail(DWORD Module)
{
	return ((gModuleFail & Module) ? TRUE : FALSE);
}

void device_set_fail(DWORD Module)
{
	gModuleFail |= Module;
}

bool device_is_disable(DWORD Module)
{
	return ((gModuleDisable & Module) ? TRUE : FALSE);
}

void device_set_enable(DWORD Module)
{
	gModuleDisable &= ~Module;

#if (MODULE_DEVICE_S1 == 1)
	if (Module & MB_POLL_IDCARD) {
		GSC_Cmd51_ModuleEnable(ME_BUZZER | ME_LOCK | ME_KEYPAD | ME_POLLIDCARD);
	}
#endif
}

void device_set_disable(DWORD Module)
{
	gModuleDisable |= Module;

#if (MODULE_DEVICE_S1 == 1)
	if (Module & MB_POLL_IDCARD) {
		GSC_Cmd51_ModuleEnable(ME_BUZZER | ME_LOCK | ME_KEYPAD);
	}
#endif
}

//===================================================================================
//
void _device_init(void)
{
#if 0
	// 将中断向量表从FLASH加载到RAM
	memcpy((void *)VectorTable, (void *)Vectors_START, Vectors_END - Vectors_START);
	MEMMAP = 0x02;					// Vector Map: 00 - Bootloader; 01 - FLASH; 02 - RAM;

	// 设置系统时钟
	PLLCON = 1;
	APBDIV = APBDIV_BITS(APB_DIV);
	PLLCFG = MSEL_BITS(PLL_M - 1) | PSEL_BITS(PLL_P);
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	while((PLLSTAT & (1 << PLOCK)) == 0);			// 等待PLL锁定
	PLLCON = (1 << EN_PLLE) | (1 << EN_PLLC);		// 使能并连接PLL
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	// 设置存储器加速模块
	MAMCR = 0x0;
#if (FCCLK < 20000000)
	MAMTIM = 0x1;
#elif (FCCLK < 40000000)
	MAMTIM = 0x2;
#else
	MAMTIM = 0x3;
#endif
	MAMCR = 0x2;

    // 初始化VIC
    VICIntEnClr = 0xffffffff;
    VICVectAddr = 0;
    VICIntSelect = 0;
#endif
}

void device_trace_soc_register(void)
{
#if 0
	TRACEd(SI_FLASH_SIZE);
	TRACEx(SI_CPU_U_ID, 12);			//37 ff ff ff 59 34 35 30 47 29 10 43 
	TRACEh(__Vectors);
	TRACEx(0x1ffff800, 0x10);								// Flash option bytes
#endif

#if 0
	TRACEh(__Vectors);
	TRACEh((DWORD)__Vectors - 0x08000000);
#endif

#if 0
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_ClocksTypeDef RCC_ClocksStatus;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_MCOConfig(0x04);

	TRACEd(HSE_VALUE);
	TRACEd(SystemCoreClock);
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	TRACEd(RCC_ClocksStatus.SYSCLK_Frequency);
	TRACEd(RCC_ClocksStatus.HCLK_Frequency);
	TRACEd(RCC_ClocksStatus.PCLK1_Frequency);
	TRACEd(RCC_ClocksStatus.PCLK2_Frequency);
	TRACEd(RCC_ClocksStatus.ADCCLK_Frequency);

	TRACEh(RCC->BDCR);
	TRACEh(RCC->CR);
	TRACEh(RCC->CFGR);
	TRACEh(RCC->CIR);
	TRACEh(RCC->CSR);
	TRACEh(RCC->APB1ENR);
	TRACEh(RCC->APB2ENR);

	TRACEb(DWTOB_L1(RCC->BDCR));
	TRACEb(DWTOB_L2(RCC->BDCR));
	TRACEb(DWTOB_L3(RCC->BDCR));
	TRACEb(DWTOB_L4(RCC->BDCR));

	TRACEb(DWTOB_L1(RCC->CR));
	TRACEb(DWTOB_L2(RCC->CR));
	TRACEb(DWTOB_L3(RCC->CR));
	TRACEb(DWTOB_L4(RCC->CR));

	TRACEb(DWTOB_L1(RCC->CFGR));
	TRACEb(DWTOB_L2(RCC->CFGR));
	TRACEb(DWTOB_L3(RCC->CFGR));
	TRACEb(DWTOB_L4(RCC->CFGR));
}
#endif

#if 0
	TRACEh(AFIO->MAPR);
	TRACEh(AFIO->MAPR2);

	TRACEh(GPIOA->CRH);
	TRACEh(GPIOA->CRL);
	TRACEh(GPIOA->ODR);

	TRACEh(GPIOB->CRH);
	TRACEh(GPIOB->CRL);
	TRACEh(GPIOB->ODR);

	TRACEh(GPIOC->CRH);
	TRACEh(GPIOC->CRL);
	TRACEh(GPIOC->ODR);

	TRACEh(GPIOD->CRH);
	TRACEh(GPIOD->CRL);
	TRACEh(GPIOD->ODR);

	TRACEh(GPIOE->CRH);
	TRACEh(GPIOE->CRL);
	TRACEh(GPIOE->ODR);

	TRACEh(AFIO->MAPR);
	TRACEh(AFIO->MAPR2);

	TRACEh(SPI1->CR1);
	TRACEh(SPI1->I2SCFGR);
	TRACEh(SPI1->CRCPR);

	TRACEh(SPI2->CR1);
	TRACEh(SPI2->I2SCFGR);
	TRACEh(SPI2->CRCPR);
#endif

#if 0
	TRACEh(USART1->CR1);
	TRACEh(USART1->CR2);
	TRACEh(USART1->CR3);
	TRACEh(USART1->BRR);

	TRACEh(USART2->CR1);
	TRACEh(USART2->CR2);
	TRACEh(USART2->CR3);
	TRACEh(USART2->BRR);
#endif
}

void device_check_soc_reset_flag(void)
{
#if 0
	if (SET == RCC_GetFlagStatus(RCC_FLAG_PINRST))
		TRACE("PIN RESET!\n");

	if (SET == RCC_GetFlagStatus(RCC_FLAG_PORRST))
		TRACE("POWER RESET!\n");

	if (SET == RCC_GetFlagStatus(RCC_FLAG_SFTRST))
		TRACE("SOFT RESET!\n");

	if (SET == RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
		TRACE("IWDG RESET!\n");

	if (SET == RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
		TRACE("WWDG RESET!\n");

	if (SET == RCC_GetFlagStatus(RCC_FLAG_LPWRRST))
		TRACE("LPWR RESET!\n");
#endif
}

void device_clear_soc_reset_flag(void)
{
#if 0
	/* Clear reset flags */
	RCC_ClearFlag();
#endif
}

void Device_Init(void)
{
#if 0
	gModuleDisable = 0;
	gModuleFail = 0;

	SystemInitA();
#if 0
	set_hse_clock(8000000);
	SetSysClockTo36();
#elif 1
	set_hse_clock(11059200);
	SetSysClockTo33_at_hse11mhz();
#else
	set_system_clock_to_36_by_hsi();
#endif

#if 0
	RCC_LSICmd(ENABLE);
	while (RESET == RCC_GetFlagStatus(RCC_FLAG_LSIRDY));						// 等待LSI稳定
#endif

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, (DWORD)__Vectors - 0x08000000);

	// 片内设备初始化
	//
	_device_init();
	VIC_Init();
	IO_Init();
	TIM_Init();
	uart_init();
	RTC_Init();
	WDT_Init();
	device_check_soc_reset_flag();
	device_clear_soc_reset_flag();
	device_trace_soc_register();

	// 片外设备硬件复位
	//
	Device_HardReset();

	// 片外设备初始化
	//
	Key_Init();
	Buzzer_Init();
	indicator_init();
	output_port_init();
	FlashMgr_Init();
	dir_file_init();
	LCD_Init(0);
	fm1702sl_init();
	dm9000_io_init();
	em4100_init();
	Audio_Init();

#if 0
	//ADC_Connect(ENCH_AD01);
	//PWM_Init(ENCH_PWM5);
	//PWM_voice_Init();
	SPI0_Init();
	SPI1_Init();
	WDT_Init();
	Audio_Init();

	pm_set_power_on();						// 开机上电锁定
	Device_HardReset();
	Device_SoftReset();

	LED_Init();
	Key_Init();
	Buzzer_Init();
	indicator_init();
	LCD_Init(0);
	RC500_Init();
	//W5100_Init(GetProductSN(), 0x00000000, 0xffffff00, 0xc0a80001);
#endif
#endif
}

void test_rcc_trim(WORD KeyCode)
{
#if 0
	DWORD rcc_cr = RCC->CR;
	if (('8' == KeyCode) && ((DWTOB_L4(rcc_cr) & ~0x03) > 0x00))
	{
		rcc_cr -= 0x08;
		RCC->CR = rcc_cr;
		TRACEb(DWTOB_L4(RCC->CR));
		//RCC_MCOConfig(0x04); Delay(300); RCC_MCOConfig(0x00);
	}
	if (('9' == KeyCode) && ((DWTOB_L4(rcc_cr) & ~0x03) < 0xf8))
	{
		rcc_cr += 0x08;
		RCC->CR = rcc_cr;
		TRACEb(DWTOB_L4(RCC->CR));
		//RCC_MCOConfig(0x04); Delay(300); RCC_MCOConfig(0x00);
	}
#endif
}
