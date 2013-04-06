#include "range.h"
#include <p18F66K22.h>


void mux_en()
{
	LATA3 = 1;	
}

void mux_sel(int S0,int S1)
{
	 LATB3 = S0;
	 LATB2 = S1;
}

void V_relay(int sel)
{
	LATB4 = sel;
}

void I_relay(int I1,int I2)
{
	LATA0 = I1;
	LATG3 = I2;
}

