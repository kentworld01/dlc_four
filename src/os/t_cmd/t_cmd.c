


#include "os/t_cmd/t_cmd.h"
#include <string.h>


char g_t_cmd_result_buffer[ 128 ];
char* t_cmd_result_buffer = g_t_cmd_result_buffer;



//typedef unsigned (*t_cmd_function)( char** param, char* rel_buf, int max_rel_buf_size );

unsigned t_time_set( char **ki, char *rel_buf, int max_rel_buf_size )
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
unsigned t_time_get( char **ki, char *rel_buf, int max_rel_buf_size )
{
		DATETIME dt;
		RTC_GetTime( &dt );
		s_sprintf( rel_buf, "%04u-%02u-%02u_%02u:%02u:%02u",
				dt.year, dt.month, dt.day,
				dt.hour, dt.minute, dt.second);
		_d_str( rel_buf );
		return 1;
}
unsigned t_fdelete( char **ki, char *rel_buf, int max_rel_buf_size )
{
	int rel = fdelete( ki[2] );
	if( rel > 0 ){
		sprintf( rel_buf, "fdelete file error" );
		return 0;
	}
	sprintf( rel_buf, "ok" );
	return 1;
}


_d_t_cmd_item g_t_cmd_item[]={
	{	"time_get",		0,		0,		0,		t_time_get	},
	{	"time_set",		0,		0,		0,		t_time_set	},
	{	"fdelete",		0,		0,		0,		t_fdelete	},
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


