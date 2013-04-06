#include <stdio.h>
#include <p18F26K22.h>



#define data PORTA		//data bus

//#define rs   PORTCbits.RC4	//register select 0 for command 1 for data
//#define cs   PORTEbits.RE1  //PORTCbits.RC3	//chip select low
//#define wr   PORTDbits.RD2 //PORTCbits.RC1	//write line 0 for write
//#define rs   PORTBbits.RB7	//register select 0 for command 1 for data
#define cs   PORTBbits.RB5  //PORTCbits.RC3	//chip select low
#define wr   PORTBbits.RB7 //PORTCbits.RC1	//write line 0 for write
#define rs   PORTCbits.RC0  

#define 	LCD_RESET			0x01
#define 	LCD_WAKEUP  		0x11
#define 	LCD_SET16BPP 		0x3A
#define 	LCD_COLUMNSET		0x2A
#define 	LCD_ROWSET			0x2B
#define 	LCD_ON				0x29
#define 	LCD_MEM_WRITE		0x2C
#define		MEM_ACCESS			0x36
#define 	LOGOC				0x18ee

int LCD_draw_round_corner_box ( int Hstart, int Vstart, int Hend, int Vend,
                                int radius, int color, int fill);
void X_Y_Setting();
