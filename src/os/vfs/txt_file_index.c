
#include "os.h"
#include "vfs/dir_file.h"
#include "txt_file_index.h"
//#include "tcpip/net.h"

//#ifdef _d_dir_file_modules
#if 1



int txt_file_check_sum( char *fn )
{
	unsigned check_sum = 0;
	void *h;
	unsigned char buf[64];
	int i;
	int size;
	int count = 0;
	unsigned char uc;
	
	h = df_open( fn, 0 );
	if( h == 0 )
		return 0;
	while( 1 ){
		size = df_read( h, buf, sizeof( buf ) );
		if( size == 0 )
			break;
		for( i=0; i<size; i++ ){
			check_sum += (unsigned char)buf[i];
			count ++;
			WDT_Clear();
			uc = (unsigned char)buf[i];
			//if( uc == 0xd )
			//	uc = 0x20;
			//if( uc == 0xa )
			//	uc = 0x20;
			//TRACE("%d, %d, %c %d\n", count, (unsigned char)buf[i], uc, check_sum );		
		}
	}
	df_close(h);
	_d_int( count );
	_d_int( check_sum );
	return check_sum;
}
int debug_show_pointer_file( char* txt_file_name, char* pointer_file_name )
{
	char buf[100];
	u16 i;
	void *tf, *lpf;
	tf = df_open( txt_file_name, 0 );
	lpf = df_open( pointer_file_name, 0 );
	while( 1 ){
		if( df_read( lpf, (char*)&i, 2 ) <= 0 )
			break;
		_d_hex( i );
		df_seek( tf, i, 0 );
		df_read_line( tf, buf, 100 );
		_d_str( buf );
		}
	df_close( lpf );
	df_close( tf );
	return 0;
}
int sub_txt_file_read_line( void* h, void* lph, int line_no, char* buf, int buf_size )
{
	int pos;
	int offset;
	int size;
	pos = line_no * 4;
	//_d_hex( pos );
	offset = df_seek( lph, pos, 0 );
#if _d_sd
	offset = ftell( lph );
#elif _d_uffs
	offset = uffs_tell( (int)lph-1 );
#endif
	//_d_hex( offset );
	if( offset < pos ){
		error_message( "set position is big file length", pos );
		return 0;
	}
	df_read( lph, (char*)&pos, 4 );
	//_d_hex( pos );
	df_seek( h, pos, 0 );
	size = df_read_line( h, buf, buf_size );
	if( pos == 0 ){
		// for the 0xff , 0xfe unicode file
		//_d_hex( buf[0] );
		//_d_hex( buf[1] );
		//_d_hex( buf[2] );
		//_d_hex( buf[3] );
		if( 
				(unsigned char)buf[0] == (unsigned char)0xef 
				&& (unsigned char)buf[1] == (unsigned char)0xbb 
				&& (unsigned char)buf[2] == (unsigned char)0xbf 
			){
			memcpy( &buf[0], &buf[3], size );
			size -= 3;
		}
	}
	return size;
}
int txt_file_read_line( char* txt_file_name, int line_no, char* buf, int buf_size )
{
	char lp_file_name[64];
	void *h;
	void *lph;
	int rel;

	_d_str( txt_file_name );
	s_sprintf( lp_file_name, "%s.lp", txt_file_name );
	h = df_open( txt_file_name, 0 );
	if( h == 0 ){
		error_message( "can not open file", 0 );
		_d_str( txt_file_name );
		return -1;
	}
	lph = df_open( lp_file_name, 0 );
	if( lph == 0 ){
		txt_file_create_line_pointer( txt_file_name, lp_file_name );
		lph = df_open( lp_file_name, 0 );
		if( lph == 0 ){
			error_message( "create line_pointer error", 0 );
			//error_message( "can not open file", 0 );
			_d_str( lp_file_name );
			df_close( h );
			return -1;
		}
	}
	//_d_hex( h );
	//_d_hex( lph );
	//_d_int( line_no );
	rel = sub_txt_file_read_line( h, lph, line_no, buf, buf_size );
	df_close( h );
	df_close( lph );
	//_d_int( rel );
	return rel;
}
int txt_file_search_line( char* txt_file_name , char* key, char* buf, int buf_size )
{
	char lp_file_name[64];
	void *h;
	void *lph;
	int rel;
	int line_no;
	int key_len;

	//_d_str( txt_file_name );
	s_sprintf( lp_file_name, "%s.lp", txt_file_name );
	h = df_open( txt_file_name, 0 );
	if( h == 0 ){
		error_message( "cat not open file", 0 );
		_d_str( txt_file_name );
		return -1;
	}
	lph = df_open( lp_file_name, 0 );
	if( lph == 0 ){
		txt_file_create_line_pointer( txt_file_name, lp_file_name );
		lph = df_open( lp_file_name, 0 );
		if( lph == 0 ){
			error_message( "create line_pointer error", 0 );
			df_close( h );
			_d_line();
			return -1;
		}
	}
	line_no = 0;
	key_len = strlen( key );
	while( 1 ){
		//_d_int( line_no );
 		rel = sub_txt_file_read_line( h, lph, line_no, buf, buf_size );
		if( rel <= 0 ){
			//_d_int( rel );
			break;
		}
		//_d_str( key );
		//_d_str( buf );
		//_d_int( key_len );
		if( s_strncmp( key, buf, key_len ) == 0 ){
			rel = 1;
			break;
		}
		line_no ++;
	}
	df_close( h );
	df_close( lph );
	return rel;
}
int txt_file_create_line_pointer( char* txt_file_name, char* line_pointer_file_name )
{
	void *tf, *lpf;
	int tf_size;
	int size = 0;
	u32 i;
	tf_size = df_get_file_size( txt_file_name );
	if( tf_size == 0 )	return -1;
	tf = df_open( txt_file_name, 0 );
	if( tf == 0 )	return -1;
	df_delete( line_pointer_file_name, 0);
	lpf = df_create( line_pointer_file_name, 0, 0);
	_d_str( line_pointer_file_name );
	if( lpf == 0 ){
		df_close( tf );
		return -1;
		}
	i=0;
	df_write( lpf, (char*)&i, 4 );
	for( i=0; i<tf_size; i++ ){
		char bchar;
		df_read( tf, &bchar, 1 );
		if( bchar == '\n' ){
			// write the tf offset.
			u32 pos = i + 1;
			df_write( lpf, (char*)&pos, 4 );
			size += 4;
		}
	}
	_d_int( tf_size );
	_d_int( size );
	df_close( lpf );
	df_close( tf );

	// debug
	//debug_show_pointer_file( txt_file_name, line_pointer_file_name);
	return 0;
}


int sort_line( void* txt_handle, u32* lpfs, int size )
{
#if 0
	char *str1, *str2;
	int i,j;

	_d_int( size );
	str1 = &g_out_buf[0];
	str2 = &g_out_buf[300];
	for( i=0; i<size-1; i++){
		_d_hex( lpfs[i] );
		df_seek( txt_handle, lpfs[i], 0 );
		df_read_line( txt_handle, str1, sizeof( str1 ));
		for( j=i+1; j<size; j++ ){
			_d_hex( lpfs[j] );
			df_seek( txt_handle, lpfs[j], 0 );
			df_read_line( txt_handle, str2, sizeof( str2 ));
			//_d_str( str1 );
			//_d_str( str2 );
			if( s_strcmp( str1, str2 ) > 0 ){
				// swap two point.
				u16 tval;
				tval = lpfs[j]; lpfs[j] = lpfs[i]; lpfs[i] = tval;
				strcpy( str1, str2 );
			}
		}
	}
	return size;
#else
	return 0;
#endif
}

int split_group_and_sort( void* txt_handle, void* line_pointer_handle,
			void* tmp_handle, 
			u32* line_po,  int line_po_number )
{
	int line_pointer_size;
	int tmp_size;
	int total_size = 0;
	int group_count = 0;
	_d_int( line_po_number );
	while( 1 ){
		tmp_size = df_read( line_pointer_handle, (char*)line_po, line_po_number * 4 );
		if( tmp_size == 0 )
			break;
		tmp_size /= 4;
		_d_int( tmp_size );
		total_size += tmp_size;
		sort_line( txt_handle, line_po, tmp_size );
		df_write( tmp_handle, (char*)line_po, tmp_size*4 );
		group_count ++;
		_d_int( group_count );
	}
	_d_int( total_size );
	return total_size;
}


int group_sort_in_file( void* txt_handle, void* tmp_handle, void* index_handle, 
			int line_pointer_count, u32* group_array, int group_size )
{
#if 0
	int i;
	int pos;
	int group_count;
	int group_gap = group_size * 4;
	char *str1, *str2;
	int val, val1, val2;

	str1 = &g_out_buf[0];
	str2 = &g_out_buf[300];
	group_count = line_pointer_count / group_size + 1;
	// maybe need add 1.
	_d_int( group_count );
	if( group_count*2  > group_size ){
		_d_line();
		_d_str( "error: not enough memory for sort the group" );
		return -1;
	}
	// read all group first item for init. 
	for( i=0; i<group_count; i++ ){
		df_seek( tmp_handle, i*group_gap, 0 );
		df_read( tmp_handle, (char*)&group_array[i*2+1], 4 );
		group_array[i*2+0] = 0;
	}
	// begin select flow.
	while( 1 ){
		// get the first group value.
		// loop cmpara all group
		// position the min value group.
		// write value to file and inc the group offset.
		// continue next cmpare.
		// if the group is end, the group offset is 0xffffffff, donot in any calc.
		
		// get the first group value.
		for( pos=0; pos<group_count; pos++ ){
			val1 = group_array[pos*2];
			if( val1 == (u32)-1 )
				continue;
			val1 = group_array[pos*2+1];
			df_seek( txt_handle, val, 0 );
			df_read_line( txt_handle, str1, 100 );
			break;
		}
		if( pos == group_count ){
			// end.
			_d_str( "end sort group" );
			// save the last data.
			return 0;
		}
		// loop cmpara all group
		// position the min value group.
		for( i=1; i<group_count; i++ ){
			val2 = group_array[i*2+1];
			if( val2 == (u32)-1 )
				continue;
			df_seek( txt_handle, val2, 0 );
			df_read_line( txt_handle, str2, 100 );
			if( strcmp( str1, str2 ) > 0 ){
				_d_str( str1 );
				_d_str( str2 );
				strcpy( str1, str2 );
				val1 = val2;
				pos = i;
			}
		}
		_d_int( pos );
		// write value to file and inc the group offset.
		df_write( index_handle, (char*)&val1, 4 );
		group_array[ pos*2 ] ++;
		// if the group is end, the group offset is 0xffffffff, donot in any calc.
		if( group_array[ pos*2 ] > group_gap ){
			group_array[ pos*2 ] = (u32)-1;
			group_array[ pos*2 + 1 ] = 0;
			continue;
		}
		if( pos == group_count-1 && group_array[ pos*2 ] 
				>= (line_pointer_count % group_size ) ){
			group_array[ pos*2 ] = (u32)-1;
			group_array[ pos*2 + 1 ] = 0;
			continue;
		}
		df_seek( tmp_handle, pos*group_gap + group_array[pos*2]*4/*gourp offset*/, 0 );
		df_read( tmp_handle, (char*)&group_array[pos*2+1], 4 );
		// continue next cmpare.
	}
#endif
	return 0;
}

int txt_file_create_line_index( char* txt_file_name, char* line_pointer_file_name, char* index_file_name )
{
#if 0
	// FIXME
	// this function only create txt_file size < 64K 's index file.
	// this function only create line_pointer size < 2K's index file.  only 1024 line.
	int size;
	int pos, val1, val2;
	int line_pointer_count;
	void *tf, *lpf, *indexf;
	//FIXME
	//u16 lpfs[ 128 ];
	u16 *lpfs;
	char *str1, *str2;
	int i,j;
	int group_size ;
	void *txt_handle, *line_pointer_handle, *tmp_handle, *index_handle;


	str1 = &g_out_buf[0];
	str2 = &g_out_buf[300];
	group_size = sizeof( g_in_buf )/4;
	lpfs = (u16*)g_in_buf;
	str1 = &g_out_buf[100];
	str2 = &g_out_buf[300];
	
	size = df_get_file_size( txt_file_name );
_d_int( size );
	if( size <= 0 )
		return -1;
	size = df_get_file_size( line_pointer_file_name );
_d_int( size );
	if( size <= 0 )
		return -1;
	line_pointer_count = size / 4;

	// split group and sort group
	txt_handle = df_open( txt_file_name, 0 );
	line_pointer_handle = df_open( line_pointer_file_name, 0 );
	tmp_handle = df_create( "/c/tmp/t", 0, 0 );
	if( txt_handle == 0 )
		return -1;
	if( line_pointer_handle == 0 ){
		df_close( txt_handle );
		return -1;
	}
	if( tmp_handle == 0 ){
		df_close( txt_handle );
		df_close( line_pointer_handle );
		return -1;
	}
	split_group_and_sort( txt_handle, line_pointer_handle, tmp_handle, 
			(u32*)g_in_buf, group_size ); 
	df_close( line_pointer_handle );
	// close and open tmp_handle , let the file size write in flash.
	df_close( tmp_handle );
	tmp_handle = df_open( "/c/tmp/t" , 0 );
	if( tmp_handle == 0 ){
		df_close( txt_handle );
		df_close( line_pointer_handle );
		return -1;
	}

	// group_sort_in_file.
	index_handle = df_create( index_file_name, 0, 0 );
	if( index_handle == 0 ){
		df_close( txt_handle );
		df_close( tmp_handle );
		return -1;
	}
	group_sort_in_file( txt_handle, tmp_handle, index_handle, 
				line_pointer_count, (u32*)g_in_buf, group_size );
	df_close( tmp_handle );
	df_delete( "/c/tmp/t" , 0 );
	df_close( txt_handle );
	df_close( index_handle );
#endif
	return 0;
}

int test_create_index()
{
	// create  data file. 700 line, every line rand string.
	int i;
	int val;
	for ( i=0; i<700; i++ ){
		val = rand();
		_d_int( val );
		val = rand();
		_d_int( val );
	}
}

#if 0
int txt_file_create_line_pointer( char* txt_file_name, char* line_pointer_file_name )
{
	void *tf, *lpf;
	int tf_size;
	u16 i;
	tf_size = df_get_file_size( txt_file_name );
	if( tf_size == 0 )	return -1;
	tf = df_open( txt_file_name, 0 );
	if( tf == 0 )	return -1;
	lpf = df_create( line_pointer_file_name, 0, 0);
	if( lpf == 0 ){
		df_close( tf );
		return -1;
		}
	i=0;
	df_write( lpf, (char*)&i, 2 );
	for( i=0; i<tf_size; i++ ){
		char bchar;
		df_read( tf, &bchar, 1 );
		if( bchar == '\n' ){
			// write the tf offset.
			int pos = i + 1;
			df_write( lpf, (char*)&pos, 2 );
			}
		}
	df_close( lpf );
	df_close( tf );

	// debug
	//debug_show_pointer_file( txt_file_name, line_pointer_file_name);
	return 0;
}


int txt_file_create_line_index( char* txt_file_name, char* line_pointer_file_name, char* index_file_name )
{
	// FIXME
	// this function only create txt_file size < 64K 's index file.
	// this function only create line_pointer size < 2K's index file.  only 1024 line.
	int size;
	void *tf, *lpf, *indexf;
	//FIXME
	//u16 lpfs[ 128 ];
	u16 *lpfs;
	char *str1, *str2;
	int i,j;


	lpfs = (u16*)g_in_buf;
	str1 = &g_out_buf[100];
	str2 = &g_out_buf[300];
	
	size = df_get_file_size( txt_file_name );
_d_int( size );
	if( size >= 64*1024 )
		return -1;
	size = df_get_file_size( line_pointer_file_name );
_d_int( size );
	if( size >= 2 * 1024 )
		return -1;
	// read the line pointer table.
	size = df_get_file_content( line_pointer_file_name, (char*)lpfs, 1024 );
	//size = df_get_file_content( line_pointer_file_name, (char*)lpfs, sizeof( lpfs ) );
_d_int( size );

	tf = df_open( txt_file_name, 0 );
_d_hex( tf );
	if( tf == 0 )
		return -1;

	// Insertion sort
_d_line();
	size /= 2;
	//size += 1;
	//_d_int( size );
	for( i=0; i<size-1; i++){
		_d_hex( lpfs[i] );
		df_seek( tf, lpfs[i], 0 );
		df_read_line( tf, str1, sizeof( str1 ));
		for( j=i+1; j<size; j++ ){
			_d_hex( lpfs[j] );
			df_seek( tf, lpfs[j], 0 );
			df_read_line( tf, str2, sizeof( str2 ));
			//_d_str( str1 );
			//_d_str( str2 );
			if( s_strcmp( str1, str2 ) > 0 ){
				// swap two point.
				u16 tval;
				tval = lpfs[j]; lpfs[j] = lpfs[i]; lpfs[i] = tval;
				strcpy( str1, str2 );
				}
			}
		}

	// save the point to file.
	indexf = df_create( index_file_name, 0, 0 );
	if( indexf == 0 )
		return -1;
_d_line();
	df_write( indexf, (char*)lpfs, size*2 );
_d_line();
	df_close( indexf );
_d_line();
	return 0;
}

#endif

// three step.
// 1 get line point and write to file.
// 2 sort the string by line point
//// 3 sort the key value by line point






#else

int txt_file_create_line_pointer( char* txt_file_name, char* line_pointer_file_name )
{
	return 0;
}
int txt_file_create_line_index( char* txt_file_name, char* line_pointer_file_name, char* index_file_name )
{
	return 0;
}

#endif


