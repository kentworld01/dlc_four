//===================================================================================
//
#include "Device.h"
#include "OS.h"


//===================================================================================
//
/** @addtogroup STM3210B_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                             2

static USART_TypeDef* COM_USART[COMn] = {USART1, USART2}; 
static GPIO_TypeDef* COM_TX_PORT[COMn] = {GPIOA, GPIOD};
static GPIO_TypeDef* COM_RX_PORT[COMn] = {GPIOA, GPIOD};
static const uint32_t COM_USART_CLK[COMn] = {RCC_APB2Periph_USART1, RCC_APB1Periph_USART2};
static const uint32_t COM_TX_PORT_CLK[COMn] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOD};
static const uint32_t COM_RX_PORT_CLK[COMn] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOD};
static const uint16_t COM_TX_PIN[COMn] = {GPIO_Pin_9, GPIO_Pin_5};
static const uint16_t COM_RX_PIN[COMn] = {GPIO_Pin_10, GPIO_Pin_6};
static const uint8_t COM_IRQn[COMn] = {USART1_IRQn, USART2_IRQn};

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  *     @arg COM2  
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void STM_MES_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

	if (COM == COM1)
	{
		RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
	}
	else
	{
		/* Enable the USART2 Pins Software Remapping */
		GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
		RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
	}

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
	GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

	/* USART configuration */
	USART_Init(COM_USART[COM], USART_InitStruct);

	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = COM_IRQn[COM];
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USARTy Receive and Transmit interrupts */
	USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);
	//USART_ITConfig(COM_USART[COM], USART_IT_TXE, ENABLE);

	/* Enable USART */
	USART_Cmd(COM_USART[COM], ENABLE);
}

void uart_setup(COM_TypeDef USARTx, DWORD Baud, BYTE DataBits, BYTE StopBits, BYTE Parity)
{
	USART_InitTypeDef USART_InitStructure;

	/* USARTx configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/

	switch (DataBits)
	{
	case 9:		USART_InitStructure.USART_WordLength = USART_WordLength_9b; break;
	case 8:
	default: 	USART_InitStructure.USART_WordLength = USART_WordLength_8b; break;
	}

	switch (StopBits)
	{
	case 2:		USART_InitStructure.USART_StopBits = USART_StopBits_2; break;
	case 5:		USART_InitStructure.USART_StopBits = USART_StopBits_0_5; break;
	case 15:	USART_InitStructure.USART_StopBits = USART_StopBits_1_5; break;
	case 1:
	default:	USART_InitStructure.USART_StopBits = USART_StopBits_1; break;
	}

	switch (DataBits)
	{
	case PARITY_EVEN:	USART_InitStructure.USART_Parity = USART_Parity_Even; break;
	case PARITY_ODD:	USART_InitStructure.USART_Parity = USART_Parity_Odd; break;
	case PARITY_NONE:
	default: 			USART_InitStructure.USART_Parity = USART_Parity_No; break;
	}

	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	STM_MES_COMInit(USARTx, &USART_InitStructure);
}

void uart_init(void)
{
	Uart1_Init(DEVICE_UART1_BAUD, 8, 1, PARITY_NONE);
	Uart0_Init(DEVICE_UART0_BAUD, 8, 1, PARITY_NONE);

	TRACE("\n\n"); DBG_PutChar(0x0f);		// Shift in
	TRACE("UART1: OK\n");
	TRACE("UART0: OK\n");
}

void uart_set_baud_rate(COM_TypeDef usart_x, DWORD baud_rate)
{
	const DWORD table_baud_rate[] = 
	{
		115200L,	57600,		38400,		19200,
		14400,		9600,		4800,		2400,
		1200,		600,		300,		110
	};
	DWORD new_baud_rate = 115200L, i;

	for(i = 0; i < sizeof(table_baud_rate) / sizeof(DWORD); i++)
	{
		if (baud_rate == table_baud_rate[i])
		{
			new_baud_rate = baud_rate;
			break;
		}
	}

	uart_setup(usart_x, new_baud_rate, 8, 1, PARITY_NONE);
	//TRACEd(new_baud_rate);
}

//===================================================================================
//
char U0RxQueue[MAXUARTRXBUF];
char U1RxQueue[MAXUARTRXBUF];

void (*Uart0Proc)(void) = 0;
void (*Uart1Proc)(void) = 0;

DWORD Uart1EVF;
DWORD Uart0EVF;

//-----------------------------------------------------------------------------------
void Uart0_Init(DWORD Baud, BYTE DataBits, BYTE StopBits, BYTE Parity)
{
	uart_setup(COM1, Baud, DataBits, StopBits, Parity);

	Uart0Proc = 0;
	Queue_Create((void *)U0RxQueue, MAXUARTRXBUF, sizeof(char), NULL, Uart0_ReceiveBufFull);
}

void Uart0_SetBaud(DWORD baud)
{
	uart_setup(COM1, baud, 8, 1, PARITY_NONE);
}

void Uart0_PutByte(char c)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) c);
#ifdef _d_basic_modules
	debug_log_put_byte( c );
#endif
	Uart0_WaitForSend();
}

//-----------------------------------------------------------------------------------
char Uart0_GetChar(void)
{
	Uart0_WaitForReceive();

	return (char)(BYTE)USART_ReceiveData(USART1);
}

//-----------------------------------------------------------------------------------
void Uart0_PutChar(char c)
{
	if ('\n' == c)
		Uart0_PutByte('\r');

	Uart0_PutByte(c);
}

//-----------------------------------------------------------------------------------
void Uart0_Print(char *s)
{
	for(; *s; s++)
		Uart0_PutChar(*s);
}

//-----------------------------------------------------------------------------------
DWORD Uart0_GetReceiveCount(void)
{
	int cnt = 0;

#if 0
	if (Uart0_IsReceiveData())
		return 1;
#endif

	CPU_Enter_Critical();
	cnt = Queue_GetCount((void *)U0RxQueue);
	CPU_Exit_Critical();

	return cnt;
}

//-----------------------------------------------------------------------------------
int Uart0_ReceiveBufFull(void)
{
	TRACEd(1);

	CPU_Enter_Critical();
	Queue_Empty((void *)U0RxQueue);
	CPU_Exit_Critical();

	return QUEUE_OK;
}

//-----------------------------------------------------------------------------------
void Uart0_ClearReceiveBuffer(void)
{
	CPU_Enter_Critical();
	Queue_Empty((void *)U0RxQueue);
	CPU_Exit_Critical();
}

//-----------------------------------------------------------------------------------
DWORD Uart0_Read(char *buffer, int size)
{
	int e;
	int realSize = 0;

#if 0
	if (size && Uart0_IsReceiveData())
	{
		*buffer = Uart0_GetChar();

		return 1;
	}
	else
		return 0;
#endif

	for(realSize = 0; realSize < size; )
	{
		CPU_Enter_Critical();
		e = Queue_Read((void *)U0RxQueue, (void *)buffer);
		CPU_Exit_Critical();

		if (e == QUEUE_OK)
		{
			realSize++;
			buffer++;
		}
		else if (e == QUEUE_EMPTY)
			break;
		else if (e == QUEUE_FAIL)
			return (DWORD)-1;
	}

	return realSize;
}

DWORD Uart0_Write(char *buffer, int size)
{
	while (size--)
		Uart0_PutByte(*buffer++);

	return 0;
}

BOOL _Uart0_AddReceiveData(BYTE *data, WORD count)
{
	int e;

	for(e = QUEUE_OK; (QUEUE_OK == e) && count; count--)
		e = Queue_Write((void *)U0RxQueue, (void *)data++);

	return (QUEUE_OK == e) ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------------
void Uart0_SetEvent(DWORD event)
{
	Uart0EVF |= EVF_UARTRXCHAR;							// 置通讯事件标志
}

DWORD Uart0_GetEvent(void)
{
	DWORD evf;

	CPU_Enter_Critical();

	evf = Uart0EVF;
	Uart0EVF = 0;

	CPU_Exit_Critical();

	return evf;
}

void Uart0_SetEventHandler(void (*UartProc)(void))
{
	Uart0Proc = UartProc;
}

BOOL Uart0_Handler(EVENT *Event)
{
	if (Event->Type == EV_U0RXCHAR)
	{
		if (Uart0Proc)
		{
			Uart0Proc();
		}
		else
		{
			Uart0_ClearReceiveBuffer();
			TRACE("UART0 had not handle!\n");
		}

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------------
void Uart1_Init(DWORD Baud, BYTE DataBits, BYTE StopBits, BYTE Parity)
{
	uart_setup(COM2, Baud, DataBits, StopBits, Parity);

	Uart1Proc = 0;
	Queue_Create((void *)U1RxQueue, MAXUARTRXBUF, sizeof(char), NULL, Uart1_ReceiveBufFull);
}

void Uart1_SetBaud(DWORD baud)
{
	uart_setup(COM2, baud, 8, 1, PARITY_NONE);
}

void Uart1_PutByte(char c)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART2, (uint8_t) c);

	Uart1_WaitForSend();
}

//-----------------------------------------------------------------------------------
char Uart1_GetChar(void)
{
	Uart1_WaitForReceive();

	return (char)(BYTE)USART_ReceiveData(USART2);
}

void Uart1_PutChar(char c)
{
	if ('\n' == c)
		Uart1_PutByte('\r');

	Uart1_PutByte(c);
}

void Uart1_Print(char *s)
{
	for(; *s; s++)
		Uart1_PutChar(*s);
}

//-----------------------------------------------------------------------------------
DWORD Uart1_GetReceiveCount(void)
{
	int cnt = 0;

#if 0
	CPU_Enter_Critical();
	cnt = Queue_GetCount((void *)U1RxQueue);
	CPU_Exit_Critical();
#endif

	return cnt;
}

//-----------------------------------------------------------------------------------
int Uart1_ReceiveBufFull(void)
{
#if 0
	CPU_Enter_Critical();
	Queue_Empty((void *)U1RxQueue);
	CPU_Exit_Critical();
#endif

	return QUEUE_OK;
}

//-----------------------------------------------------------------------------------
void Uart1_ClearReceiveBuffer(void)
{
#if 0
	CPU_Enter_Critical();
	Queue_Empty((void *)U1RxQueue);
	CPU_Exit_Critical();
#endif
}

//-----------------------------------------------------------------------------------
DWORD Uart1_Read(char *buffer, int size)
{
	int e;
	int realSize;

#if 0
	for(realSize=0; realSize<size; )
	{
		CPU_Enter_Critical();
		e = Queue_Read((void *)U1RxQueue, (void *)buffer);
		CPU_Exit_Critical();

		if (e == QUEUE_OK)
		{
			realSize++;
			buffer++;
		}
		else if (e == QUEUE_EMPTY)
			break;
		else if (e == QUEUE_FAIL)
			return (DWORD)-1;
	}
#endif

	return realSize;
}

//-----------------------------------------------------------------------------------
DWORD Uart1_Write(char *buffer, int size)
{
	while (size--)
		Uart1_PutByte(*buffer++);

	return 0;
}

//-----------------------------------------------------------------------------------
DWORD Uart1_GetEvent(void)
{
	DWORD evf;

	CPU_Enter_Critical();

	evf = Uart1EVF;
	Uart1EVF = 0;

	CPU_Exit_Critical();

	return evf;
}

//-----------------------------------------------------------------------------------
void Uart1_SetEventHandler(void (*UartProc)(void))
{
	Uart1Proc = UartProc;
}

//-----------------------------------------------------------------------------------
BOOL Uart1_Handler(EVENT *Event)
{
	if (Event->Type == EV_U1RXCHAR)
	{
		if (Uart1Proc)
			Uart1Proc();

		return TRUE;
	}

	return FALSE;
}
