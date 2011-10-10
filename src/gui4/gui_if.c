
#include "gui4/gui_if.h"
#include <string.h>



int get_lines( char *page, char* buf, int max_buf_size )
{
	char *po;
	int pos;
	int size;
	po = strchr( page, '\n' );
	if( po == 0 ){
		size = strlen( page );
	}
	else{
		size = po-page;
		if( size > max_buf_size )
			size = max_buf_size;
	}
	memcpy( buf, page, size );
	buf[size] = 0;
	s_chop( buf );
	return size;
}

int gui_show_page( char* page )
{
	char line[128];
	int size;
	int kc;
	char *ki[ 30 ];
	int skc;
	char *ski[ 10 ];
	int x,y,xx,yy;
	WCHAR wstr[30];
	
	//LCD_CLS( 0 );
	_LCD_FBClear(0);
	do{
		size = get_lines( page, line, sizeof( line ) );
		page += size;
		if( *page == '\n' )
			page += 1;
		if( size == 0 )
			break;
		kc = analysis_string_to_strings_by_decollator( line, ":", ki, 30 );
		if( strcmp( ki[0], "outtextxy" ) == 0 ){
			skc = analysis_string_to_strings_by_decollator( ki[1], ",", ski, 10 );
			x = atoi( ski[0] );
			y = atoi( ski[1] );
			{
				// change system string value.
				system_string_value( ski[2], (char*)wstr, sizeof( wstr ) );
				strcpy( line, (char*)wstr );
			}
			string_utf8_to_unicode( line, wstr, sizeof(wstr)/2 );
			Text_DisplayWString( x,y,127-x,63-y, wstr );
		}
		else if( strcmp( ki[0], "line" ) == 0 ){
			skc = analysis_string_to_strings_by_decollator( ki[1], ",", ski, 10 );
			x = atoi( ski[0] );
			y = atoi( ski[1] );
			xx = atoi( ski[2] );
			yy = atoi( ski[3] );
			Graph_Line( x,y,xx,yy );
		}
	}while( 1 );
	LCD_Refresh();
	return 0;
}

int show_logo()
{
	extern const unsigned char bmp_dlc[];
	MONOBMP mb;
	MonoBmp_Load( &mb, (BYTE*)&bmp_dlc );
	Graph_PutBitmap(0,0,128,64, &mb );
	LCD_Refresh();
	return 0;
}

