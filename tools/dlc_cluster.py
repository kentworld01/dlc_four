#!/usr/bin/python
# -*- coding:gb2312 -*- 

import pymssql
import uuid
#from decimal import Decimal
import decimal
import _mssql

import getopt
import socket
import sys
import shutil
import os
import time
import datetime
import math
import urllib
import signal,time  
from array import array

import thread  
import re  
import time  
import gear_misc
import struct

import pprint
  
#import urllib2_file
#import urllib2
from poster.encode import multipart_encode
from poster.streaminghttp import register_openers
#import urllib2
import urllib2, urllib
import tempfile
import string


#for database process




output_line = []
input_file_data = ''
output_file_name = 'tmp_output.txt'
database_ip = "192.168.1.174"
#database_ip = "127.0.0.1"
database_user = "dlcuser"
database_pass = "dlc"
database_db = "dlcdata"



def sigint_handler(signum,frame):  
	print 'eeexxeexx'  
	#s.close()  
	print 'close'  
	sys.exit()  
def _d( val ):
    """Return the frame object for the caller's stack frame."""
    try:
        raise Exception
    except:
        f = sys.exc_info()[2].tb_frame.f_back
        print ("_"*10, f.f_lineno, f.f_code.co_name, val)
    return (f.f_code.co_name, f.f_lineno)




def get_value( s ):
	return eval( s )

	if s[0:2] == '0x':
		v = int( s[2:],  16 )
	else :
		v = int( s )
	return v
def get_value_array( s, size ):
	str = ''
	if s[0:2] == '0x':
		ss = s[2:]
		if len( ss ) == 0 :
			return ''
		size = len( ss )
		if (size % 2) == 1:
			str = chr( int( ss[0:1], 16 ) )
			s = ss[1:]
		else :
			str = chr( int( ss[0:2], 16 ) )
			s = ss[2:]
		while len( s ) > 0:
			str += chr( int( s[0:2], 16 ) )
			s = s[2:]
	else :
		hs = hex( int( s ) )
		return get_value_array( hs )

	gear_misc.out_buffer_binary( 0, str )
	return str

def tmp_unpack( d ):
	v = (ord(d[3]) << 24 ) + (ord(d[2]) << 16) + ( ord(d[1]) << 8) + ( ord(d[0]) )
	return v
def tmp_pack( v ):
	s = chr( v&0xff ) + chr( (v>>8)&0xff ) + chr( (v>>16)&0xff ) + chr( (v>>24)&0xff )
	return s



DaysOnMonthNY = [ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 ]
DaysOnMonthLY = [ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 ]
DaysOnYear = [
		0,
		366, 731, 1096, 1461, 1827, 2192, 2557, 2922, 3288, 3653,
		4018, 4383, 4749, 5114, 5479, 5844, 6210, 6575, 6940, 7305,
		7671, 8036, 8401, 8766, 9132, 9497, 9862, 10227, 10593, 10958,
		11323, 11688, 12054, 12419, 12784, 13149, 13515, 13880, 14245, 14610,
		14976, 15341, 15706, 16071, 16437, 16802, 17167, 17532, 17898, 18263,
		18628, 18993, 19359, 19724, 20089, 20454, 20820, 21185, 21550, 21915,
		22281, 22646, 23011, 23376, 23742, 24107, 24472, 24837, 25203, 25568,
		25933, 26298, 26664, 27029, 27394, 27759, 28125, 28490, 28855, 29220,
		29586, 29951, 30316, 30681, 31047, 31412, 31777, 32142, 32508, 32873,
		33238, 33603, 33969, 34334, 34699, 35064, 35430, 35795, 36160, 36525
]


def IsLeapYear(y):
	return ((  (not (y % 4)) and (y % 100)) or ( not (y % 400) )  );

'''
BOOL IsLeapYear(DWORD y)
{
	return ((!(y % 4) && (y % 100)) || !(y % 400));
}
void RelatvieToDateTime(void *DateTime, DWORD RELATIVETIME)
{
		DATETIME *dt;
		DWORD D, S;
		WORD *dom;

		dt = (DATETIME *)DateTime;
		dt->weekday = 0;

		S = RELATIVETIME % 86400;
		dt->hour = (BYTE)(S / 3600);
		S %= 3600;
		dt->minute = (BYTE)(S / 60);
		dt->second = (BYTE)(S % 60);

		//printf("RelatvieToDateTime(), D = %u\n", D);
		D = RELATIVETIME / 86400;
		dt->year = (WORD)(D / 365);
		if (DaysOnYear[dt->year] > D)
			dt->year--;
		D -= DaysOnYear[dt->year];
		dt->year += 2000;
		if (IsLeapYear(dt->year))
			dom = (WORD *)DaysOnMonthLY;
		else
			dom = (WORD *)DaysOnMonthNY;
		for(dt->month=1; dt->month<=12; dt->month++)
		if (D < dom[dt->month])
			break;
		dt->day = (BYTE)(D - dom[dt->month - 1] + 1);
}
'''

def relatvie_to_data_time_str( relative_time ):
	( year, month, day, hour, minute, second ) = relatvie_to_data_time( relative_time )
	return "%04d-%02d-%02d,\t%02d:%02d:%02d" % ( year, month, day, hour, minute, second ) 

def relatvie_to_data_time( relative_time ):
	s = relative_time % 86400
	hour = s / 3600
	s %= 3600
	minute = s / 60
	second = s % 60
	d = relative_time / 86400
	#print "d=%d" % d
	year = d / 365
	if (DaysOnYear[year] > d):
		year -= 1
	d -= DaysOnYear[year]
	year += 2000
	if (IsLeapYear(year)):
		dom = DaysOnMonthLY;
	else:
		dom = DaysOnMonthNY;
	m = 0
	for i in range(12):
		m = i + 1
		if d < dom[ m ]:
			break
	day = d - dom[ m-1 ] + 1

	#print
	#print
	#print
	#print year, m, day, hour, minute, second
	return ( year, m, day, hour, minute, second )




def get_dlc_info_wl1_count_by_vmm( s ):
	return get_dlc_info_wl_count_by_vmm( s, 0x370*528 )

def get_dlc_info_wl2_count_by_vmm( s ):
	return get_dlc_info_wl_count_by_vmm( s, 0x211ffc )
	#return get_dlc_info_wl_count_by_vmm( s, 0x212000 )

def get_dlc_info_wl_count_by_vmm( s, address ):
	d = gear_misc.memory_read( s, 0xa0000000 + address, 8 )
	v = tmp_unpack( d[6:10] )
	return v
def get_dlc_info_prop_dword( s, address ):
	v = gear_misc.get_property( s, address )  # dlc ip
	t = tmp_unpack( v[2:6] )
	print t
	return t
def get_dlc_info_server_ip( s ):
	v = gear_misc.get_property( s, 7 )  # dlc ip
	t = v[2:6]
	ip = "%d.%d.%d.%d" % (ord(t[3]), ord(t[2]), ord(t[1]), ord(t[0]))
	return ip
def set_dlc_info_server_ip( s, ip_val ):
	v = gear_misc.set_property( s, 7, ip_val )  # dlc ip

def get_dlc_info_record_info( str ):
	m1 = re.findall(r"Count:(\d+),", str)
	m2 = re.findall(r"TotalCount:(\d+)", str)
	return 'C:%5s\tTC:%8s' % ( m1[0], m2[0] )

def get_dlc_info_wl_count( str ):
	m1 = re.findall( r" (\d+) ", str )
	return 'WLC:%5s' % ( m1[0] )

def udp_socket_connect( host, port ):
	#port = 80
	#s_vmm = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s_vmm = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	try :
		s_vmm.connect((host, port))
	except:
		#print "udp socket connect error"
		return -1
	return s_vmm

def sub_write_file( ip, cmd_opt, data, type, address, length ):
	try_count = 35
	s_vmm = udp_socket_connect( ip, 8080 )
	if s_vmm == -1:
		return -1
	count = 0
	page_size = 1024
	while count < length :
		if count + page_size >= length:
			page_size = length - count
		dd = data[ count : count + page_size ]
		print "%s %s type:%d offset:%08x length:%d buf_len:%d" % ( ip, cmd_opt, type, address + count, page_size, len(dd) )
		try:
			d = '\x00\x02'

			if cmd_opt == '--flash_download_file' or cmd_opt == '--update_main_firmware' :
				d = gear_misc.download_file( s_vmm, type, address + count, page_size, dd )
			if cmd_opt == '-w' or cmd_opt == '--vmm_write_file' or cmd_opt == '--update_main_firmware_vmm' :
				d = gear_misc.memory_write( s_vmm, address + count, page_size, dd )
				try:
					# add for verify data flow
					vd = gear_misc.memory_read( s_vmm, address + count, page_size )
					vdd = vd[6:]
					if dd != vdd :
						print '%s show write data' % ip
						#gear_misc.out_buffer_binary( 0, dd )
						print '%s show read data' % ip
						#gear_misc.out_buffer_binary( 0, vdd )
				except:
					print '%s read time out' % ip
					dd = ''
					vdd = '\x11'
				if dd != vdd :
					raise EOFError("write and read buffer data not equ", ip)
					#raise EOFError("not same buffer error", ip)  

			#if ord( d[1] ) != 1 :
				#print "%s %s error" % ( ip, cmd_opt )
			count += page_size
			try_count = 20
		except Exception, e:
			print '%s %s' % ( ip, e )
			print "%s %s failed at offset:%08x length:%d, try again" % ( ip, cmd_opt, address + count, page_size )
			try:
				s_vmm.close()
			except:
				s_vmm = 0
			time.sleep( 3 )
			s_vmm = udp_socket_connect( ip, 8080 )
			try_count -= 1
			if try_count <= 0 :
				print "%s try %d failed" % ( ip, try_count )
				return 0
	return 1


#typedef struct
#{
	#DWORD CardID;
	#char EmplyeeID[12];
	#char Name[12];
	#BYTE Grade;
	#BYTE Group;
	#BYTE Dummy1;
	#BYTE Dummy2;
#} GSRI_F04_WL;
#typedef struct
#{
	#DWORD CardID;
	#DWORD Balance;
	#DWORD TollMoney;
	#DWORD TollSN;
	#DWORD RecordSN;
	#DWORD TollTime;
#
	#BIT Offline:		1;
	#BIT DidNotUpload:	1;
	#BIT Reserve1:		30;
#
	#DWORD Reserve2;
#} GSRI_F06;
def gsri_f06_record_process_byte( buf ):
	if len(buf) < 32 :
		#print 'error record size'
		return [0]

	#gear_misc.out_buffer_binary( 0, buf )

	#t = struct.unpack( 'I', buf[0:4] )
	#pprint.pprint( t )
	cardid = struct.unpack( 'I', buf[0:4] )[0]
	#print hex( cardid )
	balance = struct.unpack( 'I', buf[4:8] )[0]
	toll_money = struct.unpack( 'I', buf[8:12] )[0]
	toll_sn = struct.unpack( 'I', buf[12:16] )[0]
	record_sn = struct.unpack( 'I', buf[16:20] )[0]
	#print hex( record_sn )
	toll_time = struct.unpack( 'I', buf[20:24] )[0]
	tmp0 = struct.unpack( 'I', buf[24:28] )[0]
	tmp1 = struct.unpack( 'I', buf[28:32] )[0]
	#print "cardid: %08x, balance: %d" % ( cardid, balance ) 
	return [cardid, balance, toll_money, toll_sn, record_sn, toll_time, tmp0, tmp1 ]

def record_ptr_process_byte( buf ):
	#print len( buf )
	if len(buf) < 0x2c :
		print 'error record size'
		return [0]

	tmp0 = struct.unpack( 'I', buf[0:4] );
	tmp1 = struct.unpack( 'I', buf[4:8] );
	tmp = struct.unpack( '3B', buf[8:11] );
	tmp2 = tmp[2] + (tmp[1] << 8) + (tmp[0] << 16)
	tmp = struct.unpack( '3B', buf[11:14] );
	tmp3 = tmp[2] + (tmp[1] << 8) + (tmp[0] << 16)
	tmp4 = struct.unpack( 'B', buf[14:15] );
	tmp5 = struct.unpack( 'B', buf[15:16] );
	tmp14 = struct.unpack( 'H', buf[42:44] );
	#print "no=%d, RDPtr=%08x, WRPtr=%08x, RecCnt=%d, RecTotalCnt=%d, RecordType=%d, RecordSize=%d, CRC16=%04x )" % ( no, tmp0[0], tmp1[0], tmp2, tmp3, tmp4[0], tmp5[0], tmp14[0] );
	return ( tmp0[0], tmp1[0], tmp2, tmp3, tmp4[0], tmp5[0], tmp14[0] );
		
	#print "no=%d, RDPtr=%08x, WRPtr=%08x, RecCnt=%d, RecTotalCnt=%d, RecordType=%d, RecordSize=%d, CRC16=%04x )" % ( no, RDPtr, WRPtr, RecCnt, RecTotalCnt, RecordType, RecordSize, CRC16 );

	#DWORD RDPtr
	#DWORD WRPtr
	#BYTE RecCnt[3]
	#BYTE RecTotalCnt[3]
	#BYTE RecordType
	#BYTE RecordSize
	#DWORD TotalSale
	#DWORD MonthSale
	#DWORD DaySale
	#BYTE CurMonth
	#BYTE CurDay
	#BYTE Dummy1[2]
	#DWORD TotalTime
	#BYTE Dummy2[6]
	#WORD CRC16

tas_error_no_name = '''
	none0			= 0,
	none1			= 1,
	TEC_POWER_SHORTCUT			= 1000,
	TEC_POWER_SHORTCUT_12V		= 1010,
	TEC_POWER_SHORTCUT_5V			= 1020,
	TEC_POWER_SHORTCUT_BAT0		= 1030,
	TEC_POWER_SHORTCUT_3V3		= 1040,
	TEC_POWER_SHORTCUT_BATRTC	= 1050,
	TEC_POWER_SHORTCUT_BATV		= 1060,
	TEC_POWER_SHORTCUT_NET3V3	= 1070,
	TEC_POWER_SHORTCUT_NET1V8	= 1080,
	TEC_POWER_SUPPLY				= 1200,
	TEC_POWER_SUPPLY_12V			= 1210,
	TEC_POWER_SUPPLY_5V			= 1220,
	TEC_POWER_SUPPLY_BAT0			= 1230,
	TEC_POWER_SUPPLY_3V3			= 1240,
	TEC_POWER_SUPPLY_BATRTC		= 1250,
	TEC_POWER_SUPPLY_BATV			= 1260,
	TEC_POWER_SUPPLY_NET3V3		= 1270,
	TEC_POWER_SUPPLY_NET1V8		= 1280,
	TEC_POWER_SUPPLY_CURRENT		= 1290,
	TEC_POWER_CHARGER				= 1400,
	TEC_POWER_CHARGER_FULL_I		= 1410,
	TEC_POWER_CHARGER_FULL_V		= 1420,
	TEC_POWER_CHARGER_CHG_I		= 1430,
	TEC_POWER_CHARGER_CHG_V		= 1440,
	TEC_POWER_BATTERY				= 1600,
	TEC_POWER_BATTERY_DIS_I		= 1610,
	TEC_POWER_BATTERY_DIS_V		= 1620,
	TEC_POWER_BATTERY_DIS_5V		= 1630,
	TEC_POWER_BATTERY_DIS_BAT0	= 1640,
	TEC_POWER_BATTERY_DIS_3V3	= 1650,
	TEC_POWER_BATTERY_OFF_I		= 1660,
	TEC_POWER_BATTERY_OFF_V		= 1670,
	TEC_POWER_BATTERY_OFF_5V		= 1680,
	TEC_POWER_BATTERY_OFF_BAT0	= 1690,
	TEC_POWER_BATTERY_OFF_3V3	= 1700,
	TEC_CRYSTAL					= 1800,
	TEC_CRYSTAL_M32K				= 1810,
	TEC_CRYSTAL_M11M				= 1820,
	TEC_CRYSTAL_S11M				= 1830,
	TEC_CRYSTAL_X13M				= 1840,
	TEC_CRYSTAL_X4M				= 1850,
	TEC_CRYSTAL_X25M				= 1860,
	TEC_CRYSTAL_X125K				= 1870,
	TEC_FIRMWARE					= 2000,
	TEC_FIRMWARE_NOLOADER			= 2010,				
	TEC_FIRMWARE_LOADFW_FAIL		= 2020,				
	TEC_FIRMWARE_UPGRADE_FAIL	= 2030,				
	TEC_FIRMWARE_OLDVERSION		= 2040,
	TEC_FIRMWARE_SN_ERROR			= 2050,				
	TEC_FIRMWARE_MFW_ERROR		= 2060,				
	TEC_FIRMWARE_S1FW_ERROR		= 2070,				
	TEC_FIRMWARE_S2FW_ERROR		= 2080,				
	TEC_FIRMWARE_INIT_ERROR		= 2090,				
	TEC_FIRMWARE_MPLDR_LOAD_FAIL	= 2093,				
	TEC_FIRMWARE_S2_LOAD_FAIL	= 2095,				
	TEC_FIRMWARE_S1_LOAD_FAIL	= 2096,				
	TEC_FIRMWARE_S1LDR_LOAD_FAIL	= 2097,				
	TEC_FIRMWARE_MFW_RUNERROR	= 2098,				
	TEC_COMMUNTION					= 3000,
	TEC_COMMUNTION_RS232			= 3010,				
	TEC_COMMUNTION_RS485			= 3020,				
	TEC_COMMUNTION_UART1			= 3030,				
	TEC_COMMUNTION_ETHERNET		= 3040,				
	TEC_COMMUNTION_GSCC			= 3050,
	TEC_COMMUNTION_GSCC_GETADR	= 3051,				
	TEC_COMMUNTION_GSCC_ALIVE	= 3052,				
	TEC_COMMUNTION_GSCC_GETVER	= 3053,				
	TEC_COMMUNTION_GSCC_GETVERA	= 3054,				
	TEC_COMMUNTION_DIAG_SETETH	= 3055,				
	TEC_COMMUNTION_DIAG_MODIS	= 3056,				
	TEC_COMMUNTION_GSCC_GETSN	= 3057,				
	TEC_COMMUNTION_GSCC_CHKSUM	= 3058,				
	TEC_COMMUNTION_TEST_MODULE	= 3059,				
	TEC_COMMUNTION_GSCC_RDGPIO	= 3060,				
	TEC_COMMUNTION_GSCC_SETPERH	= 3061,				
	TEC_COMMUNTION_GSCC_TRGPERH	= 3062,
	TEC_COMMUNTION_GSCC_SETTIME	= 3063,				
	TEC_COMMUNTION_GSCC_GETRTC	= 3064,				
	TEC_COMMUNTION_GSCC_ETHTEST	= 3065,				
	TEC_COMMUNTION_GSCC_VOCTEST	= 3066,				
	TEC_COMMUNTION_GSCC_IDPRE	= 3067,				
	TEC_COMMUNTION_GSCC_ICPRE	= 3068,				
	TEC_COMMUNTION_GSCC_SETADR	= 3069,				
	TEC_MODULE						= 4000,
	TEC_MODULE_LCD					= 4010,
	TEC_MODULE_W5100				= 4020,
	TEC_MODULE_NORFLASH			= 4025,
	TEC_MODULE_RC500				= 4030,
	TEC_MODULE_SDCARD				= 4040,
	TEC_MODULE_SDWP				= 4050,
	TEC_MODULE_DCSTATE			= 4060,
	TEC_MODULE_CHGSTATE			= 4070,
	TEC_MODULE_RFIC				= 4080,
	TEC_MODULE_RFID				= 4090,
	TEC_MODULE_RTC					= 4100,
	TEC_OUTPUT						= 5000,
	TEC_OUTPUT_LCDCS1				= 5005,
	TEC_OUTPUT_LCDBL1				= 5010,
	TEC_OUTPUT_LCDBL2				= 5015,
	TEC_OUTPUT_LED_G				= 5020,
	TEC_OUTPUT_LED_Y				= 5025,
	TEC_OUTPUT_BUZDRV				= 5030,
	TEC_OUTPUT_BUZZER1			= 5035,
	TEC_OUTPUT_BUZZER2			= 5036,
	TEC_OUTPUT_SPKAMP				= 5040,
	TEC_OUTPUT_SPEAKER1			= 5045,
	TEC_OUTPUT_SPEAKER2			= 5046,
	TEC_OUTPUT_LOCK1				= 5060,
	TEC_OUTPUT_LOCK2				= 5065,
'''

tas_error_output_format = '''
	{ TEC_POWER_SHORTCUT_12V,	    (WCHAR *)str_087,W_mV }, //TP7018 (12V):
	{ TEC_POWER_SHORTCUT_5V	,	    (WCHAR *)str_088,W_mV }, //TP9007 (5V):
	{ TEC_POWER_SHORTCUT_BAT0,	 	(WCHAR *)str_089,W_mV }, //TP9008 (BAT0):
	{ TEC_POWER_SHORTCUT_3V3,	 	(WCHAR *)str_090,W_mV }, //TP9010 (3V3):
	{ TEC_POWER_SHORTCUT_BATRTC,	(WCHAR *)str_091,W_mV }, //T30 (BATRTC):
	{ TEC_POWER_SHORTCUT_BATV,	 	(WCHAR *)str_092,W_mV }, //TP9002 (BATV):
	{ TEC_POWER_SHORTCUT_NET3V3,	(WCHAR *)str_093,W_mV }, //TP8004 (NET3V3):
	{ TEC_POWER_SHORTCUT_NET1V8,	(WCHAR *)str_094,W_mV }, //TP8008 (NET1V8):
	{ TEC_POWER_SUPPLY_12V,	        (WCHAR *)str_087,W_mV }, //TP7018 (12V):
	{ TEC_POWER_SUPPLY_5V,	 	    (WCHAR *)str_088,W_mV }, //TP9007 (5V):
	{ TEC_POWER_SUPPLY_BAT0,	    (WCHAR *)str_089,W_mV }, //TP9008 (BAT0):
	{ TEC_POWER_SUPPLY_3V3,	        (WCHAR *)str_090,W_mV }, //TP9010 (3V3):
	{ TEC_POWER_SUPPLY_BATRTC,	 	(WCHAR *)str_091,W_mV }, //T30 (BATRTC):
	{ TEC_POWER_SUPPLY_BATV,	 	(WCHAR *)str_092,W_mV }, //TP9002 (BATV):
	{ TEC_POWER_SUPPLY_NET3V3,	 	(WCHAR *)str_093,W_mV }, //TP8004 (NET3V3):
	{ TEC_POWER_SUPPLY_NET1V8,	 	(WCHAR *)str_094,W_mV }, //TP8008 (NET1V8):
	{ TEC_POWER_SUPPLY_CURRENT,	    (WCHAR *)str_095,W_mA }, //TP9004 (工作电流):
	{ TEC_POWER_CHARGER_FULL_I,	    (WCHAR *)str_096,W_mA }, //(充电电流):
	{ TEC_POWER_CHARGER_FULL_V,	    (WCHAR *)str_097,W_mV }, //TP9002 (锂电池):
	{ TEC_POWER_CHARGER_CHG_I,	 	(WCHAR *)str_096,W_mA }, //(充电电流):
	{ TEC_POWER_CHARGER_CHG_V,	 	(WCHAR *)str_097,W_mV }, //TP9002 (锂电池):
	{ TEC_POWER_BATTERY_DIS_I,	 	(WCHAR *)str_098,W_mA }, //TP9004 (放电电流):
	{ TEC_POWER_BATTERY_DIS_V,	 	(WCHAR *)str_097,W_mV }, //TP9002 (锂电池):
	{ TEC_POWER_BATTERY_DIS_5V,	 	(WCHAR *)str_088,W_mV }, //TP9007 (5V):
	{ TEC_POWER_BATTERY_DIS_BAT0,	(WCHAR *)str_089,W_mV }, //TP9008 (BAT0):
	{ TEC_POWER_BATTERY_DIS_3V3,	(WCHAR *)str_090,W_mV }, //TP9010 (3V3):
	{ TEC_POWER_BATTERY_OFF_I,	 	(WCHAR *)str_099,W_mA }, //TP9004 (关机电流):
	{ TEC_POWER_BATTERY_OFF_V,	 	(WCHAR *)str_097,W_mV }, //TP9002 (锂电池):
	{ TEC_POWER_BATTERY_OFF_5V,	 	(WCHAR *)str_088,W_mV }, //TP9007 (5V):
	{ TEC_POWER_BATTERY_OFF_BAT0,	(WCHAR *)str_089,W_mV }, //TP9008 (BAT0):
	{ TEC_POWER_BATTERY_OFF_3V3, 	(WCHAR *)str_090,W_mV }, //TP9010 (3V3):
	{ TEC_CRYSTAL_M32K,	            (WCHAR *)str_100,W_mV }, //TP4000 (32.768KHz):
	{ TEC_CRYSTAL_M11M,	            (WCHAR *)str_101,W_mV }, //TP4001 (11.0592MHz):
	{ TEC_CRYSTAL_X13M,	            (WCHAR *)str_102,W_mV }, //TP10017 (13.056MHz):
	{ TEC_CRYSTAL_X4M,	        	(WCHAR *)str_103,W_mV }, //TP11002 (4MHz):
	{ TEC_CRYSTAL_X25M,         	(WCHAR *)str_104,W_mV }, //TP8007 (25MHz):
	{ TEC_CRYSTAL_X125K,	        (WCHAR *)str_105,W_mV }, //TP11001 (125KHz):
	{ TEC_FIRMWARE_LOADFW_FAIL,  	(WCHAR *)str_111,W_0 },	// 固件加载
	{ TEC_FIRMWARE_SN_ERROR	,	    (WCHAR *)str_106,W_010u },	// 产品序号:
	{ TEC_FIRMWARE_MFW_ERROR,	 	(WCHAR *)str_107,W_08x },	//MainMCU数据：	
	{ TEC_FIRMWARE_S1FW_ERROR,	 	(WCHAR *)str_108,W_08x },	//SubMCU数据：	
	{ TEC_FIRMWARE_S2FW_ERROR,	 	(WCHAR *)str_109,W_08x },	//Flash数据:	
	{ TEC_FIRMWARE_INIT_ERROR,	 	(WCHAR *)str_110,W_0 },	// 格式化S2:
	{ TEC_FIRMWARE_MPLDR_LOAD_FAIL,	(WCHAR *)str_111,W_0 },	// 加载固件:
	{ TEC_FIRMWARE_S2_LOAD_FAIL,	(WCHAR *)str_112,W_0 },	// 加载S2：
	{ TEC_FIRMWARE_S1_LOAD_FAIL,	(WCHAR *)str_113,W_0 },	// 加载S1:
	{ TEC_FIRMWARE_S1LDR_LOAD_FAIL,	(WCHAR *)str_114,W_0 },	// 加载S1LDR:
	{ TEC_FIRMWARE_MFW_RUNERROR,	(WCHAR *)str_156,W_0 },	// 主程序运行错误
	{ TEC_COMMUNTION_RS232,	        (WCHAR *)str_115,W2_0},	// RS232:
	{ TEC_COMMUNTION_RS485,	        (WCHAR *)str_116,W2_0},	// RS485:
	{ TEC_COMMUNTION_UART1,	        (WCHAR *)str_117,W_0 }, // UART1:
	{ TEC_COMMUNTION_ETHERNET	,	(WCHAR *)str_118,W2_0},	// ETHERNET:
	{ TEC_COMMUNTION_GSCC_GETADR,	(WCHAR *)str_120,W_0 },	// 取设备地址
	{ TEC_COMMUNTION_GSCC_GETVER,	(WCHAR *)str_119,W_0 },	// 取版本信息
	{ TEC_COMMUNTION_DIAG_SETETH,	(WCHAR *)str_121,W_0 },	// 设置以太网
	{ TEC_COMMUNTION_DIAG_MODIS,	(WCHAR *)str_122,W_0 },	// 屏蔽读卡
	{ TEC_COMMUNTION_GSCC_GETSN,	(WCHAR *)str_123,W_0 },	// GetSN:
	{ TEC_COMMUNTION_GSCC_CHKSUM,	(WCHAR *)str_124,W_0 },	// Checksum:
	{ TEC_COMMUNTION_TEST_MODULE,	(WCHAR *)str_125,W_0 },	// Module:
	{ TEC_COMMUNTION_GSCC_SETPERH,	(WCHAR *)str_126,W_0 },		// 外设控制
	{ TEC_COMMUNTION_GSCC_SETTIME,	(WCHAR *)str_127,W_0 },	// 设置时间
	{ TEC_COMMUNTION_GSCC_GETRTC,	(WCHAR *)str_128,W_0 },	// 取RTC:
	{ TEC_COMMUNTION_GSCC_ETHTEST,	(WCHAR *)str_117,W_0 }, // ETHERNET
	{ TEC_COMMUNTION_GSCC_VOCTEST,	(WCHAR *)str_129,W_0 }, // 语音:
	{ TEC_COMMUNTION_GSCC_IDPRE,	(WCHAR *)str_130,W_0 },	// 屏蔽ID
	{ TEC_COMMUNTION_GSCC_ICPRE,	(WCHAR *)str_132,W_0 }, // 屏蔽IC
	{ TEC_COMMUNTION_GSCC_SETADR,	(WCHAR *)str_132,W_0 }, // 设置设备地址
	{ TEC_MODULE_LCD,           	(WCHAR *)str_133,W_0 }, //LCD:
	{ TEC_MODULE_W5100,         	(WCHAR *)str_134,W_0 }, //W5100:
	{ TEC_MODULE_NORFLASH,      	(WCHAR *)str_135,W_0 }, //NORFLASH:
	{ TEC_MODULE_RC500,         	(WCHAR *)str_136,W_0 }, //RC500:
	{ TEC_MODULE_SDCARD,        	(WCHAR *)str_137,W_0 }, //SD接口：
	{ TEC_MODULE_SDWP,          	(WCHAR *)str_138,W_0 }, //SD写保护状态：
	{ TEC_MODULE_DCSTATE,       	(WCHAR *)str_139,W_0 }, //电源状态：
	{ TEC_MODULE_CHGSTATE,      	(WCHAR *)str_140,W_0 }, //充电检测：
	{ TEC_MODULE_RFIC,          	(WCHAR *)str_141,W_u }, //读IC卡：
	{ TEC_MODULE_RFID,          	(WCHAR *)str_142,W_u }, //读ID卡:
	{ TEC_MODULE_RTC,           	(WCHAR *)str_143,DW_dw1 }, //CPURTC:
	{ TEC_OUTPUT_LCDCS1,        	(WCHAR *)str_144,W2_0}, //TP (LCDCS1):
	{ TEC_OUTPUT_LCDBL2,        	(WCHAR *)str_145,W2_0}, //TP (BL2):
	{ TEC_OUTPUT_BUZDRV,	 	    (WCHAR *)str_148,W2_dv_12 }, //TP (BUZ_DRV):
	{ TEC_OUTPUT_BUZZER1,	 	    (WCHAR *)str_149,W2_dv_21 }, //BUZZER1:
	{ TEC_OUTPUT_BUZZER2,	 	    (WCHAR *)str_150,W2_dv_21 }, //BUZZER2:
	{ TEC_OUTPUT_SPKAMP,	 	    (WCHAR *)str_151,W2_dv }, //TP10000 (SPKAMP):
	{ TEC_OUTPUT_SPEAKER1,	 	    (WCHAR *)str_152,W2_dv_12 }, //(SPEAKER1):
	{ TEC_OUTPUT_SPEAKER2,	 	    (WCHAR *)str_153,W2_dv_21 }, //(SPEAKER2):
	{ TEC_OUTPUT_LOCK1,	 	        (WCHAR *)str_154,W2_0}, //TP (LOCK1):
	{ TEC_OUTPUT_LOCK2,	 	        (WCHAR *)str_155,W2_0}, //TP (LOCK2):
	{ TEC_OUTPUT_LED_G,	 	        (WCHAR *)str_146,W2_0}, //TP10014 (GLED):
	{ TEC_OUTPUT_LED_Y,	 	        (WCHAR *)str_147,W2_0}, //TP10013 (YLED
'''

'''
#if (PLATFORM_TAS_GS04 == 1)
	{ TEC_OUTPUT_LED_G,	 	        (WCHAR *)str_146,W2_0}, //TP10014 (GLED):
	{ TEC_OUTPUT_LED_Y,	 	        (WCHAR *)str_147,W2_0}, //TP10013 (YLED
#endif
#if (PLATFORM_TAS_GS05 == 1)
	{ TEC_OUTPUT_LED_G,	 	        (WCHAR *)str_146,W2_dv_21}, //TP10014 (GLED):
	{ TEC_OUTPUT_LED_Y,	 	        (WCHAR *)str_147,W2_dv_21}, //TP10013 (YLED
#endif
'''


def analysis_tas_record_data( flag, address, length, data ):
	global tas_error_no_name 
	print "analysis_tas_record_data : address = %08x, length = %08x, file_length = %08x" % ( address, length, len( data ) )
	at45_page_size = 528

	error_no_to_string = {}
	error_string_to_no = {}
	#analysis string.
	lines = tas_error_no_name.split('\n');
	for l in lines:
		l = l.replace( ' ', '' )
		l = l.replace( '\t', '' )
		l = l.replace( ',', '' )
		items = l.split( '=' )
		if len( items ) == 2:
			error_no_to_string[ int(items[1]) ] = items[0]
			error_string_to_no[ items[0] ] = int(items[1]) 
			#print "%s%s" % (items[0], items[1])
	#print error_no_to_string[ 5065 ]
	error_output_format = {}
	lines = tas_error_output_format.split('\n');
	for l in lines:
		l = l.replace( ' ', '' )
		l = l.replace( '\t', '' )
		l = l.replace( '(WCHAR*)', '' )
		l = l.replace( '{', '' )
		l = l.replace( '}', '' )
		items = l.split( ',' )
		if len( items ) >= 3:
			no = error_string_to_no[ items[0] ]
			#print no
			error_output_format[ no ] = items[2]
	#return
	error_output_format[ 0 ] = 'W_END'
	error_output_format[ 1 ] = 'W_END'

	#find the REC_TER_HEAD
	rec_ter_begin_address = 0x40 * at45_page_size + address
	rec_ter_end_address = 0x50 * at45_page_size + address
	tmp_bpo = rec_ter_begin_address
	while( tmp_bpo < rec_ter_end_address ):
		tmp_epo = tmp_bpo + 12
		(index_ptr, data_ptr, check_sn) = struct.unpack( 'III', data[tmp_bpo:tmp_epo] )
		if index_ptr != 0 and index_ptr != 0xffffffff:
			break
		tmp_bpo = tmp_epo;
	print
	if index_ptr == 0 or index_ptr == 0xffffffff:
		print "TER HEAD error, return"
		return
	print "TER HEAD info: index_ptr = %08x\tdata_ptr = %08x\tcheck_sn = %d" % (index_ptr, data_ptr, check_sn)
	#show index and data item
	index_bpo = 0x100 * at45_page_size
	index_epo = index_ptr
	index_step = 12
	data_bpo = 0xb00 * at45_page_size
	data_epo = data_ptr
	data_step = -12
	# for show index
	index_target_sn = []
	index_test_time = []
	index_error_code = []
	index_data_ptr = []
	tmp_bpo = index_bpo
	while tmp_bpo < index_epo:
		tmp_epo = tmp_bpo + index_step
		(target_sn, test_time, data_ptr) = struct.unpack( 'III', data[tmp_bpo:tmp_epo] )
		#if target_sn != 0 and target_sn != 0xffffffff:
			#break
		error_code = data_ptr & 0xff
		data_ptr >>= 8
		#print "target_sn = %08x\ttest_time = %08x\terror_code = %d\tdata_ptr = %08x" % (target_sn, test_time, error_code, data_ptr)
		index_target_sn.append( target_sn )
		index_test_time.append( test_time )
		index_error_code.append( error_code )
		index_data_ptr.append( data_ptr )
		# begin list the data item
		tmp_bpo = tmp_epo;
	for i in range( len(index_data_ptr)-1 ):
		#print index_test_time[i]
		print "-----------------------------------------------------------------------------------------------------"
		print "target_sn = %10d\ttest_time = %s\terror_code = %d\tdata_ptr = %08x" % (index_target_sn[i],  relatvie_to_data_time_str( index_test_time[i] ), index_error_code[i], index_data_ptr[i])
		tmp_bpo = index_data_ptr[i]
		tmp_limit_po = index_data_ptr[i+1]
		while( tmp_bpo > tmp_limit_po ):
			tmp_epo = tmp_bpo + data_step;
			#print "\tbegin_po = %08x, end_po = %08x" % ( tmp_bpo, tmp_epo )
			(result_type, state_code, error_code, tmp, tmp) = struct.unpack( 'BBHII', data[tmp_epo:tmp_bpo] )
			tmp_pos = tmp_bpo - 8
			tmp_b = struct.unpack( 'BBBBBBBB', data[ tmp_pos:tmp_bpo ] )
			tmp_w = struct.unpack( 'HHHH', data[ tmp_pos:tmp_bpo ] )
			tmp_dw = struct.unpack( 'II', data[ tmp_pos:tmp_bpo ] )
			#calc error data
			format = error_output_format[ error_code ]
			if format == 'W_mV':
				str = ":%u%s"%(tmp_w[0],"[mV]")
				#s_sprintf(buf, ":%u%s",w,"[mV]");
			if format == 'W_mA':
				str = ":%u%s"%(tmp_w[0],"[mA]")
				#s_sprintf(buf, ":%u%s",w,"[mA]");
			if format == 'W_0':
				str = 'none'
			if format == 'W_u':
				str = ":%u"%tmp_w[0]
				#s_sprintf(buf, ":%u",w);
			if format == 'W_010u':
				str = ":%010u"%tmp_w[0]
				#s_sprintf(buf, ":%010u",w);
			if format == 'W_08x':
				str = ":0x%08x"%tmp_w[0]
				#s_sprintf(buf, ":0x%08x",w);
			if format == 'W2_0':
				str = ":%u,%u"%(tmp_w[0],tmp_w[1])
				#s_sprintf(buf, ":%u,%u",w1,w2);
			if format == 'W2_dv_12' or format == 'W2_dv_21' or format == 'W2_dv':
				str = ":%d,%d,%d"%(tmp_w[0],tmp_w[1],tmp_w[2]);
				#s_sprintf(buf, ":%d,%d,%d",w1,w2,dv);
			if format == 'DW_dw1':
				str = ":%u"%tmp_dw[0];
				#s_sprintf(buf, ":%u",dw1);
			if format == 'W_END':
				str = 'end'

			#print error_code
			print "\tresult_type = %d\tstate_code = %d\terror_code = %d(%30s)\terror_data = %s" % (result_type, state_code, error_code, error_no_to_string[error_code], str )
			tmp_bpo = tmp_epo;


def analysis_record_data( flag, address, length, data ):
	print "analysis_record_data : address = %08x, length = %08x, file_length = %08x" % ( address, length, len( data ) )
	#deal the pointer area.
	if flag == 0 :
		print "at45"
		begin_ptr_address = 0x60 * 528 - address
		end_ptr_address = 0xba * 528 - address
		begin_address = 0xc0 * 528 - address
		end_address = 0x36c * 528 - address
	else:
		print "w25x"
		begin_ptr_address = 0xc * 4096 - address
		end_ptr_address = 0x18 * 4096 - address
		begin_address = 0x18 * 4096 - address
		end_address = 0x71 * 4096 - address
	record_size = 0x2c
	tmp_addr = begin_ptr_address
	no = 0

	if tmp_addr >= 0:
		while( tmp_addr < end_ptr_address ):
			tmp_end_addr = tmp_addr + record_size
			tmp_buf = data[ tmp_addr : tmp_end_addr ]
			tmp = record_ptr_process_byte( tmp_buf );
			if tmp[0] != 0 and tmp[0] != 0xffffffff :
				#print "no=%d, RDPtr=%08x, WRPtr=%08x, RecCnt=%d, RecTotalCnt=%d, RecordType=%d, RecordSize=%d, CRC16=%04x )" % ( no, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6] );
				break
			no += 1
			tmp_addr += record_size
		print "\nRecord Ptr Info:"
		print "no=%d, RDPtr=%08x, WRPtr=%08x, RecCnt=%d, RecTotalCnt=%d, RecordType=%d, RecordSize=%d, CRC16=%04x )" % ( no, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6] );
		record_size = tmp[5]
	else:
		record_size = 32

	tmp_addr = begin_address
	no = 0

	if tmp_addr >= 0 :
		print "\nRecord List:"
		print "no\tcardid\t\tbalance\tmoney\ttoll_sn\tsn\ttoll_time" 
		while( tmp_addr >= 0 and tmp_addr < end_address ):
			tmp_end_addr = tmp_addr + record_size
			tmp_buf = data[ tmp_addr : tmp_end_addr ]
			tmp = gsri_f06_record_process_byte( tmp_buf );
			if tmp[0] != 0 and tmp[0] != 0xffffffff :
				print "%d,\t%10d,\t%d,\t%d,\t%d,\t%d,\t%s" % ( no, tmp[0] , tmp[1] , tmp[2] , tmp[3] , tmp[4] , relatvie_to_data_time_str( tmp[5] ) )
				#print "no=%d\tcardid=%d\tbalance=%d\ttoll_money=%d\ttoll_sn=%d\trecord_sn=%d\ttoll_time=%s\ttmp0=%08x\ttmp1=%08x" % ( no, tmp[0] , tmp[1] , tmp[2] , tmp[3] , tmp[4] , relatvie_to_data_time_str( tmp[5] ), tmp[6] , tmp[7] )
			no += 1
			tmp_addr += record_size




def dlc_thread_cmd( ip, cmd_opt, cmd_arg, cmd_address, cmd_length ):
	global input_file_data
	s_vmm = udp_socket_connect( ip, 8080 )

	if cmd_opt == "--reset" :
		#print "restart machine"
		gear_misc.diag_restart( s_vmm )
	if cmd_opt == "--set_dlc_ip" :
		#print "set_dlc_ip %s" % cmd_arg
		m = re.findall(r"(\d+)", cmd_arg)
		v = ( int(m[0]) << 24 ) + ( int(m[1]) << 16 ) + ( int(m[2]) << 8 ) + int(m[3])
		gear_misc.set_property( s_vmm, 7, v )
	if cmd_opt == "--get_prop" :
		#print "get_prop %s %s" % ( cmd_address, cmd_arg )
		address = get_value( cmd_address )
		#value = get_value( cmd_arg )
		d = gear_misc.get_property( s_vmm, address )
		gear_misc.out_buffer_binary( 0, d )
	if cmd_opt == "--set_prop" :
		#print "set_prop %s %s" % ( cmd_address, cmd_arg )
		address = get_value( cmd_address )
		value = get_value( cmd_arg )
		gear_misc.set_property( s_vmm, address, value )
	if cmd_opt == "-r" :
		print "memory_read %s %s" % ( cmd_address, cmd_length )
		address = get_value( cmd_address )
		length = get_value( cmd_length )
		d = gear_misc.memory_read( s_vmm, address, length )
		gear_misc.out_buffer_binary( 0, d[6:] )
	if cmd_opt == "-w" :
		address = get_value( cmd_address )
		length = get_value( cmd_length )
		data = get_value_array( cmd_arg, length )
		type = 0
		r = sub_write_file( ip, cmd_opt, data, type, address, length )
		if r == -1 :
			r = 'failed'
		else :
			r = 'success'
		print "%s %s %s" % ( ip, cmd_opt, r )
	if cmd_opt == "--vmm_write_file" :
		address = get_value( cmd_address )
		length = get_value( cmd_length )
		type = 0
		r = sub_write_file( ip, cmd_opt, input_file_data, type, address, length )
		#print "%s %s completed" % ( ip, cmd_opt )
		if r == -1 :
			r = 'failed'
		else :
			r = 'success'
		print "%s %s %s" % ( ip, cmd_opt, r )

	if cmd_opt == '--flash_checksum' :
		try:
			address = get_value( cmd_address )
			#print cmd_length
			length = get_value( cmd_length )
			#print cmd_arg
			print "%s get flash_checksum %d %08x %d" % ( ip, int(cmd_arg), address, length )
			socket.setdefaulttimeout( 20 )
			d = gear_misc.diag_get_flash_checksum( s_vmm, int( cmd_arg ), address, length )
			r = ord( d[1] )
			if r == 1 :
				v = tmp_unpack( d[2:] )
				s = "%s flash checksum %d 0x%08x" % ( ip, int(cmd_arg), v )
			else :
				s = "%s flash checksum failed" % ( ip )

			output_line.append( s )
		except:
			output_line.append( "%s flash checksum failed" % ( ip ) )

	if cmd_opt == '--flash_download_file' :
		address = get_value( cmd_address )
		length = get_value( cmd_length )
		type = get_value( cmd_arg )
		r = sub_write_file( ip, cmd_opt, input_file_data, type, address, length )
		#print "%s %s completed" % ( ip, cmd_opt )
		if r == -1 :
			r = 'failed'
		else :
			r = 'success'
		output_line.append( "%s %s %s" % ( ip, cmd_opt, r ) )

	"""
	if cmd_opt == '--update_main_firmware' or cmd_opt == '--update_main_firmware_vmm' :
		address = get_value( cmd_address )
		length = get_value( cmd_length )
		type = get_value( cmd_arg )
		tmp_data = '\xff' * 16
		tmp_data += input_file_data
		length += len( tmp_data )
		r = sub_write_file( ip, cmd_opt, tmp_data, type, address, length )
		if r == -1 :
			r = 'failed'
			output_line.append( "%s %s %s" % ( ip, cmd_opt, r ) )
			return

		tmp_data = '\xf1\x17\x55\xaa\x16\x00\x00\x00'
		tmp_data += tmp_pack( len( input_file_data ) )
		tmp_data += '\x00\x00\x00\x00'
		gear_misc.out_buffer_binary( 0, tmp_data )
		address = get_value( cmd_address )
		#address = 0
		#address = 0xa0194400
		length = 16
		r = sub_write_file( ip, cmd_opt, tmp_data, type, address, length )
		#print "%s %s completed" % ( ip, cmd_opt )
		if r == -1 :
			r = 'failed'
		else :
			r = 'success'
		output_line.append( "%s %s %s" % ( ip, cmd_opt, r ) )
	"""
			
	if cmd_opt == '--flash_erase' :
		try:
			address = get_value( cmd_address )
			length = get_value( cmd_length )
			web_add = "http://%s/m?t=f&a=e&b=%u&e=%u" % (ip, address, address+length)
			print web_add
			socket.setdefaulttimeout( 20 )
			f = urllib.urlopen( web_add )
			s = read_time_out( f, 10 )
			f.close()
			print s
		except:
			output_line.append( "%s error" % ip )

	if cmd_opt == '--version' :
		try:
			d = gear_misc.get_version( s_vmm )
			s = d[2:]
			str = '%s version : %s' % (ip, s)
			output_line.append( str )
		except:
			output_line.append( "%s error" % ip )

	if cmd_opt == '--analysis_record_data' :
		address = get_value( cmd_address )
		length = get_value( cmd_length )
		flag = get_value( cmd_arg )
		analysis_record_data( flag, address, length, input_file_data )

	if cmd_opt == '--analysis_tas_record_data' :
		address = get_value( cmd_address )
		length = get_value( cmd_length )
		flag = get_value( cmd_arg )
		analysis_tas_record_data( flag, address, length, input_file_data )


	try:
		s_vmm.close()
	except:
		s_vmm = 0

def dlc_thread(ip, cmd_opt, cmd_arg, cmd_address, cmd_length ):
	if len( cmd_opt ) > 0 :
		dlc_thread_cmd( ip, cmd_opt, cmd_arg, cmd_address, cmd_length )
		return

	if 0:
		try:
			# get the record info
			web_add = "http://%s/" % (ip)
			f = urllib.urlopen( web_add )
			s = read_time_out( f, -1 )
			s_ri = get_dlc_info_record_info( s )
			f.close()
		except:
			#print "%s get record info except" % ip
			s_ri = ' ' * 20

	if 0:
		time.sleep( 2 )
		try:
			# get the wl number
			web_add = "http://%s/wl" % (ip)
			f = urllib.urlopen( web_add )
			s = read_time_out( f, 35 )
			s_wl = get_dlc_info_wl_count( s )
			f.close()
		except:
			#print "%s get white name count except" % ip
			s_wl = ' ' * 20

	#s_vmm = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s_vmm = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	host = ip
	port = 8080
	#port = 80
	try :
		s_vmm.connect((host, port))
	except:
		#print "connect error"
		port = 8080

	try:
		d = gear_misc.get_record_info( s_vmm )
		v = tmp_unpack( d[10:14] )
		s_c = "C:%d" % v;
	except:
		s_c = ' ' * 10

	try:
		v = get_dlc_info_wl1_count_by_vmm( s_vmm );
		s_wl1 = "WL1C:%10d" % v;
	except:
		s_wl1 = ' ' * 10
	try:
		v = get_dlc_info_wl2_count_by_vmm( s_vmm );
		s_wl2 = "WL2C:%10d" % v;
	except:
		s_wl2 = ' ' * 10
	try:
		s = get_dlc_info_server_ip( s_vmm )
		s_dlc_ip = "DIP:%s" % s
	except:
		s_dlc_ip = ' ' * 20

	s_vmm.close();

	#str = "%s\t%s\t%s" % ( ip, s_ri, s_wl )
	str = "%s\t%s\t%s\t%s\t%s" % ( ip, s_c, s_wl1, s_wl2, s_dlc_ip )
	output_line.append( str )

def read_time_out( f, size ) :
	str = ''
	while 1:
		try :
			s = f.read( 1 )
			if size > 0 :
				size -= 1
			if size == 0 :
				break
		except:
			break
		str += s;
	return str

def get_file_data( arg ):
	#print arg
	size = os.path.getsize( arg )
	f = open( arg, 'rb' )
	fd = f.read( size )
	f.close()
	return fd

global g_system_running
g_system_running = 1;
global g_socket_no
g_socket_no = 0

def server_test_write_file( str ):
	f = open( output_file_name, 'a' )
	f.write( str )
	f.close

def server_test_input_thread( gss, addr ):
	global g_system_running
	global g_socket_no
	while( 1 ):
		buf = raw_input()
		g_system_running = 0

def server_test_thread( gss, addr ):
	global output_file_name
	addr_str = addr[0] + ":%d "%addr[1]
	try:
		while 1:
			data = gss.recv(1000)
			#for test the tcpclient send and receive function.
			#gss.send( "ok,123456789\n" );
			if len(data) == 0 :
				break
			if data[0] == '\xa5':
				print addr
				gear_misc.out_buffer_binary( 0, data )
			else:
				print addr_str + data
				if output_file_name != '':
					server_test_write_file( addr_str + data + '\n' )
	except:
		str = "socket error"
		print str
		gss.close()
	finally:
		str = "Closed Connect"
		print str
		server_test_write_file( str + '\n' )
		gss.close()

def server_test_main( arg1, arg2 ):
	while 1:
		gss, addr = g_socket_no.accept()
		str = '\nclient is at ' + addr[0] + ":%d"%addr[1]
		print str
		server_test_write_file( str + '\n' )
		thread.start_new_thread( server_test_thread, (gss,addr) )

def server_test( arg ):
	global g_system_running
	global g_socket_no
	print "server test run..."
	g_socket_no = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	port = 8001
	host = ''
	g_socket_no.bind((host, port))
	g_socket_no.listen(1)
	print "wait connect..."
	thread.start_new_thread( server_test_input_thread, (0,0) )
	thread.start_new_thread( server_test_main, (0,0) )
	while g_system_running:
		time.sleep( .5 )
	g_socket_no.close
	return 0


def cluster_server_write_file( str ):
	f = open( output_file_name, 'a' )
	f.write( str )
	f.close

def cluster_server_input_thread( gss, addr ):
	global g_system_running
	global g_socket_no
	while( 1 ):
		buf = raw_input()
		g_system_running = 0


g_termial_ip_list=[]
g_termial_list = { 
	#"t1":"192.168.1.201" ,
	#"t2":"192.168.1.202" ,
}
g_white_name_list = "none"
g_white_name_file_check_sum = 0
g_white_name_record_length = 96
g_white_name_record_count = 1000

def cluster_server_database_process( arg ):
	global database_ip
	global database_user
	global database_pass
	global database_db
	#_d( database_ip )
	#_d( database_db )
	#_d( database_user )
	#_d( database_pass )
	#conn = pymssql.connect( host=database_ip,user="dlceasy",password="dlc",database="dlceasy" )
	conn = pymssql.connect( host=database_ip,user=database_user,password=database_pass,database=database_db )
	#conn = pymssql.connect( host=database_ip,user="dlcuser",password="dlc",database="dlcdata" )
	cur = conn.cursor()
	arg = "exec " + arg
	#print arg
	cur.execute( arg )
	datas = None;
	datas = cur.fetchall()
	conn.commit()
	conn.close()
	#print datas
	return datas
def cluster_server_save_record( ip, port, data ):
	# save the record in txt file
	#_d( "%s:%s\t%s"%(ip , port , data) )
	if output_file_name != '':
		addr_str = "%s:%s"%(ip,port)
		cluster_server_write_file( addr_str + data + '\n' )
	# save the record in database
	#define _ds_record_format "machine_no,date,time,sys_tick,record_id,card_id,ok_flag"
	items = data.split(" ")
	#_d( "card no: %s"%items[5] )
	buf = "sp_process_003_001 @cardno='%s',@ipaddrno='%s'"%(items[5], ip)
	#buf = "sp_process_003_001 @ipaddrno='%s',@machno='%s',@date='%s',@time='%s',@machtime='%s',@cardno='%s', "%(ip,items[0],items[1],items[2],items[3],items[5])
	#try:
	if 1:
		rel = cluster_server_database_process( buf )
		#for r in rel:
			#_d( r )
		r, no, name, result = rel[0]
		#print r
		#print no
		#print name
		#print result
		#_d(result)
		# send tcmd to show.
		name = name.split(':')[1]
		tcmd = "T,gui,edit,disable,__TIME__;T,gui,edit,text,card_id,%s%s;T,gui,show;T,gui,edit,text,card_id,Welcome;T,timer,set_timer,5000"%(name,result)
		#print tcmd
		tcmd = tcmd.decode( 'gbk' ).encode( 'utf8' )
		#tcmd = tcmd.decode( 'gbk' )
		do_tcmd( ip, tcmd )
		

	#except:
		#_d( "process error" )

def cluster_server_termial_process(no, ip):
	global g_white_name_file_check_sum 
	#_d( "%d %s start:"%(no,ip) )
	# check wether need download whitename
	if( g_white_name_file_check_sum != 0 ):
		sum = do_tcmd( ip, "T,property_get,white_name_cs" )
		#sum = do_tcmd( ip, "T,prop_get,/p/white_name_cs" )
		if sum == "Error":
			sum = "-1"
		#print sum
		try:
			#sum = int( sum[0:-3] )
			sum = int( sum )
		except:
			sum = 0;
		if( g_white_name_file_check_sum != sum and sum != -1 ):
			# need sync the white name
			print "ready sync the white name. local check sum:%d, termial check sum:%d"%( g_white_name_file_check_sum , sum )
			#do_post_file( ip, "tmp_white_name", "/c/white_name" )
			print "send white_name.rf to %s" % ip
			http_post_file( ip, "white_name.rf" )
			print "send white_name.rf.ri to %s" % ip
			http_post_file( ip, "white_name.rf.ri" )
			#sum = do_tcmd( ip, "T,reset_white_name_data" )
			sum = do_tcmd( ip, "T,white_name_change,/white_name.rf" )
	# check has record data need upload
	if 0:
		count = do_tcmd( ip, "T,record_get_count" )
		if( count == "Error" ):
			count = 0
		count = int( count )
		if( count > 0 ):
			#_d( count )
			print "read %s %d records"%( ip, count )
			for i in range(count):
				print i
				record = do_tcmd( ip, "T,record_get_string %d"%i )
				#_d( record )
				cluster_server_save_record( ip, 0, record )
			do_tcmd( ip, "T,record_remove_all" )
	#print "%d %s end."%(no,ip)

def cluster_server_net_connect_thread( gss, addr ):
	# beats or server state report.
	global output_file_name
	addr_str = addr[0] + ":%d "%addr[1]
	#try:
	if 1:
		while 1:
			try:
				data = gss.recv(1000)
			except:
				print "receive time out"
				data = ""
			#for test the tcpclient send and receive function.
			#gss.send( "ok,123456789\n" );
			if len(data) == 0 :
				break
			if data[0] == '\xa5':
				# dlc package
				#print addr
				gear_misc.out_buffer_binary( 0, data )
			elif data[0:5] == "beats":
				print "beats"
				try:
					g_termial_ip_list.index( addr[0] )
				except:
					g_termial_ip_list.append( addr[0] )
					print "Add " + addr[0] + " into termial list."
			elif data[0:5] == "check":
				gss.send( "ok with %d termial."%len(g_termial_ip_list) )
			else:
				#_d( "receive record" )
				print data
				cluster_server_save_record( addr[0], addr[1], data )

	try:
		str = "abcd"
	except:
		str = "%s socket error"%addr_str
		_d(str)
		gss.close()
	finally:
		str = "%s Closed Connect"%addr_str
		print str
		server_test_write_file( str + '\n' )
		gss.close()

def cluster_server_main( arg1, arg2 ):
	while 1:
		gss, addr = g_socket_no.accept()
		str = '\nclient is at ' + addr[0] + ":%d"%addr[1]
		print str
		server_test_write_file( str + '\n' )
		thread.start_new_thread( cluster_server_net_connect_thread, (gss,addr) )



def cluster_server_termial_main( arg1, arg2 ):
	# loop deal all register termail.
	global g_termial_list
	while 1:
		no = 1
		for t in g_termial_ip_list:
			print "."
			cluster_server_termial_process( no, t )
		time.sleep( 10 )

def file_check_sum( fn ):
	f = open( fn, "rb" )
	buf = f.read()
	f.close()
	check_sum = 0;
	count = 0;
	for c in buf:
		check_sum += ord(c)
		count = count + 1
		s = c
		#if ord(s) == 13:
			#s = ' '
		#if ord(s) == 10:
			#s = ' '
		#print "%d,%d,%c,%d"%(count, ord(c), s, check_sum)

	#print "%s\tcheck_sum = %d\n"%(fn,check_sum)
	return check_sum

def cluster_server_create_white_name_file():
	global g_white_name_file_check_sum 
	global g_white_name_list
	global g_white_name_record_length 
	global g_white_name_record_count 
	#f = os.tmpfile()
	fn = tempfile.mktemp (".txt")
	fn2 = tempfile.mktemp (".txt")
	print fn
	print fn2
	f = open( fn, "w" )
	f2 = open( fn2, "w" )
	val = [ g_white_name_record_count, g_white_name_record_length ]
	str = struct.pack( "II", g_white_name_record_length, g_white_name_record_count );
	#str = struct.pack( "II", val );
	f2.write( str )
	#f.write( "White Name File\n" )
	for i in g_white_name_list:
		#print i
		str = "%s,02,1,2,%s,%s,0000,0\n"%(i[0],i[2],i[1])
		#str = "%d,02,1,2,%s,%s,0000,0\n"%(i[0],i[2],i[1])
		str = (str).decode("GB2312") 
		str = str.encode("UTF-8") 
		while len(str) < g_white_name_record_length-1:
			str += '\xff';
		f.write( str )
		f2.write( '\x01' )
	t = g_white_name_record_count - len( g_white_name_list ) 
	str = '\xff' * g_white_name_record_length
	for i in range( t ):
		f.write( str )
		f2.write( '\xff' )
	f.close()
	f2.close()
	g_white_name_file_check_sum = file_check_sum( fn )
	_d( g_white_name_file_check_sum )
	try:
		os.unlink( "white_name.rf" )
	except:
		print "not file white_name.rf" 
	try:
		os.unlink( "white_name.rf.ri" )
	except:
		print "not file white_name.rf.ri" 
	os.rename( fn, "white_name.rf" )
	os.rename( fn2, "white_name.rf.ri" )
	print "white_name.rf and white_name.rf.ri created"
def cluster_server_create_white_name_file_not_test():
	global g_white_name_file_check_sum 
	global g_white_name_list
	global g_white_name_record_length 
	global g_white_name_record_count 
	#f = os.tmpfile()
	fn = tempfile.mktemp (".txt")
	print fn
	f = open( fn, "w" )
	#f.write( "White Name File\n" )
	for i in g_white_name_list:
		#print i
		str = "%s,02,1,2,%s,%s,0000,0\n"%(i[0],i[2],i[1])
		#str = "%d,02,1,2,%s,%s,0000,0\n"%(i[0],i[2],i[1])
		str = (str).decode("GB2312") 
		str = str.encode("UTF-8") 
		while len(str) < g_white_name_record_length-1:
			str += '\xff';
		f.write( str )
	t = g_white_name_record_length - len( g_white_name_list ) 
	str = '\xff' * g_white_name_record_length
	for i in range( t ):
		f.write( str )
	f.close()
	g_white_name_file_check_sum = file_check_sum( fn )
	try:
		os.unlink( "white_name.rf" )
	except:
		print "not file white_name.rf" 
	os.rename( fn, "white_name.rf" )


	fn2 = tempfile.mktemp (".txt")
	print fn2
	f2 = open( fn2, "w" )
	str = struct.pack( "II", g_white_name_record_count, g_white_name_record_length );
	f2.write( str )
	for i in g_white_name_list:
		f2.write( '\x01' )
	t = g_white_name_record_length - len( g_white_name_list ) 
	for i in range( t ):
		f2.write( '\xff' )
	f2.close()
	try:
		os.unlink( "white_name.rf.ri" )
	except:
		print "not file white_name.rf.ri" 
	os.rename( fn2, "white_name.rf.ri" )
	print "white_name.rf and white_name.rf.ri created"

def cluster_server_white_name_list_main( arg1, arg2 ):
	global g_white_name_list
	while 1:
		buf = "sp_process_000_038"
		#buf = "sp_process_000_036"
		rel = cluster_server_database_process( buf )
		#_d(rel)
		#_d( g_white_name_list )
		if g_white_name_list == "none":
			g_white_name_list = rel
			cluster_server_create_white_name_file()
		elif g_white_name_list != rel :
			g_white_name_list = rel
			print "white name list is update"
			# create white name file
			cluster_server_create_white_name_file()

		#else :
			#print "normal check ok"
		time.sleep( 10 )	# 2 minute

def cluster_server( arg ):
	global g_system_running
	global g_socket_no
	print "cluster server test run..."
	socket.setdefaulttimeout( None )
	g_socket_no = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	port = 8001
	host = ''
	g_socket_no.bind((host, port))
	g_socket_no.listen(1)
	print "wait connect..."
	thread.start_new_thread( cluster_server_input_thread, (0,0) )
	thread.start_new_thread( cluster_server_termial_main, (0,0) )
	thread.start_new_thread( cluster_server_white_name_list_main, (0,0) )
	thread.start_new_thread( cluster_server_main, (0,0) )
	while g_system_running:
		time.sleep( .5 )
	g_socket_no.close
	return 0

def init_termial_cam( ip, arg ):
	# prop_file.
	cam_prop_txt_temp="""hostip=1,192.168.1.72
hport=1,8001
hosttype=1,tcmd
main_info=1,Welcome
title_info=1,DLC
lock_time=1,3000
save_mode=1,disconnect
"""

	hw_address = do_tcmd( ip, "T,prop_get,/p/hw_address" )
	fn = tempfile.mktemp (".txt")
	#print fn
	f = open( fn, "w" )
	f.write( "hw_address=1,%s\n"%hw_address )
	f.write( "ip=1,%s\n"%ip )
	f.write( cam_prop_txt_temp )
	f.close()
	do_post_file( ip, fn, "/c/prop.dat" )

	# show file.
	cam_show_txt_temp="""非法卡
s;4;;, 允许进入
s;4;;, 禁止进入
"""
	fn = tempfile.mktemp (".txt")
	f = open( fn, "w" )
	str = cam_show_txt_temp.decode("GB2312") 
	str = str.encode("UTF-8") 
	f.write( str )
	f.close()
	do_post_file( ip, fn, "/c/show" )

	# reset data set
	do_tcmd( ip, "T,reset_white_name_data" )
	do_tcmd( ip, "T,reset_cam_data" )
	do_tcmd( ip, "T,reset" )

	return

def init_termial( ip, arg ):
	return init_termial_cam( ip,arg )

def database_process_test( arg ):
	global database_ip
	print database_ip
	conn = pymssql.connect( host=database_ip,user="dlceasy",password="dlc",database="dlceasy" )
	#conn = pymssql.connect( host=database_ip,user="dlcuser",password="dlc",database="dlcdata" )
	cur = conn.cursor()
	#cur.execute( "exec sp_process_000_036 " );
	arg = "exec " + arg
	print arg
	cur.execute( arg )
	datas = None;
	datas = cur.fetchall()
	for d in datas:
		print d
	conn.commit()
	conn.close()
	return 0

def my_urlencode(str) :
       reprStr = repr(str).replace(r'\x', '%')
       return reprStr[1:-1]


def do_post_file( ipaddr, local_file_name, remote_file_name ):
	return http_post_file( ipaddr, local_file_name, local_file_name )

	global input_file_data
	input_file_data = get_file_data( local_file_name )
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	host = ipaddr
	port = 80
	s.connect((host, port))

	str = {'sys_action':'save_post_file'}
	data = urllib.urlencode( str );
	str = {'file_name':remote_file_name}
	data += '&' + urllib.urlencode( str );
	str = {'file_content':input_file_data}
	data += '&' + urllib.urlencode( str );
	#print data
	f = urllib2.urlopen(
			url = "http://%s/disk"%(ipaddr),
			data = data
			)
	f.read()
	f.close()

def tcmd_post_file( ipaddr, arg ):
	global input_file_data
	#print 'tcmd_post_file running...'
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	host = ipaddr
	port = 80
	s.connect((host, port))

	str = {'sys_action':'save_post_file'}
	data = urllib.urlencode( str );
	str = {'file_name':arg}
	data += '&' + urllib.urlencode( str );
	str = {'file_content':input_file_data}
	data += '&' + urllib.urlencode( str );
	print data
	f = urllib2.urlopen(
			url = "http://%s/disk"%(ipaddr),
			data = data
			)
	f.read()


def http_post_file2( ipaddr, arg ):
	data = {'submit': 'Send',
		'file':  open('sss','rb')
	       }
	urllib2.urlopen('http://192.168.1.90/upload.cgi', data)
	
def http_post_file( ipaddr, arg ):
	try:
		register_openers()
		datagen, headers = multipart_encode({ "file": open( arg ,"rb") })
		#print 'datagen'
		#print datagen
		#print 'headers'
		#print headers
		request = urllib2.Request("http://%s/upload.cgi"%ipaddr, datagen, headers)
		#print 'request'
		#print request
		urllib2.urlopen(request).read()
		print "ok"
	except:
		print "failed"
def http_get_file( ipaddr, arg ):
	#print 'http_get_file'
	try:
		f = urllib2.urlopen('http://%s/%s'%( ipaddr, arg ))
		s = f.read()
		f = open( arg, 'wb' )
		f.write( s )
		f.close()
		#print s
		print "ok"
	except:
		print "failed"

def http_post_file3( ipaddr, arg ):
	f = open('sssss','rb')
	request = urllib2.Request("http://192.168.1.90/upload.cgi",f.read())
	request.add_header("Content-Type", "application/zip")
	response = urllib2.urlopen(request)
	print response


def tcmd_get_file( ipaddr, arg ):
	global output_file_name
	content = ''
	#print 'tcmd_get_file running...'
	socket.setdefaulttimeout( 2 )
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		host = ipaddr
		port = 80
		s.connect((host, port))
		arg = "GET /download?file=" + arg + '\0';
		#print 'C:\t' + arg
		s.send( arg )
		#time.sleep( 2 )
		
		header_flag = 0;
		while( 1 ):
			time.sleep( .3 )
			data = s.recv(10000)
			gear_misc.out_buffer_binary( 0, data )
			if len(data) <= 0 :
				break
			if header_flag == 0:
				items = data.split( '\n\n' )
				if len( items ) >= 2 :
					header_flag = 1
					content += items[1]
			else:
				content += data;
		s.close()
	except:
		print "socket error"

	print 'output_file_name = ' + output_file_name
	if output_file_name == '' :
		print content
	else :
		#print 'save file'
		f = open( output_file_name, 'wb' )
		f.write( content )
		f.close()

def http_do_tcmd( ipaddr, arg ):
	socket.setdefaulttimeout( 2 )
	try:
		f = urllib2.urlopen('http://%s/t.cgi?%s'%( ipaddr, arg ))
		s = f.read()
		#f = open( arg, 'wb' )
		#f.write( s )
		#f.close()
		#print "ok"
		print s
	except:
		#print "time out"
		#s = "time out"
		print "do_tcmd {%s,%s} failed"%(ipaddr,arg)
		return "Error"
	while len(s) > 0 :
		if ord(s[-1]) == 0 or ord(s[-1]) == 0xd or ord(s[-1]) == 0xa :
			s = s[0:-1]
		else :
			break
	return s
def do_tcmd( ipaddr, arg ):
	return http_do_tcmd( ipaddr, arg )
	try:
		socket.setdefaulttimeout( 2 )
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		host = ipaddr
		port = 80
		s.connect((host, port))
		arg += '\0'
		s.send( arg )
		data = s.recv(1000000)
		s.close()
		socket.setdefaulttimeout( None )
	except:
		print "do_tcmd {%s,%s} failed"%(ipaddr,arg)
		return "Error"
	while len(data) > 0 :
		if ord(data[-1]) == 0 or ord(data[-1]) == 0xd or ord(data[-1]) == 0xa :
			data = data[0:-1]
		else :
			break
	return data

def tcmd_test( ipaddr, arg ):
	data = do_tcmd( ipaddr, arg )
	print data
	return

	

def test():
	#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	#host = sys.argv[1] # server address
	host = "192.168.1.203"
	#port = int(sys.argv[2]) # server port
	port = 8080
	#port = 80
	s.connect((host, port))
	d = gear_misc.get_record_info( s )
	print "%02x" % ord(d[0])
	d = gear_misc.memory_read( s, 0xa0000000 + 0x370*528, 16 )
	print len( d )
	print "%02x" % ord(d[0])
	d = gear_misc.memory_read( s, 0xa0000000 + 0x200ffc, 16 )
	print len( d )
	print "%02x" % ord(d[0])
	print "%02x" % ord(d[1])
	s.close()

def test_w51():
	begin_ip_str = sys.argv[1:][0]
	#end_ip_str = sys.argv[2:][0]
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	host = begin_ip_str
	port = 8080
	s.connect((host, port))

	d = gear_misc.memory_read( s, 0xb0000000 + 0x700, 0x29 )
	gear_misc.out_buffer_binary( 0, d )

	s.close()

def test_getopt():
	try: 
		opts, args = getopt.getopt(sys.argv[1:], "b:e:", ["help", "output=", "reset", "set_dlc_ip="]) 
	except getopt.GetoptError: 
		print "usage: xx"
		return
	

	for opt, arg in opts:
		print opt
		print arg

def usage():		
		print
		print "usage: %s options" % sys.argv[0:]
		print "\t-d : debug output "
		print "\t-b --begin_ip address : begin address range "
		print "\t-e --end_ip address : begin address range "
		print "\t--reset : reset the machine "
		print "\t--set_dlc_ip ip_address : reset the machine "
		print "\t-a : ( memory / prop ) operate address "
		print "\t-l : memory operate length "
		print "\t-r : read memory "
		print "\t-w hex_string : write hex_string to memory "
		print "\t--get_prop : read prop value "
		print "\t--set_prop value : write prop value "
		print "\t--flash_checksum type : write prop value "
		print "\t--flash_erase "
		print "\t--flash_download_file : write file data to flash "
		print "\t--input_file : read file data "
		print "\t--output_file : write file data "
		print "\t--database_ip : set the mssql database server ip address "
		print "\t--update_main_firmware filename : read file data "
		print "\t--update_main_firmware_vmm filename : read file data "
		print "\t--version : get version "
		print "\t--vmm_write_file : write the file data to memory by vmm "
		print "\t--analysis_record_data flag: display record data from bin file, flag = 0 is AT45, flag = 1 is W25X "
		print "\t--analysis_tas_record_data flag: display TAS record data from bin file, flag is any number, the -a param is dump bin file offset from whole memory "
		print "\t--server_test"
		print "\t--cluster_server"
		print "\t--init_termial type      sync file into termial by type"
		print "\t--database_process_test"
		print '\t--tcmd_test "T,{Command},{Parm1}" '
		print '\t--tcmd "T,{Command},{Parm1}" '
		print '\t--tcmd_get_file "/path/file_name" '
		print '\t--tcmd_post_file "file_name" '
		print '\t--http_post_file "file_name" '
		print '\t--http_get_file "file_name" '
		print "\t\t\tCommand List:"
		print "\t\tprop_get,/p/prop_name"
		print "\t\tprop_set,/p/prop_name,string"
		print "\t\tdf_open,file_name,"
		print "\t\tdf_seek"
		print "\t\tdf_read"
		print "\t\tdf_write"
		print "\t\tdf_close"
		print "\t\tdf_create"
		print "\t\tdf_delete"
		print "\t\tdf_lock"
		print "\t\tdf_sync"
		print "\t\tdf_readline"
		print "\t\tindex_create"
		print "\t\tindex_delete"
		print "\t\tindex_add_key"
		print "\t\tindex_remove_key"
		print "\t\tindex_open"
		print "\t\tindex_close"
		print "\t\tindex_set_key"
		print "\t\tindex_append"
		print "\t\tindex_insert"
		print "\t\tindex_search"
		print "\t\tindex_get_count"
		print "\t\tindex_set_index"
		print "\t\tindex_begin"
		print "\t\tindex_next"
		print "\t\tindex_end"
		print "\t\tindex_get_record"
		print "\t\tcam"
		

def main():

	global input_file_data
	global output_file_name
	global database_ip
	global database_user
	global database_pass
	global database_db

	cmd_opt = ''
	cmd_arg = ''
	cmd_address = ''
	cmd_length = ''
	end_ip_str = ''


	#print len( sys.argv )

	if len( sys.argv ) == 1:
		usage()
		return

	try: 
		opts, args = getopt.getopt(sys.argv[1:], "di:rw:a:l:b:e:", ["begin_ip=", "end_ip=", "help", "output=", "reset", "set_dlc_ip=", "get_prop", "set_prop=", "flash_checksum=", "flash_erase", 'flash_download_file=', 'input_file=', 'database_ip=', 'database_user=', 'database_pass=', 'database_db=', 'output_file=', 'update_main_firmware_vmm=', 'update_main_firmware=', 'version', 'vmm_write_file', 'analysis_record_data=', 'analysis_tas_record_data=', 'init_termial=', 'cluster_server', 'server_test', 'database_process_test=', 'tcmd=', 'tcmd_test=', 'tcmd_get_file=', 'http_get_file=' , 'http_post_file=' , 'tcmd_post_file=' ])
	except getopt.GetoptError: 
		usage();
		return
	#print 

	for opt, arg in opts:
		if opt in ( '-b', '--begin_ip' ):
			begin_ip_str = arg
		if opt in ( '-e', '--end_ip' ):
			end_ip_str = arg
		if opt in ( '--get_prop', '--set_prop', '-r', '-w', '--reset', '--set_dlc_ip', '--flash_checksum', '--flash_erase', '--flash_download_file', '--update_main_firmware_vmm', '--update_main_firmware', '--version', '--vmm_write_file', '--analysis_record_data', '--analysis_tas_record_data' ):
			cmd_opt = opt
			cmd_arg = arg
		if opt == '-a' :
			cmd_address = arg
		if opt == '-l' :
			cmd_length = arg
		if opt == '-d' :
			gear_misc.debug_level = 1
		if opt in ( '--output_file' ):
			output_file_name = arg;
		if opt in ( '--database_ip' ):
			database_ip = arg;
		if opt in ( '--database_user' ):
			database_user = arg;
		if opt in ( '--database_pass' ):
			database_pass = arg;
		if opt in ( '--database_db' ):
			database_db = arg;
		if opt in ( '--input_file' ):
			input_file_data = get_file_data( arg );
			if cmd_length == '':
				cmd_length = "%d" % len( input_file_data )
			print "read file %s size %d" % ( arg, len( input_file_data ) )

		# special for update_main_firmware para deal.
		# change update_main_firmware para format as flash_download_file
		if opt == '--update_main_firmware' :
			input_file_data = get_file_data( cmd_arg )
			cmd_arg = '2'
			cmd_address = '0'
			#cmd_address = '0xa0194400'
			cmd_length = '0'

		if opt == '--update_main_firmware_vmm' :
			input_file_data = get_file_data( cmd_arg )
			cmd_arg = '2'
			cmd_address = '0xa0194400'
			cmd_length = '0'
		if opt == '--server_test' :
			server_test(0)
			return
		if opt == '--cluster_server' :
			cluster_server(0)
			return
		if opt == '--init_termial' :
			init_termial(begin_ip_str, arg)
			return
		if opt == '--database_process_test' :
			#print "database_process_test"
			database_process_test( arg )
			return
		if opt == '--tcmd' :
			http_do_tcmd( begin_ip_str, arg )
			return
		if opt == '--tcmd_test' :
			tcmd_test( begin_ip_str, arg )
			return
		if opt == '--tcmd_get_file' :
			tcmd_get_file( begin_ip_str, arg )
			return
		if opt == '--tcmd_post_file' :
			tcmd_post_file( begin_ip_str, arg )
			return
		if opt == '--http_get_file' :
			http_get_file( begin_ip_str, arg )
			return
		if opt == '--http_post_file' :
			http_post_file( begin_ip_str, arg )
			return
	
	#not use the ip deal flow
	if cmd_opt in ( '--analysis_record_data' ):
		dlc_thread_cmd( 0, cmd_opt, cmd_arg, cmd_address, cmd_length )
		print 
		return
	if cmd_opt in ( '--analysis_tas_record_data' ):
		dlc_thread_cmd( 0, cmd_opt, cmd_arg, cmd_address, cmd_length )
		print 
		return

	if end_ip_str == '' :
		end_ip_str = begin_ip_str
	try:
		tpos = end_ip_str.rindex( '.' )
		if tpos > 0 :
			end_ip_str = end_ip_str[ tpos+1: ]
	except:
		tpos = 0

	tpos = begin_ip_str.rindex( '.' )
	begin_ip = int( begin_ip_str[ tpos+1: ] )
	end_ip = int( end_ip_str ) + 1
	pre_ip = begin_ip_str[ :tpos+1 ]

	socket.setdefaulttimeout( 2 )

	# check cmd_length.
	if cmd_opt in ( '--flash_download_file', '--vmm_write_file' ):
		cmd_length = "%d" % len( input_file_data )
		print "cmd_length : %s" % cmd_length

	print "cmd_length : %s" % cmd_length
	for i in range( begin_ip , end_ip ):
		ip = "%s%d" % (pre_ip, i)
		thread.start_new_thread( dlc_thread, ( ip, cmd_opt, cmd_arg, cmd_address, cmd_length ) )

	#time.sleep( 6 )
	#sys.stdin.read
	raw_input("Press Enter to output result ! \n")

	output_line.sort()

	count = 0
	for i in output_line:
		count += 1
		print "%d\t%s" % (count, i)


relatvie_to_data_time( 0x13e07b56 )
signal.signal(signal.SIGINT,sigint_handler) 

#test_getopt()
main()
#test()
#test_w51()

