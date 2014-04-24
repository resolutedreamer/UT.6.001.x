// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void)
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x02; // turn on the clock for ports B
	delay = SYSCTL_RCGC2_R;           // allow time for clock to start

	// port B
	GPIO_PORTB_AFSEL_R &= 0x00;// turn off alternate function
	GPIO_PORTB_AMSEL_R &= 0x00; // turn off analog function
	GPIO_PORTB_PCTL_R &= 0x00;   // GPIO clear bit PCTL
	GPIO_PORTB_DR8R_R |= 0x0F;    // enable 8 mA drive on Port B 0 1 2 3

	GPIO_PORTB_DIR_R &= 0x00; // set all Port B input
	GPIO_PORTB_DIR_R |= 0x0F; // then set Port B Pin 0 1 2 3 Output
	GPIO_PORTB_DEN_R |= 0x0F; // turn on digital function for Pin B 0 1 2 3
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none

unsigned long Bit0, Bit1, Bit2, Bit3, outputlevel, debugvalue;

void DAC_Out(unsigned long data)
{
	Bit0 = data & 0x01;
	Bit1 = data & 0x02;
	Bit2 = data & 0x04;
	Bit3 = data & 0x08;
	
	outputlevel = Bit0 + Bit1 + Bit2 + Bit3;
	
	GPIO_PORTB_DATA_R = outputlevel;
	debugvalue = GPIO_PORTD_DATA_R;
	debugvalue = debugvalue;
}
