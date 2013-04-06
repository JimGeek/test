#include <p18F66K22.h>
#include <pic.h>
void SPI_Init()
{
	SSP1STAT.SMP = 1;  // SMP=1 The output data is sampled at the end.
	SSP1STAT.CKE = 0  // CKE=1 Transmit occurs on the transition from active to Idle clock state.

	SSP1CON1.WCOL = 0; // Write collision detect clered at the initialization.
	SSP1CON1.SSPOV = 0; // used only in the slave mode.
	SSP1CON1.SSPEN = 1; // Enables serial port and configures SCKx, SDOx, SDIx and SSx as serial port pins
    SSP1CON1.CKP = 0; // Idle state for the clock is a low level
	SSP1CON1.SSPM = 1010; //SPI Master mode: clock = FOSC/8

	TRISCbits.TRISC4 = 1; // Direction for SPI pins.
	TRISCbits.TRISC5 = 0;
	TRISCbits.TRISC3 = 0;
	TRISFbits.TRISF7 = 1;
}

void SPI_Send(unsigned short data)
{
	SSP1BUF = data; // data to be transmited is copied to the buffer
	delay_ms(15); // small delay for reading from the buffer.
	while(!SP1STAT.BF); // Remain in infinite loop till a byte is transfered.
	SP1STAT.BF = 0; // Clears the buffer full bit to continue with operation.
}
unsigned short SPI_Read()
{
	