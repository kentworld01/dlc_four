
#include "os.h"
#include "gui4/gui_if.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int g_device_no = 888;
char g_card_man[32];
char g_card_group[32];


#define _d_card_group_max_count 1
int g_card_group_list[_d_card_group_max_count];
int g_card_group_flag[_d_card_group_max_count];
int g_card_group_count = 0;



void* df_create( char*name, int flag, char* value )
{
	FILE *fp;
	fp = fopen( name, "wb" );
	return (void*)fp;
}

void* df_open( char* fn, int flag )
{
	FILE *fp;
	fp = fopen( fn, "rb" );
	return (void*)fp;
}
int df_read( void* h, char* buf, int size )
{
	return fread( buf, 1, size, (FILE*)h );
}
int df_write( void* h, char* buf, int size )
{
	return fwrite( buf, 1, size, (FILE*)h );
}
int df_close( void* h )
{
	return fclose( h );
}
int df_delete( char* name, int flag )
{
	return fdelete( name );
}
int df_seek( void* h, int offset, int pos )
{
	return fseek( (FILE*)h, offset, pos );
}
int df_get_file_size( char* name )
{
	void *h;
	int size;
	h = df_open( name , 0 );
	if( h == 0 )
		return 0;
	size = df_seek( h, 0, 2 );
	size = ftell( h );
	if( size <= 0 )
		return 0;
	df_close( h );
	return size+1;
}
int df_read_line( void* h, char* buf, int max_size )
{
	#define _d_try_read_size 50
	int base = 0;
	int pos;
	int size;
	char *tstr = "\n";
	int try_size = _d_try_read_size;
	if( try_size > max_size )
		try_size = max_size;
	while( 1 ){
		size = df_read( h, &buf[base], try_size );
		if( size <= 0 )		{
			if( base > 0 ){ 
				buf[base] = 0;
				return base; 
			}
			break;
		}
		base += size;
#if 0
		pos = s_memmem(buf, base, tstr, 1 );
		if( pos < 0 )
			continue;
#else
		//_d_buf( buf, base );
		for( pos = 0; pos < base; pos ++ ){
			if( buf[pos] == '\n' )
				break;
			}
		//_d_int( pos );
		if( pos == base )
			continue;
#endif
		// get the pos, and fix the offset.
		if( pos > base )
			while( 1 ) _d_line();
		df_seek( h, pos-base+1, 1 );
		buf[pos] = 0;
		return pos;
		}
	return -1;
}





// here use Liu eng. 's cam method.
int today_is_holiday( int group_id, int year, int month, int day )
{
	char buf[20];
	char date_str[20];
	s_sprintf( date_str, "%02d%04d%02d%02d", group_id,year, month, day );
	if( txt_file_search_line( "holiday.txt", date_str, buf, sizeof( buf ) ) == 1 )
		return 1;
	return 0;
}

int cam_read_group_id_from_white_name_record( DWORD card_id, char* buf, int buf_size )
{
	char card_id_str[15];
	int no;
	s_sprintf( card_id_str, "%u", card_id );
	//_d_str( card_id_str );
	if( txt_file_search_line( "white_name.txt", card_id_str, buf, buf_size ) == 1 ){
		return 1;
	}
	return -1;
}
int cam_read_time_area_from_time_area_file( int time_area_no, DWORD* begin_time, DWORD* end_time )
{
	char key[16];
	char buf[32];
	int buf_size;
	int hour;
	int minute;
	DATETIME dt;
	
	buf_size = sizeof( buf );
	//s_sprintf( key, "%02d", time_area_no );
	RTC_GetTime( &dt );
	key[2] = 0;
	if( txt_file_read_line( "time_area.txt", time_area_no-1, buf, buf_size ) >= 10 ){
		_d_str( buf );
		key[0] = buf[2];
		key[1] = buf[3];
		hour = atoi( key );
		key[0] = buf[4];
		key[1] = buf[5];
		minute = atoi( key );
		if( begin_time ){
			dt.hour = hour;
			dt.minute = minute;
			dt.second = 0;
			*begin_time = DateTimeToRelatvie( &dt );
		}
		key[0] = buf[6];
		key[1] = buf[7];
		hour = atoi( key );
		key[0] = buf[8];
		key[1] = buf[9];
		minute = atoi( key );
		if( end_time ){
			dt.hour = hour;
			dt.minute = minute;
			dt.second = 0;
			*end_time = DateTimeToRelatvie( &dt );
		}
		return 1;
	}
	return -1;
}

int cam_get_no_from_time_area_list( char* list, int no )
{
	int len;
	char buf[3];
	len = strlen( list );
	if( no * 2 + 3 + 1>= len )
		return -1;
	buf[0] = list[ 3+no*2 ];
	buf[1] = list[ 3+no*2+1 ];
	buf[2] = 0;
	return atoi( buf );
}

int cam_read_time_area_list_from_week_table( int group_id, DWORD time, char* buf, int buf_size )
{
	DATETIME dt;
	char key[16];
	char *week_table_file_name;
	// use normal week plan
	week_table_file_name = "week_table.txt";
	RelatvieToDateTime( &dt, time );
	if( today_is_holiday( group_id, dt.year, dt.month, dt.day ) == 1 ){
		_d_str( "use holiday week plan" );
		week_table_file_name = "holiday_week_table.txt";
	}

	_d_str( week_table_file_name );
	dt.weekday = date_to_week_day( &dt );
	s_sprintf( key, "%02d%d", group_id, dt.weekday );
	_d_str( key );
	if( txt_file_search_line( week_table_file_name, key, buf, buf_size ) == 1 ){
		return 1;
	}
	return -1;
}

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

int card_group_operate_key_deal()
{
#if 0	
	int key = 0;
	extern int g_key_val;
	key = g_key_val;
	g_key_val = 0;
	if( key != 0 ) return key;
	basic_idle( &main_basic );
	socket_interrupt();
	//if( a0106_has_pool_card() ){
	//	return 'a';
	//}
#endif
	return 0;
}

int card_group_operate()
{
	int i;
	DWORD card_id;
	if( g_card_group_count <= 0 )
		return 0;
#if 0
	while( 1 ){
		// check end?
		for( i=0; i<g_card_group_count; i++ ){
			if( 	g_card_group_flag[i] != 1 )
				break;
		}
		if( i==g_card_group_count )
			return 0;
		// get card_id
		if (rc500_poll_card(&card_id, 0, 0)){
			// change the flag
			for( i=0; i<g_card_group_count; i++ ){
				if( card_id == g_card_group_list[i] )
					g_card_group_flag[i] = 1;
			}
		}
		// do other event.
		if( card_group_operate_key_deal() > 0 ){
			// has key event
			// or other event exit the card_group operate.
			return -1;
		}
	}
	// all card is reader. 
	// sucess return .
#endif
	return 0;
}

int sub_do_cam( int device_no, DWORD card_id, DWORD time, char* show_buf, int max_show_buf )
{
	char buf[64];
	char time_area_list[32];
	int group_id;
	int allow_id;
	int not_allow_id;
	char white_name_content[32];
	int no, time_area_no;
	DWORD begin_time, end_time;
	int password = 0;
	
	//cam_read_white_name_record( card_id, buf );
	_d_u( card_id );
	if( cam_read_group_id_from_white_name_record( card_id, buf, sizeof(buf) ) < 0 ){
		error_message( "can not search this card_id", card_id );
		goto do_cam_error_exit;
	}
	//old design: ְԱ���ţ�ְԱ�������������ţ�����û����룬��Ǳ�����
	// card_id,group_id,show_mode_id,data....
	//_d_str( buf );
	strcpy( white_name_content, buf );
	_d_str( white_name_content );
	if( white_name_list_get_value( buf, &group_id, &allow_id, &not_allow_id, &password ) < 0 )
		goto not_allow_enter;
		//goto do_cam_error_exit;
	
	//_d_str( white_name_content );
	if( cam_read_time_area_list_from_week_table( group_id, time, time_area_list, sizeof(time_area_list) ) < 0 ){
		error_message( "read time_area_list error", group_id );
		goto not_allow_enter;
		//goto do_cam_error_exit;
	}
	// loop check the time_area
	_d_str( time_area_list );
	no = 0;
	while( 1 ){
		_d_int( no );
		time_area_no = cam_get_no_from_time_area_list( time_area_list, no );
		if( time_area_no <= 0 )
			break;
		_d_int( time_area_no );
		//get time area, and check ok.
		if( cam_read_time_area_from_time_area_file( time_area_no, &begin_time, &end_time ) < 0 ){
			error_message( "read time_area from time_area_file error", time_area_no );
			break;
		}
		_d_int( begin_time );
		_d_int( end_time );
		_d_int( time );
		if( begin_time <= time && time <= end_time ){
			if( show_buf ){
				// check the password.
#if 0
				if( password ){
					buf[0] = 0;
					if( main_basic_gui_get_string( buf, sizeof(buf) ) < 0 ){
						// or hint password input problem?
						goto not_allow_enter;
					}
					if( password != atoi( buf ) ){
						// or hint password error?
						goto not_allow_enter;
					}
				}
#endif
				if( card_group_operate() < 0 )
					goto not_allow_enter;

#if 0
				// the password is ok.
				// deal the show info.
				if( txt_file_read_line( "/c/show", allow_id, buf, sizeof(buf) ) > 0 ){
					//_d_str( buf );
					cam_show( buf, white_name_content, show_buf, max_show_buf );
				}
				else{
					s_sprintf( show_buf, "%u, Allowed", card_id );
				}
#endif
			}
			return allow_id;
		}
		no ++;
	}
	_d_line();
not_allow_enter:
	_d_line();
#if 0	
	if( show_buf ){
		_d_line();
		// deal the show info.
		if( txt_file_read_line( "/c/show", not_allow_id, buf, sizeof(buf) ) > 0 ){
			//_d_str( buf );
			cam_show( buf, white_name_content, show_buf, max_show_buf );
		}
		else{
			s_sprintf( show_buf, "%u, Not Allowed", card_id );
		}
		return -1;
	}
#endif
	return -1;
	
do_cam_error_exit: 	
	_d_line();
#if 0	
	if( show_buf ){
		_d_line();
		if( txt_file_read_line( "/c/show", 0, buf, sizeof(buf) ) > 0 ){
			char *po = show_buf;
			char *buf_po;
			int len ;
			//_d_str( buf );
			white_name_content[0] = 0;
			s_sprintf( po, "%u", card_id );
			len = strlen( po );
			po += len;
			buf_po = buf;
			_d_hex( buf_po[0] );
			_d_hex( buf_po[1] );
			_d_hex( buf_po[2] );
			_d_hex( buf_po[3] );
			if( (unsigned char)buf_po[0] == (unsigned char)0xef && (unsigned char)buf_po[1] == (unsigned char)0xbb && (unsigned char)buf_po[2] == (unsigned char)0xbf )
				buf_po += 3;
			cam_show( buf_po, white_name_content, po, max_show_buf-len );
		}
		else{
			s_sprintf( show_buf, "%u, Error", card_id );
		}
	}
#endif	
	//return -1;
	// illegal card
	return -2;

}

int df_file_exist( char* fn )
{
	FILE *fp;
	fp = fopen( fn, "r" );
	if( fp == 0 )
		return 0;
	fclose( fp );
	return 1;
}
int do_cam_check_file()
{
	char *fn[] = { "white_name.txt","time_area.txt","week_table.txt","holiday_week_table.txt","holiday.txt" };
	#define _d_fn_count (sizeof( fn ) /sizeof(char*))
	char *lpfn[] = { "white_name.txt.lp","time_area.txt.lp","week_table.txt.lp","holiday_week_table.txt.lp","holiday.txt.lp" };
	#define _d_lpfn_count (sizeof( lpfn ) /sizeof(char*))
	int i;
	for( i=0; i<_d_fn_count; i++ ){
		if( df_file_exist( fn[i] ) == 0 ){
			return 0;
		}
	}
	for( i=0; i<_d_lpfn_count; i++ ){
		if( df_file_exist( lpfn[i] ) == 0 ){
			txt_file_create_line_pointer( fn[i], lpfn[i] );
		}
	}
	return 1;
}

int do_cam()
{
	char show[30];
	extern unsigned g_deal_card_no;
	if( do_cam_check_file() <= 0 )
		return -1;
	return sub_do_cam( g_device_no, g_deal_card_no, RTC_Get(), show, sizeof(show) );
}

int do_cam2()
{
	return -1;
}


char* get_card_man()
{
	return g_card_man;
}
char* get_card_group()
{
	return g_card_group;	
}

int send_record()
{
	return 0;
}
int save_record()
{
	return 0;
}

//char _s_main_page[] = "outtextxy:30,30,欢迎光临\nouttextxy:0,50,$DATETIME$";
char _s_main_page[] = "outtextxy:0,0,DLC\nline:0,13,128,13\nouttextxy:40,20,Welcome\nouttextxy:35,34,$TIME$\nouttextxy:0,50,$DATE2$";
char _s_illegal_card_page[] = "outtextxy:0,0,DLC\nline:0,13,128,13\nouttextxy:30,25,Illegal Card\nouttextxy:0,50,$DATETIME$";
char _s_forbid_enter_page[] = "outtextxy:0,0,DLC\nline:0,13,128,13\nouttextxy:30,25,Forbid Enter\nouttextxy:0,50,$DATETIME$";
char _s_allow_enter_page[] = "outtextxy:0,0,DLC\nline:0,13,128,13\nouttextxy:30,25,Allow Enter\nouttextxy:0,50,$DATETIME$";
char g_main_page[ 128 ] = {0};
int g_page_keep_second = 0;

int read_file( char* fn, char *buf, int max_buf_size)
{
	FILE *f;
	if (finit() != 0) 
		return;
	f = fopen ( fn , "r" );
	if( f == NULL )
		return 0;
	fread (buf,1,max_buf_size,f);
	fclose (f);
	return 1;
}

int show_main_page()
{
	if( g_main_page[0] )
		gui_show_page( g_main_page );
	return 0;
}
int set_main_page()
{
	if( read_file( "main_page.txt", g_main_page, sizeof( g_main_page ) ) <= 0 )
		strcpy( g_main_page , _s_main_page );
	return 0;
}
int set_illegal_card_page()
{
	if( read_file( "illegal_card_page.txt", g_main_page, sizeof( g_main_page ) ) <= 0 )
		strcpy( g_main_page , _s_illegal_card_page );
	return 0;
}
int set_forbid_enter_page()
{
	if( read_file( "forbid_enter_page.txt", g_main_page, sizeof( g_main_page ) ) <= 0 )
		strcpy( g_main_page , _s_forbid_enter_page );
	return 0;
}
int set_allow_enter_page()
{
	if( read_file( "allow_enter_page.txt", g_main_page, sizeof( g_main_page ) ) <= 0 )
		strcpy( g_main_page , _s_allow_enter_page );
	return 0;
}
int check_card_action()
{
	if( g_card_no != 0 )
		return 1;
	return 0;
}
int set_page_keep_second( int s )
{
	g_page_keep_second = s;
	return 0;
}
int timer_process()
{
	static unsigned second = 0;
	unsigned val = RTC_Get();
	if( second != val ){
		if( g_page_keep_second > 0 ){
			g_page_keep_second -= val - second;
			if( g_page_keep_second <= 0 ){
				set_main_page();
				g_page_keep_second = 0;
			}
		}
		second = val;
		show_main_page();
	}
	return 0;
}


int access_init()
{
	set_main_page();
	show_main_page();
	return 0;
}

int access_loop()
{
	int rel;
	if( check_card_action() > 0 ){
		g_deal_card_no = g_card_no;
		g_card_no = 0;

		rel = do_cam();
		if( rel <= -2 ){
			// invailed card
			set_illegal_card_page();
			set_page_keep_second( 5 );
		}
		else if( rel == -1 ){
			// disable enter
			set_forbid_enter_page();
			set_page_keep_second( 5 );
		}
		else if( rel == 1 ){
			set_allow_enter_page();
			set_page_keep_second( 5 );
		}
		send_record();
		save_record();
		show_main_page();
	}
	timer_process();
}