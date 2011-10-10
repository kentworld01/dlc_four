//#define _d_pc_test

#ifdef _d_pc_test
	#define _d_dir_file_modules
	//#include <unistd.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <stdlib.h>
	//#include <process.h>
#else
	#include "os.h"
	#include "vfs/dir_file.h"
	#include "txt_file_index.h"
	#include "config_file.h"
	#include "mp_misc.h"
#endif

	#include "index.h"

#ifdef _d_pc_test
	#define _d_line() TRACE("%d\t%s\n", __LINE__, __func__ )
	#define _d_hex(x) TRACE("%d\t%s\t"#x"=%08x\n", __LINE__, __func__, x )
	#define _d_int(x) TRACE("%d\t%s\t"#x"=%d\n", __LINE__, __func__, x )
	#define _d_u(x) TRACE("%d\t%s\t%s\t"#x"=%u\n", __LINE__, __func__, __FILE__, x )
	#define _d_str(x) TRACE("%d\t%s\t"#x"=%s\n", __LINE__, __func__, x )
	#define _d_buf(address, size) TRACEx(address, size)
	#define _d_text(address, size) TRACEt(address, size)
	#define s_sprintf sprintf
	#define TRACE printf

//#define error_message(x,y) _error_message(x,y, __LINE__, __func__ )
int _error_message( char* str, int val, int line_no, const char* file_name )
{
	TRACE("\tError Message\n");
	TRACE("%d\t%s\t%s\t%d\n", line_no, file_name, str, val );
	return 0;
}
void* df_open( char*name, int flag )
{
	int h;
	if( flag == 1 ){
		h = open( name, O_RDONLY );
	}
	else{
		h = open( name, O_RDWR );
	}
	if( h == 0 ){
		//strerror( "h error" );
	}
	//_d_int( h );
	if( h == -1 )
		return 0;
	return (void*)(long)h;
}
int df_close( void* h )
{
	return close( (int)(long)h );
}
int df_write( void* h, char* buf, int size )
{
	return write( (int)(long)h, buf, size );
}
int df_read( void* h, char* buf, int size )
{
	return read( (int)(long)h, buf, size );
}
void* df_create( char*name, int flag, char* value )
{
	int h;
	df_delete( name );
	h = open( name, O_CREAT | O_RDWR, 0666 );
	if( flag > 0 ){
		// write ff content.
		char *buf;
		buf = (char*)malloc( flag );
		memset( buf, 0xff, flag );
		write( h, buf, flag );
	}
	else{
		// write ff content.
		char *buf;
		int len = 4096 - 128;
		buf = malloc( len );
		memset( buf, 0xff, flag );
		write( h, buf, len );
	}
	close( h );
	return df_open( name, 0 );
}
int df_delete( char*name, int flag )
{
	unlink( name );
}
int df_seek( void* h, int offset, int pos )
{
	return lseek( (int)(long)h, offset, pos );
	//return tell( (int)h );
}

#endif

#ifdef _d_dir_file_modules

// file struct
// struct file
//    block_flag[128] // block_flag[0] is flag for loop block
//    byte record_size[2]
//    struct string
// index file
//    record_flag[4096] // set the index record status
//    key node[4096]
//  record file
//    record_flag[4096] // set the record file record status
//    record_content.



#define _d_index_block_record_count (4*1024)
#define _d_struct_file_block_flag_count (128)
#define _d_index_file_record_flag_count _d_index_block_record_count
#define _d_index_key_node_struct_size (3*4)
#define _d_record_file_record_flag_count _d_index_block_record_count

#define _d_index_struct_file_ext_name ".s"
#define _d_index_index_file_ext_name ".i0"
#define _d_index_index_file_ext_name_base ".i"
#define _d_index_record_file_ext_name ".r0"
#define _d_index_record_file_ext_name_base ".r"
#define _d_index_key_file_ext_name ".k0"
#define _d_index_key_file_ext_name_base ".k"


#define _d_record_max_count 64




// function define
void *index_alloc_block_handle( _s_index *pi, int block_number );
int index_set_key( _s_index *pi, int key_no );
void* index_alloc_key_handle( _s_index *pi, int block );








int index_init()
{
	return 0;
}
int index_get_block_and_record_by_physical_id( _s_index *pi, int physical_id, int* block, int* record )
{
	int tmp_block, tmp_record;
	tmp_block = physical_id / _d_index_block_record_count;
	tmp_record = physical_id % _d_index_block_record_count;
	if( block )	*block = tmp_block;
	if( record )	*record = tmp_record;
	return 0;
}
int index_get_physical_id_by_block_and_record( _s_index *pi, int block, int record )
{
	return block * _d_index_block_record_count + record;
}
int index_get_physical_id( _s_index *pi, int block, int record )
{
	return  index_get_physical_id_by_block_and_record( pi, block, record );
}
int index_use_new_block_file( _s_index *pi, int block_number )
{
	void *h;
	char fn[128];
	// check weather out of range for block
	if( block_number > 127 ){
		error_message( "too big block_number", 0 );
		return -1;
	}
	if( pi->loop_blocks > 0 && block_number >= pi->loop_blocks ){
		error_message( "block number is too big for loop blocks",0);
		return -1;
	}
	// write block flag.
	s_sprintf( fn, "%s%s", pi->file_name, _d_index_struct_file_ext_name );
	h = df_open( fn,0 );
	if( h == 0 ){
		error_message( "can not open struct file",0);
		return -1;
	}
	df_seek( h, block_number+1, 0 );
	df_write( h, "\xfe", 1 );
	df_close( h );
	// create record file.
	s_sprintf( fn, "%s%s%d", pi->file_name, _d_index_record_file_ext_name_base, block_number );
	h = df_create( fn, pi->record_size * _d_index_block_record_count + _d_index_block_record_count, 0 );
	if( h == 0 ){
		error_message( "can not create record file", 0 );
		return -1;
	}
	// change pi->handle
	if( pi->h )
		df_close( pi->h );
	pi->h = h;
	pi->loop_blocks_tail = block_number;
	
	// for key file.
	if( pi->now_key == -1 )
		return 0;
	s_sprintf( fn, "%s%s%d_%d", pi->file_name, _d_index_key_file_ext_name_base, pi->now_key, block_number );
	h = df_create( fn, _d_index_block_record_count * 12, 0 );
	if( h == 0 ){
		error_message( "can not create key file", 0 );
		return 0;
	}
	if( pi->kh )
		df_close( pi->kh );
	pi->kh = h;
	return 0;
}
int index_delete( char *file_name, int flag )
{
	char fn[40];
	int no;
	
	s_sprintf( fn, "%s%s", file_name, _d_index_struct_file_ext_name );
	df_delete( fn, 0 );

	for( no=0; no<16; no++ ){
		s_sprintf( fn, "%s%s%d", file_name, _d_index_record_file_ext_name_base, no);
		df_delete( fn, 0 );
		s_sprintf( fn, "%s%s%d_%d", file_name, _d_index_key_file_ext_name_base, 0, no );
		df_delete( fn, 0 );
	}
	return 0;
}
int index_create( _s_index *pi, char *file_name, int flag, int record_size, char* record_struct )
{
	char fn[128];
	void *h;
	memset( pi, 0, sizeof( _s_index ) );
	strcpy( pi->file_name, file_name );
	pi->record_size = record_size;
	if( flag > 0 )
		pi->loop_blocks = flag;
	pi->now_block = 0;
	pi->now_record = 0;
	pi->now_key = -1;
	// now create files.
	// create struct file.
	strcpy( fn, pi->file_name );
	strcat( fn, _d_index_struct_file_ext_name );
	h = df_create( fn, 4096-128, 0 );
	if( h == 0 ){
		error_message( "can not create struct file", 0 );
		return 0;
	}
	memset( fn, 0xff, sizeof(fn) );
	fn[0] = pi->loop_blocks;
	if( pi->loop_blocks == 0 ){
		// set use first block file.
		fn[1] = 0xfe;
	}
	df_write( h, fn, sizeof( fn ) );
	df_write( h, (char*)(&pi->record_size), 2 );
#if 0
	if( record_struct ){
		df_write( h, record_struct, strlen( (char*)record_struct ) );
	}
#endif
	df_close( h );
	index_use_new_block_file( pi, 0 );
	df_close( pi->h );
	return index_open( pi, file_name, 0 );
}
int index_check_record_file_exist( char* file_name, int no )
{
	void *h;
	char fn[40];
	s_sprintf( fn, "%s%s%d", file_name, _d_index_record_file_ext_name_base, no);
	//_d_str( fn );
	h = df_open( fn, 0 );
	if( h == 0 )
		return -1;
	df_close(h);
	return 1;
}
int index_check_index_and_record_file_exist( char* file_name )
{
	void *h;
	char fn[40];
	strcpy( fn, file_name );
	strcat( fn, _d_index_struct_file_ext_name );
	h = df_open( fn, 0 );
	if( h == 0 )
		return -1;
	df_close( h );
	strcpy( fn, file_name );
	strcat( fn, _d_index_record_file_ext_name );
	h = df_open( fn, 0 );
	if( h == 0 )
		return -1;
	df_close(h);
	return 1;
}
#if 0
int index_get_now_record( void *h )
{
	#define _d_check_now_record_buffer_count 64
	int offset;
	char buffer[ _d_check_now_record_buffer_count ];
	int size = _d_index_file_record_flag_count;
	int now_record;
	int rel;
	int i;
	
	now_record = 0;
	offset = df_seek( h, 0, 1 );
	df_seek( h, 0, 0 );
	while( 1 ){
		rel = df_read( h, buffer, _d_check_now_record_buffer_count );
		if( rel <= 0 ){
			goto index_get_now_record_end;
		}
		for( i=0; i<_d_check_now_record_buffer_count; i++ ){
			if( buffer[i] != 0xff )	break;
		}
		if( i<_d_check_now_record_buffer_count ){
			// maybe find the record position.
			now_record += i;
			goto index_get_now_record_end;
		}
		now_record += _d_check_now_record_buffer_count;
	}
	return -1;
index_get_now_record_end:
	df_seek( h, offset, 0 );
	return now_record;
}
#endif
int index_close( _s_index *pi )
{
	if( pi->h ){
		df_close( pi->h );
		pi->now_block = -1;
	}
	if( pi->now_key != -1 ){
		df_close( pi->kh );
		pi->now_key = -1;
	}
	return 0;
}
int index_open( _s_index *pi, char *file_name, int flag )
{
	int i;
	void *h;
	char fn[128];
	
	//_d_line();
	if( index_check_index_and_record_file_exist( file_name ) != 1 )
		return -1;
	memset( pi, 0, sizeof( _s_index ) );
	strcpy( pi->file_name, file_name );
	
	strcpy( fn, pi->file_name );
	strcat( fn, _d_index_struct_file_ext_name );

	//_d_line();
	h = df_open( fn, 0 );
	df_read( h, (char*)&pi->loop_blocks, 1 );
	df_seek( h, 128, 0 );
	df_read( h, fn, sizeof(fn) );
	pi->record_size = fn[0];
	df_seek( h, 0, 0 );
	df_read( h, fn, sizeof(fn) );
	
	//_d_line();
	if( pi->loop_blocks > 0 ){
		//_d_line();
	}
	else{
		//_d_line();
		// not is loop block, get the now_block.
		for( i=1; i<sizeof(fn); i++ ){
			if( (unsigned char)fn[i] != 0xff )
				break;
		}
		if( i == sizeof(fn) ){
			//_d_line();
			while(1);
		}
		if( i < sizeof(fn) ){
			int now_block = i-1;
			if( index_check_record_file_exist( file_name, now_block ) < 0 ){
				error_message( "can not found block file", now_block );
				memset( pi, 0, sizeof(_s_index) );
				return -1;
			}
		}
	}
	df_close( h );

	strcpy( fn, pi->file_name );
	strcat( fn, _d_index_record_file_ext_name );
	h = df_open( fn, 0 );
	pi->h = h;
	pi->now_key = -1;
#if 0
	// find the next record .
	pi->now_record = index_get_now_record( h );
	if( pi->now_record == -1 ){
		df_close( h );
		error_message( "now record error", 0 );
		memset( pi, 0, sizeof(_s_index) );
		return -1;
	}
#endif 
	_d_line();
	if( index_set_end_physical_id( pi ) < 0 ){
	_d_line();
		error_message( "set end record pointer error", 0 );
		df_close( pi->h );
		memset( pi, 0, sizeof( _s_index ) );
		return -1;
	}
	_d_line();

	return 0;
}
int sub_index_calc_record_point_by_now_record_and_now_block( _s_index *pi )
{
	if( pi->loop_blocks > 0 ){
		//count = index_get_valid_block_count( pi );
		pi->now_block = (pi->now_block) % pi->loop_blocks;
	}
	if( index_use_new_block_file( pi, pi->now_block ) < 0 ){
		error_message( "use new block file error", pi->now_block );
		//pi->now_record = old_record;
		//pi->now_block = old_block;
		return -1;
	}
	return 0;
}
int index_create_next_new_record( _s_index *pi )
{
	char fn[40];
	int tmp_block;
	void *h;
	int old_record = pi->now_record;
	int old_block = pi->now_block;
	int count;
	int rel;

	pi->now_record ++;
	if( pi->now_record < _d_index_block_record_count )
		return 0;
	pi->now_record = 0;
	pi->now_block ++;
	rel = sub_index_calc_record_point_by_now_record_and_now_block( pi );
	if( rel < 0 ){
		pi->now_record = old_record;
		pi->now_block = old_block;
	}
	return 0;
}
int index_insert( _s_index *pi, int index, void* content )
{
	void *h;
	int pos;
	int count;
	int tmp_now_record = 0;
	int now_physical_id;
	_s_key_node kn;
	int rel;
	int stop_physical_id;
	int found_id;

	_d_int( pi->now_block );
	_d_line();

	// all new record content will write the tail.
	// write the key pointer.
	if( pi->loop_blocks > 0 && index != -1 ){
		error_message( "can not insert in loop block record file",0);
		return -1;
	}
	_d_line();
	if( pi->now_key != -1 ){
		if( pi->now_block == 0 && pi->now_record == 0 && pi->loop_blocks == 0 ){
			found_id = -1;
			stop_physical_id = -1;
		}
		else{
			found_id = index_search( pi, content, &stop_physical_id );
			_d_int( stop_physical_id );
			// found_id not equ -1 , find the same record. here not alow insert.
			// FIXME
			if( found_id != -1 ){
				_d_int( found_id );
				error_message( "can not insert same record", found_id );
				return -1;
			}
		}
	}
	_d_line();
	h = index_alloc_block_handle( pi, pi->now_block );
	_d_line();
	// get record count.
	tmp_now_record = count = index_get_block_record_count( pi, pi->now_block );
	// write content.
	pos = tmp_now_record * pi->record_size + _d_index_file_record_flag_count;
	df_seek( h, pos, 0 );
	df_write( h, content, pi->record_size );
	// write record flag.
	pos = tmp_now_record;
	df_seek( h, pos, 0 );
	df_write( h, "\xfe", 1 );

	_d_line();
	now_physical_id =  index_get_physical_id( pi, pi->now_block, tmp_now_record );
	// write index key struct 
	_d_line();
	if( pi->now_key != -1 && stop_physical_id != -1 ){
		_d_line();
		// have key 
		// X 1 write new record content.
		// 2 calc the parent point.
		// 	use the search function. need move it to function head ??????
		// 3 write parent's left or right point.
		index_get_key_node_by_physical_id( pi, stop_physical_id, &kn );
		if( found_id != -1 ){
			if( kn.right != -1 ){
				error_message( "this node right not empty", (int)(long)kn.left );
				return -1;
			}
			kn.right = now_physical_id;
		}
		else{
			// left or right
			char buf[ _d_record_max_count ];
			index_get_record_by_physical_id( pi, stop_physical_id, buf, sizeof(buf) );
			rel = index_compare_action( pi, content, buf );
			if( rel < 0 ){
				kn.left = now_physical_id;
			}
			else if( rel > 0 ){
				kn.right = now_physical_id;
			}
			else{
				error_message( "should not equ.", rel );
				return -1;
			}
		}
		index_set_key_node_by_physical_id( pi, stop_physical_id, &kn );
		// 4 write parent point.
		index_get_key_node_by_physical_id( pi, now_physical_id, &kn );
		if( kn.parent != -1 ){
			error_message( "this node left not empty", (int)(long)kn.left );
			return -1;
		}
		kn.parent = stop_physical_id;
		index_set_key_node_by_physical_id( pi, now_physical_id, &kn );
	}


	_d_line();
	if( index_create_next_new_record( pi ) < 0 )
		return -1;

	_d_line();
	return 0;
}
int old_index_insert( _s_index *pi, int index, void* content )
{
	void *h;
	int pos;
	// no key only append.
	if( pi->now_key == -1 ){
		if( pi->loop_blocks > 0 && index != -1 ){
			error_message( "can not insert in loop block record file",0);
			return -1;
		}
		if( index == -1 ){
			// do append action.
			int pos;
			int count;
			// get record file handle.
			h = index_alloc_block_handle( pi, pi->now_block );
			// get record count.
			index = count = index_get_block_valid_record_count( pi, pi->now_block );
			// write content.
			pos = index * pi->record_size + _d_index_file_record_flag_count;
			df_seek( h, pos, 0 );
			df_write( h, content, pi->record_size );
			// write record flag.
			pos = index;
			df_seek( h, pos, 0 );
			df_write( h, "\xfe", 1 );
			if( index_create_next_new_record( pi ) < 0 )
				return -1;
		}
		else{
			// do insert action.
		}
	}
	else{
		// have key 
		// 1 write new record content.
		// 2 calc the parent point.
		// 3 write parent point.
		// 4 write parent's left or right point.
	}
	return 0;
}
int index_append( _s_index *pi, void* content )
{
	// FIXME the same record not insert in DB.
	index_insert( pi, -1, content );
	return 0;
}
void *index_alloc_block_handle( _s_index *pi, int block_number )
{
	void *h;
	int block;
	int offset;
	char fn[40];

	block = block_number;
	if( block == pi->now_block )
		h = pi->h;
	else{
		s_sprintf( fn, "%s%s%d", pi->file_name, _d_index_record_file_ext_name_base, block );
		h = df_open( fn, 0 );
	}
	return h;
}
void *index_free_block_handle( _s_index *pi, int block_number, void *h )
{
	if( block_number == pi->now_block ){
	}
	else{
		df_close( h );
	}
	return 0;
}
int index_remove( _s_index *pi, int index )
{
	void *h;
	int block;
	int offset;
	int physical_id;

	_d_int( index );
	physical_id = index_get_physical_id_by_logic_id( pi, index );
	_d_int( physical_id );
	block = physical_id / _d_index_block_record_count;
	offset = physical_id % _d_index_block_record_count;
	_d_line();
	h = index_alloc_block_handle( pi, block );
	df_seek( h, offset, 0 );
	df_write( h, "", 1 );
	_d_line();
	index_free_block_handle( pi, block, h );
	_d_line();
	return 0;
}
int index_get_block_record_valid_flag( _s_index *pi, int index )
{
	// sum the deleted record and valid record. 
	// if flag is 0xff,  end sum action.
	void *h;
	int size;
	int block_number;
	int pos;
	char flag;
	
	block_number = index / _d_index_block_record_count ;
	pos = index % _d_index_block_record_count ;
	h = index_alloc_block_handle( pi, block_number );
	if( h == 0 )
		return 0;
	df_seek( h, pos, 0 );
	size = df_read( h, &flag, 1 );
	if( size <=0 ){
		index_free_block_handle( pi, block_number, h );
		error_message( "read error", pos );
		return -1;
	}
	index_free_block_handle( pi, block_number, h );
	return flag;
}
int index_get_block_record_count( _s_index *pi, int block_number )
{
	// sum the deleted record and valid record. 
	// if flag is 0xff,  end sum action.
	void *h;
	int count = 0;
	char fn[128];
	int i,size;
	int total_size = _d_index_block_record_count;
	
	h = index_alloc_block_handle( pi, block_number );
	if( h == 0 )
		return 0;
	df_seek( h, 0, 0 );
	while( total_size > 0 ){
		size = df_read( h, fn, sizeof(fn) );
		if( size <=0 ){
			goto index_get_block_count_end;
		}
		for( i=0; i<size; i++ ){
			if( (unsigned char)fn[i] == 0xff )
				break;
			//if( (unsigned char)fn[i] == 0 )
				//continue;
			count ++;
		}
		if( i < size )
			goto index_get_block_count_end;
		total_size -= size;
	}
	_d_int( count );
	return count;
	
index_get_block_count_end:	
	index_free_block_handle( pi, block_number, h );
	return count;
}
int index_get_block_valid_record_count( _s_index *pi, int block_number )
{
	// sum the deleted record and valid record. 
	// if flag is 0xff,  end sum action.
	void *h;
	int count = 0;
	char fn[128];
	int i,size;
	int total_size = _d_index_block_record_count;
	
	h = index_alloc_block_handle( pi, block_number );
	if( h == 0 )
		return 0;
	df_seek( h, 0, 0 );
	while( total_size > 0 ){
		size = df_read( h, fn, sizeof(fn) );
		if( size <=0 ){
			goto index_get_block_count_end;
		}
		for( i=0; i<size; i++ ){
			if( (unsigned char)fn[i] == 0xff )
				break;
			if( (unsigned char)fn[i] == 0 )
				continue;
			count ++;
		}
		if( i < size )
			goto index_get_block_count_end;
		total_size -= size;
	}
	_d_int( count );
	return count;
	
index_get_block_count_end:	
	index_free_block_handle( pi, block_number, h );
	return count;
}
int index_get_valid_block_count( _s_index *pi )
{
	void *h;
	char fn[_d_struct_file_block_flag_count];
	int i;
	int block= 0;
	
	if( pi->loop_blocks > 0 )
		return pi->loop_blocks;
	
	s_sprintf( fn, "%s%s", pi->file_name, _d_index_struct_file_ext_name );
	//s_sprintf( fn, "%s%s%d", pi->file_name, _d_index_record_file_ext_name_base, block );
	h = df_open( fn, 0 );
	if( h == 0 )
		return 0;
	df_read( h, fn, _d_struct_file_block_flag_count );

	for( i=1; i<_d_struct_file_block_flag_count; i++ ){
		if( (unsigned char)fn[i] == 0xff )
			break;
		block ++;
	}
	df_close( h );
	
	return block;
}
int index_get_valid_count( _s_index *pi )
{
	//sum all block record .
	int block_count;
	int i;
	int count = 0;
	
	block_count = index_get_valid_block_count( pi );
	
	for( i=0; i<block_count; i++ ){
		count += index_get_block_valid_record_count( pi, i );
	}
	return count;
}
#if 1
int index_get_count( _s_index *pi )
{
	//sum all block record .
	int block_count;
	int i;
	int count = 0;
	
	block_count = index_get_valid_block_count( pi );
	
	for( i=0; i<block_count; i++ ){
		count += index_get_block_record_count( pi, i );
	}
	return count;
}
#endif

int index_set_block( _s_index *pi, int now_block )
{
	int block;
	if( pi->now_block == now_block )
		return 0;
	block = index_get_valid_block_count( pi );
	if( pi->loop_blocks > 0 ){
		if( now_block >= pi->loop_blocks )
			now_block = now_block % pi->loop_blocks;
	}
	else{
		if( now_block >= block ){
			error_message( "too big block", block );
			return -1;
		}
	}
	if( index_check_record_file_exist( pi->file_name, now_block ) < 0 ){
		error_message(  "index_check_record_file_exist error", now_block );
		return -1;
	}
	//pi->now_block = now_block;
	if( pi->h )
		df_close( pi->h );
	pi->now_block = -1;
	if( pi->now_key != -1 ){
		if( pi->kh )
			df_close( pi->kh );
		pi->kh = index_alloc_key_handle( pi, now_block );
	}
	pi->h = index_alloc_block_handle( pi, now_block );
	pi->now_block = now_block;
	return 0;
}
int index_move_next_valid_block_point( _s_index *pi )
{
	#if 1
	return index_set_block( pi, pi->now_block + 1 );
	#else
	
	int block;
	block = index_get_valid_block_count( pi );
	if( pi->loop_blocks > 0 ){
		if( pi->now_block + 1 >= pi->loop_blocks )
			pi->now_block = 0;
	}
	else{
		if( pi->now_block + 1 >= block )
			return -1;
	}
	if( index_check_record_file_exist( pi->file_name, pi->now_block ) < 0 )
		return -1;
	return 0;

	#endif
}
int index_move_valid_record_point_in_block( _s_index *pi )
{
#if 1
	void *h;
	char flag;
	int pos;
	int rel;

	h = pi->h;
	pos = pi->now_record;
	while( pos < _d_index_block_record_count ){
		df_seek( h, pos, 0 );
		rel = df_read( h, &flag, 1 );
		if( rel <= 0 )
			return -1;
		if( (unsigned char)flag == 0xff )
			return -2;
		if( (unsigned char)flag == 0xfe )
			return pos;
		pos ++;
	}
	return -3; // need next block

#else
	char fn[128];
	int pos = 0;
	int size = sizeof( fn );
	int i;
	void *h;
	
	h = pi->h;
	while( 1 ){
		df_seek( h, pos, 0 );
		size = sizeof( fn );
		size = df_read( h, fn, size );
		if( size <= 0 )
			return -1;
		for( i= pi->now_record ; i<size; i++ ){
			// no next record , found failed.
			//_d_int( fn[i] );
			if( (unsigned char)fn[i] == 0xff )
				return -2;
			if( (unsigned char)fn[i] == 0xfe )
				return pos + i;
			// this record has been delete. find next.
			//if( fn[i] == 0 )	return 0;
		}
		pos += size;
	}
	return -1;
#endif
}
int index_move_valid_record_point( _s_index *pi )
{
	void *h;
	int rel;
	int next_block_flag = 0;
	while( 1 ){
		next_block_flag = 0;
		pi->now_record ++;
		if( pi->now_record >= _d_index_block_record_count ){
			next_block_flag = 1;
		}
		else{
			// check the record valid.
			rel = index_move_valid_record_point_in_block(pi);
			// no next record.
			if( rel == -3 ){
				next_block_flag = 1;
			}
			else if( rel == -2 )
				return -1;
			// read error
			else if( rel == -1 ){
				error_message( "move valid record point read error" , 0 );
				return -1;
			}
			else if( rel >= 0 ){
				return rel;
			}
		}
		// this block all record has delete.
		//if( rel == -1 )		break;
		if( next_block_flag == 1 ){
			pi->now_record = 0;
			if( index_move_next_valid_block_point(pi) < 0 )
				return -1;
			h = index_alloc_block_handle( pi, pi->now_block );
			if( h==0 )
				return -1;
			rel = index_move_valid_record_point_in_block(pi);
			if( rel == -2 )
				return -1;
			// read error
			else if( rel == -1 ){
				error_message( "move valid record point read error" , 0 );
				return -1;
			}
			else if( rel >= 0 ){
				return rel;
			}
			break;
		}
	}
	return 0;
}
int index_set_index( _s_index *pi, int lid )
{
	// if the record is deleted. return -1;
	int block;
	int index;
	index = lid;
	if( pi->now_key != -1 ){
		index = index_get_physical_id_by_logic_id( pi, lid );
		if( index == -1 )
			return -1;
	}
	block = index / _d_index_block_record_count;
	if( index_set_block( pi, block ) < 0 ){
		error_message( "set block error", block );
		return -1;
	}
	pi->now_record = index % _d_index_block_record_count;
	pi->now_index = lid;
	return 0;
}
#if 0
int index_get_index( _s_index *pi, int index, void *content, int content_size )
{
	// if the record is deleted. return -1;
	int rel;
	int pos;
	int size;
	rel = index_set_index( pi, index );
	if( rel == -1 ){
		error_message( "set index error", index );
		return -1;
	}
	pos = index * pi->record_size + _d_index_file_record_flag_count;
	df_seek( pi->h, pos, 0 );
	size = df_read( pi->h, content, content_size );
	return size;
}
#endif
int index_get_record( _s_index *pi, void *content, int content_size )
{
	// if the record is deleted. return -1;
	int rel;
	int pos;
	int size;
	pos = pi->now_record * pi->record_size + _d_index_file_record_flag_count;
	df_seek( pi->h, pos, 0 );
	size = df_read( pi->h, content, content_size );
	return size;
}
int index_get_record_by_physical_id( _s_index *pi, int pid, void *content, int content_size )
{
	int block, record;
	void *h;
	int rel;
	int pos;
	int size;
	block = pid / _d_index_block_record_count;
	record = pid % _d_index_block_record_count;
	h = index_alloc_block_handle( pi, block );

	pos = record * pi->record_size + _d_index_file_record_flag_count;
	df_seek( h, pos, 0 );
	if( content_size > pi->record_size )
		content_size = pi->record_size;
	size = df_read( h, content, content_size );
	index_free_block_handle( pi, block, h );

	return size;
}
int index_get_record_flag_by_block_and_record( _s_index *pi, int block, int record )
{
	void *h;
	int rel;
	int pos;
	int size;
	char flag;
	h = index_alloc_block_handle( pi, block );

	pos = record ;
	df_seek( h, pos, 0 );
	size = df_read( h, &flag, 1 );
	index_free_block_handle( pi, block, h );

	return (unsigned)flag;
}
int index_get_record_flag_by_physical_id( _s_index *pi, int pid )
{
	int block, record;
	index_get_block_and_record_by_physical_id( pi, pid, &block, &record );
	return index_get_record_flag_by_block_and_record( pi, block, record );
}


int index_compare_action( _s_index *pi, void* c1, void* c2, int length )
{
	int v1,v2;
	_d_int( pi->key_type );
	switch( pi->key_type ){
		case 1:
			s_memcpy( &v1, c1, sizeof(v1) );
			s_memcpy( &v2, c2, sizeof(v2) );
			return (int)((int)v1 - (int)v2);
		case 2:
			s_memcpy( &v1, c1, sizeof(v1) );
			s_memcpy( &v2, c2, sizeof(v2) );
			return (int)((unsigned)v1 - (unsigned)v2);
		case 3:
			return s_memcmp( c1, c2, length );
	}
	//return 0;
}








void* index_alloc_key_handle( _s_index *pi, int block )
{
	char fn[48];
	void *kh;
	if( block == pi->now_block ){
		return pi->kh;
	}
	else{
		s_sprintf( fn, "%s%s%d_%d", pi->file_name, _d_index_key_file_ext_name_base, pi->now_key, block );
		kh = df_open( fn, 0 );
		if( kh == 0 ){
			_d_str( fn );
			error_message( "can not open key file", block );
			return 0;
		}
		return kh;
	}
	return 0;
}
int index_free_key_handle( _s_index *pi, int block, void *h )
{
	if( block == pi->now_block ){
		return 0;
	}
	df_close( h );
	return 0;
}
int index_key_node_recursive( _s_index *pi, int pid )
{
	_s_key_node kn;
	int tid;
	index_get_key_node_by_physical_id( pi, pid, &kn );
	if( kn.left != -1 ){
		tid = index_key_node_recursive( pi, kn.left );
		if( tid >= 0 )
			return tid;
	}
	// show T
	if( pi->recursive_action == 1 ){
		if( pi->recursive_find_logic_id == pi->recursive_now_logic_id )
			return pid;
	}
	else if( pi->recursive_action == 2 ){
		// show content.
	}
	pi->recursive_now_logic_id ++;
	if( kn.right != -1 ){
		tid = index_key_node_recursive( pi, kn.right );
		if( tid >= 0 )
			return tid;
	}
	return -1;
}
int index_get_physical_id_by_logic_id( _s_index *pi, int logic_ic )
{
	int pid;
	if( pi->now_key == -1 )
		return logic_ic;
	pi->recursive_find_logic_id = logic_ic;
	pi->recursive_now_logic_id = 0;
	pi->recursive_action = 1;
	pid = index_key_node_recursive( pi, 0 );
	return pid;
}
#if 0
int index_get_physical_id_by_logic_id( _s_index *pi, int logic_ic )
{
	_s_key_node kn;
	int lid,pid;
	if( pi->now_key == -1 ){
		return logic_ic;
	}
	// here is diffcial.
	pid = 0;
	while( 1 ){
		index_get_key_node_by_physical_id( pi, pid, &kn );
		if( kn.left == -1 )
			break;
		pid = kn.left;
	}
	// find the next logic record.
	lid = 0;
	while( 1 ){
		// enter left
		while( 1 ){
			index_get_key_node_by_physical_id( pi, pid, &kn );
			if( kn.left == -1 )
				break;
			pid = kn.left;
		}
		// show this.
		_d_int( pid );
		// enter right.
		if( kn.right != -1 ){
			pid = kn.right;
			continue;
		}
		if( kn.parent != -1 ){
			int tpid;
			while( 1 ){
				tpid = kn.parent;
				index_get_key_node_by_physical_id( pi, pid, &kn );
				if( kn.right != pid ){
					break;
				}
				pid = tpid;
			}
			// enter right.
			if( kn.right != -1 ){
				pid = kn.right;
				continue;
			}
		}
	}





	while( 1 ){
		if( lid == logic_id )
			return pid;
		index_get_key_node_by_physical_id( pi, pid, &kn );
		// find the left
		if( kn.left != -1 ){
			pid = kn.left;
			continue;
		}
		if( kn.right != -1 ){
			pid = kn.right;
			continue;
		}
		if( kn.parent != -1 ){
			int tpid;
			while( 1 ){
				tpid = kn.parent;
				index_get_key_node_by_physical_id( pi, pid, &kn );
				if( kn.right != pid ){
					break;
				}
				pid = tpid;
			}
		}
	}
	return -1;
}
#endif
int index_get_key_node_by_physical_id( _s_index *pi, int index, _s_key_node *kn )
{
	int block, record;
	void *h;
	char fn[64];

	if( kn == 0 )
		return 0;
	index_get_block_and_record_by_physical_id( pi, index, &block, &record );
	h = index_alloc_key_handle( pi, block );
	df_seek( h, record * _d_index_key_node_struct_size, 0 );
	df_read( h, (char*)kn, sizeof( _s_key_node ) );
	index_free_key_handle( pi, block, h );
	return 0;
}
int index_set_key_node_by_physical_id( _s_index *pi, int index, _s_key_node *kn )
{
	int block, record;
	void *h;
	char fn[64];

	if( kn == 0 )
		return 0;
	index_get_block_and_record_by_physical_id( pi, index, &block, &record );
	h = index_alloc_key_handle( pi, block );
	df_seek( h, record * _d_index_key_node_struct_size, 0 );
	df_write( h, (char*)kn, sizeof( _s_key_node ) );
	index_free_key_handle( pi, block, h );
	return 0;
}
int index_key_search( _s_index *pi, void *content, int* stop_physical_id )
{
	_s_key_node kn;
	int index = 0;
	char buf[ _d_record_max_count ];
	int rel;
	while( 1 ){
		index_get_key_node_by_physical_id( pi, index, &kn );
		index_get_record_by_physical_id( pi, index, buf, sizeof(buf) );
		*stop_physical_id = index;
		rel = index_compare_action( pi, content, buf, pi->record_size );
		_d_str( (char*)content );
		_d_str( buf );
		_d_int( rel );
		if( rel < 0 ){
			index = (int)(long)kn.left;
			if( index == -1 ) return -1;
		}
		else if( rel > 0 ){
			index = (int)(long)kn.right;
			if( index == -1 ) return -1;
		}
		else	break;
	}
	return index;
}
int index_search( _s_index *pi, void* content, int *stop_physical_id )
{
	int index;
	char buf[ _d_record_max_count ];
	if( pi->now_key == -1 ){
		// use normal list squ. to search record.
		index_begin( pi );
		*stop_physical_id = index_get_physical_id( pi, pi->now_block, pi->now_record );
		while( 1 ){
			index_get_record( pi, buf, pi->record_size );
			// compare function.
			if( index_compare_action( pi, content, buf, pi->record_size ) == 0 ){
				return 0;
			}
			//index_move_next_valid_record( pi );
			if( index_move_valid_record_point( pi ) < 0 ){
				return -1;
			}
			*stop_physical_id = index_get_physical_id( pi, pi->now_block, pi->now_record );
		}
	}
	else{
		// use key search list.
		index = index_key_search( pi, content, stop_physical_id );
		return index;
	}
	return -1;
}
int index_update( _s_index *pi, int index, void* content )
{
	// if no key , will do append action.
	// if have key:
	// 	delete old index content
	// 	new insert one content.
	index_remove( pi, index );
	index_append( pi, content );
	return 0;
}


int index_begin( _s_index *pi )
{
	int block;
	if( pi->now_key != -1 ){
		// find the logic_ic is 0.
		int record;
		int pid;
		pid = index_get_physical_id_by_logic_id( pi, 0 );
		if( pid < -1 )
			return -1;
		index_get_block_and_record_by_physical_id( pi, pid, &block, &record );
		index_set_block( pi, block );
		pi->now_record = record;
		pi->now_index = 0;
		return 0;
	}
	if( pi->loop_blocks > 0 ){
		//pi->now_block = pi->loop_blocks_tail;
		block = pi->loop_blocks_tail ;
		block += 1;
		block %= pi->loop_blocks;
		index_set_block( pi, block );
		pi->now_record = 0;
	}
	else{
		//pi->now_block = 0;
		index_set_block( pi, 0 );
		pi->now_record = 0;
	}
	return 0;
}
int index_set_end_physical_id( _s_index *pi )
{
	int i;
	int block;
	char flag;

	_d_line();
	block = index_get_valid_block_count( pi );
	if( pi->loop_blocks > 0 ){
		pi->now_block = -1;
	}
	else{
	}
	for( i=0; i<block; i++ ){
		_d_int( i );
		if( index_set_block( pi, i ) < 0 )
			break;
		flag = index_get_block_record_valid_flag( pi, i * _d_index_block_record_count + (_d_index_block_record_count-1) );
		if( (unsigned char)flag == 0xff )
			break;
	}
	if( i<block ){
	_d_line();
		pi->loop_blocks_tail = i-1;
	}
	else{
	_d_line();
		error_message( "found end block failed", i );
		return -1;
	}
	pi->now_block = pi->loop_blocks_tail;
	//pi->now_record = index_get_block_valid_record_count( pi, pi->now_block );
	pi->now_record = index_get_block_record_count( pi, pi->now_block );
	_d_int( pi->now_block );
	_d_int( pi->now_record );
	return 0;
}
int index_next( _s_index *pi )
{
	unsigned flag;
	if( pi->now_key == -1 ){
		return index_move_valid_record_point( pi );
	}
	if( index_set_index( pi, pi->now_index + 1 ) < 0 )
		return -1;
	// check the record valid.
	flag = index_get_record_flag_by_block_and_record( pi, pi->now_block, pi->now_record );
	if( (unsigned)flag == 0xff )
		return -1;
	_d_hex(flag);
	_d_int( pi->now_block );
	_d_int( pi->now_record );
	return 0;
}





int index_add_key( _s_index *pi, int key_offset, int key_length, int key_type )
{
	// now only support one key
	
	// add key info in struct file.
	int key_no = 0;
	void *h;
	char fn[64];
	strcpy( fn, pi->file_name );
	strcat( fn, _d_index_struct_file_ext_name );
	h = df_open( fn, 0 );
	if( h == 0 ){
		error_message( "can not open the struct file", 0 );
		return -1;
	}
	df_seek( h, 128 + 2/* record size */, 0 );
	df_write( h, (char*)&key_offset, 4 );
	df_write( h, (char*)&key_length, 4 );
	df_write( h, (char*)&key_type, 4 );
	df_close( h );
	// create the *.k(no) file.
	s_sprintf( fn, "%s%s%d_%d", pi->file_name, _d_index_key_file_ext_name_base, key_no, 0 );
	h = df_create( fn, _d_index_block_record_count * 12, 0 );
	if( h == 0 ){
		error_message( "can not open the struct file", 0 );
		return -1;
	}
	df_close( h );
	// sort the exist record.
	// index_resort_record_by_key( pi, key_no );
	return 0;
}
int index_set_key( _s_index *pi, int key_no )
{
	// set key no into pi struct
	// all read record by index_get_index will use key file index.

	void *h;
	void *kh;
	char fn[64];
	int key_offset, key_length, key_type;

	s_sprintf( fn, "%s%s", pi->file_name, _d_index_struct_file_ext_name );
	h = df_open( fn, 0 );
	if( h == 0 ){
		error_message( "can not open struct file", 0 );
		return -1;
	}
	s_sprintf( fn, "%s%s%d_%d", pi->file_name, _d_index_key_file_ext_name_base, key_no, pi->now_block );
	kh = df_open( fn, 0 );
	if( kh == 0 ){
		error_message( "can not open key file", 0 );
		return -1;
	}
	pi->kh = kh;
	pi->now_key = key_no;
	df_seek( h, 128 + 2/*record_size*/ + 4*4 * key_no, 0 );
	df_read( h, (char*)&key_offset, 4 );
	df_read( h, (char*)&key_length, 4 );
	df_read( h, (char*)&key_type, 4 );
	df_close( h );

	pi->key_offset = key_offset;
	pi->key_length = key_length;
	pi->key_type = key_type;
	pi->key_function = 0;

	return 0;
}
int index_close_key( _s_index *pi )
{
	// set no use key
	pi->now_key = -1;
	df_close( pi->kh );
	pi->key_offset = 0;
	pi->key_length = 0;
	pi->key_type = 0;
	pi->key_function = 0;
	return 0;
}
int index_remove_key( _s_index *pi, int key_no )
{
	// delete the key record.
	// now only support one key
	void *h;
	void *kh;
	char fn[64];
	char buf[4*4];

	s_sprintf( fn, "%s%s", pi->file_name, _d_index_struct_file_ext_name );
	h = df_open( fn, 0 );
	if( h == 0 ){
		error_message( "can not open struct file", 0 );
		return -1;
	}
	pi->kh = kh;
	pi->now_key = key_no;
	df_seek( h, 128 + 4*4 * key_no, 0 );
	memset( buf, 0xff, sizeof( buf ) );
	df_write( h, buf, sizeof( buf ) );
	df_close( h );

	s_sprintf( fn, "%s%s%d", pi->file_name, _d_index_key_file_ext_name, key_no );
	df_delete( fn, 0 );
	return 0;
}
int index_move_next_record_by_key( _s_index pi, int key_no )
{
	// get key_no next index.
	// calc index to now_block and now_record.
	return 0;
}












int index_debug_list_record2( char* fn )
{
	_s_index spi;
	_s_index *pi = &spi;
	char buf[32];
	int count;
	int block;
	int record;
	int pid;
	int i;

	if( index_open( pi, fn, 0 ) < 0 ){
		error_message( "no this index file", 0 );
		return -1;
	}
	index_set_key( pi, 0 );
	count = index_get_valid_count( pi );
	_d_int( pi->now_key );
	_d_int( count );
	//index_set_begin_record( pi );
	for( i=0; i<count; i++ ){
		pid = index_get_physical_id_by_logic_id( pi, i );
		if( pid < -1 ){
			error_message( "find physical id by logic id error", i );
			return -1;
		}
		_d_int( pid );
		index_get_block_and_record_by_physical_id( pi, pid, &block, &record );
		index_set_block( pi, block );
		pi->now_record = record;
		if( index_get_record( pi, buf, sizeof( buf ) ) < 0 ){
			error_message( "get index error", pi->now_record );
			return -1;
		}
		buf[ pi->record_size ] = 0;
		//_d_str( buf );
		switch( pi->key_type ){
			case 2:
				_d_int( *(int*)buf );
				break;
			case 3:
			default:
				_d_str( buf );
				break;
		}
	}
	_d_line();
	index_close( pi );
}


static int index_debug_list_record( char* fn )
{
	_s_index spi;
	_s_index *pi = &spi;
	char buf[32];

	if( index_open( pi, fn, 0 ) < 0 ){
		error_message( "no this index file", 0 );
		return -1;
	}
	index_set_key( pi, 0 );
	index_begin( pi );
#if 0
	if( index_set_index( pi, 0 ) < 0 ){
		error_message( "set index 0 error", 0 );
		return -1;
	}
#endif
	while( 1 ){
		if( index_get_record( pi, buf, sizeof( buf ) ) < 0 ){
			error_message( "get index error", pi->now_record );
			return -1;
		}
		buf[ pi->record_size ] = 0;
		_d_str( buf );
		if( index_move_valid_record_point( pi ) < 0 ){
			//error_message( "move valid_record point error", 0 );
			//return -1;
			break;
		}
	}
	index_close( pi );
	return 0;
}
static int basic_append_test( char* fn )
{
	_s_index spi;
	_s_index *pi = &spi;
	char buf[13];

	if( index_open( pi, fn, 0 ) < 0 )
		index_create( pi, fn, 0, 12, 0 );
	strcpy( buf, "123456789012" );
	index_append( pi, buf );
	index_append( pi, buf );
	index_close( pi );
	index_debug_list_record( fn );

	return 0;
}
static int loop_blocks_board_read_test( char* fn )
{
	int i;
	_s_index spi;
	_s_index *pi = &spi;
	char buf[13];
	int board_count = 50000;
	//int board_count = 2*_d_index_block_record_count + 1;
	//int board_count = _d_index_block_record_count ;
	//int board_count = _d_index_block_record_count + 1;

	//index_open( pi, fn, 0 );
	//index_close( pi );
	index_debug_list_record( fn );

	return 0;
}
static int loop_blocks_board_append_test( char* fn )
{
	int i;
	_s_index spi;
	_s_index *pi = &spi;
	char buf[13];
	int board_count = 60000;
	//int board_count = 2*_d_index_block_record_count + 1;
	//int board_count = _d_index_block_record_count ;
	//int board_count = _d_index_block_record_count + 1;

	index_create( pi, fn, 4, 12, 0 );
	for( i=0; i<board_count; i++ ){
		s_sprintf( buf, "%d", i );
		index_append( pi, buf );
	}
	index_close( pi );
	index_debug_list_record( fn );

	return 0;
}
static int board_append_test( char* fn )
{
	int i;
	_s_index spi;
	_s_index *pi = &spi;
	char buf[13];
	int board_count = 30000;
	//int board_count = 2*_d_index_block_record_count + 1;
	//int board_count = _d_index_block_record_count;

	index_create( pi, fn, 0, 12, 0 );
	for( i=0; i<board_count; i++ ){
		s_sprintf( buf, "%d", i );
		index_append( pi, buf );
	}
	index_close( pi );
	index_debug_list_record( fn );

	return 0;
}
static int rand_sort_insert_test( char* fn )
{
	_s_index spi;
	_s_index *pi = &spi;
	char buf[13];
	int no = 0;
	int val;
	int i;

	if( index_open( pi, fn, 0 ) < 0 ){
		index_create( pi, fn, 0, 12, 0 );
		no = index_add_key( pi, 0, 12, 2/*unsigned*/ );
	}
	index_set_key( pi, no );

	for( i=0; i<4096*2-1; i++ ){
		//val = rand();
		val = 4096*2-1-i;
		//_d_int( val );
		if( pi->key_type == 3 ){
			s_sprintf( buf, "%d", val );
			index_append( pi, buf );
		}
		else{
			index_append( pi, &val );
		}
	}

	index_close( pi );

	//index_debug_list_record( fn );
}
static int base_sort_insert_test( char* fn )
{
	_s_index spi;
	_s_index *pi = &spi;
	char buf[13];
	int no = 0;

	_d_line();
	if( index_open( pi, fn, 0 ) < 0 ){
		index_create( pi, fn, 0, 12, 0 );
		no = index_add_key( pi, 0, 12, 3/*string*/ );
	}
	index_set_key( pi, no );
	strcpy( buf, "5" );
	index_append( pi, buf );
	strcpy( buf, "1" );
	index_append( pi, buf );
	strcpy( buf, "6" );
	index_append( pi, buf );
	strcpy( buf, "3" );
	index_append( pi, buf );
	index_close( pi );

	index_debug_list_record( fn );
}
static int test()
{
	char *fn = "/c/ttt";
	_d_line();
	//basic_append_test( fn );
	//board_append_test( fn );
	//loop_blocks_board_append_test( fn );
	//loop_blocks_board_read_test( fn );
	index_delete( "/c/ttt", 0 );
	base_sort_insert_test( fn );
	//rand_sort_insert_test( fn );
	index_debug_list_record2( fn );
	return 0;
}

#ifdef _d_pc_test

// simluate file interface.

// test function.
int pc_test_index()
{
	test();
}

#endif


int index_test()
{
	_d_line();
	test();
}



#else


int index_init()
{
	return 0;
}
int index_test()
{
	return 0;
}



#endif

