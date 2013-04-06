#include <i2c.h>
#include <EEP.h>
#include "functions.h"

#define NORMAL_MODE      0
#define DAC_DATA_START_ADDR       0x030


void I2CBusy(void)
{
	int timeout = 0;
	while(!PIR1bits.SSP1IF)		// Wait for 9th clock cycle
	{
		delay_ms(1);
		timeout++;
		if(timeout > 100)
			break;
	}
	PIR1bits.SSP1IF=0;
}

void enable_DAC(unsigned int voltage)
{
	unsigned char msb_input = 0,lsb_input = 0;
    int timeout = 0;

	TRISCbits.TRISC3 = 1;
	TRISCbits.TRISC4 = 1;

    //if(mode == NORMAL_MODE)
    //{
    //  msb_input = Read_b_eep (DAC_DATA_START_ADDR + ((voltage*2)-2));	//read DAC input MSB, 0x30 is the start address of DAC iput DATA in EEPROM
    //  lsb_input = Read_b_eep (DAC_DATA_START_ADDR + ((voltage*2)-1));	//read DAC input LSB, 0x30 is the start address of DAC iput DATA in EEPROM
    //  voltage = ((msb_input << 8)  | lsb_input);
    //}

	voltage   = (voltage << 4);
    msb_input = (voltage & 0xFF00) >> 8;
    lsb_input = (voltage & 0x00F0);


//	sync_mode = MASTER;
//	slew = SLEW_OFF;

	//SSP1CON1 = 0x38;
	SSP1CON1 = 0x3B;	//try i2c firmware controlled master mode.

	SSP1ADD = 0x7f;  // 64Mhz Fosc, 100kHz sck 9f--4e
	OpenI2C1(MASTER,SLEW_OFF);
	IdleI2C1();



	StartI2C1();

	timeout = 0;
	while(SSP1CON2bits.SEN)	//Wait till Start sequence is completed
	{
		delay_ms(1);
		timeout++;
		if(timeout > 100)
			break;
	}
	PIR1bits.SSP1IF=0;			//clear interrupt flag

	SSP1BUF = 0xc0;
	I2CBusy();

	SSP1BUF = 0x60;
	I2CBusy();

	SSP1BUF = msb_input;
	I2CBusy();

	SSP1BUF = lsb_input;
	I2CBusy();

	StopI2C1();
	timeout = 0;
	while(SSP1CON2bits.PEN)	//Wait till Stop sequence is completed
	{
		delay_ms(1);
		timeout++;
		if(timeout > 100)
			break;
	}
	CloseI2C1();		        //close I2C module
	SSP1CON2bits.SEN=0;
}
