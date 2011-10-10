
/*
 * all node operate, only read and write value, can not map the value.
 */

#include "node_flash.h"
#include "dir_file.h"
#include <string.h>

//#include "stm32_eval_spi_flash.h"


#ifndef _d_dir_file_modules

#else

// define flash phy data
#define _d_flash_page_size 512
#define _d_flash_sector_size (_d_flash_page_size*8)	// 4k
#define _d_flash_block_size (_d_flash_page_size*128)	// 64k
#define _d_sector_number(x) (x/_d_flash_sector_size)
#define _d_sector_magic_id (13159)

// define use flash area address
#define _d_flash_area_begin (0x100000)
#define _d_flash_area_length (32*_d_flash_sector_size)
//#define _d_flash_area_length (64*1024*3)
#define _d_flash_area_end ( _d_flash_area_begin + _d_flash_area_length - 1)

// define big file use flash area address
#define _d_big_file_flash_area_begin (0x200000)
#define _d_big_file_flash_area_length (16*64*1024)
//#define _d_flash_area_length (64*1024*3)
#define _d_big_file_flash_area_end ( _d_big_file_flash_area_begin + _d_big_file_flash_area_length - 1)
#define _d_big_file_flash_area_length_block_count (_d_big_file_flash_area_length/_d_flash_block_size)



// file operate function.
const _s_dir_file_operate node_flash_file_operate = 
{
	node_flash_open,	//open
	node_flash_seek,	//seek
	node_flash_read,	//read
	node_flash_write,	//write
	node_flash_close,	//close
	node_flash_create,	//create
	node_flash_delete,	//delete
	0	//lock
};


// device_struct for dir_file register struct.
typedef struct{
	_s_dir_file_operate *o;
	char* device_name;
}_s_node_flash_device_struct;

const _s_node_flash_device_struct g_node_flash_device_struct={
	(_s_dir_file_operate*)&node_flash_file_operate,
	"node_flash"
};


// node ram operate item.
// here define rom file content.
// here define rom file item struct.
#define _d_node_flash_item_max_count 16
//#define _d_node_flash_item_max_count 128
typedef struct{
	//_s_dir_file_operate *o;
	_s_node_flash_device_struct *device_struct;
	u32 node_flash_address;
	u32 file_flash_address;
	u32 size;
	int offset;
}_s_node_flash_item;





_s_node_flash_item g_node_flash_items[_d_node_flash_item_max_count];
/*
	{
		(_s_node_flash_device_struct*)&g_node_flash_device_struct, 
		"doit",
		"10 print 12345678",
		-1
	},
*/

// ---------------------------
//  here for block struct 
typedef struct{
	u16 sector_id;
	u16 sector_address;
	unsigned char name[96];
	u32 size;
	u32 address;
	u8 tmp[ 128 - 2 - 2 - 96 - 4 - 4 ];
	//u8 tmp[ 128 - 96 - 4 ];
}_s_sector_header;
// ---------------------------

#define _d_big_file_min_size (4096-sizeof(_s_sector_header))



int sector_check_valid( u8 *d, u32 a )
{
	u16 id;
	a = _d_sector_number(a);
	id = *(u16*)(&d[2]);
	//_d_hex( a );
	//_d_hex( id );
	//_d_hex( *(u32*)d );
	if( id == _d_sector_magic_id && a == *(u16*)(&d[0]) )
		return 1;
	return 0;
}
int sector_init( u32 a )
{
	//void sFLASH_EraseSector(uint32_t SectorAddr);
	u32 id;
	u32 no;
	_d_hex( a );
	//sFLASH_EraseSector(a);
	W25X_EnableWrite();
	W25X_SectorErase(a);
	no = _d_sector_number(a);
	id = _d_sector_magic_id;
	id = (id<<16) + no;
	_d_hex( id );
	Flash_Write( a, (u8*)&id, sizeof( id ) );
	//sFLASH_WritePage( (u8*)&id, a, sizeof( id ) );
	//id = 0;
	//sFLASH_ReadBuffer( (u8*)&id, a, sizeof( id ) );
	//_d_hex( id );
	return 0;
}
int node_flash_remove_all_file(void)
{
	u32 a;
	_d_line();
	for( a = _d_flash_area_begin; a <= _d_flash_area_end; a += _d_flash_sector_size ){
		sector_init( a );
	}
	return 0;
}

//-------------------------------------------
//-------------------------------------------
//
//	Big File function code begin
//
//-------------------------------------------
//-------------------------------------------

static u32 set_block_node_str( char *str, u32 address, int size, char tag )
{
	u32 a;
	//_d_hex( address );
	//_d_int( size );
	if( tag == 0 )
		tag = 'u';
	for( a = address; a <= address + size; a += _d_flash_block_size ){
		int no;
		no = (a-_d_big_file_flash_area_begin) / _d_flash_block_size;
		str[ no ] = tag;
	}
	return 0;
}
static u32 alloc_big_file_address( u32 size )
{
	// search all big file node
	// use exist big file begin address and length make alloc_big_file_string.
	// use strstr function search alloc_big_file_string wether include (size/64) space char
	// it done for alloc 
	
	// note:  when one file delete, need erase all data.
	int i,j;
	u32 a;
	char *po;
	char block_node_str[ _d_big_file_flash_area_length_block_count+1 ];
	char need_alloc_str[ _d_big_file_flash_area_length_block_count+1 ];
	_s_sector_header sh;

	//_d_line();
	memset( block_node_str, 0, sizeof(block_node_str) );
	memset( block_node_str, ' ', sizeof(block_node_str)-1 );
	memset( need_alloc_str, 0, sizeof(need_alloc_str) );
	for( i = _d_flash_area_begin; i <= _d_flash_area_end; i += _d_flash_sector_size ){
	//_d_line();
		//sFLASH_ReadBuffer( (u8*)&sh, i, sizeof( sh ) );
		Flash_Read(i, (u8*)&sh, sizeof( sh ) );
		if( sh.name[0] != 0xff && sh.size >= _d_big_file_min_size ){
			//_d_line();
			//_d_hex( sh.address );
			//_d_hex( sh.size );
			//_d_hex( _d_big_file_flash_area_begin );
			//_d_hex( _d_big_file_flash_area_length );
			if( _d_big_file_flash_area_begin <= sh.address && sh.address+sh.size <= _d_big_file_flash_area_end ){
				//_d_line();
				set_block_node_str( block_node_str, sh.address, sh.size, 0 );
			}
		}
	}
	//_d_line();
	//_d_text( block_node_str, _d_big_file_flash_area_length_block_count );
	_d_buf( block_node_str, _d_big_file_flash_area_length_block_count );

	//_d_line();
	set_block_node_str( need_alloc_str, _d_big_file_flash_area_begin, size, ' ' );
	//_d_text( need_alloc_str, _d_big_file_flash_area_length_block_count );
	_d_buf( need_alloc_str, _d_big_file_flash_area_length_block_count );

	//_d_line();
	po = strstr( block_node_str, need_alloc_str );
	if( po ){
		// find free memory.
		int no = po-block_node_str;
		u32 addr;
		//_d_int( no );
		addr = _d_big_file_flash_area_begin + no * _d_flash_block_size;
		//_d_hex( addr );
		return addr;
	}
	//_d_line();
	return 0;
}

//-------------------------------------------
//-------------------------------------------
//
//	Big File function code end
//
//-------------------------------------------
//-------------------------------------------

int node_flash_init()
{
	u32 a;
	u32 tmp_val;
	memset( g_node_flash_items, 0, sizeof( g_node_flash_items ) );
	//check the item block valid. init the block valid when non-valid.
	for( a = _d_flash_area_begin; a <= _d_flash_area_end; a += _d_flash_sector_size ){
		Flash_Read(a, (u8*)&tmp_val, 4);
		//sFLASH_ReadBuffer( (u8*)&tmp_val, a, 4 );
		
		//_d_hex( tmp_val );
		if( sector_check_valid( (u8*)&tmp_val, a ) <= 0 ){
			//_d_hex(a);
			// init block
			sector_init( a );
		}
	}
	// register 
	register_dir_file_node( "/c/", (void*)&g_node_flash_device_struct, 0 );
	return 0;
}

void* node_flash_open( char* name, int flag )
{
	int i,j;
	_s_sector_header sh;
	//_s_node_flash_item *po = 0;
	//_d_str( name );
	for( i = _d_flash_area_begin; i <= _d_flash_area_end; i += _d_flash_sector_size ){
		//_d_hex(i);
		//sFLASH_ReadBuffer( (u8*)&sh, i, sizeof( sh ) );
		Flash_Read(i, (u8*)&sh, sizeof( sh ) );

		//_d_str( sh.name );
		//_d_hex( sh.address );
		//_d_hex( sh.name[0] );
		if( sh.name[0] != 0xff && s_strcmp( name, sh.name ) == 0 ){
			for( j=0; j<_d_node_flash_item_max_count; j++ ){
				_s_node_flash_item *po;
				po = &g_node_flash_items[j];
				if( po->device_struct == 0 ){
					//_d_int( j );
					// have null node, use it for open file.
					po->device_struct = (_s_node_flash_device_struct*)&g_node_flash_device_struct;
					po->node_flash_address = i;
					po->file_flash_address = sh.address;
					po->size = sh.size;
					if( po->size == 0xffffffff )
						po->size = _d_flash_sector_size - sizeof(_s_sector_header);
					po->offset = 0;
					return po;
				}
			}
		}
	}
	return 0;
}
void* node_flash_create( char* name, int size, char* value )
{
	_s_sector_header sh;
	int i;
	_s_node_flash_item *po = 0;
	//_d_str( name );
	if( name[0] == 0 )
		return 0;
	//_d_line();
	po = node_flash_open( name, 0 );
	//_d_hex( po );
	if( po==0 ){
		//_d_line();
		// create file node
		for( i = _d_flash_area_begin; i <= _d_flash_area_end; i += _d_flash_sector_size ){
			//_d_hex(i);
			//sFLASH_ReadBuffer( (u8*)&sh, i, sizeof( sh ) );
			Flash_Read( i, (u8*)&sh, sizeof( sh ) );
			//_d_hex( sh.name[0] );
			if( sh.name[0] == 0xff ){
				//_d_line();
				strcpy( sh.name, name );

				// here search the big size file area.
				//_d_int( size );
				if( size >= _d_big_file_min_size || *value == 1 ){
					sh.address =  alloc_big_file_address( size );
					if( sh.address == 0 )
						return 0;
					if( *value == 1 ){
						// delay write size field.
						// but set the size = 0x7fffffff, let it can write for ever.
						sh.size = 0x7fffffff;
					}
					else{
						sh.size = size;
					}
				}
				else{
					sh.address = i + sizeof( sh );
					// for card_record.
					if( size > 0 ){
						sh.size = size;
					}
				}

				//_d_str( sh.name );
				//_d_hex( sh.address );
				//sFLASH_WritePage( (u8*)&sh, i, sizeof( sh ) );
				Flash_Write(i, (u8*)&sh, sizeof( sh ) );
				// for debug.
				//memset( &sh, 0, sizeof(sh) );
				//sFLASH_ReadBuffer( (u8*)&sh, i, sizeof( sh ) );
				//_d_str( sh.name );
				break;
			}
		}
		// re open it.
		po = node_flash_open( name, 0 );
		//_d_hex(po);
	}
	//_d_line();
	return po;
}
int node_flash_read( void* po, char* buf, int size )
{
	unsigned a;
	int i;
	_s_node_flash_item* tpo = (_s_node_flash_item*)po;
	//_d_line();
	//if( tpo->size <= 0 ) return 0;
	if( tpo->offset >= tpo->size - 1 )
		return 0;
	if( size + tpo->offset >= tpo->size ){
		size = tpo->size - tpo->offset;
	}
	// maybe is 512
	//a = tpo->node_flash_address + tpo->offset + sizeof( _s_sector_header );
	a = tpo->file_flash_address + tpo->offset ;
	//sFLASH_ReadBuffer( (u8*)buf, a, size );
	Flash_Read( a, (u8*)buf, size );
	tpo->offset += size;
	return size;
}
// *************************
//  FIXME: here only use new file write. delete the file if it is exist.
// *************************
int node_flash_write( void* po, char* buf, int size )
{
	unsigned a;
	int i;
	_s_node_flash_item* tpo = (_s_node_flash_item*)po;
	//_d_int(size);
	//_d_hex( tpo->size );
	//_d_int( size );
	//_d_int( tpo->offset );
	//_d_int( tpo->size );
	if( tpo->size != 0x7fffffff && size > tpo->offset + tpo->size ){
		//_d_line();
		size = tpo->size - tpo->offset;
	}
	//_d_int(size);
	// maybe is 512
	//sFLASH_ReadBuffer( buf, tpo->node_flash_address + tpo->offset, size );
	//a = tpo->node_flash_address + tpo->offset + sizeof( _s_sector_header );
	a = tpo->file_flash_address + tpo->offset ;
	//_d_hex( a );
	//_d_hex( size );
#if 1
	//sFLASH_WritePage( (u8*)buf, a, size );
	Flash_Write( a, (u8*)buf, size );
#else
	for( i=0; i<size; i++ ){
		//sFLASH_WritePage( (u8*)&buf[i], a+i, 1 );
	}
#endif
	tpo->offset += size;
	//_d_int( tpo->offset );
	return size;
}
int node_flash_close( void* po )
{
	//_d_line();
	// write the size.
	_s_node_flash_item* tpo = (_s_node_flash_item*)po;
	_s_sector_header sh;
	//_d_hex( tpo->offset );
	//sFLASH_ReadBuffer( (u8*)&sh, tpo->node_flash_address, sizeof( sh ) );
	Flash_Read( tpo->node_flash_address, (u8*)&sh, sizeof( sh ) );
	//_d_hex( tpo->offset );
	//_d_hex( sh.size );
	if( sh.size == 0xffffffff || sh.size == 0x7fffffff )
	{
		sh.size = tpo->offset;
		//sh.size = tpo->offset+1;
		_d_int( sh.size );
		//sFLASH_WritePage( (u8*)&sh, tpo->node_flash_address, sizeof( sh ) );
		Flash_Write( tpo->node_flash_address, (u8*)&sh, sizeof( sh ) );
	}

	memset( (char*)po, 0, sizeof( _s_node_flash_item ) );
	return 0;
}
int node_flash_delete( char* name, int flag )
{
	_s_sector_header sh;
	_s_node_flash_item* tpo;

	//_d_str( name );
	tpo = node_flash_open( name, 0 );
	if( tpo ){
		//_d_hex(tpo->node_flash_address);
		//_d_hex( tpo->file_flash_address );
		//_d_hex( tpo->size );
		if( tpo->file_flash_address >= _d_big_file_flash_area_begin  ){
			u32 a;
			// erase big file content.
			// erase tpo->file_flash_address , to tpo->size flash block.
			for( a = tpo->file_flash_address; a < tpo->file_flash_address + tpo->size; a += _d_flash_block_size ){
				//void sFLASH_EraseSector(uint32_t SectorAddr);
				if( _d_big_file_flash_area_begin <= a && a < _d_big_file_flash_area_end ){
					_d_hex( a );
					//sFLASH_EraseBlock(a);
					W25X_EnableWrite();
					W25X_BlockErase(a);
				}
			}
		}
		sector_init(tpo->node_flash_address);
		//sFLASH_ReadBuffer( (u8*)&sh, tpo->node_flash_address, sizeof( sh ) );
		//_d_str( sh.name );
		//_d_hex( sh.name[0] );
		memset( (char*)tpo, 0, sizeof( _s_node_flash_item ) );
	}
	return 0;
}
int node_flash_seek( void* po, int offset, int pos )
{
	_s_node_flash_item* tpo = (_s_node_flash_item*)po;
	//if( tpo->size <= 0 ) return -1;
	//_d_line();
	//_d_int( tpo->offset );
	//_d_int( pos );
	switch( pos ){
		case 0:	// begin
			//_d_line();
			if( offset >= tpo->size )
				offset = tpo->size-1;
			if( offset < 0 )
				offset = 0;
			tpo->offset = offset;
			break;
		case 1: // now pos
			//_d_line();
			//_d_int( offset );
			//_d_int( tpo->offset );
			//_d_int( tpo->size );
			if( offset + tpo->offset >= tpo->size ){
				//_d_line();
				offset = tpo->size-1;
			}
			tpo->offset += offset;
			if( tpo->offset < 0 ){
				//_d_line();
				tpo->offset = 0;
			}
			break;
		case 2: // end
			if( offset = tpo->size - offset )
				offset = tpo->size-1;
			if( offset < 0 )
				offset = 0;
			tpo->offset = offset;
			break;
	}
	//_d_int( tpo->offset );
	return tpo->offset;
}


/*
 * standard file interface.
 * int open( char* name, mode )
 * int read( int h, char* buf, int size )
 * int write( int h, char* buf, int size )
 * int close( int h )
 */

int check_file_name( char *name )
{
	int len;
	if( name[0] == 0xff )	return 0;
	len = strlen( name );
	if( strcmp( &name[len-4], ".bas" ) == 0 ){
		if( _g_show_system_file == 0 )
			return 0;
	}
	return 1;
}

// if dir_name == 0 , calc total number
// if dir_name != null and max_size == -1 , will write all items in dir_name.
int node_flash_get_file_name( char**dir_name, int offset, int max_size )
{
	int no,i;
	_s_sector_header sh;
	if( offset < 0 )
		return 0;
	no = 0;
	for( i = _d_flash_area_begin; i <= _d_flash_area_end; i += _d_flash_sector_size ){
		//sFLASH_ReadBuffer( (u8*)&sh, i, sizeof( sh ) );
		Flash_Read( i, (u8*)&sh, sizeof( sh ) );
		if( 
				//sh.name[0] != 0xff 
				check_file_name( sh.name ) == 1
				){
			if( no >= offset ){
				if( dir_name ){
					strcpy( dir_name[no-offset], sh.name );
				}
			}
			no ++;
			if( dir_name && max_size >= 0 && (no-offset) >= max_size ){
				//_d_line();
				break;
			}
		}
	}
	return no-offset;
}
u32 node_flash_get_file_address( void *po )
{
	_s_node_flash_item* tpo = (_s_node_flash_item*)po;
	return tpo->file_flash_address;
}
u32 node_flash_get_file_size( void *po )
{
	_s_node_flash_item* tpo = (_s_node_flash_item*)po;
	return tpo->size;
}



int update_firmware( char *file_name )
{
	void *h;
	_s_node_flash_item *tpo;
	unsigned char *spo, *dpo;
	unsigned char buf[32];
	unsigned *upo;
	int size;
	
	dpo = (unsigned char*)0x90000;
	h = df_open( file_name, 0 );
	_d_hex( h );
	if( h == 0 ){
		error_message( "can not open file for update firmware", h );
		return -1;
	}
	tpo = (_s_node_flash_item*)h;
	size = tpo->size;
	_d_int( size );
	if( size <= 0 )
		return -1;
	spo = (unsigned char*)node_flash_get_file_address( h );
	df_close( h );
	// create the temp header.
	upo = (unsigned*)buf;
	upo[0] = 0xff5517f1;
	upo[1] = 16;
	upo[2] = size;
	upo[3] = 0;
	_d_buf( upo, 16 );
	Flash_Write( (u32)dpo, (u8*)buf, 16 );
	// copy the file content
	dpo = (unsigned char*)0x90010;
	while( size > 0 ){
		Flash_Read( (u32)spo, (u8*)buf, sizeof( buf ) );
		Flash_Write( (u32)dpo, (u8*)buf, sizeof( buf ) );
		//_d_hex( spo );
		//_d_hex( dpo );
		//_d_buf( buf, sizeof( buf ) );
		spo += sizeof(buf);
		dpo += sizeof(buf);
		size -= sizeof(buf);
		//_d_int( size );
	}
	// create the header.
	buf[0] = 0xaa;
	dpo = (unsigned char*)0x90003;
	Flash_Write( (u32)dpo, (u8*)buf, 1 );
	_d_line();
	simulate_keybaord( 'r' );
	_d_line();
	return 0;
}



int node_flash_test()
{
	int i;
	// test action:
	// open file and show it.
	void *h;
	char buf[100];
	//char *po;

#if 1
	// test big file.
	_d_line();
	h = df_create( "/c/bb0", 8192, 0 );
	_d_hex( h );
	if( h ){
		df_close( h );
	}

	_d_line();
	h = df_create( "/c/bb1", 64*1024, 0 );
	_d_hex( h );
	if( h ){
		df_close( h );
	}

	_d_line();
	h = df_create( "/c/bb2", 128*1024, 0 );
	_d_hex( h );
	if( h ){
		df_close( h );
	}

	df_delete( "/c/bb1", 0 );
	_d_line();
	h = df_create( "/c/bb1", 64*1024, 0 );
	_d_hex( h );
	if( h ){
		df_close( h );
	}
#endif
#if 0
	h = df_open( "/c/config", 0 );
	_d_hex( h );

	if( h == 0 ){
		h = df_create( "/c/config", 0, 0 );
		_d_hex( h );
		df_write( h, "abcde", 5 );
		df_seek( h, 0, 0 );
	}

	df_read( h, buf, 5 );
	//_d_hex( buf[0] );
	_d_str( buf );
	if( buf[0] == 0xff ){
		_d_line();
		df_seek( h, 0, 0 );
		df_write( h, "abcde", 5 );
	}
	df_seek( h, 0, 0 );
	df_read( h, buf, 5 );
	//_d_hex( buf[0] );
	_d_str( buf );


	df_close(h);
	//df_delete("/c/config",0);

	df_create( "/c/abcdef",0,0 );

	// for dir list
	{
		int count;
		char dir_name[30];
		char *po[1];
		po[0] = dir_name;
		count = node_flash_get_file_name( 0, 0, 0 );
		_d_int(count);
		for( i=0; i<count; i++ ){
			node_flash_get_file_name( po, i, 1 );
			_d_str( dir_name );
		}
	}
#endif
	return 0;
}

#endif

