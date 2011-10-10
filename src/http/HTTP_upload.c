/*----------------------------------------------------------------------------
 *      R T L   T C P N E T   E x a m p l e
 *----------------------------------------------------------------------------
 *      Name:    HTTP_UPLOAD.C
 *      Purpose: HTTP File Upload example
 *      Rev.:    V3.22
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <RTL.h>
#include <Net_Config.h>
#include <LM3Sxxxx.H>
#include <string.h>
#include "lcd.h"
#include "os.h"
//#include "..\..\..\rit128x96x4.h"


BOOL LCDupdate;
BOOL tick;
U32  dhcp_tout;
U8   lcd_text[4][16+1] = {"    RL-ARM",       /* Buffer for LCD text         */
	"HTTP file upload",
	" ",
	"Waiting for DHCP"};
extern LOCALM localm[];                       /* Local Machine Settings      */
#define MY_IP localm[NETIF_ETH].IpAdr
#define DHCP_TOUT   50                        /* DHCP timeout 5 seconds      */

//static void init_io (void);



typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned DWORD;

#define FALSE 0
#define TRUE 1


void delay(unsigned int i);
//static void init_io3 (void);
static void init_io2 (void);
int jtag_to_io(void);
int sendchar (int ch);




/*--------------------------- init ------------------------------------------*/

static void init () {
	/* Add System initialisation code here */ 

	init_io2 ();
#if 0   
	//init_io3 ();
	while( 1 ){
		sendchar( 'A' );
	}
#endif   
	//init_TcpNet ();

	/* Setup and enable the SysTick timer for 100ms. */
	SysTickPeriodSet(SysCtlClockGet() / 10);
	SysTickEnable();


	jtag_to_io();
	spi_init ();
  	if( W25Q32_JEDECID != W25X_ReadJEDECID() ){
		// error message :  flash chip incorrect
		while( 1 );
	}

	//w25x_test();

	//W25X_ChipErase();
	uffs_flash_interface_init();
	//cmdTest1();
	cmdTest2();

	rtc_init();
	Font_Init();

	access_init();
	//modules_test();
}

/*--------------------------- timer_poll ------------------------------------*/

static void timer_poll () {
	/* System tick timer running in poll mode */

	if ((HWREG (NVIC_ST_CTRL) >> 16) & 1) {
		/* Timer tick every 100 ms */
		timer_tick ();
		tick = __TRUE;
	}
}


/*--------------------------- init_io ---------------------------------------*/
int jtag_io_function_flag = 0;
int jtag_to_io()
{
	// delay for jtag connect
	delays( 1 );

	// clear the jtag , 
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOC);
	GPIOPadConfigSet (GPIO_PORTC_BASE, 0xf, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet (GPIO_PORTC_BASE, 0xf, GPIO_DIR_MODE_OUT);

	_d_int( jtag_io_function_flag );

	if(0){
		unsigned *po;
		// lock register
		po = (unsigned*)0x40006520;
		*po = 0x1acce551;
		// commit register
		po = (unsigned*)0x40006524;
		*po = 0xf;

	}
	GPIOPinWrite (GPIO_PORTC_BASE, 0xf, 0 );	
	if(0){
		unsigned *po;
		// commit register
		po = (unsigned*)0x40006524;
		*po = 0;
		// lock register
		po = (unsigned*)0x40006520;
		*po = 0x1;
	}
	return 0;
}
static void init_io2 ()
{

	/* Set the clocking to run from the PLL at 50 MHz */
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	//SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
	////SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);



	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
	GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet (GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_DIR_MODE_IN);
	GPIODirModeSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_DIR_MODE_OUT);


	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOE);
	GPIOPadConfigSet (GPIO_PORTE_BASE, 0xff, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet (GPIO_PORTE_BASE, 0xff, GPIO_DIR_MODE_IN);


	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
	// get jtag flag PB5 io state.
	GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet (GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_DIR_MODE_IN);
	jtag_io_function_flag = GPIOPinRead (GPIO_PORTB_BASE, GPIO_PIN_5);


	GPIOPadConfigSet (GPIO_PORTB_BASE, 0xff, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet (GPIO_PORTB_BASE, 0xff, GPIO_DIR_MODE_OUT);


	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOD);
	GPIOPadConfigSet (GPIO_PORTD_BASE, 0xff, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet (GPIO_PORTD_BASE, 0xff, GPIO_DIR_MODE_OUT);
	//LCD_RST_SET();
	//delay(1000);
	lcd_init();

	/* Configure the GPIO for the LED. */
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
	GPIOPadConfigSet (GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2  | GPIO_PIN_3 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	GPIODirModeSet (GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 , GPIO_DIR_MODE_OUT);
	GPIODirModeSet (GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN );
	GPIOPinWrite (GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_2|GPIO_PIN_3);

	/* Configure UART0 for 115200 baud. */
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable (SYSCTL_PERIPH_UART1);
	IntMasterEnable();
	GPIOPinTypeUART (GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	UARTConfigSet(UART1_BASE, 19200 * 8 / 6, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	//UARTConfigSet(UART1_BASE, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTEnable(UART1_BASE);
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	//sendchar( 'A' );
	//GPIO_O_AFSEL
}


	void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
	while(ulCount--)
	{
		UARTCharPut(UART1_BASE, *pucBuffer++);
	}
}

#if 0
static void init_io3 () 
{
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	IntMasterEnable();
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	UARTConfigSet(UART1_BASE, 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
			 UART_CONFIG_PAR_NONE));
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
	UARTSend((unsigned char *)"Enter text: ", 12);
	while(1)
	{
		//UARTSend((unsigned char *)"A", 1);
	}
}
#endif


#if 0

/*--------------------------- sendchar --------------------------------------*/

int sendchar (int ch)  {
	/* A dummy function for 'retarget.c' */
	return (ch);
}

/*--------------------------- getkey ----------------------------------------*/

int getkey (void) {
	/* A dummy function for 'retarget.c' */
	return (0);
}

#else

#define CR     0x0D


unsigned g_deal_card_no = 0;		// now deal the card no, is g_card_no copy, but not real time copy.
unsigned g_card_no = 0;
unsigned char g_card_data[ 20 ];
int g_card_data_count = 0;

	void
UARTIntHandler(void)
{
	unsigned long ulStatus;
	int val;
	ulStatus = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, ulStatus);
	while(UARTCharsAvail(UART1_BASE)){
		//UARTCharPut(UART1_BASE, UARTCharGet(UART1_BASE)+1 );
		val = UARTCharGet(UART1_BASE);
		if( val == 0x23 ){
			g_card_data_count = 0;
		}
		if( g_card_data_count < sizeof( g_card_data ) - 1 )
			g_card_data[ g_card_data_count++ ] = val ;
		if( val == 0x0a ){
			// card data end flag
			memcpy( (unsigned char*)&g_card_no, &g_card_data[2], 4 );
			g_card_no = swap_dword( g_card_no );
		}
	}
}


/*----------------------------------------------------------------------------
 *       init_serial:  Initialize Serial Interface
 *---------------------------------------------------------------------------*/
void init_serial (void) {
}

/*----------------------------------------------------------------------------
 *       sendchar:  Write a character to Serial Port
 *---------------------------------------------------------------------------*/
int sendchar (int ch) {
	UARTCharPut( UART1_BASE, ch );
	return 0;
}

/*----------------------------------------------------------------------------
 *       getkey:  Read a character from Serial Port
 *---------------------------------------------------------------------------*/
int getkey (void) 
{
	unsigned long ulStatus;
	ulStatus = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, ulStatus);
	//UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
	return UARTCharGet(UART1_BASE);
}

#if 0
	void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
	//
	// Loop while there are more characters to send.
	//
	while(ulCount--)
	{
		//
		// Write the next character to the UART.
		//
		UARTCharPut(UART0_BASE, *pucBuffer++);
	}
}
#endif



#endif



/*--------------------------- LED_out ---------------------------------------*/

void LED_out (U32 val) {
	//GPIOPinWrite (GPIO_PORTF_BASE, GPIO_PIN_0, val & 1);
}


/*--------------------------- upd_display -----------------------------------*/


/*--------------------------- init_display ----------------------------------*/



/*--------------------------- dhcp_check ------------------------------------*/

static void dhcp_check () {
	/* Monitor DHCP IP address assignment. */

	if (tick == __FALSE || dhcp_tout == 0) {
		return;
	}
	if (mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE && !(dhcp_tout & 0x80000000)) {
		/* Success, DHCP has already got the IP address. */
		dhcp_tout = 0;
		sprintf((char *)lcd_text[2],"IP address:");
		sprintf((char *)lcd_text[3],"%d.%d.%d.%d", MY_IP[0], MY_IP[1],
				MY_IP[2], MY_IP[3]);
		LCDupdate = __TRUE;
		return;
	}
	if (--dhcp_tout == 0) {
		/* A timeout, disable DHCP and use static IP address. */
		dhcp_disable ();
		sprintf((char *)lcd_text[3]," DHCP failed    " );
		LCDupdate = __TRUE;
		dhcp_tout = 30 | 0x80000000;
		return;
	}
	if (dhcp_tout == 0x80000000) {
		dhcp_tout = 0;
		sprintf((char *)lcd_text[2],"IP address:");
		sprintf((char *)lcd_text[3],"%d.%d.%d.%d", MY_IP[0], MY_IP[1],
				MY_IP[2], MY_IP[3]);
		LCDupdate = __TRUE;
	}
}


/*--------------------------- blink_led -------------------------------------*/

static void blink_led () {
	/* Blink the LEDs on an eval board */
	static U32 LEDstat = 1;

	if (tick == __TRUE) {
		/* Every 100 ms */
		tick = __FALSE;
		LEDstat = ~LEDstat & 0x01;
		//LED_out (LEDstat);
		if (LCDupdate == __TRUE) {
			//upd_display ();
		}
	}
}


/*---------------------------------------------------------------------------*/

int main (void) {
	/* Main Thread of the TcpNet */

	init ();


	dhcp_tout = DHCP_TOUT;
	while (1) {
		//sendchar( 'A' );
		timer_poll ();
		main_TcpNet ();
		dhcp_check ();
		blink_led ();

		main_loop();
	}
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/








