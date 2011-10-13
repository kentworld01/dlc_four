
#ifndef _d_property_h_
#define _d_property_h_


#include "os.h"

extern int g_device_no ;
extern u8 g_send_package_socket_ip[4] ;
extern u16 g_send_package_socket_port ;

int node_prop_change_system_value( char* name, char* value );
int property_change_all_system_value();
int property_get_index( int index, char *name, char *val );
int property_get( char *name, char *val );
int property_set( char *name, char *val );
int property_del( char *name );
int property_init();

#endif


