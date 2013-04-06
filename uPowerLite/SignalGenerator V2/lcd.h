#include <stdio.h>
#include <p18F66K22.h>



#define data PORTE		//data bus

//#define rs   PORTCbits.RC4	//register select 0 for command 1 for data
//#define cs   PORTEbits.RE1  //PORTCbits.RC3	//chip select low
//#define wr   PORTDbits.RD2 //PORTCbits.RC1	//write line 0 for write
//#define rs   PORTBbits.RB7	//register select 0 for command 1 for data
#define cs   PORTAbits.RA4  //PORTCbits.RC3	//chip select low
#define wr   PORTAbits.RA7 //PORTCbits.RC1	//write line 0 for write
#define rs   PORTAbits.RA6  

#define 	LCD_RESET			0x01
#define 	LCD_WAKEUP  		0x11
#define 	LCD_SET16BPP 		0x3A
#define 	LCD_COLUMNSET		0x2A
#define 	LCD_ROWSET			0x2B
#define 	LCD_ON				0x29
#define 	LCD_MEM_WRITE		0x2C
#define		MEM_ACCESS			0x36

#define		WHITE	0XFFFF
#define		BLACK	0X18C3
#define		BLUE	0X0457
#define		VIOLET	0X3191
#define		RED		0XF800
#define		BROWN	0X9326
#define		YELLOW	0XF661
#define		GREEN	0X4EC4
#define     GRAY    0x8410
#define 	LOGOC	0x18ee


void X_Y_Setting();
