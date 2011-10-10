#ifndef PROTECT_CODE_H
#define PROTECT_CODE_H


typedef enum
{
	PKT_NONE						= 0x00,

	PKT_ALL_TRANSMIT_V1,
	PKT_ALL_TRANSMIT_V2,
	PKT_SAFE_CONNECT_V2,
	PKT_LDR_TRANSMIT_V2,
	PKT_APP_TRANSMIT_V2,

	PKT_END
} PROTECT_KEY_TYPE;

BYTE *protect_get_key(WORD key_type);


#endif
