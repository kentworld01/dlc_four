//-----------------------------------------------------------------------------------
#include "Device.h"
#include "OS.h"
#include <stdarg.h>
#include "datatran.h"


#if (DEBUG_VERSION == 1)

//===================================================================================
//
DWORD trace_address[4];
BYTE trace_size[4];

void trace_init(void)
{
	BYTE i;

	for(i = 0; i < 4; i++)
	{
		trace_address[i] = 0;
		trace_size[i] = 0;
	}
}

void trace_setup(BYTE index, BYTE size, DWORD address)
{
	if (size)
	{
		trace_address[index] = address;
		trace_size[index] = size;
	}
}

void trace_output(BYTE index, char *function, DWORD line)
{
	if (trace_address[index] && trace_size[index])
	{
		TRACE("%s(), L: %u, [0x%08x]: ", function, line, trace_address[index]);
		TRACEx(trace_address[index], trace_size[index]);
	}
}

//
#if (0)
void trace(char *title, int n)
{
	extern void Uart1_Print(char *s);
	char ss[20];
	Uart1_Print(title);
	IntToString(ss, n);
	Uart1_Print(ss);
	Uart1_Print("\n");
}
#endif

void s_printf (const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;
	char tmp[20];

	va_start(arp, str);

	while ((d = *str++) != 0) {

		if (d != '%') {
			DBG_PutChar(d);
			continue;
		}

		d = *str++; w = r = s = l = 0;

		if (d == '0') {
			d = *str++;
			s = 1;
		}

		while (IsDigital(d)) {
			w = w * 10 + (d - '0');
			d = *str++;
		}

		if (s)
			w = -w;

		if (d == 'l') {
			l = 1;
			d = *str++;
		}

		if (!d)
			break;

		if (d == 's') {
			DBG_Print(va_arg(arp, char *));
			continue;
		}

		if (d == 'c') {
			DBG_PutChar((char)va_arg(arp, int));
			continue;
		}

		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'x') r = 16;
		if (d == 'b') r = 2;

		if (!r)
			break;

		if (l) {
			s_itoa(tmp, (long)va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				s_itoa(tmp, (unsigned long)va_arg(arp, int), r, w);
			else
				s_itoa(tmp, (long)va_arg(arp, int), r, w);
		}
		DBG_Print(tmp);
//		WDT_CLEAR();
	}

	va_end(arp);

	SingleStep();
}

//===================================================================================
void Pause(void)
{
	char c;
	//extern DWORD OSEnterCnt;

	//OSEnterCnt = 1;
	CPU_Exit_Critical();

	while (DBG_GetReceiveCount())
		DBG_Read(&c, 1);

	DBG_Print("PAUSE ...\n");

	while (!DBG_GetReceiveCount())
		WDT_Clear();

	while (DBG_GetReceiveCount())
		DBG_Read(&c, 1);
}

//===================================================================================
void Input(char *Info, char *buf)
{
	char c;

	DBG_Print(Info);

	while (1)
	{
		while (!DBG_GetReceiveCount())
			WDT_Clear();

		DBG_Read(&c, 1);
		if (c == 0xd)
		{
			DBG_Print("\n");
			*buf++ = '\0';
			break;
		}
		else
		{
			*buf++ = c;
			DBG_PutChar(c);
		}
	}
}

void input(char *info, char *buf, int max_chars)
{
	char c;
	int input_chars = 0;
	bool e = FALSE;

	if (max_chars)
		max_chars -= 1;

	if (0 >= max_chars)
		return;

	if (0 == buf)
		return;

	if (info)
		DBG_Print(info);

	while (1)
	{
		while (!DBG_GetReceiveCount())
			WDT_Clear();
		DBG_Read(&c, 1);

		if (0xd == c)
		{
			DBG_Print("\n");
			*buf++ = '\0';
			break;
		}
		else if (0x8 == c)								// BackSpace
		{
			if (input_chars > 0)
			{
				DBG_PutChar(c);
				DBG_PutChar(' ');
				DBG_PutChar(c);
				buf--;
				input_chars--;
			}
			else
				e = TRUE;	
		}
		else if (input_chars < max_chars)
		{
			DBG_PutChar(c);
			*buf++ = c;
			input_chars++;
		}
		else
			e = TRUE;

		if (e)
		{
			e = 0;
			DBG_PutChar(0x7);							// BELL
		}
	}
}

int input_int(char *info)
{
	char buf[32];

	input(info, buf, 32);

	return s_atou(buf);
}

int input_hex(char *info)
{
	DWORD i, val;
	char buf[256], *p;

	input(info, buf, 256);

	for(val = 0, p = buf; *p; p++)
	{
		i = HexToNibble(*p);
		val <<= 4;
		val |= i;
	}

	return val;
}

int InputInt(char *Info, char *buf)
{
	Input(Info, buf);

	return s_atou(buf);
}

DWORD InputHex(char *Info, char *buf)
{
	DWORD i, val;

	Input(Info, buf);

	for(val=0; *buf; buf++)
	{
		i = HexToNibble(*buf);
		val <<= 4;
		val |= i;
	}

	return val;
}

int InputHexString(char *Info, char *buf)
{
	Input(Info, buf);

	return HexToMem((BYTE *)buf, (char *)buf);
}

//===================================================================================
void Dump_Binary(BYTE *Buffer, int Count, int Width)
{
	int i;

	for(i=0; (i<Count) && Width; i++)
	{
		TRACE("%02x ", Buffer[i]);
		if ((((i + 1) % Width) == 0) || ((i + 1) == Count))
			TRACE("\n");
	}
}

void dump_decimal(BYTE *buffer, int count, int width)
{
	int i;

	for(i = 0; (i < count) && width; i++)
	{
		TRACE("%3u ", buffer[i]);
		if ((((i + 1) % width) == 0) || ((i + 1) == count))
			TRACE("\n");
	}
}

void Dump_Text(BYTE *Buffer, int Count, int Width)
{
	int i;

	for(i=0; (i<Count) && Width; i++) {
		TRACE("%c", (char)Buffer[i]);
		if ((((i + 1) % Width) == 0) || ((i + 1) == Count))
			TRACE("\n");
	}
}










int Uart1_GetReceiveCount(void)
{
	int cnt = 0;

#if 0
	CPU_Enter_Critical();
	cnt = Queue_GetCount((void *)U1RxQueue);
	CPU_Exit_Critical();
#endif

	return cnt;
}
char Uart1_GetChar(void)
{
#if 0	
	Uart1_WaitForReceive();

	return (char)(BYTE)USART_ReceiveData(USART2);
#endif	
}

int Uart1_PutChar(char c)
{
	if ('\n' == c)
		Uart1_PutByte('\r');

	Uart1_PutByte(c);
}

int Uart1_Print(char *s)
{
	for(; *s; s++)
		Uart1_PutChar(*s);
}
int Uart1_PutByte(char c)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	//USART_SendData(USART2, (uint8_t) c);

	//Uart1_WaitForSend();
	sendchar( c );
}

int Uart1_Read(char *buffer, int size)
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


#endif
