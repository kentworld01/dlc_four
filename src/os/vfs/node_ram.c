
/*
 * all node operate, only read and write value, can not map the value.
 */

#include "node_ram.h"
#include "dir_file.h"
#include <string.h>

#ifndef _d_dir_file_modules

#else

// file operate function.
_s_dir_file_operate node_ram_file_operate = 
{
	node_ram_open,		//open
	0,			//seek
	node_ram_read,		//read
	node_ram_write,		//write
	node_ram_close,		//close
	node_ram_create,	//create
	0,			//delete
	0			//lock
};


// device_struct for dir_file register struct.
typedef struct{
	_s_dir_file_operate *o;
	char* device_name;
}_s_node_ram_device_struct;

_s_node_ram_device_struct g_node_ram_device_struct={
	&node_ram_file_operate,
	"node_ram"
};

// node ram operate item.
#define _d_node_ram_item_max_count 16
//#define _d_node_ram_item_max_count 128
typedef struct{
	//_s_dir_file_operate *o;
	_s_node_ram_device_struct *device_struct;
	dword name;
	dword value;
	byte size;
}_s_node_ram_item;
_s_node_ram_item g_node_ram_items[_d_node_ram_item_max_count]={0};


int node_ram_init()
{
	memset( g_node_ram_items, 0, sizeof( g_node_ram_items ) );
	register_dir_file_node( "/a/", (void*)&g_node_ram_device_struct, 0 );
	return 0;
}

static unsigned get_dw_name( char* name )
{
	unsigned dw_name=0;
	unsigned char tb;
	tb = name[0];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[0] = tb;
	tb = name[1];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[1] = tb;
	tb = name[2];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[2] = tb;
	tb = name[3];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[3] = tb;
	return dw_name;
}
void* node_ram_open( char* name, int flag )
{
	unsigned dw_name = 0;
	int i;
	_s_node_ram_item *po = 0;
	dw_name = get_dw_name( name );
	for( i=0; i<_d_node_ram_item_max_count; i++ ){
		//_d_hex( dw_name );
		//_d_hex( g_node_ram_items[i].name );
		if( s_strncmp( (char*)&dw_name, (char*)&g_node_ram_items[i].name, 4 ) == 0 ){
			//_d_int(i);
			po = &g_node_ram_items[i];
			//_d_hex( po );
			return po;
		}
	}
	return 0;
}
void* node_ram_create( char* name, int size, char* value )
{
	int i;
	_s_node_ram_item *po = 0;
	unsigned dw_name = 0;
	if( name[0] == 0 )
		return 0;
	po = node_ram_open( name, 0 );
	if( po==0 ){
		dw_name = get_dw_name( name );
		for( i=0; i<_d_node_ram_item_max_count; i++ ){
			if( g_node_ram_items[i].name == 0 ){
				//_d_str( "create find name == 0" );
				//_d_int(i);
				po = &g_node_ram_items[i];
				//_d_hex( dw_name );
				s_memcpy( &po->name, (char*)&dw_name, 4 );
				break;
			}
		}
	}

	if( po ){
		if( size > 4 )
			size = 4;
		po->size = size;
		//_d_hex( po->name );
		if( value )
			s_memcpy( &po->value, value, size );
		po->device_struct = &g_node_ram_device_struct;
		return po;
	}
	return 0;
}
int node_ram_delete( void* po )
{
	_s_node_ram_item* tpo = (_s_node_ram_item*)po;
	if( po == 0 ) return 0;
	_d_line();
	memset( tpo, 0, sizeof( g_node_ram_items[0] ) );
	return 0;
}
int node_ram_read( void* po, char* buf, int size )
{
	_s_node_ram_item* tpo = (_s_node_ram_item*)po;
	//_d_line();
	//_d_hex( tpo->size );
	//_d_hex( tpo->value );
	//_d_str( tpo->name );
	if( po == 0 ) return 0;
	if( size > sizeof( g_node_ram_items[0].value ) )
		size = sizeof( g_node_ram_items[0].value );
	memcpy( buf, &tpo->value, size );
	return 0;
}
int node_ram_write( void* po, char* buf, int size )
{
	_s_node_ram_item* tpo = (_s_node_ram_item*)po;
	if( po == 0 ) return 0;
	//_d_line();
	if( size > sizeof( g_node_ram_items[0].value ) )
		size = sizeof( g_node_ram_items[0].value );
	memcpy( &tpo->value, buf, size );
	return 0;
}
int node_ram_close( void* po )
{
	_d_line();
	return 0;
}

/*
 * standard file interface.
 * int open( char* name, mode )
 * int read( int h, char* buf, int size )
 * int write( int h, char* buf, int size )
 * int close( int h )
 */



int node_ram_test()
{
	// test action:
	//  create two file
	//  re-create same name file
	//  close file and open it
	int val;
	int tval;
	void *h;
	void *hh;
	// init
	//dir_file_init();
	// register 

	node_ram_init();

	h = df_open( "/a/test", 0 );
	_d_hex( h );

	val = 3;
	_d_hex( val );
	h = df_create( "/a/tt", 4, (char*)&val );

	df_read( h, (char*)&tval, 4 );
	_d_hex( val );
	_d_hex( tval );

	tval = 4;
	df_write( h, (char*)&tval, 4 );
	df_read( h, (char*)&tval, 4 );
	_d_hex( val );
	_d_hex( tval );

	df_seek( h, 0, 0 );

	val = 7;
	hh = df_create( "/a/t1", 4, (char*)&val );
	_d_hex( h );
	_d_hex( hh );

	df_read( hh, (char*)&tval, 4 );
	_d_str( "t1" );
	_d_hex( val );
	_d_hex( tval );

	_d_str( "t" );
	df_read( h, (char*)&val, 4 );
	_d_hex( val );

	df_close( h );
	df_close( hh );

	// re create /a/t1
	_d_str( "re create /a/t1" );
	hh = 0;
	val = 8;
	hh = df_create( "/a/t1", 4, (char*)&val );
	df_read( hh, (char*)&tval, 4 );
	_d_hex( tval );
	df_close( hh );


	// re open /a/t1
	_d_str( "re open /a/t1" );
	hh = 0;
	hh = df_open( "/a/t1", 0 );
	df_read( hh, (char*)&tval, 4 );
	_d_hex( tval );
	df_close( hh );



	return 0;
}

#endif
