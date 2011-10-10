#ifndef UART_H
#define UART_H


#include "config.h"

//===================================================================================
//
typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;   

void STM_MES_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);


//===================================================================================
//
#define EVF_UARTRXCHAR				(1 << 0)
#define EVF_UARTTXEMPTY			(1 << 1)
#define EVF_UARTBREAK				(1 << 2)
#define EVF_UARTFRAMEERR			(1 << 3)
#define EVF_UARTPARITYERR			(1 << 4)
#define EVF_UARTOVERRUNERR		(1 << 5)


typedef enum tagPARITY
{
	PARITY_NONE = 0,
	PARITY_ODD,
	PARITY_EVEN
} PARITY;

// 串口设置数据结构
typedef  struct  UartSetting
{
	int baud;
	char datab;         // 字长度，5/6/7/8
	char stopb;         // 停止位，1/2
	char parity;    	// 奇偶校验位，0为无校验，1奇数校验，2为偶数校验
}  UARTSETTING;

void uart_init(void);
void uart_set_baud_rate(COM_TypeDef usart_x, DWORD baud_rate);


//===================================================================================
//

#define Uart_GetFlagStatus(uartx, flag)	((uartx)->SR & (flag))

#define Uart0_WaitForSend()				while (RESET == Uart_GetFlagStatus(USART1, USART_FLAG_TC))
#define Uart0_WaitForReceive()			while (RESET == Uart_GetFlagStatus(USART1, USART_FLAG_RXNE))
#define Uart0_IsReceiveData()				(RESET == Uart_GetFlagStatus(USART1, USART_FLAG_RXNE))

void Uart0_Init(DWORD Baud, BYTE DataBits, BYTE StopBits, BYTE Parity);
void Uart0_SetBaud(DWORD baud);

void Uart0_PutByte(char c);
char Uart0_GetChar(void);
void Uart0_PutChar(char c);
void Uart0_Print(char *s);

DWORD Uart0_GetReceiveCount(void);
int Uart0_ReceiveBufFull(void);
void Uart0_ClearReceiveBuffer(void);
DWORD Uart0_Read(char *buffer, int size);
DWORD Uart0_Write(char *buffer, int size);
BOOL _Uart0_AddReceiveData(BYTE *data, WORD count);

void Uart0_SetEvent(DWORD event);
DWORD Uart0_GetEvent(void);
void Uart0_SetEventHandler(void (*UartProc)(void));
BOOL Uart0_Handler(EVENT *Event);


//===================================================================================
//
#define Uart1_WaitForSend()				while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC))
#define Uart1_WaitForReceive()			while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_RXNE))

void Uart1_Init(DWORD Baud, BYTE DataBits, BYTE StopBits, BYTE Parity);
void Uart1_SetBaud(DWORD baud);

void Uart1_PutByte(char c);
char Uart1_GetChar(void);
void Uart1_PutChar(char c);
void Uart1_Print(char *s);
DWORD Uart1_GetReceiveCount(void);
int Uart1_ReceiveBufFull(void);
void Uart1_ClearReceiveBuffer(void);
DWORD Uart1_Read(char *buffer, int size);
DWORD Uart1_Write(char *buffer, int size);
DWORD Uart1_GetEvent(void);
void Uart1_SetEventHandler(void (*UartProc)(void));
BOOL Uart1_Handler(EVENT *Event);


#endif
