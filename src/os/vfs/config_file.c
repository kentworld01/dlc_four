#include "os.h"
#include "vfs/dir_file.h"
#include "txt_file_index.h"
#include "config_file.h"

#ifdef _d_dir_file_modules


typedef struct {
	void* txt_file;
	void* sort_pointer_file;
	int size;
	int tmp;
}_s_config_file_item;

#define _d_config_file_item_max_size 10
_s_config_file_item g_config_file_item[_d_config_file_item_max_size] = {0};


static void* find_free_config_file_item()
{
	int i;
	for( i=0; i<_d_config_file_item_max_size; i++ ){
		if( g_config_file_item[i].txt_file == 0 )
			return &g_config_file_item[i];
		}
	return 0;
}
void* config_open( char* txt_file_name, char* sort_pointer_file )
{
	_s_config_file_item *cfi;
	cfi = find_free_config_file_item();
	if( cfi == 0 )
		return 0;
	cfi->size = df_get_file_size( sort_pointer_file ) ;
	cfi->size /= 2;
	if( cfi->size == 0 )
		return 0;
	cfi->txt_file = df_open( txt_file_name, 0 );
	if( cfi->txt_file == 0 )
		return 0;
	cfi->sort_pointer_file= df_open( sort_pointer_file, 0 );
	if( cfi->sort_pointer_file == 0 ){
		df_close( cfi->txt_file );
		return 0;
		}
	return cfi;
}

int config_get_val( void *h, char* key, char *val, int max_size, int sub_item )
{
	// FIXME
	// here use for statement to search key, it can use binary search to speed up.
	_s_config_file_item *cfi;
	char str[100];
	int i;
	int tval;
	int len;
	char *ki[10];
	int kc;
	cfi = (_s_config_file_item*)h;
	//_d_int( cfi->size );
	//_d_str( key );
	len = strlen( key );
	if( len <= 0 )
		return -1;
	df_seek( cfi->sort_pointer_file, 0, 0 );
	for( i=0; i<cfi->size; i++ ){
		u16 offset;
		df_read( cfi->sort_pointer_file, (char*)&offset, 2 );
		//_d_int( offset );
		//_d_hex( offset );
		df_seek( cfi->txt_file, offset, 0 );
		df_read_line( cfi->txt_file, str, sizeof( str ) );
		//_d_str( str );
		tval = strncmp( key, str, len );
		//_d_int( tval );
		if( tval == 0 ){
			kc = analysis_string_to_strings_by_decollator( str, "=", ki, 5 );
			if( sub_item < 0 ){
				strcpy( val, ki[1] );
			}
			else{
				kc = analysis_string_to_strings_by_decollator( ki[1], ",", ki, 10 );
				strcpy( val, ki[sub_item] );
			}
			return 0;
			}
		}
	return -1;
}
int config_close( void *h )
{
	_s_config_file_item *cfi = (_s_config_file_item*)h;
	memset( cfi, 0, sizeof( _s_config_file_item ) );
	return 0;
}

#else


void* config_open( char* txt_file_name, char* sort_pointer_file )
{
	return 0;
}
int config_get_val( void *h, char* key, char *val, int max_size, int sub_item )
{
	return 0;
}
int config_close( void *h )
{
	return 0;
}

#endif

