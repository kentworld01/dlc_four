
#ifndef _d_white_name_h
#define _d_white_name_h

#include "os.h"

#define _d_max_white_name_count (1000)
#define _d_max_white_name_size (96)
#define _d_white_name_file_name "/white_name.rf"
#define _d_card_group_max_count 1

extern int g_white_name_rand_file_pos ;
extern char g_white_name_file_name[32] ;
extern char g_card_man[32];
extern char g_card_group[32];
extern int g_card_group_list[_d_card_group_max_count];
extern int g_card_group_flag[_d_card_group_max_count];
extern int g_card_group_count ;


int white_name_list_get_value( char* buf, int *group_id, int* allow_id, int* not_allow_id, int *password );
int cam_read_group_id_from_white_name_record( DWORD card_id, char* buf, int buf_size );
int white_name_check_card( u32 card_no );
int white_name_init();
char* get_card_man();
char* get_card_group();
int white_name_get( u32 id, char* buf, int max_buf_size );
int white_name_set( u32 id, char* buf, int max_buf_size );


#endif

