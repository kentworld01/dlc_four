
#include "os/property.h"



/* ---------------------------

property action
	get
	set
	del
	update_system_val

--------------------------- */

// system global value.

int g_device_no = 888;
u8 g_send_package_socket_ip[4] = {192,168,1,71};
u16 g_send_package_socket_port = 8001;
int g_host_type = 1;
int g_machine_no = 888;
u8 g_machine_ip[4] = {192,168,1,167};
int g_reader_id = 0;
int g_net_time_out = 500;



char *g_property_file_name = "/property.tr";
int g_property_rand_file_pos = -1;




int property_change_system_value( char* name, char* value )
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
		g_machine_ip[0] = atoi( ki[0] );
		g_machine_ip[1] = atoi( ki[1] );
		g_machine_ip[2] = atoi( ki[2] );
		g_machine_ip[3] = atoi( ki[3] );
		//debug_show_ip_str(g_machine_ip);
		return 0;
	}
	else if( strcmp( npi->name, "hostip" ) == 0 ){
		_d_str( (char*)npi->value );
		memcpy( tmp_buf, npi->value, sizeof( npi->value ) );
		kc = analysis_string_to_strings_by_decollator( tmp_buf, ".", ki, 15 );
		if( kc != 4 )
			return 0;
		g_send_package_socket_ip[0] = atoi( ki[0] );
		g_send_package_socket_ip[1] = atoi( ki[1] );
		g_send_package_socket_ip[2] = atoi( ki[2] );
		g_send_package_socket_ip[3] = atoi( ki[3] );
		//debug_show_ip_str(g_send_package_socket_ip);
		return 0;
	}
	else if( strcmp( npi->name, "hport" ) == 0 ){
		_d_str( (char*)npi->value );
		val1 = atoi( (char*)npi->value );
		g_send_package_socket_port = val1;
		_d_int( g_send_package_socket_port );
		return 0;
	}
	else if( strcmp( npi->name, "webport" ) == 0 ){
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
			//Key_EnableScan( 0 );
		}
		else{
			//Key_EnableScan( 1 );
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
int property_change_all_system_value()
{
	char *file = g_property_file_name;
	int h;
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
		kc = analysis_string_to_strings_by_decollator( buf, "=", ki, 15 );
		if( kc != 2 )
			continue;
		s_chop( ki[1] );
		property_change_system_value( ki[0], ki[1] );
	}
	df_close( h );
	return rel;
}
#if 0
int property_get_index( int index, char *name, char *val )
{
	char buf[96];
	int line_no = index;
	int kc;
	char *ki[15];
	int rel = -1;
	//while(1)
	if( txt_file_read_line( g_property_file_name, line_no, buf, sizeof( buf ) ) < 0 )
		return -1;
	kc = analysis_string_to_strings_by_decollator( buf, "=", ki, 15 );
	if( kc !=2 )
		return -1;
	strncpy( buf, ki[0], sizeof(buf) );
	s_chop( buf );
	if( name )
		strcpy( name, buf );
	strncpy( buf, ki[1], sizeof(buf) );
	s_chop( buf );
	if( val )
		strcpy( val, buf );
	return 1;
}
int property_get( char *name, char *val )
{
	char buf[96];
	int line_no = 0;
	int kc;
	char *ki[15];
	int rel = -1;
	while(1){
		if( txt_file_read_line( g_property_file_name, line_no++, buf, sizeof( buf ) ) < 0 )
			break;
		kc = analysis_string_to_strings_by_decollator( buf, "=", ki, 15 );
		if( kc !=2 )
			continue;
		if( s_strcmp( ki[0], name ) == 0 ){
			//int len;
			strncpy( buf, ki[1], sizeof(buf) );
			rel = 0;
			s_chop( buf );
			if( val )
				strcpy( val, buf );
			return 1;
		}
	}
	return 0;
}
int property_set( char *name, char *val )
{
	char buf[96];
	int line_no = 0;
	int kc;
	char *ki[15];
	int tf, f;
	char *t_property_file_name = "/tmp_perperty.tr";
	int new_key = 1;

	f = uffs_open( g_property_file_name, UO_RDONLY );
	if( f == -1 ){
		f = uffs_open( g_property_file_name, UO_RDWR|UO_CREATE );
		if( f == -1 )
			return -1;
	}
	tf = uffs_open( t_property_file_name, UO_WRONLY|UO_CREATE );
	if( tf == -1 ){
		uffs_close( f );
		return -1;
	}
	while(1){
		if( uffs_read_line( f, buf, sizeof( buf ) ) < 0 )
			break;
		kc = analysis_string_to_strings_by_decollator( buf, "=", ki, 15 );
		if( kc !=2 )
			continue;
		if( s_strcmp( ki[0], name ) == 0 ){
			//int len;
			//strncpy( buf, &ki[1], sizeof(buf) );
			if( new_key == 0 )
				continue;
			sprintf( buf, "%s=%s\n", name, val );
			new_key = 0;
		}
		else	strcat( buf, "\n" );
		uffs_write( tf, buf, strlen( buf ) );
	}
	if( new_key == 1 ){
		uffs_close( tf );
		uffs_close( f );
		f = uffs_open( g_property_file_name, UO_RDWR|UO_CREATE );
		if( f == -1 )
			return -1;
		uffs_seek( f, 0, _SEEK_END );
		sprintf( buf, "%s=%s\n", name, val );
		uffs_write( f, buf, strlen( buf ) );
		uffs_close( f );
	}
	else{
		uffs_close( tf );
		uffs_close( f );
		uffs_remove( g_property_file_name );
		uffs_rename( t_property_file_name, g_property_file_name );
	}
	return 0;
}
int property_del( char *name )
{
	return -1;
}
#endif

#define _d_property_record_value_offset 36

int property_get( char* key, char* value )
{
	char tbuf[_d_max_rand_file_record_size];
	int index;
	if( g_property_rand_file_pos < 0 )
		return -1;
	index = rand_file_find( g_property_rand_file_pos, key, 0, strlen(key), 0 );
	if( index >= 0 ){
		rand_file_get( g_property_rand_file_pos, index, tbuf, sizeof(tbuf) );
		if( value ){
			strcpy( value, &tbuf[_d_property_record_value_offset ] );
		}
	}
	return -1;
}
int property_set( char* key, char* value )
{
	char tbuf[_d_max_rand_file_record_size];
	int index;
	if( g_property_rand_file_pos < 0 )
		return -1;
	memset( tbuf, 0, sizeof( tbuf ) );
	strcpy( &tbuf[0], key );
	strcpy( &tbuf[_d_property_record_value_offset ], value );
	index = rand_file_find( g_property_rand_file_pos, key, 0, strlen(key), 0 );
	if( index >= 0 ){
		rand_file_del( g_property_rand_file_pos, index );
	}
	return rand_file_add( g_property_rand_file_pos, tbuf, sizeof(tbuf) );
}
int property_init()
{
	g_property_rand_file_pos = rand_file_open( g_property_file_name );
	if( g_property_rand_file_pos < 0 ){
		if( rand_file_create( g_property_file_name, 128,128 ) < 0 ){
			//while(1);
			return -1;
		}
		g_property_rand_file_pos = rand_file_open( g_property_file_name );
	}
	//property_change_all_system_value();
	property_test();
	return 0;
}

int property_test()
{
	char buf[64];
	property_set( "key1", "abcd" );
	property_set( "key1", "12345" );
	property_get( "key1", buf );
	return 0;
}





