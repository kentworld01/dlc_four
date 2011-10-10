
#ifndef _d_txt_file_index_h_
#define _d_txt_file_index_h_

int txt_file_create_line_index( char* txt_file_name, char* line_pointer_file_name, char* index_file_name );
int txt_file_create_line_pointer( char* txt_file_name, char* line_pointer_file_name );
int txt_file_read_line( char* txt_file_name, int line_no, char* buf, int buf_size );
int txt_file_search_line( char* txt_file_name , char* key, char* buf, int buf_size );

#endif 
