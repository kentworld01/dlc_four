
#ifndef _d_node_ram_h_
#define _d_node_ram_h_

#include "os.h"
#include "node.h"

#define _nrb(x) (byte)node_ram_read(x,1,0,0)
#define _nrw(x) (word)node_ram_read(x,2,0,0)
#define _nrd(x) (dword)node_ram_read(x,4,0,0)

void* node_ram_open( char* name, int flag );
void* node_ram_create( char* name, int size, char* value );
int node_ram_delete( void* h );
int node_ram_read( void* h, char* buf, int size );
int node_ram_write( void* h, char* buf, int size );
int node_ram_seek( void* h, int offset, int pos );
int node_ram_close( void* po );

#endif

