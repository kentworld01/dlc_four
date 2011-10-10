#ifndef _d_index_file_h_
#define _d_index_file_h_

#ifdef _d_pc_test
#else

#include "os.h"

#endif

//typedef struct _s_key_node;
typedef struct _s_key_node{
	//struct _s_key_node *left, *parent, *right;
	int left, parent, right;
}_s_key_node;

typedef struct{
	int now_block;
	int now_record;
	int now_key;
	int now_index;
	int record_size;
	int loop_blocks;
	char file_name[32];
	void *h;
	void *kh;
	int loop_blocks_tail;

	int key_offset;
	int key_length;
	int key_type;		// 1 int, 2 uint, 3 string.  0 is error.
	int key_function;

	// for recursive
	int recursive_action;		// 1 find logic id,  2 show 
	int recursive_find_logic_id;
	int recursive_now_logic_id;
}_s_index;



int index_init();
int index_create( _s_index *pi, char *file_name, int flag,  int record_size, char* record_struct );
int index_delete( char *file_name, int flag );
int index_open( _s_index *pi, char *file_name, int flag );
int index_append( _s_index *pi, void* content );
int index_insert( _s_index *pi, int index, void* content );
int index_remove( _s_index *pi, int index );
int index_get_count( _s_index *pi );
int index_get_valid_count( _s_index *pi );
//int index_get_index( _s_index *pi, int index, void *content, int content_size );
int index_search( _s_index *pi, void* content, int *stop_physical_id );
int index_update( _s_index *pi, int index, void* content );
int index_get_record( _s_index *pi, void *content, int content_size );


#endif

