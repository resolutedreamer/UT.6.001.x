// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// September 30, 2013

// Port E bits 3-0 have 4 piano keys

#include "Piano.h"
#include "..//tm4c123gh6pm.h"


// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void)
{
		volatile unsigned long delay;
		SYSCTL_RCGC2_R |= 0x10; // turn on the clock for ports E, 0b: 0011 0010
		delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	
  	// port E
    GPIO_PORTE_AFSEL_R &= 0x00;// turn off alternate function
    GPIO_PORTE_AMSEL_R &= 0x00; // turn off analog function
    GPIO_PORTE_PCTL_R &= 0x00;   // GPIO clear bit PCTL

    GPIO_PORTE_DIR_R &= 0x00; // set all Port E input
		GPIO_PORTE_DEN_R |= 0x0F; // turn on digital function for E0 E1 E2 E3
}
// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void)	
{
	unsigned long input;
  input = GPIO_PORTE_DATA_R;
	input = input;
  return input;
}
