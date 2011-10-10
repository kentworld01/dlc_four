
#ifndef _d_node_rom_h_
#define _d_node_rom_h_

#include "os.h"
#include "node.h"

//#define _nrb(x) (byte)node_rom_read(x,1,0,0)
//#define _nrw(x) (word)node_rom_read(x,2,0,0)
//#define _nrd(x) (dword)node_rom_read(x,4,0,0)

void* node_rom_open( char* name, int flag );
void* node_rom_create( char* name, int size, char* value );
int node_rom_delete( void* h );
int node_rom_read( void* h, char* buf, int size );
int node_rom_write( void* h, char* buf, int size );
int node_rom_seek( void* h, int offset, int pos );
int node_rom_close( void* po );
void* node_rom_lock( void* po, int flag );

#endif

