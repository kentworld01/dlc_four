
#ifndef _d_node_flash_h_
#define _d_node_flash_h_

#include "os.h"
#include "node.h"

//#define _nrb(x) (byte)node_flash_read(x,1,0,0)
//#define _nrw(x) (word)node_flash_read(x,2,0,0)
//#define _nrd(x) (dword)node_flash_read(x,4,0,0)

void* node_flash_open( char* name, int flag );
void* node_flash_create( char* name, int size, char* value );
int node_flash_delete( char* name, int flag );
int node_flash_read( void* h, char* buf, int size );
int node_flash_write( void* h, char* buf, int size );
int node_flash_seek( void* h, int offset, int pos );
int node_flash_close( void* po );
void* node_flash_lock( void* po, int flag );

int node_flash_get_file_name( char**dir_name, int offset, int max_size );
int node_flash_format(void);

#endif

