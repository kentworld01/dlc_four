
#ifndef _d_node_prop_h_
#define _d_node_prop_h_

#include "os.h"
#include "node.h"

#define _nrb(x) (byte)node_prop_read(x,1,0,0)
#define _nrw(x) (word)node_prop_read(x,2,0,0)
#define _nrd(x) (dword)node_prop_read(x,4,0,0)

void* node_prop_open( char* name, int flag );
void* node_prop_create( char* name, int size, char* value );
int node_prop_delete( void* h );
int node_prop_read( void* h, char* buf, int size );
int node_prop_write( void* h, char* buf, int size );
int node_prop_seek( void* h, int offset, int pos );
int node_prop_close( void* po );
int node_prop_sync( void *po, int flag );


int node_prop_set_value( char* file, char* value, int size );
int node_prop_get_value( char* file, char* value, int max_size );


#endif


