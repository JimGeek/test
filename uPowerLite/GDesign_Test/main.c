
#include <stdio.h>
#include <p18F66K22.h>
#include "lcd.h"
#include <string.h>
#include <stdlib.h>
#include "p18cxxx.h"
#include "font.h"
#include "bitmap.h"
#include "functions.h"
#include "range.h"
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

#define ESC		5
#define UP		10
#define DOWN	7
#define OK		9
#define MENU 	6
#define LEFT 	8
#define RIGHT	11

#define SIGGEN_WINDOW_SPACE			0		//for displaying home page
#define SIGGEN_VSPACE_BW_BUTTON		9
#define SIGGEN_HSPACE_BW_BUTTON		10
#define SIGGEN_LONG_BUTTON_WIDTH 	160
#define SIGGEN_LONG_BUTTON_HEIGHT	30
#define SIGGEN_MID_BUTTON_WIDTH 	70
#define SIGGEN_MID_BUTTON_HEIGHT	30
#define SIGGEN_SMALL_BUTTON_WIDTH 	43
#define SIGGEN_SMALL_BUTTON_HEIGHT	30

#define LCD_START_X					0
#define LCD_END_X					160
#define LCD_START_Y					0
#define LCD_END_Y					128

#define WAVE_TYPE_SINE				0
#define WAVE_TYPE_TRI				1
#define WAVE_TYPE_SQUARE			2

#define FREQ_VALUE_START_X	15
#define FREQ_VALUE_START_Y	32

#define FREQ_UNIT_START_X	40
#define FREQ_UNIT_START_Y	32

#define FREQ_WAVE_START_X	110
#define FREQ_WAVE_START_Y	32

#define AMP_START_Y			64				// for displaying home page

#define key_menu 	PORTFbits.RF1
#define key_down 	PORTFbits.RF2
#define key_left 	PORTFbits.RF3
#define key_ok 		PORTFbits.RF4
#define key_up 		PORTFbits.RF5
#define key_right 	PORTFbits.RF6
#define key_esc 	PORTFbits.RF7

#define LCD_X_START	1
#define LCD_Y_START	2

#define LCD_X_END	160
#define LCD_Y_END	129

unsigned long SecondTimer = 0;	// thermal shutdown counter
unsigned char OvertimeCount = 0;	// 20 seconds timer

int q=0;
int page=1;
unsigned int temp_max=0;
unsigned char buffer[10];
unsigned char buffer1[10];
unsigned long max=0;
int mode;
int x_start = 0;
int y_start = 0;
unsigned int i= 0;
int gain=0;
int gain1=0;
int timeout = 0;
int setting;
unsigned int offset;
unsigned int factor;
unsigned long Analog=0;
void delay_ms(unsigned int ms);			//function to generate 1ms delay
void display_page_1(void);
void display_page_2(void);
void display_page_3(void);
void SelectionPage(void);
void Iout(void);
void adcinit();
int adcread();
void adcrun();
void currentsetting();
void voltagesetting();

enum 
{
	DC,
	AC
};

void Timer0Second(void)
{
	INTCONbits.TMR0IF = 0; 	//Reset Timer0 interrupt flag and Return from ISR
	INTCONbits.TMR0IE = 0;
	T0CON = 0x07; 			//Stops the Timer0 and reset control reg.; prescaler 1:256; 16bit timer, Fosc/4
	TMR0H = 0x0B; 			//Clear contents of the timer register
	TMR0L = 0xDB;			// 1 second timer
	SecondTimer++;		// 15 minute thermal shutdown counter
	OvertimeCount++;
	timeout = 1;
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

void keyDeBounce(void)
{
	TMR1L = 0;
	T1CONbits.TMR1ON = 1;
	while(TMR1L < 0x64);     //50us delay
	TMR1L = 0;
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
    INTCONbits.TMR0IE = 1;
	INTCONbits.GIE=1;
	INTCONbits.GIEL=1;
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
	TRISAbits.TRISA2 = 1;
	
	ADCON1 = 0x20;
	ADCON2 = 0xB4;
	
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
	unsigned int tempcount=0;
	unsigned char lcd_id = 0;
	unsigned char FlashFlag, FirstTimeFlagAddr=0x000;
	long *tempfloat ;
	unsigned char LogoStr[]="uPower";
	unsigned char LogoStr1[]="Signal Generator";
	unsigned char logostr2[]="Benchmark";

	




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
	voltagesetting();
	currentsetting();


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
	//FillFullLcd(BLUE);
	FillFullLcd(WHITE);
	SelectionPage();
	display_page_1();
	adcinit();
	//mode = DC;
	mux_en();
	gain = 0;               //voltage
	gain1 = 0;				//current 
     
	while(1)
	{ 
			if(timeout == 1)
			{
				timeout = 0;
				ADCON0 = 0x04;
				ANCON0 = 0x02;
			
                for(i=0; i<4000; i++) 
				{
				ADCON0bits.ADON = 1;
				adcrun();
				while(ADCON0bits.GO);
				temp_max = ADRES & 0xFFF;
				Analog = Analog + temp_max;
				}
                Analog=2048000-Analog;
                //printB(32,Analog*factor);
                // Auto Range:
				sprintf(buffer,"%d",Analog);
				
                if(Analog<200000 && Analog>-200000) {   // Gain too low
                    if(gain1>0) {
                        gain1--;
                       
                    }
                }
                if(Analog>2000000 || Analog<-2000000) {    // Gain too high
                    if(gain1<3) {
                        gain1++;
                       
                    }
                }
				currentsetting();
            //	break;
            //case CURRENT:
                Analog=0;									// Voltage measurement 
				ADCON0 = 0x08;
				ANCON0 = 0x04;
                for(i=0; i<4000; i++) 
				{
				ADCON0bits.ADON = 1;
				adcrun();
				while(ADCON0bits.GO);
				temp_max = ADRES & 0xFFF;
				Analog = Analog + temp_max;
       			}				        
				Analog+=offset;
                Analog=2048000-Analog;
				sprintf(buffer1,"%d",Analog);
				setfont((void *)font_Calibri_10);
				
                // Auto Range:
              if(Analog<200000 && Analog>-200000) {   // Gain too low
                    if(gain>0) {
                        gain--;
                       
                    }
                }
                if(Analog>2000000 || Analog<-2000000) {    // Gain too high
                    if(gain<3) {
                        gain++;
                       
                    }
                }
				voltagesetting();

				setfont((void *)font_Calibri_10);
			//	LCD_OutText(40,40,buffer,YELLOW);
			//	LCD_OutText(80,40,buffer1,YELLOW);
			//	delay_ms(1000);
			//	FillFullLcd(BLACK);	
			}

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
void display_page_1(void)
{
	//char buffer[6];
	unsigned char V[] = "V";
		
    	//unsigned char HomeText1[] = "METER";
		
	unsigned char PressMenu[] = "PAGE 1";
	unsigned char ToChange[] = "AC/DC";
	unsigned char HomeText6[] = "V & I METER";
	unsigned char DC[] = "DC";	
	unsigned char AC[] = "AC";	
	setfont((void *)font_Calibri_10);


	/* Fill LCD background */
	

	/* Draw Title bar*/
	LCD_paint_block(x_start,y_start,x_start + SIGGEN_LONG_BUTTON_WIDTH + 5,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,VIOLET);
	LCD_bitmap (x_start,y_start,(unsigned char rom *)sine,YELLOW);
	//LCD_OutText(x_start+40 ,-2,HomeText,YELLOW)
    LCD_OutText(x_start+48,2,HomeText6,YELLOW);
	if(setting == 0)
	LCD_OutText(x_start + 70,16,DC,YELLOW);
	else if(setting == 1)
	LCD_OutText(x_start + 70,16,AC,YELLOW);
	//LCD_OutText(FREQ_UNIT_START_X +25,AMP_START_Y-2,(void *)V,GREEN);
	/* Draw Amplitude bar*/
	LCD_paint_block(x_start,128 - SIGGEN_LONG_BUTTON_HEIGHT,x_start + SIGGEN_LONG_BUTTON_WIDTH,129,VIOLET);
	LCD_paint_block(x_start,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,160,128 - SIGGEN_LONG_BUTTON_HEIGHT,BLACK);
	LCD_OutText(x_start + 53,94,(void *)PressMenu,YELLOW);
	LCD_OutText(x_start + 107,108,(void *)ToChange,YELLOW);
}

void display_page_2(void)
{
	//char buffer[6];
	unsigned char V[] = "V";
		
	unsigned char PressMenu[] = "PAGE 2";
	unsigned char ToChange[] = "AC/DC";
	unsigned char HomeText2[] = "POWER METER";
	unsigned char DC[] = "DC";
	
	setfont((void *)font_Calibri_10);
	/* Fill LCD background */
	/* Draw Title bar*/
	LCD_paint_block(x_start,y_start,x_start + SIGGEN_LONG_BUTTON_WIDTH + 5,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,VIOLET);
	LCD_bitmap (x_start,y_start,(unsigned char rom *)sine,YELLOW);
	//LCD_OutText(x_start+40 ,-2,HomeText,YELLOW);
    LCD_OutText(x_start+48,2,HomeText2,YELLOW);
	LCD_OutText(x_start + 70,16,DC,YELLOW);
	//LCD_OutText(FREQ_UNIT_START_X +25,AMP_START_Y-2,(void *)V,GREEN);
	LCD_paint_block(x_start,128 - SIGGEN_LONG_BUTTON_HEIGHT,x_start + SIGGEN_LONG_BUTTON_WIDTH,129,VIOLET);
	LCD_paint_block(x_start,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,160,128 - SIGGEN_LONG_BUTTON_HEIGHT,BLACK);
	LCD_OutText(x_start + 53,94,(void *)PressMenu,YELLOW);
	LCD_OutText(x_start + 107,108,(void *)ToChange,YELLOW);
}

void display_page_3(void)
{
	//char buffer[6];
	unsigned char V[] = "V";
		
	unsigned char PressMenu[] = "PAGE 3";
	unsigned char ToChange[] = "AC/DC";
	unsigned char HomeText2[] = "JOULE METER";
	unsigned char DC[] = "DC";
	
	setfont((void *)font_Calibri_10);
	/* Fill LCD background */
	/* Draw Title bar*/
	LCD_paint_block(x_start,y_start,x_start + SIGGEN_LONG_BUTTON_WIDTH + 5,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,VIOLET);
	LCD_bitmap (x_start,y_start,(unsigned char rom *)sine,YELLOW);
	//LCD_OutText(x_start+40 ,-2,HomeText,YELLOW);
    LCD_OutText(x_start+48,2,HomeText2,YELLOW);
	LCD_OutText(x_start + 70,16,DC,YELLOW);
	//LCD_OutText(FREQ_UNIT_START_X +25,AMP_START_Y-2,(void *)V,GREEN);
	LCD_paint_block(x_start,128 - SIGGEN_LONG_BUTTON_HEIGHT,x_start + SIGGEN_LONG_BUTTON_WIDTH,129,VIOLET);
	LCD_paint_block(x_start,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,160,128 - SIGGEN_LONG_BUTTON_HEIGHT,BLACK);
	LCD_OutText(x_start + 53,94,(void *)PressMenu,YELLOW);
	LCD_OutText(x_start + 107,108,(void *)ToChange,YELLOW);
}

void SelectionPage(void)
{
	//char buffer[6];
	unsigned char ToChange[] = "DC";
	unsigned char HomeText6[] = "CONFIRM INPUT TYPE";
	unsigned char AC[] = "AC";	
	unsigned char PressMenu[] = "ESC";
	int tempcount=0;

	setfont((void *)font_Calibri_10);

	/* Draw Title bar*/
	LCD_paint_block(x_start,y_start,x_start + SIGGEN_LONG_BUTTON_WIDTH + 5,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,VIOLET);
	//LCD_bitmap (x_start,y_start,(unsigned char rom *)sine,YELLOW);
	//LCD_OutText(x_start+40 ,-2,HomeText,YELLOW)
    LCD_OutText(x_start+5,2,(void *)HomeText6,YELLOW);
	LCD_OutText(x_start + 85,16,DC,YELLOW);
//	LCD_OutText(FREQ_UNIT_START_X +25,AMP_START_Y-2,(void *)V,GREEN);
	/* Draw Amplitude bar*/
	LCD_paint_block(x_start,128 - SIGGEN_LONG_BUTTON_HEIGHT,x_start + SIGGEN_LONG_BUTTON_WIDTH,129,VIOLET);
	LCD_paint_block(x_start,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,160,128 - SIGGEN_LONG_BUTTON_HEIGHT,BLACK);
	LCD_OutText(x_start + 53,94,(void *)PressMenu,YELLOW);
	LCD_OutText(x_start + 120,108,(void *)ToChange,YELLOW);
	LCD_OutText(x_start + 10,108,(void *)AC,YELLOW);
	setfont((void *)font_Calibri_10);
	LCD_OutText(15,40,AC,YELLOW);
	LCD_OutText(80,40,ToChange,YELLOW);
	while(key_ok != 0)
	{ 
		if(key_left==0)
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
			  setting = 1;
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
			  setting = 0;
			  OvertimeCount = 0;
			}
		}
	}
}

void Fsm(unsigned char key)
{
	
		if (key == UP)	// increse voltage display
		{
			page++;
			if(page > 3)
			{
				page = 1;
			}
		
			if(page == 1)
				display_page_1();
			else if(page == 2)
				display_page_2();
			else if(page == 3)
				display_page_3();
			Iout();
		}
		else if (key == DOWN)	// decrease voltage display
		{
			page--;
			if(page < 1)
			{
				page = 3;
			}
			if(page == 1)
				display_page_1();
			else if(page == 2)
				display_page_2();
			else if(page == 3)
				display_page_3();
			Iout();
		}
		else if (key == RIGHT)
		{
			if(q == 1)
			{
				q=0;
				mode = DC;
			}
		}
		else if (key == LEFT)
		{
			if(q == 1)
			{
				q=0;
				mode = AC;
			}
		}
		else if (key == ESC)
		{
		
		}
		else if(key == MENU)
		{
			
		}
		else if(key == OK)
		{
          
		
		}
}

void Iout(void)
{

	//while(1)
	//{
	LCD_paint_block(x_start,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,160,128 - SIGGEN_LONG_BUTTON_HEIGHT,BLACK);
	for(i=0;i<4000;i++)							// average current measurement routine for DC
	{
	ADCON0bits.ADON = 1;
	adcrun();
	while(ADCON0bits.GO);
	temp_max = ADRES & 0xFFF;
	max = max + temp_max;
	}
	
	
	temp_max = (max/4000); 
	//sprintf(buffer,"%d",temp_max);
	setfont((void *)font_Calibri_12n);
	LCD_OutText(40,40,buffer,YELLOW);
	LCD_OutText(80,40,buffer1,YELLOW);
	max = 0;

	//LCD_paint_block(30,38,90,70,BLACK);
	//LCD_paint_block(x_start,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,160,128 - SIGGEN_LONG_BUTTON_HEIGHT,BLACK);
//	}
}
void voltagesetting()
{
 		switch(gain) {
             case 0: // +/- 204.8V
                   	V_relay(1);
                    mux_sel(0,1);
                    offset=8017;
                    factor = -10;
                    break;
             case 1: // +/- 20.48V,
                    V_relay(1);
                    mux_sel(0,0);
                    offset=10173;
                    factor = 1;
                    break;
             case 2: // +/- 2.56V
                    V_relay(0);
                    mux_sel(0,1);
                    offset=8026;
                    factor =-0.125;
                    break;
             case 3: // +/- 256mV
                    V_relay(0);
                    mux_sel(0,0);
                    offset=5744;
                    factor = 0.0125;
                    break;
			}
}	
void currentsetting() 
{                   
		switch(gain1) {
             case 0: // +/- 204.8V
                   	I_relay(1,1);
                    mux_sel(1,0);
                    offset=8017;
                    factor = -10;
                    break;
             case 1: // +/- 20.48V,
                    I_relay(1,1);
                    mux_sel(0,0);
                    offset=10173;
                    factor = 1;
                    break;
             case 2: // +/- 2.56V
                    I_relay(1,1);
                    mux_sel(0,1);
                    offset=8026;
                    factor =-0.125;
                    break;
             case 3: // +/- 256mV
                    I_relay(1,0);
                    mux_sel(0,0);
                    offset=5744;
                    factor = 0.0125;
                    break;
			 case 4: // +/- 256mV
                    I_relay(1,0);
                    mux_sel(0,1);
                    offset=5744;
                    factor = 0.0125;
                    break;
                    }
}