#include "os.h"
#include "vfs/dir_file.h"
#include "txt_file_index.h"
#include "config_file.h"
#include "mp_misc.h"

#ifdef _d_dir_file_modules


#if 0
ȫ�ֱ�����
block_count
block_record_count
record_size
record_map
now_block
now_record

������
card_record_init
card_record_get_now_block_and_record
make_record // ͨ��record_map�����ɼ�¼���ݣ� �ɹ��û��Լ����Ƽ�¼���ݡ�

���̣�
1 ����д���¼���š�
2 ȡ��������Ϣ����ʱ��ȣ������û�ָ����¼���ݡ�
3 ȡ�õ�ǰ���ļ����ļ���ƫ�ơ�
4 д���¼��д��λͼ��ǡ�
5 �ر��ļ�
6 ������һ����¼λ�ã��糬��������������� ɾ�����¿飬��������һ���ļ���
7 ��ɡ�
#endif


#if 1
// debug data
int block_count=5;
int block_record_count=20;
int record_size=16;
char *record_map="dtc";
int now_block=0;
int now_record=0;
#define _d_card_record_file_name "/c/card_recor.b"
#endif

#define _d_bitmap_position(x) (x)
#define _d_bitmap_size (block_record_count)
#define _d_record_position(x) (_d_bitmap_size + x*record_size)
#define _d_block_size (_d_bitmap_size + block_record_count * record_size)


int card_record_get_now_block_and_record()
{
	char file_name[24];
	void *h;
	int i;
	char *buf;
	int size;

	// check the block
	for( i=0; i<block_count; i++ ){
		s_sprintf( file_name, "%s%d", _d_card_record_file_name, i );
		h = df_open( file_name, 0 );
		if( h == 0 )
			break;
		df_close( h );
	}
	//_d_int( i );
	if( i==block_count ){
		// error, not all block file exist.
		_d_line();
		while( 1 );
	}
	if( i==0 ){
		//_d_line();
		s_sprintf( file_name, "%s%d", _d_card_record_file_name, (block_count-1) );
		h = df_open( file_name, 0 );
		if( h == 0 )
			now_block = 0;
		else{
			now_block = block_count - 1;
			df_close( h );
		}
	}
	else now_block = i-1;
	
	s_sprintf( file_name, "%s%d", _d_card_record_file_name, now_block );
	h = df_open( file_name, 0 );
	if( h == 0 ){
		df_create( file_name, _d_block_size, 0 );
	}
	df_close( h );

	// find now record.
	buf = alloc_temp_buffer(1024);
	if( buf == 0 )
		while( 1 )		_d_line();

	now_record = -1;
	//_d_str( file_name );
	size = df_get_file_size( file_name );
	//_d_int( size );
	size = 0;
	h = df_open( file_name, 0 );
	//_d_hex( h );
	while( size < _d_bitmap_size ){
		int tmp_size = temp_buffer_size();
		tmp_size = df_read( h, buf, tmp_size );
		//_d_int( tmp_size );
		if( tmp_size <= 0 ){
			// FIXME, here maybe error.
			now_record = size;
			break;
		}
		for( i=0; i<tmp_size; i++ ){
			if( buf[i] == 0xff ){
				break;
			}
		}
		//_d_int( i );
		
		if( i < tmp_size ){
			//_d_int( size );
			//_d_int( i );
			size += i;
			now_record = size;
			break;
		}
		size += tmp_size;
	}
	_d_int( now_record );
	if( now_record == -1 ){
		_d_line();
		while(1);
	}
	return 0;
}
int card_record_next_record_position()
{
	int tmp_block = 0;
	char file_name[24];
	void *h;
	
	now_record++;
	if( now_record  < block_record_count )
		return 0;
	now_record = 0;
	now_block ++;
	if( now_block >= block_count ){
		now_block = 0;
	}
	// delete next block file
	tmp_block = now_block + 1;
	if( tmp_block >= block_count ){
		tmp_block = 0;
	}
	s_sprintf( file_name, "%s%d", _d_card_record_file_name, tmp_block );
	df_delete( file_name, 0 );
	
	// check now block file exist.
	s_sprintf( file_name, "%s%d", _d_card_record_file_name, now_block );
	df_delete( file_name, 0 );
	h = df_create( file_name, _d_block_size, 0 );
	df_close( h );
	return 1;
}
int make_record( char *card_no, char *record_map, char *record_content )
{
	memcpy( record_content, card_no, 4 );
	return 4;
}
int card_record_write_file( char* record_content, int size )
{
	char file_name[24];
	void *h;
	int pos;
	
	s_sprintf( file_name, "%s%d", _d_card_record_file_name, now_block );
	//_d_str( file_name );
	h = df_open( file_name, 0 );
	// write record
	// calc record position.
	pos = _d_record_position(now_record);
	_d_int( pos );
	df_seek( h, pos, 0 );
	df_write( h, record_content, size );
	// write bitmap
	// calc record_bitmap position.
	pos = _d_bitmap_position(now_record);
	_d_int( pos );
	df_seek( h, pos, 0 );
	df_write( h, "\xfe", 1 );

	df_close( h );

	if(0){
		int size;
		size = df_get_file_size( file_name );
		_d_int( size );
	}
	return 0;
}

int card_record_init()
{
	card_record_get_now_block_and_record();
	return 0;
}
int card_record_write(char *card_no)
{
	char record_content[64]; // or 32
	int size;
	size = make_record( card_no, record_map, record_content );
	if( size < 0 )
		return -1;
	//_d_int( now_block );
	//_d_int( now_record );
	//_d_buf( record_content, 8 );
	card_record_write_file( record_content, size );
	card_record_next_record_position();
	return 0;
}
int card_record_get_count()
{
	return 0;
}
int card_record_get_content( int pos, char* record_content, int size )
{
	return 0;
}
int card_record_debug_list_all_record()
{
	int i,j;
	int nb;
	char file_name[24];
	void *h;
	int pos;
	char content[64];

	_d_int( now_block );
	_d_int( now_record );
	nb = now_block+1;
	for( i=0; i<block_count; i++ ){
		if( nb >= block_count )
			nb = 0;
		s_sprintf( file_name, "%s%d", _d_card_record_file_name, nb );
		_d_str( file_name );
		h = df_open( file_name, 0 );
		if( h == 0 ){
			_d_str( "not file" );
		}
		else{
			for( j=0; j<block_record_count; j++ ){
				char flag;
				int pos = _d_record_position(j);
				//_d_int( pos );
				df_seek( h, pos, 0 );
				df_read( h, content, record_size );
				_d_buf( content, record_size );
				pos = _d_bitmap_position(j);
				//_d_int( pos );
				df_seek( h, pos, 0 );
				df_read( h, &flag, 1 );
				_d_hex( flag );
			}
			df_close( h );
		}
		nb ++;
	}
	return 0;
}
int card_record_test()
{
	int i;
	for( i=0; i<50; i++ ){
		card_record_write( (char*)&i );
	}
	card_record_debug_list_all_record();
	return 0;
}


#else


int card_record_init()
{
	return 0;
}
int card_record_write(char *card_no)
{
	return 0;
}
int card_record_test()
{
	return 0;
}



#endif

