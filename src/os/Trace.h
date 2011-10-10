#ifndef TRACE_H
#define TRACE_H


//===================================================================================
#if (DEBUG_PORT == 0)
	#define DBG_GetChar				Uart0_GetChar
	#define DBG_PutChar				Uart0_PutChar
	#define DBG_Print					Uart0_Print
	#define DBG_GetReceiveCount		Uart0_GetReceiveCount
	#define DBG_ClearReceiveBuffer	Uart0_ClearReceiveBuffer
	#define DBG_ClearAnotherPort		Uart1_ClearReceiveBuffer
	//#define DBG_EnableAnotherPort()	U1IER = RBRIE
	//#define DBG_DisableAnotherPort()	U1IER = 0
	#define DBG_Read					Uart0_Read
	#define DBG_Write					Uart0_Write
#elif (DEBUG_PORT == 1)
	#define DBG_GetChar				Uart1_GetChar
	#define DBG_PutChar				Uart1_PutChar
	#define DBG_Print					Uart1_Print
	#define DBG_GetReceiveCount		Uart1_GetReceiveCount
	#define DBG_ClearReceiveBuffer	Uart1_ClearReceiveBuffer
	#define DBG_ClearAnotherPort		Uart0_ClearReceiveBuffer
	//#define DBG_EnableAnotherPort()	U0IER = RBRIE
	//#define DBG_DisableAnotherPort()	U0IER = 0
	#define DBG_Read					Uart1_Read
	#define DBG_Write					Uart1_Write
#else
#endif

//===================================================================================
#if (DEBUG_SINGLESTEP == 1)
	#define SingleStep()				Pause()
#else
	#define SingleStep()				while(0)
#endif

//===================================================================================
#if (DEBUG_VERSION == 1)

void trace_init(void);
void trace_setup(BYTE index, BYTE size, DWORD address);
void trace_output(BYTE index, char *function, DWORD line);

void Pause(void);
void Input(char *Info, char *buf);
int InputInt(char *Info, char *buf);
DWORD InputHex(char *Info, char *buf);
int InputHexString(char *Info, char *buf);

int input_int(char *info);
int input_hex(char *info);

void s_printf (const char* str, ...);
void Dump_Binary(BYTE *Buffer, int Count, int Width);
void dump_decimal(BYTE *buffer, int count, int width);
void Dump_Text(BYTE *Buffer, int Count, int Width);

#define TRACE								s_printf
#define TRACE_BINARY						Dump_Binary
#define TRACE_DECIMAL						dump_decimal
#define TRACE_TEXT							Dump_Text
#define TRACE_OUTPUT						trace_output

#else

#define Input(a, b)					
#define TRACE(...)							
#define TRACE_BINARY(...)					
#define TRACE_DECIMAL(...)					
#define TRACE_TEXT(...)					
#define trace_init()					
#define trace_setup(a, b, c)					
#define TRACE_OUTPUT(...)						

#endif

#define TRACEt(a, b)						do { TRACE("%d\t%s\t%s",__LINE__,__func__,"["#a"]:\n"); TRACE_TEXT((BYTE *)(a), b, 48); } while (0)
#define TRACEx(a, b)						do { TRACE("["#a"]:\n"); TRACE_BINARY((BYTE *)(a), b, 16); } while (0)
#define TRACEdec(a, b)						do { TRACE("["#a"]:\n"); TRACE_DECIMAL((BYTE *)(a), b, 16); } while (0)
#define TRACEb(bits)						do \
											{ \
												BYTE i; \
												TRACE("%u\t%s\t"#bits"\t= ", __LINE__, __func__); \
												for(i = 0x80; i; i >>= 1) \
													TRACE("%u", ((bits) & i) ? 1 : 0); \
												TRACE("B\n"); \
											} \
											while (0)
#define TRACEh(hex)						TRACE("%d\t%s\t"#hex"\t= %08x\n", __LINE__, __func__, hex)
#define TRACEd(dec)						TRACE("%d\t%s\t"#dec"\t= %u\n", __LINE__, __func__, dec)
#define TRACEs(string)						TRACE("%d\t%s\t%s\n", __LINE__, __func__, ((char *)string))
#define TRACEo(index)						TRACE_OUTPUT(index, (char *)__func__, __LINE__)

#define _d_line() TRACE("%d\t%s\n", __LINE__, __func__ )
#define _d_hex(x) TRACE("%d\t%s\t"#x"=%08x\n", __LINE__, __func__, x )
#define _d_int(x) TRACE("%d\t%s\t"#x"=%d\n", __LINE__, __func__, x )
#define _d_u(x) TRACE("%d\t%s\t"#x"=%u\n", __LINE__, __func__, x )
#define _d_str(x) TRACE("%d\t%s\t"#x"=%s\n", __LINE__, __func__, x )
#define _d_buf(address, size) TRACEx(address, size)
#define _d_text(address, size) TRACEt(address, size)

#define debug_show_ip_str(x) _debug_show_ip_str( x, #x, __LINE__, __func__ )
#define debug_show_hw_str(x) _debug_show_hw_str(x, #x, __LINE__, __func__ )


#define error_message(x,y) _error_message(x,y, __LINE__, __func__ )
#define message(x,y) _message(x,y, __LINE__, __func__ )


#define assert_param(x)


#endif
