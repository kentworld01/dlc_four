
#ifndef _d_misc_h
#define _d_misc_h



//#include "stm32f10x.h"
//#include "stm32f10x_rcc.h"
//#include "stm32f10x_usart.h"
//#include "lib_dbg.h"
#include <stdio.h>
#include <stdarg.h>

#include "os.h"


char Uart2_GetChar(void);
void Uart2_PutChar(char c);
void Uart2_Print(char *s);
void StrReverse(char *s);
void s_itoa (char *buf, long val, int radix, int len);
void s_printf (const char* str, ...);

//void WDT_Clear();
//void CPU_Exit_Critical();
//void CPU_Enter_Critical();

extern DWORD OSEnterCnt;
unsigned short swap_word( unsigned short w );
unsigned swap_dword( unsigned d );


#define _d_hash_max_count 30
#define _fks(x) hash_find_val_by_key( #x, count, key, val )
#define _fki(x) hash_find_val_to_int_by_key( #x, count, key, val )
#define _fkl(x) hash_find_val_to_long_by_key( #x, count, key, val )


char* hash_find_val_by_key( char* mk, int count, char** key, char** val );
int hash_find_val_to_int_by_key( char* mk, int count, char** key, char** val );
long hash_find_val_to_long_by_key( char* mk, int count, char** key, char** val );
int analysis_string_to_strings_by_decollator( char* line, char* decollator, char** strings, int max_count );
int analysis_http_get_hash( char* line, char** key, char** val, int max_count, int flag );





unsigned short swap_word( unsigned short w );
unsigned swap_dword( unsigned d );



u8* alloc_temp_buffer( int size );
int free_temp_buffer( u8* buf );

unsigned GetTickCount( void );
unsigned GetInterval( unsigned val );


unsigned utf8_to_unicode( char* utf8, unsigned short *unicode );
int _string_utf8_to_unicode( char* utf8, int ut8_size, unsigned short *unicode, int unicode_size );
int string_utf8_to_unicode( char* utf8, unsigned short *unicode, int );


//int _error_message( char* str, int val, int line_no, char* file_name );
//int _message( char* str, int val, int line_no, char* file_name );

int end_swith( char* fn, char* ext_name );
int uffs_read_line( int f, char *buf, int max_buf_size );


void* df_create( char*name, int flag, char* value );
void* df_open( char* fn, int flag );
int df_read( void* h, char* buf, int size );
int df_write( void* h, char* buf, int size );
int df_close( void* h );
int df_delete( char* name, int flag );
int df_seek( void* h, int offset, int pos );
int df_get_file_size( char* name );
int df_read_line( void* h, char* buf, int max_size );
int df_exist( char *name );

int set_buf_0xff_end( char* buf, int max_buf_size );

#endif

