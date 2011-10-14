
#ifndef _d_record_h_
#define _d_record_h_

#include "os.h"

#define _d_max_record_count (1000)
#define _d_max_record_size (96)
#define _d_record_file_name "/record.rf"
extern int g_record_rand_file_pos;
extern char g_record_file_name[32];


int gen_record_string( char *buf, DWORD time, DWORD sys_tick, DWORD record_id, DWORD card_id, int ok_flag );
int record_get_unsend_index();
int record_get( u32 id, char* buf, int max_buf_size );
int record_get_first_unsend( char* buf, int max_buf_size );
int record_del_unsend_index( int index );
int save_card_info( u32 id, char* buf, int ok_send_flag );
int record_init();

#endif

