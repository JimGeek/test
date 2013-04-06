//dac calibration for unit with serial no. 002

#include <i2c.h>
void enable_DAC(unsigned char i)
{
	unsigned char sync_mode=0, slew=0,status,add1,val1,val2;
	TRISCbits.TRISC3=1;
	TRISCbits.TRISC4=1;
//	PIE3bits.SSP2IE=1;
	add1=0xc0;

	switch(i)
	{
		case 1:
				val1 = 0x0b;
				val2 = 0x90;
				break;
		case 2:

				val1 = 0x1e;
				val2 = 0xa0;
				break;
		case 3:
				val1 = 0x31;
				val2 = 0x70;
				break;
		case 4:
				val1 = 0x44;
				val2 = 0x10;
				break;
		case 5:

				val1 = 0x56;
				val2 = 0xb0;
				break;
		case 6:

				val1 = 0x69;
				val2 = 0x80;
				break;
		case 7:

				val1 = 0x7c;
				val2 = 0x10;
				break;
		case 8:

				val1 = 0x8e;
				val2 = 0x50;
				break;
		case 9:

				val1 = 0xa0;
				val2 = 0xc0;
				break;
		case 10:

				val1 = 0xb3;
				val2 = 0x90;
				break;
		case 11:

				val1 = 0xc6;
				val2 = 0x30;
				break;
		case 12:

				val1 = 0xd8;
				val2 = 0x60;
				break;
		default:
				val1 = 0;
				val2=0;
				break;
	}

	sync_mode = MASTER;
    slew = SLEW_OFF;
	OpenI2C1(sync_mode,slew);
 	SSP1ADD=0x0f;  // 8 MHz SCK
	IdleI2C1();
	SSP1CON1=0x38;

	StartI2C1();

	while(SSP1CON2bits.SEN);	//Wait till Start sequence is completed
	PIR1bits.SSP1IF=0;			//clear interrupt flag

	SSP1BUF = add1;
//	while( SSP1STATbits.BF );
//	status = WriteI2C1( add1  );	//Write Slave address and set master for transmission
//	while(SSP1STATbits.BF); 		// 8 clock cycles
	while(!PIR1bits.SSP1IF);		// Wait for 9th clock cycle
	PIR1bits.SSP1IF=0;				// Clear interrupt flag
//	while(SSP1CON2bits.ACKSTAT);	//wait for acknowledgement

	SSP1BUF = 0x60;
//	status = WriteI2C1( 0x60  );
//	while(SSP1STATbits.BF);
	while(!PIR1bits.SSP1IF);
	PIR1bits.SSP1IF=0;
//	while(SSP1CON2bits.ACKSTAT);

	SSP1BUF = val1;
//	status = WriteI2C1( val1  );
//	while(SSP1STATbits.BF);
	while(!PIR1bits.SSP1IF);
	PIR1bits.SSP1IF=0;
//	while(SSP1CON2bits.ACKSTAT);

	SSP1BUF = val2;
//	status = WriteI2C1( val2 );
//	while(SSP1STATbits.BF);
	while(!PIR1bits.SSP1IF);
	PIR1bits.SSP1IF=0;
//	while(SSP2CON2bits.ACKSTAT);

	StopI2C1();
	while(SSP1CON2bits.PEN);	//Wait till Stop sequence is completed
	CloseI2C1();		        //close I2C module
	SSP1CON2bits.SEN=0;
}