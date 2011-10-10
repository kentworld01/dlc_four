
#ifndef _d_dir_file_h_
#define _d_dir_file_h_

#include "os.h"
#include "node_ram.h"
#include "node_flash.h"
#include "node_rom.h"
//#include "node_prop.h"

typedef void* (*dir_file_open)( char*name, int flag );
typedef int (*dir_file_read)( void* h, char* buf, int size );
typedef int (*dir_file_write)( void* h, char* buf, int size );
typedef int (*dir_file_seek)( void* h, int offset, int pos );
typedef int (*dir_file_close)( void* h );
typedef void* (*dir_file_create)( char* name, int size, char* value );
typedef int (*dir_file_delete)( char*name, int flag );
typedef void* (*dir_file_lock)( void* h, int flag );
typedef void* (*dir_file_sync)( void* h, int flag );
typedef struct {
	dir_file_open open;
	dir_file_seek seek;
	dir_file_read read;
	dir_file_write write;
	dir_file_close close;
	dir_file_create create;
	dir_file_delete delete;
	dir_file_lock lock;
	dir_file_sync sync;
}_s_dir_file_operate;

int register_dir_file_node( char* master_path, /*_s_dir_file_device_struct*/ void* device_struct, u32 flag );
void* df_open( char*name, int flag );
void* df_create( char*name, int flag, char* value );
int df_read( void* h, char* buf, int size );
int df_read_line( void* h, char* buf, int max_size );
int df_write( void* h, char* buf, int size );
int df_seek( void* h, int offset, int pos );
int df_close( void* h );
void* df_lock( void* po, int flag );
void* df_sync( void* po, int flag );
int df_delete( char*name, int flag );
int df_get_file_size( char* name );
int df_read_file_content( char* name, char* buf, int max_size );
int df_write_file_content( char* name, char* buf, int max_size );


extern u8 _g_show_system_file;


#endif

