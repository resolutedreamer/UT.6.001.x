// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// March 8, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

unsigned long input, output, playback, washigh = 0, lastvalue = 0, sendout;

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void)
{ 
		volatile unsigned long delay;
		SYSCTL_RCGC2_R |= 0x01; // turn on the clock for ports A
		delay = SYSCTL_RCGC2_R;           // allow time for clock to start
		
		// port A
    GPIO_PORTA_AFSEL_R &= 0x00;// turn off alternate function
    GPIO_PORTA_AMSEL_R &= 0x00; // turn off analog function
    GPIO_PORTA_PCTL_R &= 0x00;   // GPIO clear bit PCTL

    GPIO_PORTA_DIR_R &= 0x00; // set all Port A input
    GPIO_PORTA_DIR_R |= 0x04; // then set Port A Pin 2 Output
    GPIO_PORTA_DEN_R |= 0x0C; // turn on digital function for Pin 2 and 3

		//SysTick
	
		NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
		NVIC_ST_RELOAD_R = 90910-2;  // particular reload value (1.136 ms)/(12.5 ns)
		NVIC_ST_CURRENT_R = 0;                // any write to current clears it
																												// enable SysTick with core clock
		NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN;

}

// called at 880 Hz
void SysTick_Handler(void)
{
		// if we are playing back, every time we enter the interrupt, toggle 
		if (playback == 1)
		{
			if (lastvalue == 0)
			{
				//it was low so make it high
				sendout = 0x04;
				GPIO_PORTA_DATA_R |= sendout;
				lastvalue = 1;
			}				
			
			else if (lastvalue == 1)
			{
				//it was high so make it low
				sendout = ~(0x04);
				GPIO_PORTA_DATA_R &= sendout;
				lastvalue = 0;
			}
		}
}

int main(void)
{// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
  while(1)
	{
		input = (GPIO_PORTA_DATA_R & 0x08) >> 3;
		
		if (washigh == 0 && input == 1)
		{
			//was not high and becme high
			//rising edge, activate or deactivate the tone
			washigh = 1;
			if (playback == 0)
			{
				playback = 1;
			}
			else if (playback == 1)
			{
				playback = 0;
			}
		}
		
		if (washigh == 1 && input == 0)
		{
			// was high and became low
			//falling edge, now you know the next input is rising
			washigh = 0;
		}
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}

