#ifndef _d_config_file_h_
#define _d_config_file_h_

#include "os.h"
void* config_open( char* txt_file_name, char* sort_pointer_file );
int config_get_val( void *h, char* key, char *val, int max_size, int sub_item );
int config_close( void* h );

#endif

