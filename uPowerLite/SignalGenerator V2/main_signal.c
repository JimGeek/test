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
// Project      : tmstr-p-bm-usg
// Client       : Timstar
//
//--------------------------F I L E  D E T A I L S---------------------------------------
// Design Unit  : usg
// File         : main_usg.c
// Author       : shreyas patel - supatel@slscorp.com
// Created      : 09/10/2012
// Target Device: pic18f67k22
// Description  :
//					main_usg.c is a main file of usg which controls the unit through
//  				GUI on LCD display. It's main operation is to handle GUI through which
//  				It sets the amplitude and frequency of usg.
//  				It continuously checks for the switch press and go to FSM according to
//  				switch.
//
//
// Operation	: 1) Navigation in GUI through LCD and switch interface.
//
//                2) Set amplitude  and frequency by controlling DAC.
//
//                3)
//
//                5)
//
//                6)
//
//-----------------------S O U R C E  C O N T R O L-----------------------------
// $Id$
// $Author::                      $ Shreyas patel
// $Date::                        $ 09/10/2012
// $Rev::                         $ 00
//------------------------------------------------------------------------------

#include <stdio.h>
#include <p18F66K22.h>
#include"functions.h"
#include"lcd.h"
#include "bitmap.h"
#include <EEP.h>
#include "define.h"
#include<string.h>
#include "font.h"
#include <stdlib.h>
#include "p18cxxx.h"
#include "spi.h"

/***************************configuration bits***************************/
#pragma config SOSCSEL = DIG		//SOSC power selection mode = digital
#pragma config XINST = OFF			//exrended instruction set disabled
#pragma config FOSC = INTIO2, FCMEN = OFF,IESO = OFF,PLLCFG = ON	//Internal RC oscillator with PLL enabled
#pragma config PWRTEN = ON, WDTEN = OFF,BOREN = OFF	//power up timer on, watchdog timer off, BROWN out detect off
#pragma config MCLRE = ON	//MCLR on RG5 disabled
#pragma config CP0=OFF, CP1=OFF,CP2=OFF, CP3=OFF, CPB=OFF, CPD=OFF	// code protect disabled
#pragma config WRTC=OFF, WRTB=OFF,WRTD=OFF	//table write protect disabled
#pragma config EBRT0=OFF , EBRT1=OFF , EBRT2=OFF , EBRT3=OFF , EBRTB=OFF	//table read protect disabled
#pragma config STVREN = ON	// stack overflow reset enabled


#define MAX_DDS_FREQ				14300000


/*******define fsm states********/
enum
{
	STATE_HOME,
	STATE_SETTINGS,
	STATE_MODE,
	STATE_FREQUENCY,
	STATE_AMPLITUDE

};

/*define capsense keys*/
#define ESC		5
#define UP		10
#define DOWN	7
#define OK		9
#define MENU 	6
#define LEFT 	8
#define RIGHT	11


#define key_menu 	PORTFbits.RF1
#define key_down 	PORTFbits.RF2
#define key_left 	PORTFbits.RF3
#define key_ok 		PORTFbits.RF4
#define key_up 		PORTFbits.RF5
#define key_right 	PORTFbits.RF6
#define key_esc 	PORTFbits.RF7


#define MULTIPLIER_FOR_1HZ				14.3
void write_DDS_freq ();
void send_byte(unsigned long lv);
void update_max(int a);


//define with the macros for addresses instead of gloabal variables


#define Y_POSITION_FOCUS_START			42
#define Y_POSITION_FOCUS_END			85

#define LCD_X_START	1
#define LCD_Y_START	2

#define LCD_X_END	160
#define LCD_Y_END	129

#define ALTERNATE_XPOSITION 10
#define VOLTAGE_YPOSITION 	25

#define LCD_ROUND_CORNER_BOX_RADIUS		3

#define	AMPLITUDE_RESOLUTION		1
#define SPI_CS PORTCbits.RC2  // the chip select pin
#define DC_MULTIPLICATION_FACTOR_ADDRESS	0X80
#define AC_MULTIPLICATION_FACTOR_ADDRESS    0X84
#define DC_ADDITION_FACTOR_ADDRESS			0X88
#define AC_ADDITION_FACTOR_ADDRESS    		0X8A

/**********LOW PRIORITY ISR**************/
unsigned long SecondTimer = 0;	// thermal shutdown counter
unsigned char OvertimeCount = 0;	// 20 seconds timer

void Timer0Second(void)
{
	INTCONbits.TMR0IF = 0; 	//Reset Timer0 interrupt flag and Return from ISR
	INTCONbits.TMR0IE = 0;
	T0CON = 0x07; 			//Stops the Timer0 and reset control reg.; prescaler 1:256; 16bit timer, Fosc/4
	TMR0H = 0x0B; 			//Clear contents of the timer register
	TMR0L = 0xDB;			// 1 second timer
	SecondTimer++;		// 15 minute thermal shutdown counter
	OvertimeCount++;
	T0CONbits.TMR0ON = 1; 	//Start Timer0
	INTCONbits.TMR0IE = 1;
}

#pragma code low_vector = 0x0018

void
interrupt_at_low_vector (void)
{
	if(INTCONbits.TMR0IF == 1)
	{
		_asm GOTO Timer0Second _endasm
	}
}
#pragma code

#pragma interruptlow Timer0Second


void delay(unsigned int i)	//for small delays
{

	for(; i>1; i--);
}


void keyDeBounce(void)
{
	TMR1L = 0;
	T1CONbits.TMR1ON = 1;
	while(TMR1L < 0x64);     //50us delay
	TMR1L = 0;
}

/*************************************GLOBAL VARIABLES*************************************/
unsigned char state = STATE_HOME;
long SelectedAmplitude = 0, SelectedFrequency = 1;
//unsigned char Amplitude = 0, Frequency = 0;
long Amplitude = 10; 
long Frequency = 10;
int FrequencyResolution = 1;
unsigned char SelectedSignalType, FrequencyMultiplier, SelectedFrequencyMultiplier;
unsigned char MenuIndex = 0, FrequencyMenuIndex = 0,FrequencyUnitType = 0, FastSweepMode = 1;

unsigned char SignalType = 0;

unsigned char SerialNo[5] = "0001";
char ConvertedByte[4];
unsigned char Model = 0x03;
unsigned char Batch[7] = "1208I-", FW[3] = "10", HW[7] = "1A1A1A";

void delay_ms(unsigned int ms);			//function to generate 1ms delay
void delay_ms(unsigned int ms)
{
	unsigned int x;

	for(x = 0; x < 8*ms; x++)
		{
			TMR1L = 0x00;
			TMR1H = 0x00;
			T1CON = 0x31;			//turn on timer1 1:8 prescale
			while(TMR1L<0xFA);		//125us
			T1CON = 0x30;			// turn off timer
		}
}
void ConvertToByte (long value)
{
	unsigned char *ptr;

	ptr = (unsigned char *)&value;

	ConvertedByte[0] = 	*(ptr );
	ConvertedByte[1] = 	*(ptr +1);
	ConvertedByte[2] = 	*(ptr + 2);
	ConvertedByte[3] = 	*(ptr+3); 
}


void FillFullLcd(int color)	//paint whole lcd with color
{
	LCD_paint_block(LCD_X_START,LCD_Y_START,LCD_X_END,LCD_Y_END,color);
}

void FillViolet(void)	//bottom VIOLET strip
{
	LCD_paint_block(LCD_X_START,98,LCD_X_END,LCD_Y_END,VIOLET);
}

void FillBlack(void)	//middle black strip
{
	LCD_paint_block(LCD_X_START,34,LCD_X_END,97,BLACK);
}


void lcd_reset(void)
{
	PORTCbits.RC0=0;	//lcd reset
	delay_ms(5);
	PORTCbits.RC0=1;
	PORTCbits.RC0=1;
	PORTAbits.RA5=1;
}
void Config_IO(void)
{
	TRISA = 0x00;
	TRISB = 0X00;	//output
    TRISC = 0X00;	//output
    TRISD = 0X00;	//output
    TRISE = 0X00;	//output
	TRISF = 0Xff;	//input

//---------------A/D PORT CONFIGURETION--------------------
	ANCON0 = 0x00;
	ANCON1 = 0x00;
	ANCON2 = 0x00;
	ADCON0 = 0X00;
    ADCON1 = 0X00;
    ADCON2 = 0X00;

//---------------disable all interrupts---------------------
    INTCON = 0x00;
    PIR1 = 0x00;
    PIR2 = 0x00;
    PIR3 = 0x00;
    PIR4 = 0x00;
    PIR5 = 0x00;
    PIR6 = 0x00;
    PIE1 = 0x00;
    PIE2 = 0x00;
    PIE3 = 0x00;
    PIE4 = 0x00;
    PIE5 = 0x00;
    PIE6 = 0x00;
}
void InitializeTimer(void)
{
	INTCONbits.TMR0IF=0;	//timer 0 interrupt flag
	INTCON2bits.TMR0IP=0;	// low priority interrupt
	INTCONbits.TMR0IE=1;	// enable timer0 interrupt
	INTCONbits.TMR0IF=0; 		//Reset Timer0 interrupt flag and Return from ISR
	T0CON = 0x07; 			//Stops the Timer0 and reset control reg.; prescaler 1:256; 16bit timer, Fosc/4
	TMR0H = 0x0B; 			//Clear contents of the timer register
	TMR0L = 0xDB;
	T0CONbits.TMR0ON = 1;
}

void main(void)
{
	unsigned int tempcount=0,q;
	unsigned char lcd_id = 0;
	unsigned char FlashFlag, FirstTimeFlagAddr=0x000;
	long *tempfloat ;
	unsigned char LogoStr[]="uPower";
	unsigned char LogoStr1[]="Signal Generator";
	unsigned char logostr2[]="Benchmark";
	unsigned int temp_max=0;


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
	delay_ms(5);	//LCD reset complete time

	dds_init();			//TO INITIALIZE SIGNAL GENERATOR CHIP
	DDS_SQUARE_WAVE = 0;
	DDS_SINE_WAVE = 0;
	//RCONbits.IPEN = 1;
	//INTCONbits.GIE = 1;
	//INTCONbits.GIEL = 1;

	// Initialize EEPROM
	FlashFlag = Read_b_eep (FirstTimeFlagAddr);		//read FlashFlag for knowing first time or not
	if(FlashFlag != 0xaa)
	{
		Write_b_eep (FirstTimeFlagAddr,0xaa);			//set flag for not first time
		Busy_eep ();
		for(q=0;q<4;q++)
	    {
		   Write_b_eep (SERIAL_NO_ADDRESS+q,SerialNo[q]);	// write into to EEPROM
		   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
	    }
		ConvertToByte(Frequency);
		for(q=0;q<4;q++)
	    {
		   Write_b_eep (FREQUENCY_ADDRESS+q,ConvertedByte[q]);	// write into to EEPROM
		   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
	    }
		ConvertToByte(Amplitude);
		for(q=0;q<4;q++)
	    {
		   Write_b_eep (AMPLITUDE_ADDRESS+q,ConvertedByte[q]);	// write into to EEPROM
		   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
	    }
		Write_b_eep (SIGNAL_TYPE_ADDRESS,SignalType);			//set flag for not first time
		Busy_eep ();
		Write_b_eep (SWAP_VALUE_SLOW_FAST,FastSweepMode);			//set flag for not first time
		Busy_eep ();
		Write_b_eep (MODEL_ADDRESS, Model);   	// write into to EEPROM
		Busy_eep ();						// Checks & waits the status of ER bit in EECON1 register

	    for(q=0;q<6;q++)
	    {
		   Write_b_eep (BATCH_ADDRESS+q,Batch[q]);	// write into to EEPROM
		   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
	    }
	    for(q=0;q<2;q++)
	    {
		   Write_b_eep (FIRMWARE_ADDRESS+q,FW[q]);	// write into to EEPROM
		   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
	    }
	    for(q=0;q<6;q++)
	    {
		   Write_b_eep (HARDWARE_ADDRESS+q,HW[q]);	// write into to EEPROM
		   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
	    }

		//TBD: write initialize data to eeprom

	}
	//TBD: read EEPROM data and store to global variables
	for(q = 0; q < 4; q++)
    {
		ConvertedByte[q] = Read_b_eep (FREQUENCY_ADDRESS + q);	//read the EEPROM data written previously from corresponding address
	    Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
    }
	tempfloat = (long*)&ConvertedByte;
	Frequency = *tempfloat;

//	Amplitude = *tempfloat;
	for(q = 0; q < 4; q++)
    {
		ConvertedByte[q] = Read_b_eep (AMPLITUDE_ADDRESS + q);	//read the EEPROM data written previously from corresponding address
	    Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
    }

	tempfloat = (long*)&ConvertedByte;
	Amplitude = *tempfloat;


	SignalType = Read_b_eep (SIGNAL_TYPE_ADDRESS);				//read the EEPROM data written previously from corresponding address
	Busy_eep ();								// Checks & waits the status of ER bit in EECON1 register
	
	FastSweepMode = Read_b_eep(SWAP_VALUE_SLOW_FAST);
	Busy_eep ();

	Model = Read_b_eep (MODEL_ADDRESS);				//read the EEPROM data written previously from corresponding address
	Busy_eep ();								// Checks & waits the status of ER bit in EECON1 register

	for(q = 0; q < 6; q++)
    {
		Batch[q] = Read_b_eep (BATCH_ADDRESS + q);			//read the EEPROM data written previously from corresponding address
		Busy_eep ();								// Checks & waits the status of ER bit in EECON1 register
	}
	for(q = 0; q < 2; q++)
    {
		FW[q] = Read_b_eep (FIRMWARE_ADDRESS + q);				//read the EEPROM data written previously from corresponding address
		Busy_eep ();								// Checks & waits the status of ER bit in EECON1 register
	}
	for(q = 0; q < 6; q++)
    {
		HW[q] = Read_b_eep (HARDWARE_ADDRESS + q);				//read the EEPROM data written previously from corresponding address
		Busy_eep ();								// Checks & waits the status of ER bit in EECON1 register
	}

	LCD_init();

	font_init();
//	LCD_paint_block(LCD_X_START,LCD_Y_START,LCD_X_END,LCD_Y_END,color);
	FillFullLcd(WHITE);		//paint white whole screen
	LCD_image (0,33,159,96,(unsigned char rom *)benchmark);
	delay_ms(4000);
	//FillFullLcd(WHITE);

	LCD_draw_round_corner_box(2,33,157,96,75,LOGOC,1);
	setfont((void *)font_Calibri_10);
	
	//LCD_Rectangle()
	LCD_OutText(55, 37, LogoStr,WHITE);
	LCD_OutText(20, 55, LogoStr1, WHITE);
	delay_ms(2000);
	LCD_draw_round_corner_box(2,33,157,96,75,LOGOC,1);
	//	LCD_image (0,33,159,96,(unsigned char rom *)SignalGen);
	//while(1);
	FillFullLcd(0);
	DisplayHome();
	Display_Wave_Type(SignalType);
	//DisplayHome();
	FastSweepMode=1;
	//update_frq_SWAP();
	update_frequency_value();
	InitializeTimer();
	write_DDS_freq ();
    while(1)
    {
		if(key_up == 0)
		{
			keyDeBounce();
			if(key_up == 0)
			{
			  while(!key_up)		// if key is pressed after 125us then assume that its pressed second time
			  {
				if(TMR1L==0xFA)     //125us
				{
					tempcount++;
				}
				if(tempcount==800)
				{
					T1CONbits.TMR1ON = 0;
					break;
				}
			  }
			  tempcount=0;
			  Fsm(UP);				// go to FSM
			  OvertimeCount = 0;
			}
		}

		else if(key_down == 0)
		{
			keyDeBounce();
			if(key_down == 0)
			{
			  while(!key_down)
			  {
				if(TMR1L==0xFA)     //125us
				{
					tempcount++;
				}
				if(tempcount==800)
				{
					T1CONbits.TMR1ON = 0;
					break;
				}
			  }
			  tempcount=0;
			  Fsm(DOWN);
			  OvertimeCount = 0;
			}
		}


		else if(key_left==0)
		{
			keyDeBounce();			
			if(key_left == 0)
			{
			  while(!key_left)
			  {
				if(TMR1L==0xFA)     //125us
				{
					tempcount++;
				}
				if(tempcount==800)
				{
					T1CONbits.TMR1ON = 0;
					break;
				}
			  }
			  tempcount=0;			 
			  Fsm(LEFT);
			  OvertimeCount = 0;
			}
		}
		else if(key_right==0)
		{
			keyDeBounce();			
			if(key_right == 0)
			{
			  while(!key_right)
			  {
				if(TMR1L==0xFA)     //125us
				{
					tempcount++;
				}
				if(tempcount==800)
				{
					T1CONbits.TMR1ON = 0;
					break;
				}
			  }
			  tempcount=0;			  
			  Fsm(RIGHT);
			  OvertimeCount = 0;
			}
		}
		else if(key_menu==0)
		{
			keyDeBounce();
			if(key_menu == 0)
			{
			  while(!key_menu);			  
			  Fsm(MENU);
			  OvertimeCount = 0;
			}
		}
		else if(key_ok==0)
		{
			keyDeBounce();
			if(key_ok == 0)
			{
			  while(!key_ok)
			  Fsm(OK);
			  OvertimeCount = 0;
			}
		}
		else if(key_esc==0)
		{
			keyDeBounce();
			if(key_esc == 0)
			{
			  while(!key_esc);			  
			  Fsm(ESC);
			  OvertimeCount = 0;
			}
		}
	}
}

void Fsm(unsigned char key)
{
	unsigned char q;
		if (key == UP)	// increse voltage display
		{
		//	if(Amplitude >= 80)
		//	{
		//		Amplitude = 0;
		//	}
		//	else
		//	{
				Amplitude += AMPLITUDE_RESOLUTION;
		//	}
			
		//	update_amplitude_value();  Required
			update_max(1);
			update_amplitude_value();  //Extra

		}
		else if (key == DOWN)	// decrease voltage display
		{
		//	if(Amplitude <= 0)
		//	{
		//		Amplitude = 80;
		//	}
		//	else
		//	{
				Amplitude -= AMPLITUDE_RESOLUTION;
		//	}
		//	update_amplitude_value(); Required 
			update_max(0);
			update_amplitude_value(); //Extra
		}
		else if (key == RIGHT)
		{
			if(Frequency < 1000)
			{
				FrequencyResolution = 1;
				Frequency += (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency < 10000)
			{
				FrequencyResolution = 10;				
				Frequency += (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency < 100000)
			{
				FrequencyResolution = 100;
				Frequency += (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency < 1000000)
			{
				FrequencyResolution = 1000;
				Frequency += (FrequencyResolution * FastSweepMode);
			}
			else if (Frequency >= 1000000)
			{
				Frequency  = 0;
				FrequencyResolution = 1;
			}

			update_frequency_value();
			//write_DDS_freq();
		}
		else if (key == LEFT)
		{
			if(Frequency > 100000)		//10k to 100k
			{
				FrequencyResolution = 1000;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency > 10000)	//1k to 10k
			{
				FrequencyResolution = 100;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency > 1000)	//100hz to 1k
			{
				FrequencyResolution = 10;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency > 0)		//1h to 100hz
			{
				FrequencyResolution = 1;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency <= 0)
			{
				Frequency = 1000000;
				FrequencyResolution = 1000;
			}
			/*if(Frequency == 0)
			{
				FrequencyResolution = 1000;
				Frequency = 1000000;
			}
			else if(Frequency < 1000)
			{
				FrequencyResolution = 1;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency < 10000)
			{
				FrequencyResolution = 10;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency <= 100000)
			{
				FrequencyResolution = 100;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}
			else if(Frequency <= 1000000)
			{
				FrequencyResolution = 1000;
				Frequency -= (FrequencyResolution * FastSweepMode);
			}*/
			
			update_frequency_value();
			//write_DDS_freq();
		}
		else if (key == ESC)
		{
			if (SignalType == 0)
			{
				SignalType = 2;
			}
			else
			{
				SignalType--;
			}
			Display_Wave_Type(SignalType);
			write_DDS_freq ();
			
		}
		else if(key == MENU)
		{
			if(FastSweepMode == 1)
			{
				FastSweepMode = 10;
			}
			else
			{
				FastSweepMode = 1;
			}
			update_frequency_value();
		}
		else if(key == OK)
		{
          ConvertToByte(Frequency);
			for(q=0;q<4;q++)
			{
			   Write_b_eep (FREQUENCY_ADDRESS+q,ConvertedByte[q]);	// write into to EEPROM
			   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
			}
			ConvertToByte(Amplitude);
			for(q=0;q<4;q++)
			{
			   Write_b_eep (AMPLITUDE_ADDRESS+q,ConvertedByte[q]);	// write into to EEPROM
			   Busy_eep ();									// Checks & waits the status of ER bit in EECON1 register
			}
			Write_b_eep (SIGNAL_TYPE_ADDRESS,SignalType);			//set flag for not first time
			Write_b_eep (SWAP_VALUE_SLOW_FAST,FastSweepMode);
			Busy_eep ();	
		
		}
}



void write_DDS_freq ()
{
	unsigned long rfrq;
	unsigned long loadvalue2,loadvalue3;
	unsigned long loadvalue1;
	
	OpenSPI1(SPI_FOSC_16, MODE_10, SMPEND); 
	
	TRISCbits.TRISC4 = 1;       // define SDI pin as input
	TRISCbits.TRISC5 = 0;       // define SDO pin as output
	TRISCbits.TRISC3 = 0;       // define CLK pin as output

	SPI_CS = 0;
	WriteSPI1(0x21);
	WriteSPI1(0x00);
	
	SPI_CS = 1;

	delay(1000); 

	 
	loadvalue1 = (Frequency/0.093132257461); //integer value for loading in to the frequency register.
	loadvalue1 = 	loadvalue1/10;   //Real frequency value
	loadvalue2 = 0x3FFF & loadvalue1;
	loadvalue3 = 0x4000 | loadvalue2;
	
    send_byte(loadvalue3);

	loadvalue2 = loadvalue1 >> 14;
	loadvalue3 = 0x4000 | loadvalue2;

	send_byte(loadvalue3);

	delay(1000);                          //15uS delay

    SPI_CS = 0;
	WriteSPI1(0x20);
	if(SignalType == 0)
	{
	WriteSPI1(0x00);

	}
	else if(SignalType == 1)
	{ 
		WriteSPI1(0x28);
	}
	else if(SignalType == 2)
	{
		WriteSPI1(0x02);
	}         

		SPI_CS = 1;	
}


void send_byte(unsigned long lv)
{
	unsigned char part;
	part = lv >> 8;
	SPI_CS = 0; 
	WriteSPI1(part);
	WriteSPI1(lv);
	SPI_CS = 1;
}


void DisplayHome(void)
{
	display_home_page();
}

void update_max(int a)
{
    unsigned char f;
	OpenSPI1(SPI_FOSC_16, MODE_10, SMPEND); 
	
	TRISCbits.TRISC4 = 1;       // define SDI pin as input
	TRISCbits.TRISC5 = 0;       // define SDO pin as output
	TRISCbits.TRISC3 = 0;       // define CLK pin as output

	
	if(a == 1)
	{
		Amplitude = Amplitude + 5;
	}	
	else if(a == 0)
	{
		Amplitude = Amplitude - 5;
	}
		f = Amplitude & 0x03;
		f = f << 6;
		SPI_CS = 0;
		WriteSPI1(f);
		f = Amplitude >> 2;			 
 		WriteSPI1(f);
		WriteSPI1(0x00);
		SPI_CS = 1;
}