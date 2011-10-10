
#include "dir_file.h"
#include <string.h>
#include "mp_misc.h"

#ifdef _d_dir_file_modules


u8 _g_show_system_file = 0;


typedef struct{
	_s_dir_file_operate *o;
	char* device_name;
}_s_dir_file_device_struct;
typedef struct{
	_s_dir_file_device_struct *device_struct;
	char master_path[10];
	u32 flag;
}_s_dir_file_register_item;

#define _d_dir_file_register_items_max_count 10
_s_dir_file_register_item g_dir_file_register_item[ _d_dir_file_register_items_max_count ] = {0};


int dir_file_init()
{
	//_d_line();
	memset( g_dir_file_register_item, 0, sizeof( g_dir_file_register_item ) );
	_d_line();
	node_ram_init();
	_d_line();
	node_rom_init();
	_d_line();
	node_flash_init();
	_d_line();
	node_prop_init();
	_d_line();
	return 0;
}
int find_dir_file_node_no( char* name )
{
	int i;
	_s_dir_file_register_item* item;
	//_d_line();
	// check the same master_path
	for( i=0; i<_d_dir_file_register_items_max_count; i++ ){
		item = &g_dir_file_register_item[i];
		if( item->master_path[0] && strncmp( name, item->master_path, strlen(item->master_path) ) == 0 ){
			//_d_line();
			return i;
		}
	}
	//_d_line();
	return -1;
}
int register_dir_file_node( char* master_path, /*_s_dir_file_device_struct*/ void* device_struct, u32 flag )
{
	int i;
	_s_dir_file_register_item* item;
	int no;
	//_d_line();
	no = find_dir_file_node_no( master_path );
	//_d_line();
	if( no >= 0 ){
	//_d_line();
		item = &g_dir_file_register_item[no];
		item->device_struct = (_s_dir_file_device_struct*)device_struct;
	//_d_line();
		return no;
	}
	//_d_line();
	// not found, do register action.
	for( i=0; i<_d_dir_file_register_items_max_count; i++ ){
	//_d_line();
		item = &g_dir_file_register_item[i];
		if( item->master_path[0] == 0 ){
			strcpy( item->master_path, master_path );
			item->device_struct = (_s_dir_file_device_struct*)device_struct;
			//_d_hex( item->device_struct );
			//_d_hex( item->device_struct->o );
			//_d_hex( item->device_struct->o->create );
			return i;
		}
	}
	_d_line();
	return -1;
}
void* find_dir_file_node_no_and_calc_path( char* name, char** dir_name )
{
	_s_dir_file_register_item* item;
	int no;
	//_d_line();
	no = find_dir_file_node_no( name );
	if( no == -1 ){
		//_d_line();
		return 0;
	}
	//_d_line();
	item = &g_dir_file_register_item[no];
	*dir_name = &name[ strlen( item->master_path ) ];
	//_d_line();
	return (void*)item;
}



int df_out_no_function_string( char* device_name, char* function_name )
{
	//s_printf( "%d\t%s\tDevice (%s) no %s function\n", __LINE__, __FUNCTION__, device_name, function_name );
	return 0;
}
void* df_open( char*name, int flag )
{
	char* dir_name;
	_s_dir_file_register_item *item;

	//_d_line();
	// search the name for anyone operate function.
	item = (_s_dir_file_register_item*)find_dir_file_node_no_and_calc_path( name, &dir_name );
	// use this function to open the sub string name.
	//_d_line();
	if( item && item->device_struct->o && item->device_struct->o->open ){
		//_d_line();
		return item->device_struct->o->open( dir_name, flag );
	}
	else{
		//_d_hex( item );
		//_d_hex( item->device_struct );
		//_d_hex( item->device_struct->o );
		//_d_hex( item->device_struct->device_name );
		//_d_hex( item->device_struct->o->open );
		df_out_no_function_string( item->device_struct->device_name, "open" );
	}
	//_d_line();
	return 0;
}
void* df_create( char*name, int flag, char* value )
{
	char *dir_name;
	_s_dir_file_register_item *item;

	//_d_hex( flag );
	//_d_hex( *(unsigned*)value );
	//_d_line();
	// search the name for anyone operate function.
	item = (_s_dir_file_register_item*)find_dir_file_node_no_and_calc_path( name, &dir_name );
	// use this function to open the sub string name.
	if( item && item->device_struct->o && item->device_struct->o->create ){
		//_d_hex( item->device_struct->o->write );
		//_d_hex( item->device_struct->device_name );
		//_d_str( item->device_struct->device_name );
		return item->device_struct->o->create( dir_name, flag, value );
	}
	else{
		//_d_hex( item );
		//_d_hex( item->device_struct );
		//_d_hex( item->device_struct->o );
		//_d_hex( item->device_struct->device_name );
		//_d_hex( item->device_struct->o->create );
		df_out_no_function_string( item->device_struct->device_name, "create" );
	}
	return 0;
}
int df_read( void* h, char* buf, int size )
{
	_s_dir_file_register_item *item = (_s_dir_file_register_item*)h;
	_s_dir_file_device_struct *device_struct = item->device_struct;

	if( h == 0 ) return -1;
	if( device_struct->o && device_struct->o->read ){
		return device_struct->o->read( h, buf, size );
	}
	else{
		//_d_hex( device_struct );
		//_d_hex( device_struct->o );
		//_d_hex( device_struct->device_name );
		//_d_hex( device_struct->o->read );
		df_out_no_function_string( item->device_struct->device_name, "read" );
	}
	return -1;
}
int df_read_line( void* h, char* buf, int max_size )
{
	#define _d_try_read_size 50
	int base = 0;
	int pos;
	int size;
	char *tstr = "\n";
	int try_size = _d_try_read_size;
	if( try_size > max_size )
		try_size = max_size;
	while( 1 ){
		size = df_read( h, &buf[base], try_size );
		if( size <= 0 )		{
			if( base > 0 ){ 
				buf[base] = 0;
				return base; 
			}
			break;
		}
		base += size;
#if 0
		pos = s_memmem(buf, base, tstr, 1 );
		if( pos < 0 )
			continue;
#else
		//_d_buf( buf, base );
		for( pos = 0; pos < base; pos ++ ){
			if( buf[pos] == '\n' )
				break;
			}
		//_d_int( pos );
		if( pos == base )
			continue;
#endif
		// get the pos, and fix the offset.
		if( pos > base )
			while( 1 ) _d_line();
		df_seek( h, pos-base+1, 1 );
		buf[pos] = 0;
		return pos;
		}
	return -1;
}
int df_write( void* h, char* buf, int size )
{
	_s_dir_file_register_item *item = (_s_dir_file_register_item*)h;
	_s_dir_file_device_struct *device_struct = item->device_struct;
	if( h == 0 ) return -1;
	//_d_line();
	if( device_struct->o && device_struct->o->write ){
		//_d_hex( device_struct->o->write );
		//_d_hex( device_struct->device_name );
		//_d_str( device_struct->device_name );
		return device_struct->o->write( h, buf, size );
	}
	else{
		//_d_hex( device_struct );
		//_d_hex( device_struct->o );
		//_d_hex( device_struct->device_name );
		//_d_hex( device_struct->o->write );
		df_out_no_function_string( item->device_struct->device_name, "write" );
	}
	return -1;
}
int df_seek( void* h, int offset, int pos )
{
	_s_dir_file_register_item *item = (_s_dir_file_register_item*)h;
	_s_dir_file_device_struct *device_struct = item->device_struct;
	if( h == 0 ) return -1;
	//_d_line();
	if( device_struct->o && device_struct->o->seek ){
		return device_struct->o->seek( h, offset, pos );
	}
	else{
		//_d_hex( device_struct );
		//_d_hex( device_struct->o );
		//_d_hex( device_struct->device_name );
		//_d_hex( device_struct->o->seek );
		df_out_no_function_string( item->device_struct->device_name, "seek" );
	}
	return -1;
}
int df_close( void* h )
{
	_s_dir_file_register_item *item = (_s_dir_file_register_item*)h;
	_s_dir_file_device_struct *device_struct = item->device_struct;
	if( h == 0 ) return -1;
	//_d_line();
	if( device_struct->o && device_struct->o->close ){
		return device_struct->o->close( h );
	}
	else{
		//_d_hex( device_struct );
		//_d_hex( device_struct->o );
		//_d_hex( device_struct->device_name );
		//_d_hex( device_struct->o->close );
		df_out_no_function_string( item->device_struct->device_name, "close" );
	}
	return -1;
}
int df_delete( char*name, int flag )
{
	char* dir_name;
	_s_dir_file_register_item *item;

	//_d_line();
	// search the name for anyone operate function.
	item = (_s_dir_file_register_item*)find_dir_file_node_no_and_calc_path( name, &dir_name );
	// use this function to open the sub string name.
	//_d_line();
	if( item && item->device_struct->o && item->device_struct->o->delete ){
		//_d_line();
		return item->device_struct->o->delete( dir_name, flag );
	}
	else{
		if( item == 0 )
			return 0;
		//_d_hex( item );
		//_d_hex( item->device_struct );
		//_d_hex( item->device_struct->o );
		//_d_hex( item->device_struct->device_name );
		//_d_hex( item->device_struct->o->open );
		df_out_no_function_string( item->device_struct->device_name, "delete" );
	}
	//_d_line();
	return 0;
}
void* df_lock( void* h, int flag )
{
	_s_dir_file_register_item *item = (_s_dir_file_register_item*)h;
	_s_dir_file_device_struct *device_struct = item->device_struct;
	if( h == 0 ) return 0;
	//_d_line();
	if( device_struct->o && device_struct->o->lock ){
		return device_struct->o->lock( h, flag );
	}
	else{
		//_d_hex( device_struct );
		//_d_hex( device_struct->o );
		//_d_hex( device_struct->device_name );
		//_d_hex( device_struct->o->write );
		df_out_no_function_string( item->device_struct->device_name, "lock" );
	}
	return 0;
}
void* df_sync( void* h, int flag )
{
	_s_dir_file_register_item *item = (_s_dir_file_register_item*)h;
	_s_dir_file_device_struct *device_struct = item->device_struct;
	if( h == 0 ) return 0;
	_d_line();
	if( device_struct->o && device_struct->o->sync ){
		_d_line();
		return device_struct->o->sync( h, flag );
	}
	else{
		//_d_hex( device_struct );
		//_d_hex( device_struct->o );
		//_d_hex( device_struct->device_name );
		//_d_hex( device_struct->o->sync );
		df_out_no_function_string( item->device_struct->device_name, "sync" );
	}
	return 0;
}
int df_rename( char* old, char* new )
{
	df_copy_file( old,new );
	df_delete( old,0 );
	return 0;
}

int df_get_file_size( char* name )
{
	void *h;
	int size;
	h = df_open( name , 0 );
	if( h == 0 )
		return 0;
	size = df_seek( h, 0, 2 );
	if( size <= 0 )
		return 0;
	df_close( h );
	return size+1;
}
int df_read_file_content( char* name, char* buf, int max_size )
{
	void *h;
	int size;
	_d_str( name );
	size = df_get_file_size( name );

	if( size > max_size )
		size = max_size;
	h = df_open( name , 0 );
	if( h == 0 )
		return 0;
	df_read( h, buf, size );
	df_close( h );
	_d_int( size );
	return size;
}
int df_write_file_content( char* name, char* buf, int buf_size )
{
	void *h;
	df_delete( name, 0 );
	h = df_create(name, 0, 0 );
	if( h == 0 )
		return -1;
	df_write( h, buf, buf_size );
	df_close( h );
	return buf_size;
}

#include "tcpip/net.h"
int df_copy_file( char* n1, char* n2 )
{
	char *po ;
	int po_size ;
	int size;
	void *f1, *f2;

	po = alloc_temp_buffer( 1024 );
	po_size = 1024;

	if( po == 0 ){
		free_temp_buffer( po );
		return -1;
	}
	f1 = df_open( n1, 0 );
	if( f1 == 0 ){
		free_temp_buffer( po );
		return 0;
	}
	df_delete( n2, 0 );
	f2 = df_create( n2, 0, 0 );
	if( f2 == 0 ){
		df_close( f1 );
		free_temp_buffer( po );
		return 0;
	}
	while( 1 ){
		size = df_read( f1, po, po_size ); 
		if( size == 0 ) break;
		df_write( f2, po, size );
	}
	df_close( f1 );
	df_close( f2 );

	free_temp_buffer( po );
	return 1;
}
#if 0
int df_diff_file( char* n1, char* n2 )
{
	char *in_po = g_in_buf;
	int in_po_size = sizeof( g_in_buf );
	char *out_po = g_out_buf;
	int out_po_size = sizeof( g_out_buf );
	int in_size;
	int out_size;
	void *f1,*f2;

	f1 = df_open( n1, 0 );
	if( f1 == 0 )	return 0;
	df_delete( n2, 0 );
	f2 = df_open( n2, 0 );
	if( f2 == 0 ){
		df_close( f1 );
		return 0;
	}
	while( 1 ){
		in_size = df_read( f1, in_po, in_po_size );
		out_size = df_read( f1, out_po, out_po_size );
		if( in_size != out_size )
			return 0;
		if( memcmp( in_po, out_po, sizeof( g_in_buf ) ) != 0 ){
			return 0;
		}
	}
	return 1;
}
#endif

int df_exist( char *name )
{
	char* dir_name;
	_s_dir_file_register_item *item;

#if 0
	//_d_line();
	// search the name for anyone operate function.
	item = (_s_dir_file_register_item*)find_dir_file_node_no_and_calc_path( name, &dir_name );
	if( item && item->device_struct->o && item->device_struct->o->open ){
		return 1;
	}
	return 0;
#else
	void *h;
	h = df_open( name, 0 );
	if( h ){
		df_close( h );
		return 1;
	}
	return 0;
#endif
}



// ----------------------------------------
//
//  Here is min test dir_file code.
//
// ----------------------------------------


void* df_test_create( char* name, int size, char* value );
int df_test_delete( void* po );
int df_test_read( void* po, char* buf, int size );
int df_test_write( void* po, char* buf, int size );
int df_test_close( void* po );

const _s_dir_file_operate g_df_test_file_operate = 
{
	0,			//open
	0,			//seek
	df_test_read,		//read
	df_test_write,		//write
	df_test_close,		//close
	df_test_create,		//create
	0,			//delete
	0			//lock
};
typedef struct{
	_s_dir_file_operate *o;
	char *device_name;
}_s_df_test_device_struct;
const _s_df_test_device_struct g_df_test_device_struct={
	(_s_dir_file_operate*)&g_df_test_file_operate,
	"dir_file test"
};

typedef struct{
	_s_df_test_device_struct *device_struct;
	int tmp;
}_s_df_test_item;
const _s_df_test_item g_df_test_item[1]={
	{
		(_s_df_test_device_struct*)&g_df_test_device_struct,
		1
	},
};


void* df_test_create( char* name, int size, char* value )
{
	_d_line();
	//g_df_test_item[0].device_struct = &g_df_test_device_struct;
	//g_df_test_item[0].device_struct->o = &g_df_test_file_operate;
	_d_hex( (void*)&g_df_test_item[0] );
	return (void*)&g_df_test_item[0];
}
int df_test_delete( void* po )
{
	_d_line();
	return 0;
}
int df_test_read( void* po, char* buf, int size )
{
	_d_line();
	return 0;
}
int df_test_write( void* po, char* buf, int size )
{
	_d_line();
	return 0;
}
int df_test_close( void* po )
{
	_d_line();
	return 0;
}
int df_test()
{
	void *h;
	// init
	//dir_file_init();
	// register 
	register_dir_file_node( "/t/", (void*)&g_df_test_device_struct, 0 );
	// open
	h = df_open( "/t/test", 0 );
	// create
	h = df_create( "/t/test", 0, 0 );
	// read
	df_read( h, 0, 0 );
	// write
	df_write( h, 0, 0 );
	// seek
	df_seek( h, 0, 0 );
	// close
	df_close( h );
	return 0;
}


void dir_file_test()
{
	_d_line();
	df_test();
	_d_line();
}

#else


void dir_file_init()
{
}
void dir_file_test()
{
}



#endif
