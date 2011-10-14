

#include "os/white_name.h"
#include <stdio.h>


int g_white_name_rand_file_pos = -1;
char g_white_name_file_name[32] = {0};
char g_card_man[32];
char g_card_group[32];
int g_card_group_list[_d_card_group_max_count];
int g_card_group_flag[_d_card_group_max_count];
int g_card_group_count = 0;

#define _ds_white_name_format "u32:card_no,c8:name"
#if 0
int gen_white_name_string( char *buf, DWORD time, DWORD sys_tick, DWORD record_id, DWORD card_id, int ok_flag )
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
#endif

int white_name_list_get_value( char* buf, int *group_id, int* allow_id, int* not_allow_id, int *password )
{
	//withe name struct
	// 0 card_id
	// 1 group_id
	// 2 allow_id
	// 3 not_allow_id
	// 4 name
	// 5 password
	// 6 othe command
	int kc;
	char *ki[15];
	int skc;
	char *ski[15];

	kc = analysis_string_to_strings_by_decollator( buf, ", ", ki, 15 );
	if( kc < 7 ){
		error_message( "white_name format error", kc );
		return -1;
	}
	if( group_id ){
		*group_id = atoi( ki[1] );
		//_d_int( *group_id );
	}
	if( allow_id ){
		*allow_id = atoi( ki[2] );
		//_d_int( *allow_id );
	}
	if( not_allow_id ){
		*not_allow_id = atoi( ki[3] );
		//_d_int( *not_allow_id );
	}
	if( password ){
		*password = atoi( ki[5] );
		//_d_int( *password );
	}
	strcpy( g_card_man, ki[4] );
	strcpy( g_card_group, ki[5] );

	if( kc >= 7 ){
		int i;
		for( i=6; i<kc; i++ ){
			if( ki[i][0] == 'w' ){
				// many card operate.
				skc = analysis_string_to_strings_by_decollator( ki[i], ", ", ski, 15 );
				if( skc >= 2 && (skc-1) < _d_card_group_max_count ){
					int j;
					g_card_group_count = skc-1;
					for( j=0; j<g_card_group_count; j++ ){
						g_card_group_list[j] = atoi( ski[j+1] );
						g_card_group_flag[j] = 0;
					}
				}
			}
			else if( ki[i][0] == '0' ){
			}
		}
	}

	return 0;
}
int cam_read_group_id_from_white_name_record( DWORD card_id, char* buf, int buf_size )
{
	char card_id_str[15];
	int index;
	//int no;
	s_sprintf( card_id_str, "%u", card_id );
	//_d_str( card_id_str );
#if 0
	if( txt_file_search_line( "white_name.txt", card_id_str, buf, buf_size ) == 1 ){
		return 1;
	}
#else
	index = rand_file_find( g_white_name_rand_file_pos, card_id, 0, strlen(card_id), 1/*memcmp*/ );
	if( index >= 0 ){
		white_name_get( index, buf, buf_size );
		return 1;
	}
#endif
	return -1;
}
int g_white_name_card_offset=0;
int white_name_check_card( u32 card_no )
{
	int index;
	index = rand_file_find( g_white_name_rand_file_pos, (char*)&card_no, g_white_name_card_offset, 4, 1/*memcmp*/ );
	return index;
}



int white_name_get( u32 id, char* buf, int max_buf_size )
{
	return rand_file_get( g_white_name_rand_file_pos, id, buf, max_buf_size );
}
int white_name_set( u32 id, char* buf, int max_buf_size )
{
	rand_file_set( g_white_name_rand_file_pos, id, buf, max_buf_size );
	return 0;
}

int white_name_init()
{
	char buf[12];
	u32 cs;
	if( g_white_name_rand_file_pos >= 0 ){
		rand_file_close( g_white_name_rand_file_pos );
		g_white_name_rand_file_pos = 0;
	}
	if( property_get( "white_name_file", g_white_name_file_name ) < 0 ){
		strcpy( g_white_name_file_name, _d_white_name_file_name );
	}
	g_white_name_rand_file_pos = rand_file_open( g_white_name_file_name );
	if( g_white_name_rand_file_pos < 0 ){
		return -1;
		// not create, because this file sync from PC.
		if( rand_file_create( g_white_name_file_name, _d_max_white_name_count,_d_max_white_name_size ) < 0 ){
			//while(1);
			return -1;
		}
		g_white_name_rand_file_pos = rand_file_open( g_white_name_file_name );
	}
	cs = txt_file_check_sum( g_white_name_file_name );
	sprintf( buf, "%d", cs );
	property_set( "white_name_cs", buf );
	return cs;
}
char* get_card_man()
{
	return g_card_man;
}
char* get_card_group()
{
	return g_card_group;	
}


