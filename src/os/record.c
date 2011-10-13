
#include "os/record.h"

#define _d_max_propery

int g_record_rand_file_pos = -1;
char *g_record_file_name = "/record.rf";


#define _ds_record_format "machine_no,date,time,sys_tick,record_id,card_id,ok_flag"
int gen_record_string( char *buf, DWORD time, DWORD sys_tick, DWORD record_id, DWORD card_id, int ok_flag )
{
	//extern UCHAR  Host_ipaddr[5];	
	//extern UINT  HOST_PORT;
	int len;
	DATETIME dt;
	char tmp_buf[32];
	char record_format[64];
	extern int g_machine_no;
	int kc;
	char *ki[15];
	int i;

	RelatvieToDateTime( &dt, time );

	// get card info string.
	{
		buf[0] = 0;
		// get prop string
#if 0
		if( node_prop_get_value( "/p/record_format", record_format, sizeof( record_format ) ) < 0 )
#endif
			strcpy( record_format, _ds_record_format );
		// loop key and gen the info string to buf.
		kc = analysis_string_to_strings_by_decollator( record_format, ", ", ki, 15 );
		for( i=0; i<kc; i++ ){
			if( strcmp( ki[i], "machine_no" ) == 0 ){
				s_sprintf( tmp_buf, "%d", g_machine_no );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "date" ) == 0 ){
				s_sprintf( tmp_buf, "%4d-%02d-%02d", dt.year, dt.month, dt.day );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "time" ) == 0 ){
				s_sprintf( tmp_buf, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "sys_tick" ) == 0 ){
				s_sprintf( tmp_buf, "%d", sys_tick );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "record_id" ) == 0 ){
				s_sprintf( tmp_buf, "%d", record_id );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "card_id" ) == 0 ){
				s_sprintf( tmp_buf, "%u", card_id );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "ok_flag" ) == 0 ){
				s_sprintf( tmp_buf, "%d", ok_flag );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "reader_id" ) == 0 ){
				extern int g_reader_id;
				s_sprintf( tmp_buf, "%d", g_reader_id );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "ip" ) == 0 ){
				extern UCHAR Host_ipaddr[5] ;
				char *po = g_send_package_socket_ip;
				s_sprintf( tmp_buf, "%d.%d.%d.%d", po[0], po[1], po[2], po[3] );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "mac" ) == 0 ){
#if 0
				extern UCHAR my_hwaddr[6] ;
				char *po = my_hwaddr;
				s_sprintf( tmp_buf,"%02x.%02x.%02x.%02x.%02x.%02x", po[0], po[1], po[2], po[3], po[4], po[5] );
				strcat( buf, tmp_buf );
#else
				strcpy( tmp_buf, "01.02.03.04.05.06" );
#endif
			}
			else if( strcmp( ki[i], "card_group_flag" ) == 0 ){
				extern int g_card_group_count;
				s_sprintf( tmp_buf, "%d", g_card_group_count );
				strcat( buf, tmp_buf );
			}
			else if( strcmp( ki[i], "card_group_list" ) == 0 ){
#if 0
				extern DWORD *g_card_group_list;
				int i;
				for( i=0; i<g_card_group_count; i++ ){
					s_sprintf( tmp_buf, "%d", g_card_group_list[i] );
					strcat( buf, tmp_buf );
					if( i < g_card_group_count - 1 )
						strcat( buf, ";" );
				}
#endif
			}
			strcat( buf, " " );
		}
	}
	_d_str( buf );
	return 0;
}

int record_get_unsend_index()
{
	unsigned char flag;
	int i;
	for( i=0; i<_d_max_record_count; i++ )
	{
		flag = rand_file_get_record_flag( g_record_rand_file_pos, i );
		if( flag == 0xff )
			continue;
		if( flag != 0 )
			break;
	}
	if( i==_d_max_record_count )
		return -1;
	return i;
}
int record_get( u32 id, char* buf, int max_buf_size )
{
	return rand_file_get( g_record_rand_file_pos, index, buf, max_buf_size );
}
int record_get_first_unsend( char* buf, int max_buf_size )
{
	int index;
	index = record_get_unsend_index();
	if( index == -1 )
		return -1;
	record_get( index, buf, max_buf_size );
	return index;
}
int record_del_unsend_index( int index )
{
	return rand_file_set_record_flag(  g_record_rand_file_pos, index, 0 );
}
int save_card_info( u32 id, char* buf, int ok_send_flag )
{
	int index = id % _d_max_record_count;
	rand_file_del( g_record_rand_file_pos, index );
	rand_file_add( g_record_rand_file_pos, buf, strlen(buf) );
	rand_file_set_record_flag(  g_record_rand_file_pos, index, ok_send_file );
	//rand_file_del( g_record_rand_file_pos, (index+1)%_d_max_record_count );
	return 0;
}
int record_init()
{
	g_record_rand_file_pos = rand_file_open( g_record_file_name );
	if( g_record_rand_file_pos < 0 ){
		if( rand_file_create( g_record_file_name, _d_max_record_count,_d_max_record_size ) < 0 ){
			//while(1);
			return -1;
		}
		g_record_rand_file_pos = rand_file_open( g_record_file_name );
	}
	return 0;
}

