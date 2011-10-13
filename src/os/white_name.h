
#ifndef _d_white_name_h
#define _d_white_name_h

#include "os.h"

#define _d_max_white_name_count (1000)
#define _d_max_white_name_size (96)

int white_name_list_get_value( char* buf, int *group_id, int* allow_id, int* not_allow_id, int *password );
int cam_read_group_id_from_white_name_record( DWORD card_id, char* buf, int buf_size );
int white_name_check_card( u32 card_no );
int white_name_init();

#endif

