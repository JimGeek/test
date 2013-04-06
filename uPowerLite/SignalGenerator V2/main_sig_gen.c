#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main_sig_gen.h"
#include "lcd.h"
#include "font.h"
#include "functions.h"
#include "bitmap.h"
#include "define.h"

extern long Frequency,Amplitude;
extern unsigned char FastSweepMode;

unsigned char SineText[] = "SINE";
unsigned char TriText[] = "TRI";
unsigned char SquText[] = "SQU";
int frqUnitindex = 0;

void update_frq_unit()
{
	unsigned char Hz[] = "HZ";
	unsigned char kHz[] = "KHZ";

	setfont((void *)font_Calibri_26V);
		
	if(frqUnitindex)
	{
		LCD_paint_block(FREQ_UNIT_START_X + 25 ,FREQ_UNIT_START_Y+6,FREQ_UNIT_START_X + 65,FREQ_UNIT_START_Y + 36,0);
		LCD_OutText(FREQ_UNIT_START_X+25,FREQ_UNIT_START_Y,(void *)kHz,YELLOW);
	}
	else
	{
		LCD_paint_block(FREQ_UNIT_START_X + 25 ,FREQ_UNIT_START_Y+6,FREQ_UNIT_START_X + 80,FREQ_UNIT_START_Y + 36,0);
		LCD_OutText(FREQ_UNIT_START_X+25,FREQ_UNIT_START_Y,(void *)Hz,YELLOW);
	}
}

/*void update_frq_SWAP()
{
	unsigned char SwapMode[] = "F";
	unsigned char SwapMode1[] = "S";
	setfont((void *)font_Calibri_10);
	LCD_paint_block(FREQ_WAVE_START_X,AMP_START_Y,FREQ_WAVE_START_X + 10,AMP_START_Y + 33,0);

	if(10 == FastSweepMode)
	{
		LCD_OutText(FREQ_WAVE_START_X,AMP_START_Y+5,(void *)SwapMode,YELLOW);
	}
	else
	{
		LCD_OutText(FREQ_WAVE_START_X,AMP_START_Y+5,(void *)SwapMode1,YELLOW);
	}
}*/


void update_frequency_value()
{
	unsigned char buffer[10];
	unsigned char buffer3[10];
	long tmpval,tmpfrq,frq,tmpsto,tmpsto1;
	
/*	unsigned char SwapMode[] = "F";
	unsigned char SwapMode1[] = "S";
	setfont((void *)font_Calibri_10);
	LCD_paint_block(FREQ_WAVE_START_X,AMP_START_Y,FREQ_WAVE_START_X + 10,AMP_START_Y + 33,0);

	if(10 == FastSweepMode)
	{
		LCD_OutText(FREQ_WAVE_START_X,AMP_START_Y+5,(void *)SwapMode,YELLOW);
	}
	else
	{
		LCD_OutText(FREQ_WAVE_START_X,AMP_START_Y+5,(void *)SwapMode1,YELLOW);
	}
*/	
	frq = (Frequency / 10);	
	
	setfont((void *)font_Calibri_26V);
	LCD_paint_block(FREQ_VALUE_START_X-14,FREQ_VALUE_START_Y+8,FREQ_UNIT_START_X + 27,FREQ_VALUE_START_Y + 38,0);
	if(frq >= 0 && frq < 100)
	{		
		if(frqUnitindex)
		{
			frqUnitindex = 0;
			update_frq_unit();
		}
		tmpval = frq;
		tmpfrq = Frequency;
		if(frq < 10 && FastSweepMode == 10)
		{
			sprintf( buffer,"0%ld",tmpval);
			sprintf( buffer3,".%ld", (tmpfrq % 10));
			LCD_draw_round_corner_box(FREQ_VALUE_START_X-10,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+23,FREQ_VALUE_START_Y+32,2,BLUE,1);
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLACK);
			LCD_OutText(FREQ_VALUE_START_X+25,FREQ_VALUE_START_Y,buffer3,BLUE);
		}	
		else if(frq >= 10 && FastSweepMode == 10)
		{
			sprintf( buffer,"%ld",tmpval);
			sprintf( buffer3,".%ld", (tmpfrq % 10));
			LCD_draw_round_corner_box(FREQ_VALUE_START_X-10,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+23,FREQ_VALUE_START_Y+32,2,BLUE,1);
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLACK);
			LCD_OutText(FREQ_VALUE_START_X+25,FREQ_VALUE_START_Y,buffer3,BLUE);
		}	
		else if(frq < 10 && FastSweepMode == 1)
		{
			sprintf( buffer,"0%ld.",tmpval);
			sprintf( buffer3,"%ld", (tmpfrq % 10));
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+30,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+46,FREQ_VALUE_START_Y+32,2,BLUE,1);
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+32,FREQ_VALUE_START_Y,buffer3,BLACK);
		}
		else if(frq >= 10 && FastSweepMode == 1)
		{
			sprintf( buffer,"%ld.",tmpval);
			sprintf( buffer3,"%ld", (tmpfrq % 10));
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+30,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+46,FREQ_VALUE_START_Y+32,2,BLUE,1);
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+32,FREQ_VALUE_START_Y,buffer3,BLACK);
		}
		
	}
	else if(frq >= 100 && frq < 1000)
	{
		//LCD_paint_block(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,FREQ_VALUE_START_X + 30,FREQ_VALUE_START_Y + 30,0);
		if(frqUnitindex)
		{
			frqUnitindex = 0;
			update_frq_unit();
		}
		tmpval = frq;
		if(frq%100 == 0)
		{
			sprintf( buffer,"%ld",tmpval);
			LCD_draw_round_corner_box(FREQ_VALUE_START_X-6,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+46,FREQ_VALUE_START_Y+34,2,BLUE,1);
			LCD_OutText(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,buffer,BLACK);
		}
		if(FastSweepMode == 1 && frq%100 != 0)
		{
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+27,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+44,FREQ_VALUE_START_Y + 32,2,BLUE,1);	
			sprintf( buffer,"%ld",tmpval/10);
			sprintf( buffer3,"%ld",tmpval%10);
			LCD_OutText(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+29,FREQ_VALUE_START_Y,buffer3,BLACK);
		}
		else if(FastSweepMode == 10 && frq%100 != 0)
		{	
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+10,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+42,FREQ_VALUE_START_Y + 32,2,BLUE,1);
			sprintf( buffer,"%ld",tmpval/100);
			sprintf( buffer3,"%ld",tmpval%100);	
			LCD_OutText(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+12,FREQ_VALUE_START_Y,buffer3,BLACK);
		}
	
		
	//	LCD_OutText(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,buffer,BLUE);	
	}
	else if(frq >= 1000 && frq < 10000)
	{
		//LCD_paint_block(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,FREQ_VALUE_START_X + 30,FREQ_VALUE_START_Y + 30,0);
		if(!frqUnitindex)
		{
			frqUnitindex = 1;
			update_frq_unit();
		}
		tmpval = frq;
		tmpfrq = (tmpval % 1000);
		tmpsto1=(tmpval % 100);
		tmpsto = tmpfrq/100;
		tmpfrq = tmpfrq / 10;
		tmpval = tmpval / 1000;
		
		if(tmpfrq < 10 && tmpfrq >= 0 && FastSweepMode == 1)
		{
			sprintf(buffer,"%ld.0",tmpval);		
			sprintf(buffer3,"%ld",tmpfrq);
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+32,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+46,FREQ_VALUE_START_Y + 32,2,BLUE,1);	
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+33,FREQ_VALUE_START_Y,buffer3,BLACK);
		}
		if(tmpfrq < 10 && tmpfrq >= 0 && FastSweepMode == 10)
		{
			sprintf(buffer,"%ld.",tmpval);		
			sprintf(buffer3,"0%ld",tmpfrq);
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+13,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+46,FREQ_VALUE_START_Y + 32,2,BLUE,1);	
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+15,FREQ_VALUE_START_Y,buffer3,BLACK);
		}
		else if(tmpfrq >= 10 && FastSweepMode == 1)
		{
		
			sprintf(buffer,"%ld.%ld",tmpval,tmpsto);
			sprintf(buffer3,"%ld",tmpsto1/10);
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+30,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+46,FREQ_VALUE_START_Y + 32,2,BLUE,1);	
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+33,FREQ_VALUE_START_Y,buffer3,BLACK);
		
		}
		else if(tmpfrq >= 10 && FastSweepMode == 10)
		{
		
			sprintf(buffer,"%ld.",tmpval);
			sprintf(buffer3,"%ld",tmpfrq);
			LCD_draw_round_corner_box(FREQ_VALUE_START_X+13,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+46,FREQ_VALUE_START_Y + 32,2,BLUE,1);
			LCD_OutText(FREQ_VALUE_START_X-8,FREQ_VALUE_START_Y,buffer,BLUE);
			LCD_OutText(FREQ_VALUE_START_X+15,FREQ_VALUE_START_Y,buffer3,BLACK);
		
		}
		
		
	}
	else if(frq >= 10000 && frq <= 100000)
	{
		if(!frqUnitindex)
		{
			frqUnitindex = 1;
			update_frq_unit();
		}
		//LCD_paint_block(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,FREQ_VALUE_START_X + 30,FREQ_VALUE_START_Y + 30,0);
		tmpval = frq;
		tmpfrq = (tmpval % 1000);
		tmpfrq = tmpfrq / 100;	
		tmpval = tmpval / 1000;
		if(frq != 100000)
		{
			if(FastSweepMode == 1)
			{
				sprintf(buffer,"%ld.",tmpval);
				sprintf(buffer3,"%ld",tmpfrq);	
				LCD_draw_round_corner_box(FREQ_VALUE_START_X+34,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+48,FREQ_VALUE_START_Y + 32,2,BLUE,1);
				LCD_OutText(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,buffer,BLUE);
				LCD_OutText(FREQ_VALUE_START_X+35,FREQ_VALUE_START_Y,buffer3,BLACK);	
			}
			else 
			{		
				sprintf(buffer,"%ld",tmpval);
				sprintf(buffer3,".%ld",tmpfrq);
				LCD_draw_round_corner_box(FREQ_VALUE_START_X-6,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+25,FREQ_VALUE_START_Y+34,2,BLUE,1);
				LCD_OutText(FREQ_VALUE_START_X-4,FREQ_VALUE_START_Y,buffer,BLACK);
				LCD_OutText(FREQ_VALUE_START_X+27,FREQ_VALUE_START_Y,buffer3,BLUE);
			}
		}
		else if(frq == 100000) 
		{
			sprintf(buffer,"%ld",tmpval);	
			LCD_draw_round_corner_box(FREQ_VALUE_START_X-6,FREQ_VALUE_START_Y+6,FREQ_VALUE_START_X+41,FREQ_VALUE_START_Y+34,2,BLUE,1);	
			LCD_OutText(FREQ_VALUE_START_X-6,FREQ_VALUE_START_Y,buffer,BLACK);
		}
		
	
	}
}
void update_amplitude_value()
{
	unsigned char buffer[10];
	//unsigned char buffer1[10];
	long tmpval = Amplitude/10,tmpamp = Amplitude;
	LCD_paint_block(FREQ_VALUE_START_X,AMP_START_Y+5,FREQ_UNIT_START_X + 25,AMP_START_Y + 32,BLACK);
	//LCD_paint_block(FREQ_VALUE_START_X+24,AMP_START_Y+7,FREQ_VALUE_START_X+40,AMP_START_Y+30,BLUE); //for drawing line without round edges.
	//LCD_draw_round_corner_box(FREQ_VALUE_START_X+22,AMP_START_Y+5,FREQ_VALUE_START_X+40,AMP_START_Y+30,2,BLUE,1);
	sprintf( buffer,"%ld.%ld",tmpval,(tmpamp%10));
	//sprintf( buffer1,"%ld",(tmpamp%10));
	setfont((void *)font_Calibri_26V);
	LCD_OutText(FREQ_VALUE_START_X,AMP_START_Y-2,buffer,BLUE);
	//LCD_OutText(FREQ_VALUE_START_X+25,AMP_START_Y-2,buffer1,BLACK);
	
}

void Display_Wave_Type(unsigned char waveType)
{
	LCD_paint_block(FREQ_WAVE_START_X+10,FREQ_WAVE_START_Y+6,FREQ_WAVE_START_X + 60,FREQ_WAVE_START_Y + 30,0);
	LCD_paint_block(FREQ_WAVE_START_X+10,AMP_START_Y,FREQ_WAVE_START_X + 60,AMP_START_Y + 33,0);
	//paint block for symbol
	setfont((void *)font_Calibri_10);
	switch(waveType)
	{
		case WAVE_TYPE_SINE:
		{
			LCD_OutText(FREQ_WAVE_START_X+12,FREQ_WAVE_START_Y+6,SineText,WHITE);
			LCD_bitmap (FREQ_WAVE_START_X+12,AMP_START_Y,(unsigned char rom *)sine,WHITE);
			
			Amplitude = 10;
			update_amplitude_value();
			DDS_SINE_WAVE = 1;
			delay_ms(1000);
			DDS_SINE_WAVE = 0;
		}
		break;
		case WAVE_TYPE_TRI:
		{
		 	LCD_OutText(FREQ_WAVE_START_X+12,FREQ_WAVE_START_Y+6,TriText,WHITE);
			LCD_bitmap (FREQ_WAVE_START_X+7,AMP_START_Y,(unsigned char rom *)triangle,WHITE);
		}
		break;
		case WAVE_TYPE_SQUARE:
		{
			LCD_OutText(FREQ_WAVE_START_X+12,FREQ_WAVE_START_Y+6,SquText,WHITE);
			LCD_bitmap (FREQ_WAVE_START_X+12,AMP_START_Y,(unsigned char rom *)square,WHITE);
			/*Amplitude = 10;
			update_amplitude_value();
			DDS_SQUARE_WAVE = 1;
			delay_ms(1000);
			DDS_SQUARE_WAVE = 0;*/
		}
		break;
	}
}
void display_home_page(void)
{
	int x_start,y_start;
	char buffer[6];
	unsigned char HomeText[] = "POWER SIGNAL";
    unsigned char HomeText1[] = "GENERATOR";
	unsigned char PressMenu[] = "FREQ : LEFT/RIGHT";
	unsigned char ToChange[] = "AMP : UP/DOWN";
	//unsigned char temp;

	unsigned char V[] = "V";
	setfont((void *)font_Calibri_10);
	x_start = 0;
	y_start = 0;

	/* Fill LCD background */
	

	/* Draw Title bar*/
	LCD_paint_block(x_start,y_start,x_start + SIGGEN_LONG_BUTTON_WIDTH + 5,y_start + SIGGEN_LONG_BUTTON_HEIGHT + 7,BLUE);
	LCD_bitmap (x_start,y_start,(unsigned char rom *)sine,YELLOW);
	LCD_OutText(x_start+40 ,-2,HomeText,YELLOW);
    LCD_OutText(x_start+50,12,HomeText1,YELLOW);

	update_frequency_value();
	update_frq_unit();
	//update_frq_SWAP();

	//temp = Read_b_eep (SIGNAL_TYPE_ADDRESS);
	//Display_Wave_Type(temp);
	update_amplitude_value();

	LCD_OutText(FREQ_UNIT_START_X +25,AMP_START_Y-2,(void *)V,GREEN);

	setfont((void *)font_Calibri_10);
	/* Draw Amplitude bar*/
	LCD_paint_block(x_start,128 - SIGGEN_LONG_BUTTON_HEIGHT,x_start + SIGGEN_LONG_BUTTON_WIDTH,128,BLUE);
	LCD_OutText(x_start + 10,90,(void *)PressMenu,YELLOW);
	LCD_OutText(x_start + 13,104,(void *)ToChange,YELLOW);
}


void dds_init(void)
{
	// Set the desired output values for the DDS pins
								// want reset pin high to reset the DDS
	DDS_DATA  = LOW;
	DDS_WCLK  = LOW;
	DDS_FQ    = LOW;

	DDS_RESET = HIGH;
	// OK, DDS is currently in reset & the Xmega DDS ports are configured as outputs.
	// Wait a moment to give the DDS a nice long reset pulse
	delay(10000);
	delay(10000);

	// Take DDS out of reset, put DDS into serial data communication mode
	DDS_RESET = LOW;

	delay(10000);
	// Enable serial mode. See Fig 17 in the AD9851 datasheet. We have pins D2-0
	// wired appropriately on the PCB, so we just need to toggle WCLK & FQ.

	DDS_WCLK = HIGH;
	//delay(100);
	DDS_WCLK = LOW;
	//delay(100);

	DDS_FQ = HIGH;
	//delay(100);
	DDS_FQ = LOW;
	//delay(100);
}

