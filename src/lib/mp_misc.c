
//#include "stm32f10x.h"

#include "os.h"
#include "config.h"
#include "trace.h"
#include "mp_misc.h"

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int _k_fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */

	DBG_PutChar( ch );
	return ch;


	//USART_SendData(EVAL_COM2, (uint8_t) ch);

	/* Loop until the end of transmission */
	//while(USART_GetFlagStatus(EVAL_COM2, USART_FLAG_TC) == RESET)
	//{
	//}

	//return ch;
}



#if 0
//-----------------------------------------------------------------------------------
char Uart2_GetChar(void)
{
	//while (!(U1LSR & ULSR_RDR));
	//return U1RBR;
	return 0;
}

void Uart2_PutChar(char c)
{
	if ('\n' == c)
		fputc('\r',0);

	fputc(c,0);
}

void Uart2_Print(char *s)
{
	for(; *s; s++)
		fputc(*s,0);
}

void StrReverse(char *s)
{
	char *pr, c;

	pr = s + strlen(s) - 1;
	while (*s) {
		c = *pr;
		*pr = *s;
		*s = c;
		s++;
		pr--;
		if (pr <= s)
			break;
	}
}
void s_itoa (char *buf, long val, int radix, int len)
{
	unsigned char i, c, r, sgn = 0, pad = ' ';
	unsigned v;

	buf[0] = 0;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}

	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}

	if (len > 20)
		return;

	i = 0;
	do {
		c = (unsigned char)(v % r);
		if (c < 10)
			c += '0';
		else
			c += 'a' - 10;
		buf[i++] = c;
		v /= r;
	} while (v);

	while (i < len)
		buf[i++] = pad;

	if (sgn)
		buf[i++] = sgn;

	buf[i] = 0;

	StrReverse(buf);
}

//-----------------------------------------------------------------------------------
void s_printf (const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;
	char tmp[20];

	va_start(arp, str);

	while ((d = *str++) != 0) {

		if (d != '%') {
			Uart2_PutChar(d);
			continue;
		}

		d = *str++; w = r = s = l = 0;

		if (d == '0') {
			d = *str++;
			s = 1;
		}

		while ( '0' <= d && d <= '9' ) {
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
			Uart2_Print(va_arg(arp, char *));
			continue;
		}

		if (d == 'c') {
			Uart2_PutChar((char)va_arg(arp, int));
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
		Uart2_Print(tmp);
		//		WDT_CLEAR();
	}

	va_end(arp);

	//SingleStep();
}
#endif


ULONG web_atol(UCHAR * buf)
{
#if 0
	ULONG value;
	value=0xff & buf[0];
	value=value<<8;
	value=value | buf[1];
	value=value<<8;
	value=value | buf[2];
	value=value<<8;
	value=value | buf[3];

	return value;
#else
	return (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3] ;
#endif
}


void web_ltoa(UCHAR * buf,ULONG value)		 
{

	buf[3]=0xff & value;
	buf[2]=0xff & value>>8;
	buf[1]=0xff & value>>16;
	buf[0]=0xff & value>>24;

}

void web_itoa(UCHAR * buf,UINT value)		 
{
	buf[1]=0xff & value;
	buf[0]=0xff & value>>8;

}

UINT web_atoi(UCHAR * buf)		 
{
#if 0
	UINT value;

	value=0xff & buf[0];
	value=value<<8;
	value=value | buf[1];
	return value;
#else
	return (buf[0]<<8) | buf[1];
#endif
}
UCHAR strcmp_ip(UCHAR *s1,UCHAR *s2)
{
	UCHAR i;
	for(i=0;i<4;i++)
	{
		if(s1[i]!=s2[i]){return FALSE;}
	}
	return TRUE;

}



//void WDT_Clear(){}
//void CPU_Exit_Critical(){}
//void CPU_Enter_Critical(){}



unsigned short swap_word( unsigned short w )
{
	u16 tw;
	u8 *po = (u8*)&w;
	u8 *po2 = (u8*)&tw;
	po2[0] = po[1];
	po2[1] = po[0];
	return tw;
}
unsigned swap_dword( unsigned d )
{
	u32 td;
	u8 *po = (u8*)&d;
	u8 *po2 = (u8*)&td;
	po2[3] = po[0];
	po2[2] = po[1];
	po2[1] = po[2];
	po2[0] = po[3];
	return td;
}
char* hash_find_val_by_key( char* mk, int count, char** key, char** val )
{
	int i;
	for( i=0; i<count; i++ ){
		if( strcmp( mk, key[i] ) == 0 )
			return val[i];
	}
	return "";
}
int hash_find_val_to_int_by_key( char* mk, int count, char** key, char** val )
{
	int i;
	for( i=0; i<count; i++ ){
		if( strcmp( mk, key[i] ) == 0 )
			return atoi( val[i] );
	}
	return 0;
}
long hash_find_val_to_long_by_key( char* mk, int count, char** key, char** val )
{
	int i;
	for( i=0; i<count; i++ ){
		if( strcmp( mk, key[i] ) == 0 )
			return atol( val[i] );
	}
	return 0;
}
int analysis_string_to_strings_by_decollator( char* line, char* decollator, char** strings, int max_count )
{
	char c;
	char *po;
	int i;
	int len;
	int count = 0;
	char* old_line;

	if( max_count <= 0 )
		return 0;
	len = strlen( decollator );

	old_line = line;
	count ++;
	while( c=*line, c!=0 ){
		po = decollator;
		for( i=0; i<len; i++ ){
			if( *po == c ){
				*line = 0;
				count ++;
				if( count >= max_count-1 )
					goto decollator_end;
				break;
			}
			po ++;
		}
		line ++;
	}
decollator_end:
	po = old_line;
	for( i=0; i<count; i++ ){
		strings[i] = po;
		po += strlen( po ) + 1;
	}
	return count;
}
#if 0
int test_analysis_string_to_strings_by_decollator()
{
	int analysis_string_to_strings_by_decollator( char* line, char* decollator, char** strings, int max_count );
	char *str = "aaa&bb&c&&";
	char *buf;
	int count;
	char *strings[10];
	int i;

	buf = (char*)CmdBuf;
	my_strcpy( buf, str );
	count = analysis_string_to_strings_by_decollator( buf, "&b", strings, 10 );
	_d_int( count );
	for( i=0; i<count; i++ ){
		_d_str( strings[ i ] );
	}
	return 0;
}
#endif
int analysis_http_get_hash( char* line, char** key, char** val, int max_count, int post_flag )
{
	int kc;
	char *ki[ _d_hash_max_count ];
	char *po;
	int i;
	int count;

	po = line;
	if( post_flag == 0 ){
		while( *po != '?' && *po != 0 ) po ++;
		if( *po == 0 )
			return 0;
		po ++;
	}

	kc = analysis_string_to_strings_by_decollator( po, "&", ki, _d_hash_max_count );

	for( i=0; i<kc; i++ ){
		val[i] = key[i] = ki[i];
	}
	count = kc;
	// do the key and val 
	for( i=0; i<count; i++ ){
		po = key[i];
		while( *po != '=' && *po != 0 )	po++;
		if( *po == 0 ){
			val[i] = "";
		}
		else{
			*po = 0;
			val[i] = po+1;
		}
	}
	return count;
}
#if 0
int analysis_http_get_hash( char* line, char** key, char** val, int max_count )
{
	char *po;
	char *k, *v;
	int count = 0;
	char *null_str = "";
	po = line;
	while( *po != '?' && *po != 0 ) po ++;
	if( *po == 0 )
		return 0;
	po ++;
	while( *po ){
		if( count >= max_count-1 )
			break;
		k = po;
		v = null_str;
		key[ count ] = k;
		val[ count ] = v;
		while( *po != '=' && *po != '&' && *po != 0 )	po ++;
		if( *po == '&' ){
			*po = 0;
			po ++;
			count ++;
			continue;
		}
		if( *po == 0 ){
			count ++;
			break;
		}
		if( *po == '=' ){
			*po = 0;
			po++;
			v = po;
			val[ count ] = v;
			while( *po != '&' && *po != 0 )	po ++;
			if( *po == '&' ){
				*po = 0;
				po ++;
			}
		}
		count ++;
	}
	return count;
}
#endif
#if 0
int test_analysis_http_get_hash()
{
	char *get = "r?b&e=10&f=7=7&q";
	//char *get = "r?b=3&e=10";
	char *k[10], *v[10];
	int c;
	int i;
	char *buf;

	buf = (char*)CmdBuf;
	my_strcpy( buf, get );
	c = analysis_http_get_hash( buf, k, v, 10 );
	_d_int( c );
	for( i=0; i<c; i++ ){
		_d_str( k[i] );
		_d_str( v[i] );
	}
	return c;
}
#endif

u8 temp_buffer[1024];
int temp_buffer_flag = 0;
int temp_buffer_size()
{
	return sizeof( temp_buffer );
}
u8* alloc_temp_buffer( int size )
{
	_d_int( temp_buffer_flag );
	if( temp_buffer_flag == 0 ){
		temp_buffer_flag = 1;
		return temp_buffer;
	}
	return 0;
}
int free_temp_buffer( u8* buf )
{
	temp_buffer_flag = 0;
	return 0;
}


unsigned utf8_to_unicode( char* utf8, unsigned short *uc )
{
	// FIXME
	// here is foolish mode.
	unsigned short unicode;
	char header = utf8[0];
	int size = 0;
	if( (header>>=1) == 0x7e )	size = 6;
	else if( (header>>=1) == 0x3e )	size = 5;
	else if( (header>>=1) == 0x1e )	size = 4;
	else if( (header>>=1) == 0x0e )	size = 3;
	else if( (header>>=1) == 0x06 )	size = 2;
	else size = 1;
	//_d_int( size );
	switch( size ){
		case 1: unicode = utf8[0]; break;
		case 2:
			unicode = (utf8[0] & 0x1f) << 6;
			unicode |= (utf8[1] & 0x3f);
			break;
		case 3:
			unicode = (utf8[0] & 0xf) << 12;
			unicode |= (utf8[1] & 0x3f) << 6;
			unicode |= (utf8[2] & 0x3f);
			//unicode = swap_word( unicode );
			break;
		default:
			*uc = 0;
			return 0;
		}
	//_d_hex( unicode );
	*uc = unicode;
	return size;
#if 0	
	unicode = (utf8[0] & 0x1F) << 12;
	unicode |= (utf8[1] & 0x3F) << 6;
	unicode |= (utf8[2] & 0x3F);
#endif	
	return 0;
}

int _string_utf8_to_unicode( char* utf8, int utf8_size, unsigned short *unicode, int unicode_size )
{
	unsigned short tmp_unicode;
	int tmp_byte_size = 0;
	int tmp_unicode_size = 0;
	while( utf8_size > 0 ){
		tmp_byte_size = utf8_to_unicode( utf8, &tmp_unicode );
		if( tmp_unicode_size < unicode_size )
			unicode[ tmp_unicode_size++ ] = tmp_unicode;
		utf8 += tmp_byte_size;
		utf8_size -= tmp_byte_size;
		}
	unicode[ tmp_unicode_size ] = 0;
	return tmp_unicode_size;
}

int string_utf8_to_unicode( char* utf8, unsigned short *unicode, int size )
{
	int utf8_size = strlen( utf8 );
	return _string_utf8_to_unicode( utf8, utf8_size, unicode, size );
}
int disable_special_unicode( unsigned short *unicode, int size )
{
	int i;
	int flag = 0;
	for( i=0; i<size; i++ ){
		//_d_hex( unicode[i] );
		if( unicode[i] == 0xd ){
			//_d_line();
			unicode[i] = 0x20 ;
		}
		if( unicode[i] == (unsigned short)',' ){
			flag = 1;
		}
		if( unicode[i] == (unsigned short)'=' ){
			flag = 1;
		}
		if( flag == 1 ){
			unicode[i] = 0;
			return i;
		}
	}
	return size;
}


void utf8_to_unicode_test()
{
	// test single byte.
	char utf8[] = {0xE4, 0xBD, 0xA0,0xE4, 0xBD, 0xA0};
	u16 uc[5];
	int size;
	memset( uc, 0, sizeof( uc ) );
	size = string_utf8_to_unicode( utf8, uc, sizeof(uc)/2 );
	_d_int( size );
	_d_hex( uc[0] );
	_d_hex( uc[1] );
	_d_hex( uc[2] );
}

int _error_message( char* str, int val, int line_no, char* file_name )
{
	TRACE("\tError Message\n");
	TRACE("%d\t%s\t%s\t%d\n", line_no, file_name, str, val );
	//while( 1 );
	return 0;
}
int _message( char* str, int val, int line_no, char* file_name )
{
	TRACE("\tMessage\n");
	TRACE("%d\t%s\t%s\t%d\n", line_no, file_name, str, val );
	//while( 1 );
	return 0;
}

int end_swith( char* fn, char* ext_name )
{
	char buf[32+3];
	int ext_name_len = 0;
	int fn_len = 0;
	ext_name_len = strlen( ext_name );
	if( ext_name_len >= sizeof(buf)-3 )
		return 0;
	buf[0] = '.';
	buf[1] = 0;
	strcat( buf, ext_name );
	ext_name_len ++;
	fn_len = strlen( fn );
	if( fn_len < ext_name_len )
		return 0;
	return strcmp( &fn[ fn_len-ext_name_len ], ext_name );
}

int uffs_read_line( int f, char *buf, int max_buf_size )
{
	return df_read_line( (f+1), buf, max_buf_size );
}


