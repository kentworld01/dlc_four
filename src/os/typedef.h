#ifndef TYPEDEF_H
#define TYPEDEF_H

#if 1
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif
#endif

typedef unsigned char  uint8;                   /* 无符号8位整型变量                        */
typedef signed   char  int8;                    /* 有符号8位整型变量                        */
typedef unsigned short uint16;                  /* 无符号16位整型变量                       */
typedef signed   short int16;                   /* 有符号16位整型变量                       */
typedef unsigned int   uint32;                  /* 无符号32位整型变量                       */
typedef signed   int   int32;                   /* 有符号32位整型变量                       */
typedef float          fp32;                    /* 单精度浮点数（32位长度）                 */
typedef double         fp64;                    /* 双精度浮点数（64位长度）                 */


typedef unsigned int		DWORD;
typedef unsigned short		WORD;
typedef unsigned char		BYTE;
typedef unsigned char		BINARY;               // Data type used for microcontroller
//typedef unsigned char		BOOL;
typedef unsigned char		bool;
typedef unsigned short		WCHAR;
typedef unsigned short		wchar;
//typedef DWORD				BIT;
typedef DWORD				bit;

typedef unsigned int		dword;
typedef unsigned short		word;
typedef unsigned char		byte;

typedef unsigned int		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;



#define _d_swap_word(x)				( (x>>8) | ( x<<8 ) )
#define _d_swap_dword(x)				( ( (x&0xff)<<24 ) | ( (x&0xff00)<<8 ) | ( (x&0xff0000)>>8 ) | ( (x&0xff000000)>>24 ) )


/* These types must be 16-bit, 32-bit or larger integer */
typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef signed char		CHAR;
typedef unsigned char	UCHAR;

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WCHAR;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;


typedef BYTE				SOCKET;


typedef struct
{
	short year;
	char weekday;
	char month;
	char day;
	char hour;
	char minute;
	char second;
} DATETIME;


// for function pointer define
typedef void (*void_function_pointer)(void);

typedef void (*void_function)(void);

#endif
