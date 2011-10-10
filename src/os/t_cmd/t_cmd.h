

#ifndef _d_t_cmd_h_
#define _d_t_cmd_h_


#include "os.h"



extern char* t_cmd_result_buffer;
typedef unsigned (*t_cmd_function)( char** param, char* rel_buf, int max_rel_buf_size );
typedef struct{
	char *name;
	int type;
	int class;
	char *param;
	t_cmd_function fun;
}_d_t_cmd_item;



int t_cmd_do( char* cmd, char* rel_buf, int max_rel_buf_size );
int t_cmd_main( char* cmd );



#endif

