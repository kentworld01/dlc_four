
#include "lcd.h"
#include <LM3Sxxxx.H>
#include <string.h>

/*----------------------------------------------------------------------------
 * LCD
 *---------------------------------------------------------------------------*/

#define LCD_INTERFACE_8080 1
#define LCD_INTERFACE_6800 0

#define LCD_SEL() LCD_CS1_SEL()
#define LCD_BL_SET() LCD_BL1_SET()
#define LCD_BL_CLR() LCD_BL1_CLR()


int DB_DirIn()
{
	GPIODirModeSet(GPIO_PORTE_BASE, 0xff, GPIO_DIR_MODE_IN );
	return 0;
}
int DB_DirOut()
{
	GPIODirModeSet(GPIO_PORTE_BASE, 0xff, GPIO_DIR_MODE_OUT );
	return 0;
}
int DB_RD_SET()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_PIN_7 );
	return 0;
}
int DB_RD_CLR()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_7, 0);
	return 0;
}
int DB_WR_SET()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_PIN_6 );
	return 0;
}
int DB_WR_CLR()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_6, 0);
	return 0;
}
int DB_CLE_SET()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_5, GPIO_PIN_5 );
	return 0;
}
int DB_CLE_CLR()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_5, 0);
	return 0;
}
BYTE DB_Read(void)
{
	DB_DirIn();
	return GPIOPinRead (GPIO_PORTE_BASE, 0xff);
}

void DB_Write(BYTE x)
{
	DB_DirOut();
	GPIOPinWrite (GPIO_PORTE_BASE, 0xff, x);
}




int LCD_CS1_SEL()
{
	GPIOPinWrite (GPIO_PORTB_BASE, GPIO_PIN_4, 0);
	return 0;
}
int LCD_CS1_UNSEL()
{
	GPIOPinWrite (GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4 );
	return 0;
}
int LCD_CS2_SEL()
{
	return 0;
}
int LCD_CS2_UNSEL()
{
	return 0;
}
int LCD_UNSEL()
{
	LCD_CS1_UNSEL();
	return 0;
}
int LCD_BL1_SET()
{
	GPIOPinWrite (GPIO_PORTB_BASE, GPIO_PIN_5, 0);
	return 0;
}
int LCD_BL1_CLR()
{
	GPIOPinWrite (GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_PIN_5 );
	return 0;
}
//int LCD_BL2_SET()
//{
	//return 0;
//}
//int LCD_BL2_CLR()
//{
	//return 0;
//}
int LCD_RS_SET()
{
	DB_CLE_SET();
	return 0;
}
int LCD_RS_CLR()
{
	DB_CLE_CLR();
	return 0;
}
int LCD_RW_SET()
{
	DB_RD_SET();
	return 0;
}
int LCD_RW_CLR()
{
	DB_RD_CLR();
	return 0;
}
int LCD_E_SET()
{
	DB_WR_SET();
	return 0;
}
int LCD_E_CLR()
{
	DB_WR_CLR();
	return 0;
}
int LCD_RD_SET()
{
	DB_RD_SET();
	return 0;
}
int LCD_RD_CLR()
{
	DB_RD_CLR();
	return 0;
}
int LCD_WR_SET()
{
	DB_WR_SET();
	return 0;
}
int LCD_WR_CLR()
{
	DB_WR_CLR();
	return 0;
}
int LCD_RST_SET()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_4, GPIO_PIN_4);
	return 0;
}
int LCD_RST_CLR()
{
	GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_4, 0);
	return 0;
}



BYTE LCD_Status(void)
{
	BYTE Status;

//	CPU_Enter_Critical();

	DB_DirIn();
	LCD_RS_CLR();
	LCD_RD_SET();
	LCD_WR_SET();
	LCD_SEL();
	LCD_RD_CLR();
	Status = DB_Read();
	LCD_RD_SET();
	LCD_UNSEL();
//	CPU_Exit_Critical();

	return Status;
}







BYTE LCDPanelSelect;
BYTE __FrameBuffer[__LCD_FRAME_BUFFER_SIZE * 1];
BYTE *FrameBuffer = __FrameBuffer;


int CPU_Enter_Critical()
{
	return 0;
}
int CPU_Exit_Critical()
{
	return 0;
}

void _LCD_FBClear(BYTE BackGround)
{
	memset((void *)FrameBuffer, BackGround, __LCD_FRAME_BUFFER_SIZE);
}

void LCD_Insturction(BYTE Insturction)
{
	CPU_Enter_Critical();

	LCD_Busy();

#if (LCD_INTERFACE_8080 == 1)
	LCD_RS_CLR();
	LCD_RD_SET();
	LCD_WR_SET();
	LCD_SEL();
	DB_Write(Insturction);
	//DB_Write(Insturction);
	//LCD_SEL();
	LCD_WR_CLR();
	delay(1);
	LCD_WR_SET();
	LCD_UNSEL();
	DB_DirIn();
#endif

#if (LCD_INTERFACE_6800 == 1)
	LCD_E_CLR();
	LCD_RS_CLR();
	LCD_RW_CLR();
	LCD_SEL();
	DB_Write(Insturction);
	LCD_E_SET();
	LCD_E_CLR();
	LCD_UNSEL();
	DB_DirIn();
#endif

	CPU_Exit_Critical();
}
BYTE LCD_Read(void)
{
	BYTE Data;

//	CPU_Enter_Critical();

	LCD_Busy();

#if (LCD_INTERFACE_8080 == 1)
	DB_DirIn();
	LCD_RS_SET();
	LCD_RD_SET();
	LCD_WR_SET();
	LCD_SEL();
	LCD_RD_CLR();
	Data = DB_Read();
	LCD_RD_SET();
	LCD_UNSEL();
#endif

#if (LCD_INTERFACE_6800 == 1)
	DB_DirIn();
	LCD_E_CLR();
	LCD_RS_SET();
	LCD_RW_SET();
	LCD_SEL();
	LCD_E_SET();
	Data = DB_Read();
	LCD_E_CLR();
	LCD_UNSEL();

	LCD_Busy();

	DB_DirIn();
	LCD_E_CLR();
	LCD_RS_SET();
	LCD_RW_SET();
	LCD_SEL();
	LCD_E_SET();
	Data = DB_Read();
	LCD_E_CLR();
	LCD_UNSEL();
#endif

//	CPU_Exit_Critical();

	return Data;
}
void LCD_Write(BYTE Data)
{
	CPU_Enter_Critical();

	LCD_Busy();

#if (LCD_INTERFACE_8080 == 1)
	LCD_RS_SET();
	LCD_RD_SET();
	LCD_WR_SET();
	LCD_SEL();
	DB_Write(Data);
	LCD_WR_CLR();
	LCD_WR_SET();
	LCD_UNSEL();
	DB_DirIn();
#endif

#if (LCD_INTERFACE_6800 == 1)
	LCD_E_CLR();
	LCD_RS_SET();
	LCD_RW_CLR();
	LCD_SEL();
	DB_Write(Data);
	LCD_E_SET();
	LCD_E_CLR();
	LCD_UNSEL();
	DB_DirIn();
#endif

	CPU_Exit_Critical();
}

BOOL _lcd_write_and_read_check(void)
{
	DWORD i;

	LCD_Insturction(0xb0 | (0));				// YPAGE
	LCD_Insturction(0x10);					// Column address High 4bit
	LCD_Insturction(0x00);					// Column address Low  4bit

	for(i = 0x00; i < 0x80; i++)
		LCD_Write(i);

	LCD_Insturction(0xb0 | (0));				// YPAGE
	LCD_Insturction(0x10);					// Column address High 4bit
	LCD_Insturction(0x00);					// Column address Low  4bit
	LCD_Read();								// Dummy byte

	for(i = 0x00; i < 0x80; i++)
		if (i != LCD_Read())
			return FALSE;

	return TRUE;
}

void _LCD_FBTransmit(void)
{
	BYTE yp, x, *p;

	// 从上到下，对LCD RAM单元按YPAGE单位寻址，YPAGE=0~7，
	for(yp = 0, p = FrameBuffer + __LCD_WIDTH_LIMIT - 1; yp < __LCD_FB_Y_PAGES; yp++) {
		// 按YPAGE单位寻址
		LCD_Insturction(0xb0 | (yp));				// YPAGE
		LCD_Insturction(0x10);					// Column address High 4bit
		LCD_Insturction(0x00);					// Column address Low  4bit

		// LCD RAM显示单元从左向右按像素寻址，每次写入1列Y方向8像素
		for(x=0; x<__LCD_WIDTH_LIMIT; x++)
			LCD_Write(*p--);						// 写一列Y方向8像素数据到LCD RAM

		p += __LCD_WIDTH_LIMIT * 2;
	}
}
BOOL _LCD_FBVerify(void)
{
	BYTE yp, x, *p;

	// 从上到下，对LCD RAM单元按YPAGE单位寻址，YPAGE=0~7，
	for(yp=0,p=FrameBuffer+__LCD_WIDTH_LIMIT-1; yp<__LCD_FB_Y_PAGES; yp++) {
		// 按YPAGE单位寻址
		LCD_Insturction(0xb0 | (yp));				// YPAGE
		LCD_Insturction(0x10);					// Column address High 4bit
		LCD_Insturction(0x00);					// Column address Low  4bit
		LCD_Read();								// Dummy byte

		// LCD RAM显示单元从左向右按像素寻址，每次写入1列Y方向8像素
		for(x=0; x<__LCD_WIDTH_LIMIT; x++)
			if (*p-- != LCD_Read())
				return FALSE;

		p += __LCD_WIDTH_LIMIT * 2;
	}

	return TRUE;
}
void LCD_DisplayOn(BOOL On)
{
	if (On)
		LCD_Insturction(0xaf);			// 0xaf: ON; 0xae: Display OFF
	else
		LCD_Insturction(0xae);			// 0xaf: ON; 0xae: Display OFF
}
void LCD_SideLightOn(BOOL On)
{
	if (On)
		LCD_BL_SET();
	else
		LCD_BL_CLR();
}



BOOL _LCD_Init(void)
{
	// 初始化LCD控制器
	LCD_Insturction(0xae);			// 0xaf: ON; 0xae: Display OFF
	LCD_Insturction(0x40);			// Start line
	LCD_Insturction(0xb0);			// Page address
	LCD_Insturction(0x10);			// Column address High 4bit
	LCD_Insturction(0x00);			// Column address Low  4bit
	LCD_Insturction(0xa0);			// ADC normal; 0xa1: 反相
	LCD_Insturction(0xa6);			// 0xa6: 正相；0xa7: 反相
	LCD_Insturction(0xa4);			// 0xa4: 正常；0xa5: 全亮
	LCD_Insturction(0xa2);			// 0xa2: 1/9; 0xa3: 1/7 BIAS
	LCD_Insturction(0xc0);			// 0xc0: 正向； 0xc8: 反向
	LCD_Insturction(0x2f);			// Booster, regulator, follower, Power on
	LCD_Insturction(0x25);			// V0 分压比，set the Rab ratio to middle

#if (LCD_VENDOR_ET == 1)
	LCD_Insturction(0x81);			// E-Vol setting
	LCD_Insturction(0x27);			// 对比度，2BYTE COMMAND
#endif

//#if (LCD_VENDOR_TPV == 1)
	LCD_Insturction(0x81);			// E-Vol setting
	LCD_Insturction(0x20);			// 对比度，2BYTE COMMAND
//#endif

	LCD_Insturction(0xaf);			// 0xaf: ON; 0xae: Display OFF

	if ((LCD_Status() & LCDS_BUSY)
			|| (LCD_Status() & LCDS_DISPLAYOFF)
			|| (LCD_Status() & LCDS_RESET))
		return FALSE;

	return TRUE;
}
void LCD_On(BOOL On)
{

		LCD_DisplayOn(On);
		LCD_SideLightOn(On);
}

void LCD_Refresh(void)
{
	_LCD_FBTransmit();
}

void LCD_CLS(BYTE BackGround)
{
	_LCD_FBClear(BackGround);
	LCD_Refresh();
}

BOOL LCD_Init(BOOL KeepFB)
{
	BOOL b, ok=1;
	{
		if (0xaa != KeepFB)
			_LCD_FBClear(0x00);
		b = _LCD_Init();
		if (b)
			b = _lcd_write_and_read_check();
		if (b)
			_LCD_FBTransmit();
		if (b)
			b = _LCD_FBVerify();
		if (b && (0xaa == KeepFB))
		{
			LCD_DisplayOn(1);
			LCD_SideLightOn(1);
		}
		ok &= b;
	}
	return  ok;
}

void delay(unsigned int i)
{
	unsigned int k=1000;
	while(i>0)
	{
		i--;
	}
	while(k>1)k--;
}
void delays( int i )
{
	delay( 15000000 );
}


void delayms (unsigned int i) {                        /* Delay function */
	unsigned int n;
	while(i>1)
	{
		for(n=65535;n>1;n--);
		i--;
	}
}
void	delay1s(unsigned char i)
{
	while(i>1)
	{
		i--;
		delayms(20);
	}
}


int lcd_init()
{
	//LCD_SideLightOn(1);
	//LCD_RD_SET();
	//LCD_WR_SET();
	//LCD_RS_CLR();
	////LCD_RS_SET();
	//LCD_UNSEL();

	//LCD_RST_CLR();
	//delay( 1000 );
	//LCD_RST_SET();
	//LCD_Status();
	delay( 1000 );
	LCD_RST_CLR();
	delay( 1000 );
	LCD_RST_SET();
	//LCD_Status();

	LCD_Init(0xaa);
	//LCD_CLS( 0 );
	_LCD_FBClear(0);
	show_logo();
	//LCD_CLS( 0xa5 );
	//LCD_Refresh();
	LCD_Status();
	return 0;
}

