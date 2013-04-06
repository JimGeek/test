
#include <stdio.h>
#include <p18F66K22.h>
#include "lcd.h"
#include <string.h>
#include <stdlib.h>
#include "p18cxxx.h"
#include "font.h"
#include "bitmap.h"
#include "functions.h"
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


#define LCD_X_START	1
#define LCD_Y_START	2

#define LCD_X_END	160
#define LCD_Y_END	129

unsigned long SecondTimer = 0;	// thermal shutdown counter
unsigned char OvertimeCount = 0;	// 20 seconds timer
char ConvertedByte[4];

void delay_ms(unsigned int ms);			//function to generate 1ms delay
void adcinit();
int adcread();
void adcrun();


void ConvertToByte (long value)
{
	unsigned char *ptr;

	ptr = (unsigned char *)&value;

	ConvertedByte[0] = 	*(ptr );
	ConvertedByte[1] = 	*(ptr +1);
	ConvertedByte[2] = 	*(ptr + 2);
	ConvertedByte[3] = 	*(ptr+3); 
}


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

void Timer1Init()
{
	//timer1 TBD
}
	

void delay(unsigned int i)	//for small delays
{

	for(; i>1; i--);
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

void adcinit()
{
	TRISAbits.TRISA1 = 1; 
	ADCON0 = 0x04;
	ADCON1 = 0x00;
	ADCON2 = 0xB4;
	ANCON0 = 0x02;
	ANCON1 = 0x00;
	ANCON2 = 0x00;
}

int adcread()
{
	return ADRESL;
}

void adcrun()
{
	ADCON0bits.GO = 1;
}

void main()
{
	unsigned int tempcount=0,q;
	unsigned char lcd_id = 0;
	unsigned char FlashFlag, FirstTimeFlagAddr=0x000;
	long *tempfloat ;
	unsigned char LogoStr[]="uPower";
	unsigned char LogoStr1[]="Signal Generator";
	unsigned char logostr2[]="Benchmark";
	unsigned int temp_max=0;
	unsigned int temp_max1=0;
	unsigned char buffer[4000];
	unsigned long max=0;


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



	// Initialize EEPROM

	LCD_init();
	delay_ms(5);
	font_init();
	delay_ms(5);
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
	FillFullLcd(BLUE);

	adcinit();
	for(i=0;i<4000;i++)
	{
	ADCON0bits.ADON = 1;
	adcrun();
	while(ADCON0bits.GO);

	//sprintf(buffer,"%d",temp_max);
	//LCD_OutText(30,30,buffer,YELLOW);
	//delay_ms(2000);
	temp_max = ADRES & 0xFFF;
	
	max = max + temp_max;
	i++;
	}
	FillFullLcd(BLUE);
	temp_max = (max/4000); 
	sprintf(buffer[i],"%d",temp_max);
	setfont((void *)font_Calibri_26);
	LCD_OutText(30,30,buffer,YELLOW);
}
