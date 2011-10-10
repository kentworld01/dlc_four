
/*
 * all node operate, only read and write value, can not map the value.
 */

#include "node_rom.h"
#include "dir_file.h"
#include <string.h>

#ifndef _d_dir_file_modules

#else


#if 0
#define _d_io_bas "\
<body>\n\
10 z=(sys \"vi post_hash\")\n\
20 ` a\n\
30 if a > 2 then ` [b],[c],[d],[e],[f],[g]\n\
40 if a > 2 then z=(sys \"io s $[c] $[e] $[g]\")\n\
<form action='/io' method='post'>\n\
<select name=\"port\">\n\
<option value=\"a\">A</option>\n\
<option value=\"b\">B</option>\n\
<option value=\"c\">C</option>\n\
<option value=\"d\">D</option>\n\
</select>\n\
<select name=\"pin\">\n\
140 for i = 0 to 31\n\
150 ` \"<option value='\" ; i ; \"'>\" ; i ; \"</option>\"\n\
160 next i\n\
</select>\n\
<select name=\"val\">\n\
<option value=\"0\">L</option>\n\
<option value=\"1\">H</option>\n\
</select>\n\
<input type=\"submit\" value=\"Change\"> \n\
</form>\n\
</body>\n\
"



#define _d_upload_bas "\
<body>\n\
<form action='upload' enctype='multipart/form-data' method='post'>\n\
<input type='hidden' name='sys_action' value='save_input_file'>\n\
save_to_name_or_address:\n\
<p>\n\
<input type='text' name='name_or_address' value='/c/web/power_on.bmp' size='30'>\n\
<p>\n\
files:<br>\n\
<input type='file' name='file_content' size='64'>\n\
<p>\n\
<input type= 'submit' value='Send'>\n\
</body>\n\
"



#define _d_download_bas ""\
"10 z=(sys \"vs get_hash\")\n"\
"20 if a = 0 then ` \"usage: download?file=/c/XXXXX \" \n"\
"30 if a = 0 then end \n"\
"40 d=(sys \"file exist $c\")\n"\
"60 if d = 0 then ` \"<body>No file [\", [c] , \"] for download\" \n"\
"70 if d = 0 then end \n"\
"80 b=(sys \"file out $c\")\n"\


#endif


extern char _g_disk_bas[];
void _g_disk_bas_size();
extern char _g_download_bas[];
void _g_download_bas_size();
extern char _g_upload_bas[];
void _g_upload_bas_size();
extern char _g_io_bas[];
void _g_io_bas_size();
extern char _g_do_bas[];
void _g_do_bas_size();
extern char _g_key_bas[];
void _g_key_bas_size();
extern char _g_main_bas[];
void _g_main_bas_size();



// file operate function.
const _s_dir_file_operate node_rom_file_operate = 
{
	node_rom_open,	//open
	node_rom_seek,	//seek
	node_rom_read,	//read
	0,		//write
	node_rom_close,	//close
	0,		//create
	0,		//delete
	node_rom_lock	//lock
};


// device_struct for dir_file register struct.
typedef struct{
	_s_dir_file_operate *o;
	char* device_name;
}_s_node_rom_device_struct;

const _s_node_rom_device_struct g_node_rom_device_struct={
	(_s_dir_file_operate*)&node_rom_file_operate,
	"node_rom"
};


// node ram operate item.
// here define rom file content.
// here define rom file item struct.
#define _d_node_rom_item_max_count 16
static int file_offset[ _d_node_rom_item_max_count ] = {0};
//#define _d_node_rom_item_max_count 128
typedef struct{
	//_s_dir_file_operate *o;
	_s_node_rom_device_struct *device_struct;
	char* name;
	char* value;
	int size;
}_s_node_rom_item;

//extern const _g_disk_bas_size;

//int tmp_val = (int)_g_disk_bas_size;

const _s_node_rom_item g_node_rom_items[_d_node_rom_item_max_count]={
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"doit",
		"10 print 12345678",
		-1
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"config",
		"config_web_base_path='/c/web'\n"
			"ip=192.168.1.2\n"
			"ip_mask=255.255.255.0\n"
			"gw=192.168.1.1\n"
			,
		-1
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"autorun",
		"/b/doit",
		-1
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"admin.bas",
"\
<title>Admin Tools</title>\n\
<body>\n\
Admin Tools :\n\
		10 print [sys \"printf %20s string\"] \n\
		30 print [sys \"get_val get_key 1\"] \n\
		30 print [sys \"get_val get_key 2\"] \n\
		30 print [sys \"get_val get_val 1\"] \n\
		30 print [sys \"get_val get_val 2\"] \n\
		40 print [sys \"get_version\"] \n\
		50 print b\n\
</body>\n\
"
"\n",
		-1
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"disk.bas",
		_g_disk_bas,
		(int)_g_disk_bas_size
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"download.bas",
		_g_download_bas	,
		(int)_g_download_bas_size
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"upload.bas",
		_g_upload_bas	,
		(int)_g_upload_bas_size
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"io.bas",
		_g_io_bas	,
		(int)_g_io_bas_size
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"do.bas",
		_g_do_bas	,
		(int)_g_do_bas_size
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"key.bas",
		_g_key_bas	,
		(int)_g_key_bas_size
	},
	{
		(_s_node_rom_device_struct*)&g_node_rom_device_struct, 
		"main.bas",
		_g_main_bas	,
		(int)_g_main_bas_size
	},
	{0}
};

/*
"<a href=\"/c/\n"
"35 ` [b]\n"
"\">/c/\n"
"45 ` [b]\n"
"</a>\n"
*/
/*"30 sys \"vs post_hash\"\n"*/



int node_rom_get_item_no( void *po )
{
	u32 now_addr;
	u32 base_addr;
	int no;
	base_addr = (u32)g_node_rom_items;
	now_addr = (u32)po;
	no = (now_addr - base_addr) / sizeof( g_node_rom_items[0] );
	//_d_int( no );
	return no;
}
int node_rom_get_offset( void *po )
{
	int no = node_rom_get_item_no( po );
	//_d_int( file_offset[ no ] );
	return file_offset[ no ];
}
int node_rom_set_offset( void *po, int offset )
{
	_s_node_rom_item* tpo = (_s_node_rom_item*)po;
	int no = node_rom_get_item_no( po );
	int size;
	//_d_int(offset);
	if( offset < 0 )
		offset = 0;
	
	//_d_str( tpo->name );
	//_d_int( tpo->size );
	//_d_hex( tpo->value );
	//_d_str( tpo->value );
	
	if( tpo->size == -1 )
		size = strlen( tpo->value );
	else	size = tpo->size;
	if( offset >= size ){
		offset = size - 1;
	}
	file_offset[ no ] = offset;
	//_d_int( file_offset[ no ] );
	return no;
}
int node_rom_seek( void* po, int offset, int pos )
{
	_s_node_rom_item* tpo = (_s_node_rom_item*)po;
	int tpo__offset = node_rom_get_offset( po );
	int tpo__size = 0;
	//_d_line();
	//_d_int( tpo->offset );
	//_d_int( pos );
	//_d_int( offset );
	if( tpo->size == -1 ){
		tpo__size = strlen( tpo->value );
	}
	else	tpo__size = tpo->size;
	switch( pos ){
		case 0:	// begin
			//_d_line();
			if( offset >= tpo__size )
				offset = tpo__size-1;
			if( offset < 0 )
				offset = 0;
			tpo__offset = offset;
			break;
		case 1: // now pos
			//_d_line();
			//_d_int( offset );
			//_d_int( tpo->offset );
			//_d_int( tpo__size );
			if( offset + tpo__offset >= tpo__size ){
				//_d_line();
				offset = tpo__size-1;
			}
			tpo__offset += offset;
			if( tpo__offset < 0 ){
				//_d_line();
				tpo__offset = 0;
			}
			break;
		case 2: // end
			if( offset = tpo__size - offset )
				offset = tpo__size-1;
			if( offset < 0 )
				offset = 0;
			tpo__offset = offset;
			break;
	}
	node_rom_set_offset( po, tpo__offset );
	//_d_int( tpo__offset );
	return tpo__offset;
}
void* node_rom_open( char* name, int flag )
{
	int i;
	_s_node_rom_item *po = 0;
	//_d_str( name );
	for( i=0; i<_d_node_rom_item_max_count; i++ ){
		//_d_int(i);
		//_d_str( g_node_rom_items[i].name );
		if( s_strcmp( name, g_node_rom_items[i].name ) == 0 ){
			//_d_int(i);
			po = (_s_node_rom_item*)&g_node_rom_items[i];
			//_d_hex( po );
			node_rom_set_offset( po, 0 );
			return po;
		}
	}
	return 0;
}
#if 0
void* node_rom_create( char* name, int size, char* value )
{
	int i;
	_s_node_rom_item *po = 0;
	unsigned dw_name = 0;
	if( name[0] == 0 )
		return 0;
	po = node_rom_open( name, 0 );
	if( po==0 ){
		dw_name = get_dw_name( name );
		for( i=0; i<_d_node_rom_item_max_count; i++ ){
			if( g_node_rom_items[i].name == 0 ){
				//_d_str( "create find name == 0" );
				//_d_int(i);
				po = &g_node_rom_items[i];
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
		s_memcpy( &po->value, value, size );
		po->device_struct = &g_node_rom_device_struct;
		return po;
	}
	return 0;
}
int node_rom_delete( void* po )
{
	_s_node_rom_item* tpo = (_s_node_rom_item*)po;
	_d_line();
	memset( tpo, 0, sizeof( g_node_rom_items[0] ) );
	return 0;
}
#endif
int node_rom_read( void* po, char* buf, int size )
{
	_s_node_rom_item* tpo = (_s_node_rom_item*)po;
	int tpo__offset = node_rom_get_offset( po );
	int tpo__size = 0;
	//_d_int( tpo->size );
	//_d_int( size );
	//_d_int( tpo__offset );
	//_d_hex( tpo->size );
	//_d_hex( tpo->value );
	//_d_str( tpo->name );
	if( size == -1 ){
		strcpy( buf, tpo->value );
	}
	//if( size > sizeof( g_node_rom_items[0].value ) )
		//size = sizeof( g_node_rom_items[0].value );
	//_d_hex( tpo->value[0] );
	if( tpo->size == -1 )
		tpo__size = strlen( tpo->value );
	else	tpo__size = tpo->size;
	if( size + tpo__offset >= tpo__size ){
		size = tpo__size - tpo__offset - 1;
	}
	memcpy( buf, tpo->value + tpo__offset, size );
	tpo__offset += size;
	//_d_int( size );
	//_d_int( tpo__offset );
	//_d_int( tpo__size );
	node_rom_set_offset( po, tpo__offset );
	//tpo__offset = node_rom_get_offset( po );
	//_d_int( tpo__offset );
	return size;
}
#if 0
int node_rom_write( void* po, char* buf, int size )
{
	_s_node_rom_item* tpo = (_s_node_rom_item*)po;
	_d_line();
	if( size > sizeof( g_node_rom_items[0].value ) )
		size = sizeof( g_node_rom_items[0].value );
	memcpy( &tpo->value, buf, size );
	return 0;
}
#endif
int node_rom_close( void* po )
{
	//_d_line();
	return 0;
}
void* node_rom_lock( void* po, int flag )
{
	_s_node_rom_item* tpo = (_s_node_rom_item*)po;
	//_d_line();
	return tpo->value;
}

/*
 * standard file interface.
 * int open( char* name, mode )
 * int read( int h, char* buf, int size )
 * int write( int h, char* buf, int size )
 * int close( int h )
 */


int node_rom_init()
{
	register_dir_file_node( "/b/", (void*)&g_node_rom_device_struct, 0 );
	return 0;
}

int node_rom_test()
{
	// test action:
	// open file and show it.
	void *h;
	char buf[100];
	char *po;
	node_rom_init();
	// register 
	h = df_open( "/b/config", 0 );
	_d_hex( h );

	df_read( h, buf, -1 );
	_d_str( buf );

	po = df_lock( h, 0 );
	if( po )
		_d_str( po );

	df_close(h);

	return 0;
}

#endif

