//===========================================================================================
#include "OS.h"


//===========================================================================================
//
static const BYTE KEY_ALL_TRANSMIT_V1[8] 				= { 0xaa, 0x36, 0x55, 0x3b, 0x7a, 0x17, 0x40, 0xb5 };
static const BYTE KEY_ALL_TRANSMIT_V2[8] 				= { 0x95, 0x48, 0x39, 0x85, 0xbd, 0xf8, 0x9a, 0xc5 };

BYTE *protect_get_key(WORD key_type)
{
	const BYTE KEY_NULL[8] 							= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	BYTE *key = (BYTE *)KEY_NULL;

	switch (key_type)
	{
	case PKT_ALL_TRANSMIT_V1:		key = (BYTE *)KEY_ALL_TRANSMIT_V1;				break;
	case PKT_ALL_TRANSMIT_V2:		key = (BYTE *)KEY_ALL_TRANSMIT_V2;				break;
	default:						key = (BYTE *)KEY_NULL;							break;
	}

	return key;
}
