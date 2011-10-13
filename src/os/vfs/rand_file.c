
#include "os/vfs/rand_file.h"

/*
 *
 * note:
 * 	record size < 128
 *
 *
 */

/*
 *
 * file *.ri   record info file
 * struct :
 * 	record size   		4B
 * 	record count		4B
 * 	record stat array	(record count)		
 * 		0xff is empty
 * 		other value is user flag and not empty record slot.
 *
 *
 */

/*
 *
 * rand file function 
 * 	create
 * 	remove
 * 	open
 * 	close
 * 	add
 * 	get
 * 	del
 * 	find
 * 	get_info
 *
 *
 *
 */

struct _s_rand_file_info g_rand_file_info[ _d_max_rand_file_info ] = {0};
#if 0
int find_empty_struct( void* item, int item_size, int total_size )
{
	int count;
	int i;
	int flag;
	char *is = (char*)item;
	count = total_size / item_size;
	for( i=0; i<count; i++ ){
		memcpy( &flag, is + (i*item_size), 1 );
		if( flag == 0 )
			return i;
	}
	return -1;
}
#endif
int rand_file_info_find_empty()
{
	int i;
	for( i=0; i<_d_max_rand_file_record_size; i++ ){
		if( g_rand_file_info[i].flag == 0 )
			return i;
	}
	return -1;
}
int rand_file_info_clear_item( int p )
{
	memset( &g_rand_file_info[ p ], 0, sizeof( g_rand_file_info[0] ) );
	return 0;
}





static int rand_file_file_name( char* tfn, char* fn )
{
	strcpy( tfn, fn );
	strcat( tfn, ".ri" );
	return 0;
}
unsigned char rand_file_get_record_flag( int p, int pos )
{
	unsigned char flag = 0;
	struct _s_rand_file_info *rfi = &g_rand_file_info[ p ];
	uffs_seek( rfi->ri_f, pos + 8, USEEK_SET );
	uffs_read( rfi->ri_f, &flag, 1 );
	return flag;
}
int rand_file_set_record_flag( int p, int pos, unsigned char flag )
{
	struct _s_rand_file_info *rfi = &g_rand_file_info[ p ];
	uffs_seek( rfi->ri_f, pos + 8, USEEK_SET );
	uffs_write( rfi->ri_f, &flag, 1 );
	return 0;
}
int rand_file_create( char* fn, int record_count, int record_size )
{
	char tfn[64];
	int i,f;
	unsigned char flag;
	char tbuf[ _d_max_rand_file_record_size ];
	// check file if exist.
	if( record_size > 128 )
		return -1;
	rand_file_file_name( tfn, fn );
	f = uffs_open( tfn, UO_RDONLY );
	if( f >= 0 )
		return -1;
	uffs_close( f );
	// create and write the base info
	f = uffs_open( tfn, UO_RDWR | UO_CREATE );
	if( f < 0 )
		return -1;
	uffs_write( f, &record_size, 4 );
	uffs_write( f, &record_count, 4 );
	memset( tbuf, 0xff, sizeof( tbuf ) );
	flag = 0xff;
	for( i=0; i<record_count; i++ ){
		uffs_write( f, &flag, 1 );
	}
	uffs_close(f);
	uffs_remove( fn );
	f = uffs_open( fn, UO_RDWR | UO_CREATE );
	if( f < 0 ){
		uffs_remove( tfn );
		return -1;
	}
	for( i=0; i<record_count; i++ ){
		uffs_write( f, tbuf, record_size );
	}
	uffs_close( f );
	// for test
	f = uffs_open( "/test", UO_RDWR | UO_CREATE );
	uffs_write( f, tbuf, record_size );
	uffs_close( f );
	return 0;
}
int rand_file_remove( char* fn ) 
{
	char tfn[64];
	rand_file_file_name( tfn, fn );
	uffs_remove( tfn );
	uffs_remove( fn );
	return 0;
}

int rand_file_open( char* fn )
{
	int p;
	int f, ri_f;
	int record_size, record_count;
	char tfn[64];
	rand_file_file_name( tfn, fn );
	p = rand_file_info_find_empty();
	if( p < 0 )
		return -1;
	g_rand_file_info[p].flag = 1;
	f = uffs_open( fn, UO_RDWR );
	if( f < 0 )
		goto rand_file_open_failed;
	ri_f = uffs_open( tfn, UO_RDWR );
	if( ri_f < 0 ){
		uffs_close( f );
		goto rand_file_open_failed;
	}
	uffs_read( ri_f, &record_size, 4 );
	uffs_read( ri_f, &record_count, 4 );
	g_rand_file_info[p].ri_f = ri_f;
	g_rand_file_info[p].f = f;
	g_rand_file_info[p].record_size = record_size;
	g_rand_file_info[p].record_count = record_count;
	return p;
rand_file_open_failed:	
	g_rand_file_info[p].flag = 0;
	return -1;
}

int rand_file_close( int p )
{
	uffs_close( g_rand_file_info[p].ri_f );
	uffs_close( g_rand_file_info[p].f );
	rand_file_info_clear_item( p );
	return 0;
}


int rand_file_find_empty_record_pos( int p )
{
	int i;
	unsigned char flag;
	struct _s_rand_file_info *rfi = &g_rand_file_info[ p ];
	uffs_seek( rfi->ri_f, 8, USEEK_SET );
	for( i=0; i<rfi->record_count; i++ ){
		uffs_read( rfi->ri_f, &flag, 1 );
		if( flag == 0xff )
			return i;
	}
	return -1;
}
int rand_file_add( int p, char *buf, int buf_size )
{
	char tbuf[ _d_max_rand_file_record_size ];
	int size;
	int pos;
	struct _s_rand_file_info *rfi = &g_rand_file_info[ p ];
	memset( tbuf, 0xff, sizeof( tbuf ) );
	size = min( _d_max_rand_file_record_size, buf_size );
	memcpy( tbuf, buf, size );
	pos = rand_file_find_empty_record_pos( p );
	if( pos < 0 )
		return -1;
	uffs_seek( rfi->f, pos * rfi->record_size, USEEK_SET );
	uffs_write( rfi->f, tbuf, size );
	rand_file_set_record_flag( p, pos, 0x7f);
	return pos;
}
int rand_file_del( int p, int pos )
{
	return rand_file_set_record_flag( p, pos, 0xff);
}
int rand_file_get( int p, int pos, char* buf, int max_buf_size )
{
	unsigned char flag;
	int size;
	struct _s_rand_file_info *rfi = &g_rand_file_info[ p ];
	size = min( _d_max_rand_file_record_size, max_buf_size );
	flag = rand_file_get_record_flag( p, pos);
	if( flag == 0xff )
		return -1;
	uffs_seek( rfi->f, pos * rfi->record_size, USEEK_SET );
	uffs_read( rfi->f, buf, size );
	return size;
}
int rand_file_find( int p, char* buf, int start, int len, int type )
{
	// here only do the string find 
	char tbuf[ _d_max_rand_file_record_size ];
	struct _s_rand_file_info *rfi = &g_rand_file_info[ p ];
	int size;
	int i;
	unsigned char flag;
	if( len > _d_max_rand_file_record_size )
	       return -1;	
	for( i=0; i<rfi->record_count; i++ ){
		flag = rand_file_get_record_flag( p, i );
		if( flag == 0xff )
			continue;
		uffs_seek( rfi->f, i * rfi->record_size + start, USEEK_SET );
		uffs_read( rfi->f, tbuf, len );
		switch( type ){
			case 1: /*memcmpy*/
				if( memcmp( tbuf, buf, len ) == 0 )
					return i;
				break;
		}
	}
	return -1;
}


/*
 *
 *
 * rand file index fucntion
 * 	create
 * 	open
 * 	resort
 * 	find
 *
 * 	add
 * 	get
 * 	del
 *
 *
 */


