
#include <LM3Sxxxx.H>
#include "os.h"
#include "string.h"
#include "device\lcd.h"



int get_input_switch( int no )
{
	int val = 0;
	switch( no ){
		case 0:
	   		val = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_6);
			break;
		case 1:
			   SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
			   GPIOPadConfigSet (GPIO_PORTF_BASE, GPIO_PIN_1 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
			   GPIODirModeSet (GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN );
	   			val = GPIOPinRead (GPIO_PORTF_BASE, GPIO_PIN_1);
			break;
	}
	return val;
}

int demo_test( char** ki )
{
	if( ki[0] == 0 )
		return 0;
	_d_str( ki[1] );
	if( strcmp( ki[1], "lcd_line" ) == 0 ){
		LCD_CLS( 0xa5 );
		LCD_Refresh();
	}
	else
	if( strcmp( ki[1], "lcd_clear" ) == 0 ){
		LCD_CLS( 0 );
		LCD_Refresh();
	}
	else
	if( strcmp( ki[1], "sound_one" ) == 0 ){
   		GPIOPinWrite (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7 );
		delay(1);
   		GPIOPinWrite (GPIO_PORTA_BASE, GPIO_PIN_7, 0 );
		delay(1);
   		GPIOPinWrite (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7 );
	}
	else
	if( strcmp( ki[1], "sound_two" ) == 0 ){
   		GPIOPinWrite (GPIO_PORTA_BASE, GPIO_PIN_7, 0 );
   		//GPIOPinWrite (GPIO_PORTC_BASE, GPIO_PIN_3, GPIO_PIN_3 );
	}
	else
	if( strcmp( ki[1], "sound_three" ) == 0 ){
   		GPIOPinWrite (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7 );
   		//GPIOPinWrite (GPIO_PORTC_BASE, GPIO_PIN_3, 0 );
	}
	else
	if( strcmp( ki[1], "output_one_lock" ) == 0 ){
   		GPIOPinWrite (GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2 );
	}
	else
	if( strcmp( ki[1], "output_one_unlock" ) == 0 ){
   		GPIOPinWrite (GPIO_PORTF_BASE, GPIO_PIN_2, 0 );
	}
	else
	if( strcmp( ki[1], "output_two_lock" ) == 0 ){
   		GPIOPinWrite (GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3 );
	}
	else
	if( strcmp( ki[1], "output_two_unlock" ) == 0 ){
   		GPIOPinWrite (GPIO_PORTF_BASE, GPIO_PIN_3, 0 );
	}

	return 0;
}

int file_test()
{
	FILE *f;
	char *str = "1234567890abcdefghijklmnopqrstuvwxyz";
	
	if (finit() != 0) 
		return;
	f = fopen ((const char *)"test","w");
	
	fwrite (str,1,strlen(str),f);
	fclose (f);
	
}

int modules_test()
{
	// text
	WCHAR wstr[10];
	MONOBMP mb;
	char *str="abcdef";
	unsigned long val = 0;
	extern const unsigned char bmp_dlc[];

	file_test();
	return 0;
	
#if 1
	Font_Init();
	LCD_CLS( 0 );
	string_utf8_to_unicode( str, wstr, sizeof(wstr)/2 );
	Text_DisplayWString( 0,0,100,20, wstr );
	LCD_Refresh();
	
	MonoBmp_Load( &mb, (BYTE*)&bmp_dlc );
	Graph_PutBitmap(0,0,128,64, &mb );
	LCD_Refresh();
#endif
	HibernateEnableExpClk( SysCtlClockGet() );
	HibernateRTCEnable();
	
	//rtc_set_default_time();
	val = HibernateRTCGet();

	HibernateDataGet( &val, 1 );

	HibernateDataGet( &val, 1 );

	return val;
}

int w25x_test()
{
	char buf[16];
	char buf2[16];
	int count = 0;
	int i, address = 0, st = 0, test = 0;
	test = 1;
	_flash_select();
	W25X_EnableWrite();
	while(1) switch( test ){
		case 0:
			{
				W25X_EnableWrite();
				_flash_select();
				W25X_SPIWrite(W25X_ERASE_SECTOR);
				W25X_SPIWrite((BYTE)(address >> 16));
				W25X_SPIWrite((BYTE)(address >> 8));
				W25X_SPIWrite((BYTE)(address >> 0));
				_flash_deselect();

				W25X_WaitForReady();

				_flash_select();
				//W25X_SPIWrite(W25X_WRITE_STATUS);
				//st = W25X_SPIRead();
				W25X_SPIWrite(W25X_READ_STATUS);
				st = W25X_SPIRead();
				_flash_deselect();

				_flash_select();
				W25X_SPIWrite(W25X_READ_FAST);
				W25X_SPIWrite((BYTE)(address >> 16));
				W25X_SPIWrite((BYTE)(address >> 8));
				W25X_SPIWrite((BYTE)(address >> 0));
				W25X_SPIRead();						// Dummy byte
				//count = sizeof(buf);
				count = 1;
				for( i=0; i<count; i++ )
					 buf[i] = W25X_SPIRead();			// Receive data
				if( buf[0] == 0xff ){
					//while(1);
				}
				_flash_deselect();
			}
			W25X_WaitForReady();
			break;
		case 1:
			W25X_ChipErase();
			W25X_ReadFast( 0, buf, sizeof(buf) );
			strcpy( buf2, "1234567890" );
			W25X_PageProgram( 0, buf2, sizeof(buf) );
			W25X_ReadFast( 0, buf, sizeof(buf) );
			if( strcmp( buf, buf2 ) ){
				while( 1 );
			}
			break;
		case 2:
			W25X_BlockErase( 0 );
			W25X_ReadFast( 0, buf, sizeof(buf) );
			strcpy( buf2, "1234567890" );
			W25X_PageProgram( 0, buf2, sizeof(buf) );
			W25X_ReadFast( 0, buf, sizeof(buf) );
			if( strcmp( buf, buf2 ) ){
				while( 1 );
			}
			break;
		case 3:
			W25X_SectorErase( 0 );
			W25X_ReadFast( 0, buf, sizeof(buf) );
			strcpy( buf2, "1234567890" );
			W25X_PageProgram( 0, buf2, sizeof(buf) );
			W25X_ReadFast( 0, buf, sizeof(buf) );
			if( strcmp( buf, buf2 ) ){
				while( 1 );
			}
			break;
	}
	_flash_deselect();
	while( 1 );
	return 0;
}

