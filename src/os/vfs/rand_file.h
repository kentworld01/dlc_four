
#ifndef _d_rand_file_h
#define _d_rand_file_h

#include "os.h"

#define _d_max_rand_file_info 10
#define _d_max_rand_file_record_size 128
struct _s_rand_file_info{
	char flag;
	int ri_f;
	int f;
	int record_count;
	int record_size;
};


int rand_file_info_find_empty();
int rand_file_info_clear_item( int p );
unsigned char rand_file_get_record_flag( int p, int pos );
int rand_file_set_record_flag( int p, int pos, unsigned char flag );
int rand_file_create( char* fn, int record_count, int record_size );
int rand_file_remove( char* fn ) ;
int rand_file_open( char* fn );
int rand_file_close( int p );
int rand_file_find_empty_record_pos( int p );
int rand_file_add( int p, char *buf, int buf_size );
int rand_file_del( int p, int pos );
int rand_file_get( int p, int pos, char* buf, int max_buf_size );
int rand_file_find( int p, char* buf, int start, int len, int type );

#endif

