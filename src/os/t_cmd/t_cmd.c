


#include "os.h"
#include "os/t_cmd/t_cmd.h"
#include <string.h>


char g_t_cmd_result_buffer[ 128 ];
char* t_cmd_result_buffer = g_t_cmd_result_buffer;

#define t_func(x) t_##x


//typedef unsigned (*t_cmd_function)( char** param, char* rel_buf, int max_rel_buf_size );

unsigned t_func(sim_key)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	gui_set_key( ki[2][0] );
	sprintf( rel_buf, "ok" );
	return 0;
}
unsigned t_func(white_name_find_by_card_no)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	index = rand_file_find( g_white_name_rand_file_pos, ki[2], 0, strlen(ki[2]), 1 /*memcmp*/ );
	sprintf( rel_buf, "%d", index );
	return 0;
}
unsigned t_func(white_name_find_by_card_man)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	index = rand_file_find( g_white_name_rand_file_pos, ki[2], 0/*man offset*/, strlen(ki[2]), 1 /*memcmp*/ );
	sprintf( rel_buf, "%d", index );
	return 0;
}
unsigned t_func(white_name_find_by_card_group)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	index = rand_file_find( g_white_name_rand_file_pos, ki[2], 0/*group offset*/, strlen(ki[2]), 1 /*memcmp*/ );
	sprintf( rel_buf, "%d", index );
	return 0;
}
unsigned t_func(white_name_get)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	index = atoi( ki[2] );
	if( white_name_get( index, rel_buf, max_rel_buf_size ) < 0 )
		sprintf( rel_buf, "failed" );
	set_buf_0xff_end( rel_buf, max_rel_buf_size );
	return 0;
}
unsigned t_func(white_name_set)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	int len;
	index = atoi( ki[2] );
	len = atoi( ki[4] );
	white_name_set( index, ki[3], len );
	sprintf( rel_buf, "ok" );
	return 0;
}
unsigned t_func(white_name_change)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	u32 cs;
	property_set( "white_name_file", ki[2] );
	cs = white_name_init();
	sprintf( rel_buf, "%d", cs );
	return 0;
}
unsigned t_func(white_name_get_file_name)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	sprintf( rel_buf, g_white_name_file_name );
	return 0;
}
unsigned t_func(record_get)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	index = atoi( ki[2] );
	index = record_get( index, rel_buf, max_rel_buf_size );
	if( index == -1 )
		sprintf( rel_buf, "failed" );
	set_buf_0xff_end( rel_buf, max_rel_buf_size );
	return 0;
}
unsigned t_func(record_get_first_unsend)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	index = record_get_first_unsend( rel_buf, max_rel_buf_size );
	if( index == -1 )
		sprintf( rel_buf, "failed" );
	set_buf_0xff_end( rel_buf, max_rel_buf_size );
	return 0;
}
unsigned t_func(record_del_first_unsend )( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int index;
	index =  record_get_unsend_index();
	if( index == -1 ){
		sprintf( rel_buf, "failed" );
	}
	else{
		record_del_unsend_index( index );
		sprintf( rel_buf, "del unsend %d ok", index );
	}
	return 0;
}
unsigned t_func(property_get)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	property_get( ki[2], rel_buf );
	return 1;
}
unsigned t_func(property_set)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	property_set( ki[2], ki[3] );
	sprintf( rel_buf, "ok" );
	return 1;
}
unsigned t_func(flash_uffs_erase)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	u32 po;
	int n;
	po = 0;
	for( n=0; n< 3*1024*1024; n+= 64*1024 ){
		W25X_BlockErase( n );
	}
	sprintf( rel_buf, "ok" );
	return 1;
}
unsigned t_func(flash_chip_erase)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	W25X_ChipErase();
	sprintf( rel_buf, "ok" );
	return 1;
}
unsigned t_func(time_set)( char **ki, char *rel_buf, int max_rel_buf_size )
{
		DATETIME dt;
		int skc;
		char *ski[10];
		skc = analysis_string_to_strings_by_decollator( ki[2], "-:_ ", ski, 10 );
		if( skc < 6 ){
			sprintf( rel_buf, "symtx error: not enough item" );
			return 0;
		}
		dt.year = atoi( ski[0] );
		dt.month = atoi( ski[1] );
		dt.day = atoi( ski[2] );
		dt.hour = atoi( ski[3] );
		dt.minute = atoi( ski[4] );
		dt.second = atoi( ski[5] );
		RTC_SetTime( &dt );		
		s_sprintf( rel_buf, "ok\n%04u-%02u-%02u_%02u:%02u:%02u", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
		return 1;
}
unsigned t_func(time_get)( char **ki, char *rel_buf, int max_rel_buf_size )
{
		DATETIME dt;
		RTC_GetTime( &dt );
		s_sprintf( rel_buf, "%04u-%02u-%02u_%02u:%02u:%02u",
				dt.year, dt.month, dt.day,
				dt.hour, dt.minute, dt.second);
		_d_str( rel_buf );
		return 1;
}
unsigned t_func(fremove)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int rel = uffs_remove( ki[2] );
	if( rel > 0 ){
		sprintf( rel_buf, "fdelete file error" );
		return 0;
	}
	sprintf( rel_buf, "ok" );
	return 1;
}


unsigned t_func(show_menu_file)( char **ki, char *rel_buf, int max_rel_buf_size )
{
	char buf[64];
	sprintf( rel_buf, "show_menu_file,%s,%s", ki[2], ki[3] );
	access_command_set( rel_buf );
	sprintf( rel_buf, "ok" );
	return 1;
}


#define _d_t_cmd_item(x) { #x,0,0,0,t_##x }

_d_t_cmd_item g_t_cmd_item[]={
	// time
	_d_t_cmd_item( time_get ),
	_d_t_cmd_item( time_set ),
	// file
	_d_t_cmd_item( fremove ),
	// flash
	_d_t_cmd_item( flash_chip_erase ),
	_d_t_cmd_item( flash_uffs_erase ),
	// property
	_d_t_cmd_item( property_get ),
	_d_t_cmd_item( property_set ),
	// record
	_d_t_cmd_item( record_get ),
	_d_t_cmd_item( record_get_first_unsend ),
	_d_t_cmd_item( record_del_first_unsend ),
	// white_name
	_d_t_cmd_item( white_name_find_by_card_no ),
	_d_t_cmd_item( white_name_find_by_card_man ),
	_d_t_cmd_item( white_name_find_by_card_group ),
	_d_t_cmd_item( white_name_get ),
	_d_t_cmd_item( white_name_set ),
	_d_t_cmd_item( white_name_change ),
	_d_t_cmd_item( white_name_get_file_name ),
	// sim hardware
	_d_t_cmd_item( sim_key ),
	// system
	_d_t_cmd_item( show_menu_file ),

	// templete
	//_d_t_cmd_item( ),
	//{	0,				0,		0,		0,		0			}
};
#define _d_t_cmd_item_count (sizeof(g_t_cmd_item)/sizeof(g_t_cmd_item[0]))




int t_cmd_do( char* cmd, char* rel_buf, int max_rel_buf_size )
{
	int i;
	int kc;
	char *ki[ 30 ];
	int skc;
	char *ski[ 10 ];
	
	kc = analysis_string_to_strings_by_decollator( (char*)cmd, ",", ki, 30 );
	for( i=0; i<_d_t_cmd_item_count; i++ ){
		_d_t_cmd_item *item = &g_t_cmd_item[i];
		if( strcmp( ki[1], item->name ) == 0 ){
			return item->fun( ki, rel_buf, max_rel_buf_size );
		}
	}
	return -1;
}
int t_cmd_main( char* cmd )
{
	int rel;
	rel = t_cmd_do( cmd, g_t_cmd_result_buffer, sizeof( g_t_cmd_result_buffer ) );
	if( rel < 0 )
		sprintf( g_t_cmd_result_buffer, "no this command : %s", cmd );
	return 0;
}


