

#include "os.h"
#include "gui4/gui_if.h"
#include <string.h>



char* get_card_man(void);
char* get_card_group(void);




char *_s_date_time2[] = {
"y"
,"m"
,"d"
,"h"
,"m"
,"s"
,"w"
,"7"
,"1"
,"2"
,"3"
,"4"
,"5"
,"6"
};
char *_s_date_time[] = {
"年"
,"月"
,"日"
,"时"
,"分"
,"秒"
,"星期"
,"日"
,"一"
,"二"
,"三"
,"四"
,"五"
,"六"
};


int get_system_value_time( char* param, char *buf, int max_buf_size )
{
	DATETIME dt;
	RTC_GetTime( &dt );
	s_sprintf( (buf), "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
	return 1;
}
int get_system_value_date_time( char* param, char *buf, int max_buf_size )
{
	DATETIME dt;
	RTC_GetTime( &dt );
	s_sprintf( (buf), "%02d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second );
	return 1;
}
int get_system_value_date2( char* param, char *buf, int max_buf_size )
{
	DATETIME dt;
	RTC_GetTime( &dt );
	s_sprintf( (buf), "%04d%s%02d%s%02d%s %s%s", dt.year, _s_date_time[0], 
			dt.month, _s_date_time[1], dt.day, _s_date_time[2], 
			_s_date_time[6], _s_date_time[ 7+date_to_week_day(&dt) ] );
	return 1;
}
int get_system_value_card_no( char* param, char *buf, int max_buf_size )
{
	s_sprintf( (buf), "%d", g_deal_card_no );
	return 1;
}
int get_system_value_card_man( char* param, char *buf, int max_buf_size )
{
	char *po;
	po = get_card_man();
	s_sprintf( (buf), "%s", po );
	return 1;
}
int get_system_value_card_group( char* param, char *buf, int max_buf_size )
{
	char *po;
	po = get_card_group();
	s_sprintf( (buf), "%s", po );
	return 1;
}


typedef int (*get_system_value)( char* param, char *buf, int max_buf_size );
typedef struct{
	char* name;
	get_system_value fun;
}_s_system_value_list;

_s_system_value_list g_system_value_list[]={
	{	"$DATETIME$",	get_system_value_date_time	},
	{	"$DATE2$",	get_system_value_date2	},
	{	"$TIME$",	get_system_value_time	},
	{	"$CARDNO$",	get_system_value_card_no	},
	{	"$CARDMAN$",	get_system_value_card_man	},
	{	"$CARDGROUP$",	get_system_value_card_group	},
	{		0,				0	}
};

#define _d_system_value_list_count (sizeof( g_system_value_list )/sizeof( g_system_value_list[0] ))

int system_string_value( char *key, char *buf, int max_buf_size )
{
	int i;
	for( i=0; i<_d_system_value_list_count; i++ ){
		// maybe can smart for show the long string inclue the key string .
		_s_system_value_list *item = &g_system_value_list[i];
		if( strcmp( key, item->name ) == 0 ){
			if( item->fun( 0, buf, max_buf_size ) == 1 )
				return 0;
		}
	}
	if( key[0] == '$' )
		strcpy( buf, "no this key" );
	else
		strcpy( buf, key );
	return 0;
}

