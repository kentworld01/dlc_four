
/*
 * all node operate, only read and write value, can not map the value.
 */

#include "node_prop.h"
#include "dir_file.h"
#include <string.h>
#include "mp_misc.h"

#ifndef _d_dir_file_modules

#else



#define _d_prop_file_name "/c/prop.dat"





int node_prop_change_system_value( char* name, char* value )
{
	
	typedef struct{
		char name[64];
		char value[64];
		//byte size;
		//byte flag;		// 0 is int, 1 is string
		//byte sync;
		//byte tmp;
	}_s_node_prop_item;
	
	// system value update.
	// ipaddr etc...
	int kc;
	char *ki[ 15 ];
	//char tmp_buf[  sizeof( npi->value ) ];
	char tmp_buf[  64 ];
	extern UCHAR  my_ipaddr[5];	
	extern UCHAR  Host_ipaddr [5];	
	extern int g_net_time_out;
	extern UINT  HOST_PORT; 
	extern int  g_machine_no; 
	extern int g_host_type;
	int val1, val2, val3, val4;
	_s_node_prop_item tmp_npi;
	_s_node_prop_item *npi;

	npi = &tmp_npi;
	strcpy( npi->name, name );
	strcpy( npi->value, value );
	_d_str( name );
	_d_str( value );
	
	// add the , explain the sys command string.
	if( strcmp( npi->name, "ipaddr" ) == 0 
			|| strcmp( npi->name, "ip" ) == 0 ){
		_d_str( (char*)npi->value );
		memcpy( tmp_buf, npi->value, sizeof( npi->value ) );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, ".", ki, 15 );
		if( kc != 4 )
			return 0;
		my_ipaddr[0] = atoi( ki[0] );
		my_ipaddr[1] = atoi( ki[1] );
		my_ipaddr[2] = atoi( ki[2] );
		my_ipaddr[3] = atoi( ki[3] );
		debug_show_ip_str(my_ipaddr);
		return 0;
	}
	else if( strcmp( npi->name, "hostip" ) == 0 ){
		_d_str( (char*)npi->value );
		memcpy( tmp_buf, npi->value, sizeof( npi->value ) );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, ".", ki, 15 );
		if( kc != 4 )
			return 0;
		Host_ipaddr[0] = atoi( ki[0] );
		Host_ipaddr[1] = atoi( ki[1] );
		Host_ipaddr[2] = atoi( ki[2] );
		Host_ipaddr[3] = atoi( ki[3] );
		debug_show_ip_str(Host_ipaddr);
		return 0;
	}
	else if( strcmp( npi->name, "hport" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		HOST_PORT = val1;
		_d_int( HOST_PORT );
		return 0;
	}
	else if( strcmp( npi->name, "webport" ) == 0 ){
		//extern UINT  HOST_PORT; 
		//_d_str( (char*)npi->value );
		//val1 = atoi( (char*)npi->value );
		//HOST_PORT = val1;
		//_d_int( HOST_PORT );
		return 0;
	}
	else if( strcmp( npi->name, "timeout" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		g_net_time_out = val1;
		_d_int( g_net_time_out );
		return 0;
	}
	else if( strcmp( npi->name, "no" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		g_machine_no = val1;
		_d_int( g_machine_no );
		return 0;
	}
	else if( strcmp( npi->name, "keyboard" ) == 0 ){
		_d_str( (char*)npi->value );
		_d_buf( (char*)npi->value, sizeof( npi->value ) );
		if( s_strcmp( (char*)npi->value, "disable" ) == 0 ){
			Key_EnableScan( 0 );
		}
		else{
			Key_EnableScan( 1 );
		}
		//val1 = atoi( (char*)npi->value );
		//g_machine_no = val1;
		//_d_int( g_machine_no );
		return 0;
	}
	else if( strcmp( npi->name, "hosttype" ) == 0 ){
		_d_str( (char*)npi->value );
		if( strcmp( (char*)npi->value, "tcmd" ) == 0 )
			g_host_type = 1;
		else if( strcmp( (char*)npi->value, "dlc" ) == 0 )
			g_host_type = 2;
		else
			g_host_type = 0;
		
		//val1 = atoi( (char*)npi->value );
		//g_machine_no = val1;
		_d_int( g_host_type );
		return 0;
	}
	return -1;
}



int prop_read_record( char *key, char *value, int max_value_size )
{
	char *file = _d_prop_file_name;
	void *h;
	int kc;
	char *ki[15];
	int rel = -1;
	char buf[100];

	h = df_open( file, 0 );
	if( h == 0 )
		return -1;
	while( 1 ){
		if( df_read_line( h, buf, sizeof(buf) ) == -1 )
			break;
		//_d_str( buf );
		if( (unsigned char)buf[0] == 0xff )
			break;
		kc = analysis_string_to_strings_by_decollator( buf, "=", ki, 15 );
		//_d_int( kc );
		//_d_str( ki[0] );
		//_d_str( key );
		if( kc != 2 )
			break;
		//_d_line();
		if( s_strcmp( ki[0], &key[3] ) == 0 ){
			//int len;
			strncpy( value, &ki[1][2], max_value_size );
			rel = 0;
			s_chop( value );
			//len = strlen( value );
			//_d_str( value );
			//_d_int( strlen(value) );
			//_d_hex( value[len-1] );
			//_d_hex( value[len-2] );
			//_d_hex( value[len-3] );
			break;
		}
	}
	df_close( h );
	return rel;
}
int prop_write_record( char *key, char *value, int max_value_size )
{
	char *file = _d_prop_file_name;
	char *tmp_file = "/c/tmp_prop.dat";
	void *h, *th;
	int kc;
	char *ki[15];
	char buf[100];
	char tmp_buf[100];
	int rel = -1;
	int new_key = 1;


	_d_str( key );
	_d_str( value );

	// not use the prefix "/p/"
	key = key + 3;
	
	th = df_create( tmp_file, 0, 0 );
	if( th == 0 )
		return -1;
	h = df_open( file, 0 );
	if( h == 0 ){
		df_close( th );
		df_delete( tmp_file,0 );
		return -1;
	}
	
	//_d_str( key );
	while( 1 ){
		int tmp_rel;
		WDT_Clear();
		tmp_rel = df_read_line( h, buf, sizeof(buf) );
		//_d_int( tmp_rel );
		//_d_str( buf );
		if( tmp_rel == -1 )
			break;
		if( (unsigned char)buf[0] == 0xff )
			break;
		strcpy( tmp_buf, buf );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, "=", ki, 15 );
		if( kc != 2 )
			break;
		//_d_str( ki[0] );
		//_d_line();
		if( s_strcmp( ki[0], key ) == 0 ){
			//_d_line();
			//_d_str( key );
			//_d_str( value );
			//_d_str( buf );
			//strncpy( value, &ki[1][1], max_value_size );
			s_sprintf( buf, "%s=1,%s\n", key, value );
			//_d_str( buf );
			rel = 0;
			new_key = 0;
			//key[0] = 0;
			//_d_line();
			//goto prop_write_record_end;
			//break;
		}
		else{
			//_d_line();
			strcat( buf, "\n" );
			//_d_line();
		}
		//_d_line();
		df_write( th, buf, strlen( buf ) );
		//_d_line();
	}
	//_d_line();
	if( new_key == 1 ){
		// new key, not write yet.
		s_sprintf( buf, "%s=1,%s\n", key, value );
		df_write( th, buf, strlen( buf ) );
		//_d_str( buf );
	}
prop_write_record_end:	
	df_close( h );
	df_close( th );
	df_delete( file, 0 );
	df_rename( tmp_file, file );
	node_prop_change_system_value( key, value );

	//while( 1 )	WDT_Clear();
	return rel;
}




int prop_get_value( char* file, char* value, int max_size )
{
	return prop_read_record( file, value, max_size );
}
int prop_set_value( char* file, char* value, int max_size )
{
	int rel;
	WDT_Clear();
	rel = prop_write_record( file, value, max_size );
	// for debug
#if 0	
	_d_line();
	WDT_Clear();
	prop_read_record( file, value, max_size );
	WDT_Clear();
	_d_line();
#endif
	return rel;
}


int node_prop_change_all_system_value()
{
	char *file = _d_prop_file_name;
	void *h;
	int kc;
	char *ki[15];
	int rel = -1;
	char buf[100];

	h = df_open( file, 0 );
	if( h == 0 )
		return -1;
	while( 1 ){
		if( df_read_line( h, buf, sizeof(buf) ) == -1 )
			break;
		if( (unsigned char)buf[0] == 0xff )
			break;
		kc = analysis_string_to_strings_by_decollator( buf, "=", ki, 15 );
		if( kc != 2 )
			break;
		s_chop( ki[1] );
		node_prop_change_system_value( ki[0], &ki[1][2] );
	}
	df_close( h );
	return rel;
}

int prop_init()
{
	//prop_file_format_check();
	node_prop_change_all_system_value();
	return 0;
}
int node_prop_init()
{
	return prop_init();
}
int node_prop_get_value( char* file, char* value, int max_size )
{
	return prop_get_value( file, value, max_size );
}
int node_prop_set_value( char* file, char* value, int max_size )
{
	return prop_set_value( file, value, max_size );
}


#endif





//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================






#ifdef 0 //_d_dir_file_modules_old_version

// this design use rand access struct 
// can not sync the user edit method. so donot useful.





// design text
// simlate the uml flow write down the design method.


/*



system user case
	node_prop_set_value
	node_prop_get_value


misc function
	node_prop_file_format_check


node_prop_class:
	prop_init()
	prop_file_format_check()
	prop_get_value( char* file, char* value, int max_size )
		node_prop_get_value( char* file, char* value, int max_size )
	prop_set_value( char* file, char* value, int max_size )
		node_prop_set_value( char* file, char* value, int max_size )


	

// file format

record length 128 byte.
file:
record[0] = "\nn:name[64]s:string[64]"
	record[0][0-1]='\r\n'
	record[0][2]='n'			// name flag
	record[0][3]=':'
	record[0][4-68]=name
	record[0][69]='s'			// type flag
	record[0][70]=':'
	record[0][71-135]=string	// record content

if( 	
	record[0][0] != '\n' || 
	
	|| record[0][2] != 'n'
	|| record[0][3] != ':'
	|| record[0][4] < 'A'
	|| record[0][4] > 'z'
	
	|| record[0][69] != 's'	// or other type
	|| record[0][70] != ':'
	|| record[0][71] < 'A'
	|| record[0][71] > 'z'

	}
		// enter reformat the file.
	
	
*/


typedef struct{
	char cr_flag[2];
	char name_flag[2];
	char name[64];
	char content_flag[2];
	char content[64];
} _s_prop_record;



#define _d_prop_file_name "/c/prop.dat"




int node_prop_change_system_value( char* name, char* value )
{
	
	typedef struct{
		char name[64];
		char value[64];
		//byte size;
		//byte flag;		// 0 is int, 1 is string
		//byte sync;
		//byte tmp;
	}_s_node_prop_item;
	
	// system value update.
	// ipaddr etc...
	int kc;
	char *ki[ 15 ];
	//char tmp_buf[  sizeof( npi->value ) ];
	char tmp_buf[  64 ];
	extern UCHAR  my_ipaddr[5];	
	extern UCHAR  Host_ipaddr [5];	
	extern int g_net_time_out;
	extern UINT  HOST_PORT; 
	extern int  g_machine_no; 
	extern int g_host_type;
	int val1, val2, val3, val4;
	_s_node_prop_item tmp_npi;
	_s_node_prop_item *npi;

	npi = &tmp_npi;
	strcpy( npi->name, name );
	strcpy( npi->value, value );
	
	// add the , explain the sys command string.
	if( strcmp( npi->name, "ipaddr" ) == 0 
			|| strcmp( npi->name, "ip" ) == 0 ){
		_d_str( (char*)npi->value );
		memcpy( tmp_buf, npi->value, sizeof( npi->value ) );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, ".", ki, 15 );
		if( kc != 4 )
			return 0;
		my_ipaddr[0] = atoi( ki[0] );
		my_ipaddr[1] = atoi( ki[1] );
		my_ipaddr[2] = atoi( ki[2] );
		my_ipaddr[3] = atoi( ki[3] );
		return 0;
	}
	else if( strcmp( npi->name, "hostip" ) == 0 ){
		_d_str( (char*)npi->value );
		memcpy( tmp_buf, npi->value, sizeof( npi->value ) );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, ".", ki, 15 );
		if( kc != 4 )
			return 0;
		Host_ipaddr[0] = atoi( ki[0] );
		Host_ipaddr[1] = atoi( ki[1] );
		Host_ipaddr[2] = atoi( ki[2] );
		Host_ipaddr[3] = atoi( ki[3] );
		return 0;
	}
	else if( strcmp( npi->name, "hport" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		HOST_PORT = val1;
		_d_int( HOST_PORT );
		return 0;
	}
	else if( strcmp( npi->name, "webport" ) == 0 ){
		//extern UINT  HOST_PORT; 
		//_d_str( (char*)npi->value );
		//val1 = atoi( (char*)npi->value );
		//HOST_PORT = val1;
		//_d_int( HOST_PORT );
		return 0;
	}
	else if( strcmp( npi->name, "timeout" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		g_net_time_out = val1;
		_d_int( g_net_time_out );
		return 0;
	}
	else if( strcmp( npi->name, "no" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		g_machine_no = val1;
		_d_int( g_machine_no );
		return 0;
	}
	else if( strcmp( npi->name, "keyboard" ) == 0 ){
		_d_str( (char*)npi->value );
		_d_buf( (char*)npi->value, sizeof( npi->value ) );
		if( s_strcmp( (char*)npi->value, "disable" ) == 0 ){
			Key_EnableScan( 0 );
		}
		else{
			Key_EnableScan( 1 );
		}
		//val1 = atoi( (char*)npi->value );
		//g_machine_no = val1;
		//_d_int( g_machine_no );
		return 0;
	}
	else if( strcmp( npi->name, "hosttype" ) == 0 ){
		_d_str( (char*)npi->value );
		if( strcmp( (char*)npi->value, "tcmd" ) == 0 )
			g_host_type = 1;
		else if( strcmp( (char*)npi->value, "dlc" ) == 0 )
			g_host_type = 2;
		else
			g_host_type = 0;
		
		//val1 = atoi( (char*)npi->value );
		//g_machine_no = val1;
		_d_int( g_host_type );
		return 0;
	}
	return -1;
}


int prop_file_format_check()
{
	char record[1][200];
	// driect use flash address to check file ?
	
	// get record
	// look check
	if( 	
		record[0][0] != '\n'
		
		|| record[0][2] != 'n'
		|| record[0][3] != ':'
		|| record[0][4] < 'A'
		|| record[0][4] > 'z'
		
		|| record[0][69] != 's'	// or other type
		|| record[0][70] != ':'
		|| record[0][71] < 'A'
		|| record[0][71] > 'z'

		){
	}
	return 0;
}
int prop_read_record_by_steam( int no, char *buf, int buf_size )
{
	char *file = _d_prop_file_name;
	void *h;
	int i;

	h = df_open( file, 0 );
	if( h == 0 )
		return -1;
	for( i=0; i<no; i++ ){
		if( df_read_line( h, buf, buf_size ) == -1 )
			return -1;
		if( (unsigned char)buf[0] == 0xff )
			return -1;
	}
	return 0;
}
int prop_read_record( int no, char* name, char* content )
{
	void *h;
	int pos;
	_s_prop_record pr;
	pos = no * sizeof( pr );
	h = df_open( _d_prop_file_name, 0 );
	df_seek( h, pos, 0 );
	df_read( h, (char*)&pr, sizeof(pr) );
	// check 
	if( 
		pr.cr_flag[0] == '\r'
		&& pr.cr_flag[1] == '\n'
		&& pr.name_flag[0] == 'n'
		&& pr.name_flag[1] == ':'
		&& pr.content_flag[0] == 'c'
		&& pr.content_flag[1] == ':'
		){
	
			if( name )
				strcpy( name, pr.name );
			if( content )
				strcpy( content, pr.content );
	}
	else
		return -1;
	return 0;
}
int prop_write_record( int no, char* name , char* content )
{
	void *h;
	int pos;
	_s_prop_record pr;
	pos = no * sizeof( pr );
	pr.cr_flag[0] = '\r';
	pr.cr_flag[1] = '\n';
	pr.name_flag[0] = 'n';
	pr.name_flag[1] = ':';
	pr.content_flag[0] = 'c';
	pr.content_flag[1] = ':';
	h = df_open( _d_prop_file_name, 0 );
	if( h == 0 )
		return -1;
	df_seek( h, pos, 0 );
	df_write( h, (char*)&pr, sizeof(pr) );
	df_close( h );
	return 0;
}
int prop_file_format()
{
	char buf[200];
	char *tmp_file = "/c/tmp_prop_file";
	int i=0; 
	void *h;
	// create new tmp_file
	h = df_create( tmp_file, 60*1024, 0 );
	df_close( h );
	// loop 
	while( 1 ){
		// 	read one record from prop_file by steam
		if( prop_read_record_by_steam( i, buf, sizeof(buf) ) < 0 )
			break;
		_d_str( buf );
		//	write record
		//if( prop_write_record( i, name, content ) < 0 )
			//break;
		i ++;
	}
	// del prop_file
	df_delete( _d_prop_file_name, 0 );
	// change tmp_file to prop_file
	df_rename( "/c/tmp_prop_file", _d_prop_file_name );
	return 0;
}
int prop_check_prop_name( char* file_name )
{
	if( 
		file_name[0] != 0
		&& file_name[0] != '/'
		&& file_name[1] != 'p'
		&& file_name[2] != '/'
		)
		return 0;
	return -1;
}
int prop_init()
{
	prop_file_format_check();
	return 0;
}
int prop_get_value( char* file, char* value, int max_size )
{
	_s_prop_record pr;
	int no = 0;
	if( prop_check_prop_name( file ) < 0 )
		return -1;
	while( 1 ){
		if( prop_read_record( no, (char*)pr.name, (char*)pr.content ) < 0 )
			return -1;
		if( strcmp( &file[3], pr.name ) == 0 ){
			strncpy( value, pr.content, max_size );
			break;
		}
		no ++;
	}
	return 0;
}
int prop_set_value( char* file, char* value, int max_size )
{
	_s_prop_record pr;
	int no = 0;
	if( prop_check_prop_name( file ) < 0 )
		return -1;
	
	node_prop_change_system_value( &file[3], value );

	while( 1 ){
		if( prop_read_record( no, (char*)pr.name, (char*)pr.content ) < 0 )
			break;
		if( strcmp( &file[3], pr.name ) == 0 ){
			//strcpy( pr.content, value );
			return prop_write_record( no, pr.name, value );
		}
		no ++;
	}
	// may end record and not found the same name record
	return prop_write_record( no, &file[3], value );
}


int node_prop_init()
{
	return prop_init();
}
int node_prop_get_value( char* file, char* value, int max_size )
{
	return prop_get_value( file, value, max_size );
}
int node_prop_set_value( char* file, char* value, int max_size )
{
	return prop_set_value( file, value, max_size );
}

















int node_prop_test()
{
	{
		void *h;
		int no = 6;
		h = df_open( "/p/test", 0 );
		if( h ) {
			_d_line();
			df_read( h, (char*)&no, 4 );
			_d_line();
			df_close( h );
			_d_line();
			no ++;
			_d_int( no );
		}
		h = df_create( "/p/test", 0, 0 );
		_d_hex( h );
		df_write( h, (char*)&no, 4 );
		_d_line();
		df_sync( h,0 );
		_d_line();
		df_close(h);

		_d_int( no );
	}
}
int node_prop_test2()
{
	// test action:
	//  create two file
	//  re-create same name file
	//  close file and open it
	int val;
	int tval;
	void *h;
	void *hh;
	// init
	//dir_file_init();
	// register 

	node_prop_init();

	h = df_open( "/a/test", 0 );
	_d_hex( h );

	val = 3;
	_d_hex( val );
	h = df_create( "/a/tt", 4, (char*)&val );

	df_read( h, (char*)&tval, 4 );
	_d_hex( val );
	_d_hex( tval );

	tval = 4;
	df_write( h, (char*)&tval, 4 );
	df_read( h, (char*)&tval, 4 );
	_d_hex( val );
	_d_hex( tval );

	df_seek( h, 0, 0 );

	val = 7;
	hh = df_create( "/a/t1", 4, (char*)&val );
	_d_hex( h );
	_d_hex( hh );

	df_read( hh, (char*)&tval, 4 );
	_d_str( "t1" );
	_d_hex( val );
	_d_hex( tval );

	_d_str( "t" );
	df_read( h, (char*)&val, 4 );
	_d_hex( val );

	df_close( h );
	df_close( hh );

	// re create /a/t1
	_d_str( "re create /a/t1" );
	hh = 0;
	val = 8;
	hh = df_create( "/a/t1", 4, (char*)&val );
	df_read( hh, (char*)&tval, 4 );
	_d_hex( tval );
	df_close( hh );


	// re open /a/t1
	_d_str( "re open /a/t1" );
	hh = 0;
	hh = df_open( "/a/t1", 0 );
	df_read( hh, (char*)&tval, 4 );
	_d_hex( tval );
	df_close( hh );



	return 0;
}













#endif















//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================






#ifdef _d_dir_file_modules_old_version



// file operate function.
_s_dir_file_operate node_prop_file_operate = 
{
	node_prop_open,		//open
	0,			//seek
	node_prop_read,		//read
	node_prop_write,		//write
	node_prop_close,		//close
	node_prop_create,	//create
	0,			//delete
	0,			//lock
	node_prop_sync 	// sync
};


// device_struct for dir_file register struct.
typedef struct{
	_s_dir_file_operate *o;
	char* device_name;
}_s_node_prop_device_struct;

_s_node_prop_device_struct g_node_prop_device_struct={
	&node_prop_file_operate,
	"node_prop"
};

// node ram operate item.
#define _d_node_prop_item_max_count 32
#define _d_node_prop_item_value_max_size 8
//#define _d_node_prop_item_max_count 128
typedef struct{
	//_s_dir_file_operate *o;
	_s_node_prop_device_struct *device_struct;
	char name[12];
	dword value[4];
	byte size;
	byte flag;		// 0 is int, 1 is string
	byte sync;
	byte tmp;
}_s_node_prop_item;
_s_node_prop_item g_node_prop_items[_d_node_prop_item_max_count]={0};
byte node_prop_need_flush_flag = 0;


#define _d_prop_file_name "/c/prop.dat"

int node_prop_file_read()
{
	_s_node_prop_item *npi = g_node_prop_items;
	int npi_pos = 0;
	int i;
	char* po ;
	int po_size = 1024;
	void *h;
	int line_size = 0;
	int kc;
	char *ki[10];
	char *tmp_po;
	int len;
	
	po = alloc_temp_buffer( 1024 );

	//_d_hex( po );
	if( po == 0 ){
		error_message( "not alloc temp_buffer", po );
		free_temp_buffer( po );
		return 0;
	}
	
	h = df_open( _d_prop_file_name, 0 );
	_d_hex( h );
	if( h == 0 ){
		free_temp_buffer( po );
		error_message( "can not open prop file", 0 );
		return -1;
	}

	// clear the ram prop value.
	//memset( g_node_prop_items, 0, sizeof( g_node_prop_items ) );
	while( 1 ){
		line_size = df_read_line( h, po, po_size );
		//_d_int( line_size );
		if( line_size <= 0 )
			break;
		kc = analysis_string_to_strings_by_decollator( po, "=", ki, 10 );
		//_d_int( kc );
		if( kc == 2 ){
			if( strlen( ki[0] ) > sizeof( npi[ npi_pos ].name )-1 )
				continue;
			tmp_po = ki[0];
			_d_str( tmp_po );
			kc = analysis_string_to_strings_by_decollator( ki[1], ",", ki, 10 );
			//_d_int( kc );
			if( kc == 2 ){
				_d_str( ki[0] );
				npi[ npi_pos ].flag = atoi( ki[0] );
				//_d_int( npi[ npi_pos ].flag );
				if( npi[ npi_pos ].flag == 0 ){
					//_d_line();
					strcpy( npi[ npi_pos ].name, tmp_po );
					npi[ npi_pos ].value[0] = atoi( ki[1] );
					npi[ npi_pos ].device_struct = &g_node_prop_device_struct;
					npi_pos ++;
				}
				else
				if( npi[ npi_pos ].flag == 1 ){
					_d_line();
					_d_str( tmp_po );
					//_d_str( ki[1] );
					strcpy( npi[ npi_pos ].name, tmp_po );
					
					// out the 0xd and 0xa char.
					len = strlen( ki[1] )-1;
					if( ki[1][len] == 0xd || ki[1][len] == 0xa )
						ki[1][len] = 0;
					len = strlen( ki[1] )-1;
					if( ki[1][len] == 0xd || ki[1][len] == 0xa )
						ki[1][len] = 0;
					
					if( strlen( ki[1] ) < sizeof( npi[ npi_pos ].value ) -1 )
						strcpy( (char*)npi[ npi_pos ].value, ki[1] );
					//_d_str( (char*)npi[ npi_pos ].value );
					npi[ npi_pos ].device_struct = &g_node_prop_device_struct;
					npi_pos ++;
				}
			}
		}
	}
	df_close( h );
	free_temp_buffer( po );
	return 0;
}
int node_prop_change_system_value(_s_node_prop_item *npi)
{
	// system value update.
	// ipaddr etc...
	int kc;
	char *ki[ 15 ];
	char tmp_buf[  sizeof( npi->value ) ];
	extern UCHAR  my_ipaddr[5];	
	extern UCHAR  Host_ipaddr [5];	
	extern int g_net_time_out;
	extern UINT  HOST_PORT; 
	extern int  g_machine_no; 
	extern int g_host_type;
	int val1, val2, val3, val4;
	
	// add the , explain the sys command string.
	if( strcmp( npi->name, "ipaddr" ) == 0 
			|| strcmp( npi->name, "ip" ) == 0 ){
		_d_str( (char*)npi->value );
		memcpy( tmp_buf, npi->value, sizeof( npi->value ) );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, ".", ki, 15 );
		if( kc != 4 )
			return 0;
		my_ipaddr[0] = atoi( ki[0] );
		my_ipaddr[1] = atoi( ki[1] );
		my_ipaddr[2] = atoi( ki[2] );
		my_ipaddr[3] = atoi( ki[3] );
	}
	else if( strcmp( npi->name, "hostip" ) == 0 ){
		_d_str( (char*)npi->value );
		memcpy( tmp_buf, npi->value, sizeof( npi->value ) );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, ".", ki, 15 );
		if( kc != 4 )
			return 0;
		Host_ipaddr[0] = atoi( ki[0] );
		Host_ipaddr[1] = atoi( ki[1] );
		Host_ipaddr[2] = atoi( ki[2] );
		Host_ipaddr[3] = atoi( ki[3] );
	}
	else if( strcmp( npi->name, "hport" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		HOST_PORT = val1;
		_d_int( HOST_PORT );
	}
	else if( strcmp( npi->name, "webport" ) == 0 ){
		//extern UINT  HOST_PORT; 
		//_d_str( (char*)npi->value );
		//val1 = atoi( (char*)npi->value );
		//HOST_PORT = val1;
		//_d_int( HOST_PORT );
	}
	else if( strcmp( npi->name, "timeout" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		g_net_time_out = val1;
		_d_int( g_net_time_out );
	}
	else if( strcmp( npi->name, "no" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		g_machine_no = val1;
		_d_int( g_machine_no );
	}
	else if( strcmp( npi->name, "keyboard" ) == 0 ){
		_d_str( (char*)npi->value );
		_d_buf( (char*)npi->value, sizeof( npi->value ) );
		if( s_strcmp( (char*)npi->value, "disable" ) == 0 ){
			Key_EnableScan( 0 );
		}
		else{
			Key_EnableScan( 1 );
		}
		//val1 = atoi( (char*)npi->value );
		//g_machine_no = val1;
		//_d_int( g_machine_no );
	}
	else if( strcmp( npi->name, "hosttype" ) == 0 ){
		_d_str( (char*)npi->value );
		if( strcmp( (char*)npi->value, "tcmd" ) == 0 )
			g_host_type = 1;
		else if( strcmp( (char*)npi->value, "dlc" ) == 0 )
			g_host_type = 2;
		else
			g_host_type = 0;
		
		//val1 = atoi( (char*)npi->value );
		//g_machine_no = val1;
		_d_int( g_host_type );
	}
	return 0;
}
int node_prop_value_debug()
{
	int i;
	_s_node_prop_item *npi = g_node_prop_items;
	for( i=0; i<_d_node_prop_item_max_count; i++ ){
		if( npi[i].name[0] == 0 )
			continue;
		_d_str( npi[i].name );
		_d_str( (char*)npi[i].value );
	}
	return 0;
}
int node_prop_value_update()
{
	int i;
	_s_node_prop_item *npi = g_node_prop_items;
	for( i=0; i<_d_node_prop_item_max_count; i++ ){
		if( npi[i].name[0] == 0 )
			continue;
		_d_str( npi[i].name );
		node_prop_change_system_value( &npi[i] );
	}
	return 0;
}
int node_prop_file_write()
{
	_s_node_prop_item *npi = g_node_prop_items;
	int i;
	char* po = alloc_temp_buffer( 1024 );
	int po_size = 1024;
	void *h;

	_d_hex( po );
	if( po == 0 ){
		_d_str( "not alloc temp_buffer" );
		free_temp_buffer( po );
		return 0;
	}
	
	df_delete( _d_prop_file_name, 0 );
	h = df_create( _d_prop_file_name, 0, 0 );
	if( h == 0 ){
		free_temp_buffer( po );
		return -1;
	}
	
	for( i=0; i<_d_node_prop_item_max_count; i++ ){
		if( npi[i].name[0] == 0 )
			continue;
		_d_str( npi[i].name );
		if( npi[i].flag == 0 ){
			s_sprintf( po, "%s=%d,%d\n", npi[i].name, npi[i].flag, npi[i].value[0] );
			_d_str( po );
			df_write( h, po, strlen( po ) );
		}
		else
		if( npi[i].flag == 1 ){
			s_sprintf( po, "%s=%d,%s\n", npi[i].name, npi[i].flag, (char*)npi[i].value );
			df_write( h, po, strlen( po ) );
			_d_str( po );
		}
		//node_prop_change_system_value( &npi[i] );
	}
	df_close( h );
	free_temp_buffer( po );
	node_prop_value_update();
	return 0;
}

int node_prop_init()
{
	memset( g_node_prop_items, 0, sizeof( g_node_prop_items ) );
	register_dir_file_node( "/p/", (void*)&g_node_prop_device_struct, 0 );
	node_prop_file_read();
	node_prop_value_update();
	return 0;
}

static unsigned get_dw_name( char* name )
{
	unsigned dw_name=0;
	unsigned char tb;
	tb = name[0];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[0] = tb;
	tb = name[1];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[1] = tb;
	tb = name[2];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[2] = tb;
	tb = name[3];
	if( tb == 0 )	return dw_name;
	((char*)&dw_name)[3] = tb;
	return dw_name;
}
void node_prop_debug( int flag )
{
	unsigned dw_name = 0;
	int i;
	_s_node_prop_item *po = 0;
	//_d_line();
	for( i=0; i<_d_node_prop_item_max_count; i++ ){
			//_d_int(i);
			po = &g_node_prop_items[i];
			//_d_hex( po );
			if( po->name[0] ){
				_d_str( po->name );
				_d_str( (char*)(po->value) );
			}
	}
}
void* node_prop_open( char* name, int flag )
{
	unsigned dw_name = 0;
	int i;
	_s_node_prop_item *po = 0;
	//_d_line();
	for( i=0; i<_d_node_prop_item_max_count; i++ ){
		if( s_strcmp( name, g_node_prop_items[i].name ) == 0 ){
			//_d_int(i);
			po = &g_node_prop_items[i];
			//_d_hex( po );
			return po;
		}
	}
	return 0;
}
void* node_prop_create( char* name, int size, char* value )
{
	int i;
	_s_node_prop_item *po = 0;
	
	_d_line();
	if( name[0] == 0 )
		return 0;
	po = node_prop_open( name, 0 );
	if( po==0 ){
		for( i=0; i<_d_node_prop_item_max_count; i++ ){
			if( g_node_prop_items[i].name[0] == 0 ){
				po = &g_node_prop_items[i];
				po->flag = 1;
				s_memcpy( &po->name, name, sizeof(po->name) );
				break;
			}
		}
	}

	if( po ){
		if( size > sizeof( g_node_prop_items[0].value ) )
			size = sizeof( g_node_prop_items[0].value );
		po->size = size;
		//_d_hex( po->name );
		if( value )
			s_memcpy( &po->value, value, size );
		po->device_struct = &g_node_prop_device_struct;
		node_prop_need_flush_flag= 1;
		return po;
	}
	return 0;
}
int node_prop_delete( void* po )
{
	_s_node_prop_item* tpo = (_s_node_prop_item*)po;
	if( po == 0 ) return 0;
	_d_line();
	memset( tpo, 0, sizeof( g_node_prop_items[0] ) );
	node_prop_need_flush_flag= 1;
	return 0;
}
int node_prop_read( void* po, char* buf, int size )
{
	_s_node_prop_item* tpo = (_s_node_prop_item*)po;
	//_d_line();
	//_d_hex( tpo->size );
	//_d_hex( tpo->value );
	//_d_str( tpo->name );
	if( po == 0 ) return 0;
	if( size > sizeof( g_node_prop_items[0].value ) )
		size = sizeof( g_node_prop_items[0].value );
	//_d_line();
	memcpy( buf, tpo->value, size );
	//_d_line();
	return 0;
}
int node_prop_write( void* po, char* buf, int size )
{
	_s_node_prop_item* tpo = (_s_node_prop_item*)po;
	if( po == 0 ) return 0;
	//_d_line();
	if( size > sizeof( g_node_prop_items[0].value ) )
		size = sizeof( g_node_prop_items[0].value );
	tpo->flag = 1;
	_d_int( size );
	_d_str( buf );
	memcpy( &tpo->value, buf, size );
	node_prop_need_flush_flag= 1;
	return 0;
}
int node_prop_sync( void *po, int flag )
{
	_d_line();
	if( node_prop_need_flush_flag == 1 ){
		_d_line();
		node_prop_file_write();
		node_prop_need_flush_flag= 0;
	}
}
int node_prop_close( void* po )
{
	//_d_line();
	return 0;
}

/*
 * standard file interface.
 * int open( char* name, mode )
 * int read( int h, char* buf, int size )
 * int write( int h, char* buf, int size )
 * int close( int h )
 */

int node_prop_set_system_value( char* file, char* value, int max_size )
{
	// system prop.
	// date and time
	DATETIME dt;
	int kc;
	char *ki[ 15 ];
	if( strcmp( file, "/p/date" ) == 0 ){
		kc = analysis_string_to_strings_by_decollator( value, "-", ki, 15 );
		if( kc!=3 )
			return -1;
		RTC_GetTime(&dt);
		dt.year = atoi( ki[0] );
		dt.month = atoi( ki[1] );
		dt.day = atoi( ki[2] );
		RTC_SetTime(&dt);
		return 1;
	}
	else if( strcmp( file, "/p/time" ) == 0 ){
		kc = analysis_string_to_strings_by_decollator( value, ":", ki, 15 );
		if( kc!=3 )
			return -1;
		RTC_GetTime(&dt);
		dt.hour = atoi( ki[0] );
		dt.minute= atoi( ki[1] );
		dt.second = atoi( ki[2] );
		RTC_SetTime(&dt);
		return 1;
	}
	else if( strcmp( file, "/p/whiteCount" ) == 0 ){
		return 1;
	}
	return 0;
}

int node_prop_set_value( char* file, char* value, int size )
{
		void *h;
		if( size >= sizeof( g_node_prop_items[0].value ) )
			size = sizeof( g_node_prop_items[0].value );

		if( node_prop_set_system_value( file, value, size ) != 0 )
			return 0;
		
		h = df_create( file, 0, 0 );
		_d_hex( h );
		_d_int( size );
		_d_str( value );
		df_write( h, value, size );
		_d_line();
		//df_sync( h,0 );
		_d_line();
		df_close(h);
		//node_prop_value_debug();
		node_prop_file_write();
		return 0;
}
int node_prop_get_system_value( char* file, char* value, int max_size )
{
	// system prop.
	// date and time
	DATETIME dt;
	if( strcmp( file, "/p/date" ) == 0 ){
		RTC_GetTime(&dt);
		s_sprintf( value, "%04d-%02d-%02d", dt.year, dt.month, dt.day );
		return 1;
	}
	else if( strcmp( file, "/p/time" ) == 0 ){
		RTC_GetTime(&dt);
		s_sprintf( value, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
		return 1;
	}
	else if( strcmp( file, "/p/white_count" ) == 0 ){
		int size = df_get_file_size( "/c/white_name.lp" );
		s_sprintf( value, "%d", size>=4?size/4-1:0 );
	}
	else if( strcmp( file, "/p/record_count" ) == 0 ){
		int size = record_get_count();
		if( size > -1 )
			s_sprintf( value, "%d", size );
		else strcpy( value, "no" );
	}
	return 0;
}
int node_prop_get_value( char* file, char* value, int max_size )
{
		void *h;
		//char tmp_str[32];

		//_d_str( file );
		if( node_prop_get_system_value( file, value, max_size ) == 1 )
			return 0;
		
		h = df_open( file, 0 );
		//_d_hex( h );
		if( h == 0 )
			return -1;
#if 0
		df_read( h, tmp_str, sizeof( tmp_str ) );
		if( value ){
			if( max_size > sizeof( tmp_str ) )
				max_size = sizeof( tmp_str );
			memcpy( value, tmp_str, max_size );
		}
#else
		if( value ){
			df_read( h, value, max_size );
			//_d_str( value );
		}
#endif
		//_d_line();
		df_close(h);
		return 0;
}

int node_prop_test()
{
	{
		void *h;
		int no = 6;
		h = df_open( "/p/test", 0 );
		if( h ) {
			_d_line();
			df_read( h, (char*)&no, 4 );
			_d_line();
			df_close( h );
			_d_line();
			no ++;
			_d_int( no );
		}
		h = df_create( "/p/test", 0, 0 );
		_d_hex( h );
		df_write( h, (char*)&no, 4 );
		_d_line();
		df_sync( h,0 );
		_d_line();
		df_close(h);

		_d_int( no );
	}
}
int node_prop_test2()
{
	// test action:
	//  create two file
	//  re-create same name file
	//  close file and open it
	int val;
	int tval;
	void *h;
	void *hh;
	// init
	//dir_file_init();
	// register 

	node_prop_init();

	h = df_open( "/a/test", 0 );
	_d_hex( h );

	val = 3;
	_d_hex( val );
	h = df_create( "/a/tt", 4, (char*)&val );

	df_read( h, (char*)&tval, 4 );
	_d_hex( val );
	_d_hex( tval );

	tval = 4;
	df_write( h, (char*)&tval, 4 );
	df_read( h, (char*)&tval, 4 );
	_d_hex( val );
	_d_hex( tval );

	df_seek( h, 0, 0 );

	val = 7;
	hh = df_create( "/a/t1", 4, (char*)&val );
	_d_hex( h );
	_d_hex( hh );

	df_read( hh, (char*)&tval, 4 );
	_d_str( "t1" );
	_d_hex( val );
	_d_hex( tval );

	_d_str( "t" );
	df_read( h, (char*)&val, 4 );
	_d_hex( val );

	df_close( h );
	df_close( hh );

	// re create /a/t1
	_d_str( "re create /a/t1" );
	hh = 0;
	val = 8;
	hh = df_create( "/a/t1", 4, (char*)&val );
	df_read( hh, (char*)&tval, 4 );
	_d_hex( tval );
	df_close( hh );


	// re open /a/t1
	_d_str( "re open /a/t1" );
	hh = 0;
	hh = df_open( "/a/t1", 0 );
	df_read( hh, (char*)&tval, 4 );
	_d_hex( tval );
	df_close( hh );



	return 0;
}

#endif

