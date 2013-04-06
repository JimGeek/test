//-----------------!!S L S  C O N F I D E N T I A L!!------------------------------------
// @   Copyright (c) 2001-2010, System Level Solutions (India) Pvt. Ltd.   @
// @                         All rights reserved.                          @
// @                                                                       @
// @    THIS SOFTWARE CONTAINS VALUABLE CONFIDENTIAL AND PROPRIETARY       @
// @    INFORMATION OF SYSTEM LEVEL SOLUTIONS (INDIA) PVT. LTD. AND IS     @
// @    SUBJECTED TO APPLICABLE LICENSING AGREEMENTS. UNAUTHORIZED         @
// @    REPRODUCTION, TRANSMISSION OR DISTRIBUTION OF THIS FILE AND ITS    @
// @    CONTENTS IS A VIOLATION OF APPLICABLE LAWS.                        @
//
//---------------------------C O N T A C T-----------------------------------------------
// Website      : http://www.slscorp.com
// Email        : info@slscorp.com
// Phone        : +91-2692-232501/232502
// Fax          : +91-2692-232503
//
//----------------------------P R O J E C T----------------------------------------------
// Project      : tmstr-p-bm-std-psu
// Client       : Timstar
//
//--------------------------F I L E  D E T A I L S---------------------------------------
// Design Unit  : uPower12
// File         : main.c
// Author       : shreyas patel - supatel@slscorp.com
// Created      : 11/10/2011
// Target Device: pic18f66K22
// Description  :
//					main.c is a main firmware of uPower20 which controls the unit through
//  				GUI on LCD display. It's main operation is to handle GUI through which
//  				It sets the voltage and other features of uPower12. It also handles the
//  				wireless communication. It checks the packets received through Rf and
//  				Operates according to commands in that packet.
//  				It continuously checks for the switch press and go to FSM according to
//  				switch.
//  				It also monitors for overload and thermal shutdown.
//
//
// Operation	: 1) Navigation in GUI through LCD and switch interface.
//
//                2) Set voltage by controlling relays and DAC.
//
//                3) Handles RF functions.
//
//                4) Overload and thermal shutdown.
//
//                5) Fan control through PWM
//
//-----------------------S O U R C E  C O N T R O L-----------------------------
// $Id$
// $Author::                      $ Shreyas patel
// $Date::                        $ 08/11/2011
// $Rev::                         $ 412
//------------------------------------------------------------------------------

/*******************************INCLUDE FILES*****************************/
#include <stdio.h>
#include "p18F26K22.h"
#include "functions.h"
#include "lcd.h"
#include "bitmap.h"
#include <string.h>
#include "font.h"
//include "rfmodule.h"

/***************************configuration bits***************************/
#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config PLLCFG = ON     // 4X PLL Enable (Oscillator used directly)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)

/***************************defines***************************************/


/*define capsense keys*/
//#define ESC		5
#define UP		10
#define DOWN	7
#define OK		9
//#define MENU 	6
//#define LEFT 	8
//#define RIGHT	11

#define key_up 		PORTCbits.RC7
#define key_down 	PORTCbits.RC5
//#define key_left 	PORTAbits.RA3
//#define key_right 	PORTAbits.RA6
#define key_ok 		PORTCbits.RC6
//#define key_menu 	PORTAbits.RA1
//#define key_esc 	PORTAbits.RA7


#define		WHITE		0XFFFF
#define		BLACK		0X18C3
#define		BLUE		0X0457
#define		VIOLET		0X3191
#define		RED			0XF800
#define		BROWN		0X9326
#define		YELLOW		0XF661
#define		GREEN		0X4EC4
#define 	BACKGROUND	0xf800

#define PRESSED		0

//define with the macros for addresses instead of gloabal variables


#define LCD_X_START	1
#define LCD_Y_START	2

#define LCD_X_END	160
#define LCD_Y_END	129


#define MAX_STEPS		2472
#define STEPS_FOR_VOLT	147
#define LCD_ROUND_CORNER_BOX_RADIUS		3
void SetVoltageFocus(void);

unsigned char VtgIndex, timeoutCount,OkKeyEvent;


void delay(unsigned int i)	//for small delays
{
	for(; i > 1; i--);
}

void keyDeBounce(void)
{
	TMR1L = 0;
	T1CONbits.TMR1ON = 1;
	while(TMR1L < 0x64);     //50us delay
	TMR1L = 0;
}

void Timer0Shutdown(void)
{
	INTCONbits.TMR0IF = 0; 		//Reset Timer0 interrupt flag and Return from ISR
	T0CON = 0x07; 			//Stops the Timer0 and reset control reg.; prescaler 1:256; 16bit timer, Fosc/4
	TMR0H = 0x0B; 			//Clear contents of the timer register
	TMR0L = 0xDB;				// 1 second timer
	timeoutCount++;
	T0CONbits.TMR0ON = 1; 	//Start Timer0
}

#pragma code low_vector = 0x0018

void
interrupt_at_low_vector (void)
{
	if(INTCONbits.TMR0IF == 1)
	{
		_asm GOTO Timer0Shutdown _endasm
	}
}
#pragma code

#pragma interruptlow Timer0Shutdown



/*************************************GLOBAL VARIABLES*************************************/

unsigned char voltage [12][3] = {	"01",
							"02",
							"03",
							"04",
							"05",
							"06",
							"07",
							"08",
							"09",
							"10",
							"11",
							"12"
						};


void delay_ms(unsigned int ms)	//function to generate 1ms delay
{
	unsigned int x;

	for(x = 0; x < 8*ms; x++)
		{
			TMR1L = 0x00;
			TMR1H = 0x00;
			T1CON = 0x31;			//turn on timer1 1:8 PRESCALE
			while(TMR1L < 0xFA);	//125uS
			T1CON = 0x30;			// turn off timer
		}
}

void FillFullLcd(int color)		//fill whole lcd with color
{
	LCD_paint_block(LCD_X_START,LCD_Y_START,LCD_X_END,LCD_Y_END,color);
}

void FillViolet(void)		//fill bottom VIOLET strip
{
	LCD_paint_block(LCD_X_START,98,LCD_X_END,LCD_Y_END,VIOLET);
}

void FillBlack(void)		//fill middle BLACK strip
{
	LCD_paint_block(LCD_X_START,34,LCD_X_END,97,BLACK);
}

void SetVoltageFocus(void)
{
	setfont((void *)font_Calibri_26);
	LCD_draw_round_corner_box ( 35, 42, 89, 85,3, BLUE, 1);	//focus on voltage
	LCD_OutText(40,25,voltage[VtgIndex],BLACK);	//voltage
}

void lcd_reset(void)
{
	PORTCbits.RC2 = 0;	//lcd reset
	delay_ms(5);
	PORTCbits.RC2 = 1;
	PORTCbits.RC2 = 1;
	PORTBbits.RB6 = 1;
}

void Config_IO(void)
{
	TRISA = 0x00;
	TRISB = 0X00;	//output
    TRISC = 0XE2;	//output E0, C1 as Input for ss/sd - overload detection: E2
    
    //TRISD = 0X00;	//output
    //TRISE = 0X00;	//output
	//TRISF = 0Xff;	//input

//---------------A/D PORT CONFIGURETION--------------------
	//ADCON0 = 0x00;
	//ADCON1 = 0x00;
	//ADCON2 = 0x00;
	ADCON0 = 0X00;
    ADCON1 = 0X00;
    ADCON2 = 0X00;
	ANSELC = 0X00;

	INTCON = 0x00;
    PIR1 = 0x00;
    PIR2 = 0x00;
    PIR3 = 0x00;
    PIR4 = 0x00;
    PIR5 = 0x00;
//  PIR6 = 0x00;
    PIE1 = 0x00;
    PIE2 = 0x00;
    PIE3 = 0x00;
    PIE4 = 0x00;
    PIE5 = 0x00;
//  PIE6 = 0x00;
}


void DisplayVoltage(void)			//displays voltage screen
{
	char t1[] = "V";
	char t2[] = "HOME";
   	char t3[] = "DC";
   

	FillFullLcd(VIOLET);
	FillBlack();
	LCD_bitmap (0,0,(unsigned char rom *)home,YELLOW);
	setfont((void *)font_Calibri_26V);
	LCD_OutText(95,25,t1,GREEN);
	setfont((void *)font_Calibri_10);
	LCD_OutText(60,6,t2,YELLOW);
	LCD_OutText(125,65,t3,GREEN);

}

void DisplayUpMsg(void)			//displays msg press up/down to change
{
	char t3[] = "PRESS UP/DOWN";
	char t4[] = "TO CHANGE";

	FillViolet();	//bottom VIOLET strip
	setfont((void *)font_Calibri_10);
	LCD_OutText(20,94,t3,YELLOW);				//"PRESS UP/DOWN"
	LCD_OutText(40,108,t4,YELLOW);				//"TO CHANGE"
}

void DisplayOkMsg(void)			// displays msg "press ok to confirm"
{
	char t3[] = "PRESS OK";
	char t4[] = "TO CONFIRM";

	FillViolet();
	setfont((void *)font_Calibri_10);
	LCD_OutText(40,94,t3,YELLOW);		//"PRESS OK"
	LCD_OutText(30,108,t4,YELLOW);		//"TO CONFIRM"CONFIRM"

}


void DisplayVtg(void)	//display scrolling voltage when up/down is perssed
{
	int x,y,color;
	
	x = 35;
	y = 45;

	LCD_draw_round_corner_box ( 35, 42, 89, 85,3, BLACK, 1);	//focus on voltage

	x = 40;
	y = 25;

	if(OkKeyEvent)
	{
		color = GREEN;
	}
	else
	{
		color = YELLOW;
	}

	setfont((void *)font_Calibri_26);
	LCD_OutText(x,y,voltage[VtgIndex],color);					//voltage[VtgIndex]
	
}

void main()
{
	int steps;
	unsigned char productstr[]="UPOWERLITE";
	unsigned char currentIndex;
	unsigned char firstTime = 0;
    char OC[] = "SHORT CIRCUIT";
	//-------- PIC initialization-------------------------------

	OSCCONbits.IRCF0 = 1;	//HFINTOSC 16MHz with PLLEN
	OSCCONbits.IRCF1 = 1;
	OSCCONbits.IRCF2 = 1;
	OSCTUNEbits.PLLEN = 1;
	delay_ms(5);
	while(!OSCCONbits.IRCF2);	//wait for HF-INTOSC oscillator frequency is stable
	delay_ms(5);
    Config_IO();

	lcd_reset();	//LCD reset
	//steps = MAX_STEPS + STEPS_FOR_VOLT;
steps = 3300;	
enable_DAC(steps);

	INTCON3 = 0x00;
	INTCON2 = 0x00;

	INTCON2bits.INTEDG0 = 0;		//RF interrupt
	INTCONbits.INT0IF = 0;
	INTCONbits.INT0IE = 0;
	RCONbits.IPEN = 0;
	INTCONbits.GIE = 0;
	INTCONbits.GIEL = 0;

	INTCONbits.TMR0IF = 0;		//timer 0 interrupt flag
	INTCON2bits.TMR0IP = 0;		// low priority interrupt
	INTCONbits.TMR0IF = 0; 		//Reset Timer0 interrupt flag and Return from ISR
	INTCONbits.TMR0IE = 1;		// enable timer0 interrupt
	T0CON = 0x07; 			//Stops the Timer0 and reset control reg.; prescaler 1:256; 16bit timer, Fosc/4
	TMR0H = 0x0B; 			//Clear contents of the timer register
	TMR0L = 0xDB;

	RCONbits.IPEN = 1;
	INTCONbits.GIE = 1;
	INTCONbits.GIEL = 1;

	LCD_init();
	font_init();

	FillFullLcd(WHITE);
	LCD_image (0,33,159,96,(unsigned char rom *)benchmark);
	delay_ms(4000);

	setfont((void *)font_Calibri_10);
	//FillFullLcd(WHITE);
//	LCD_image (0,33,159,96,(unsigned char rom *)benchmark);
	LCD_draw_round_corner_box(2,33,157,96,75,LOGOC,1);
	LCD_OutText(35, 55, productstr,WHITE);
	delay_ms(2000);

	//FillFullLcd(VIOLET);
	DisplayVoltage();

	delay_ms(1000);

	VtgIndex = 0;
	currentIndex = 0;
	OkKeyEvent = 0;
	DisplayVtg();
	DisplayOkMsg();
	while(1)
	{
		if(timeoutCount >=20)
		{
			timeoutCount = 0;
			VtgIndex = currentIndex;
			OkKeyEvent = 1;
			if(firstTime)
				DisplayUpMsg();
			else
			{
				OkKeyEvent = 0;
				DisplayOkMsg();

			}
			SetVoltageFocus();
			DisplayVtg();

			T0CONbits.TMR0ON = 0;
		}
		if(key_up == PRESSED)
		{
			keyDeBounce();
			if(key_up == PRESSED)
			{
				while(key_up == PRESSED);
				VtgIndex++;
				if(VtgIndex == 12)
					VtgIndex = 0;
				OkKeyEvent = 0;
		
				//DisplayVtg();
				SetVoltageFocus();
				DisplayOkMsg();
				timeoutCount = 0;
				T0CONbits.TMR0ON = 1;
			}
		}
		if(key_down == PRESSED)
		{
			keyDeBounce();
			if(key_down == PRESSED)
			{
				while(key_down == PRESSED);
				if(VtgIndex == 0)
					VtgIndex = 11;
				else
					VtgIndex--;
				OkKeyEvent = 0;
				
				//DisplayVtg();
				SetVoltageFocus();
				DisplayOkMsg();
				timeoutCount = 0;
				T0CONbits.TMR0ON = 1;
			}
		}
		if(key_ok == PRESSED)
		{
			keyDeBounce();
			if(key_ok == PRESSED)
			{
				while(key_ok == PRESSED);
			//	steps = MAX_STEPS - VtgIndex*STEPS_FOR_VOLT;
			
					switch(VtgIndex+1) //since variable denotes 0 to 11
					{
						case  1 : steps = 2476 ; break;  // 0.99  ok
						case  2 : steps = 2335 ; break;  // 1.97  ok
						case  3 : steps = 2190 ; break;  // 3.17  -
						case  4 : steps = 2035 ; break;  // 4.03  ok
						case  5 : steps = 1883 ; break;  // 5.09  -
						case  6 : steps = 1740 ; break;  // 5.97  ok 
						case  7 : steps = 1586 ; break;  // 7.06  -
						case  8 : steps = 1450 ; break;  // 7.94  +
						case  9 : steps = 1305 ; break;  // 9.18  -
						case 10 : steps = 1160 ; break;  //10.13  -
						case 11 : steps = 1008 ; break;  //10.99  ok
						case 12 : steps = 860  ; break;  //12.06  ok
					//	default : steps = 2475 ; break;
					}
				currentIndex = VtgIndex;
				OkKeyEvent = 1;
				DisplayVtg();
				DisplayUpMsg();
				enable_DAC(steps);
				T0CONbits.TMR0ON = 0;
				timeoutCount = 0;
				firstTime = 1;
			
			}
		}
				if(PORTCbits.RC1==0)
				{
					FillFullLcd(RED); //message display
					setfont((void *)font_Calibri_10);
					LCD_OutText(27, 55, OC ,WHITE);
					steps = 3300;	
                    enable_DAC(steps);	// Output minimum Voltage
					while(key_ok != PRESSED);
					DisplayVoltage();

					delay_ms(1000);

			//	VtgIndex = 0;
			//currentIndex = 0;
			OkKeyEvent = 0;
			DisplayVtg();
			DisplayOkMsg();
				}


	}
}
