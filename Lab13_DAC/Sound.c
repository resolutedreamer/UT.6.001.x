// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "Piano.h"

#include "helpers.h"

#include "..//tm4c123gh6pm.h"

#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value


// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none

int i = 0;

void Sound_Init(void)	
{
	DAC_Init();
	
	
	
	//SysTick
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
	NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // max  reload value
	NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
	
	// enable SysTick with core clock
	NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN;

}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none

unsigned long checkSysTick, outputThis;

void Sound_Tone(unsigned long period)
{
	
	// this routine sets the RELOAD and starts SysTick
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
	NVIC_ST_RELOAD_R = period;  // max  reload value
	NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	
	// enable SysTick with core clock
	NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN;
	checkSysTick = NVIC_ST_CTRL_R;
	checkSysTick = checkSysTick;
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void)
{
	// this routine stops the sound output
	DAC_Out(0);
}



// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void)
{
	
	//using 32 element sine table
	if (i == 32)
	{
		i = 0;
	}
	outputThis = wave[i];
	
	
	/*
	// using 16 element sine table
	if (i == 16)
	{
		i = 0;
	}
	outputThis = SineWave[i];
	*/
	
	outputThis = outputThis;
	DAC_Out(outputThis);
	i++;	
}

/*
Period Calculations for N = 32

We want to set the SysTick fs appropriately so that
fs = f*N
where N is 32, since 32 elements in our sine table
and f is the frequency of the sound

Then once we know fs, we need to set fs using the
appropriate value of the period in the RELOAD register
desired period = 1/fs = T
T = 12.5ns*(RELOAD+1)
RELOAD = (T/12.5ns) - 1

//frequency for C is 523.251
fs = 523.251*32
= 16744.032

RELOAD = ((1/16744.032)s/12.5ns) - 1
= 4777

//period for D is 587.330
fs = 587.330*32
= 18794.560

RELOAD = ((1/18794.560)s/12.5ns) - 1
= 4256

//period for E is 659.255
fs = 659.255*32
= 21096.16

RELOAD = ((1/21096.16)s/12.5ns) - 1
= 3791

//period for G is 783.991
fs = 783.991*32
= 25087.712

RELOAD = ((1/25087.712)s/12.5ns) - 1
= 3188

*/

/*
Period Calculations for N = 32, John Hall Method

RELOAD = (fclock)/(fout*N) - 1

//frequency for C is 523.251

RELOAD = (80000000)/(523.251*32) - 1
= 4777

//period for D is 587.330

RELOAD = (80000000)/(587.330*32) - 1
= 4256

//period for E is 659.255

RELOAD = (80000000)/(659.255*32) - 1
= 3791

//period for G is 783.991

RELOAD = (80000000)/(783.991*32) - 1
= 3188

*/




/*
Period Calculations for N = 16

We want to set the SysTick fs appropriately so that
fs = f*N
where N is 16, since 16 elements in our sine table
and f is the frequency of the sound

Then once we know fs, we need to set fs using the
appropriate value of the period in the RELOAD register
desired period = 1/fs = T
T = 12.5ns*(RELOAD+1)
RELOAD = (T/12.5ns) - 1

//frequency for C is 523.251
fs = 523.251*16
= 8372.016

RELOAD = ((1/8372.016)s/12.5ns) - 1
= 9555

//period for D is 587.330
fs = 587.330*16
= 9397.28

RELOAD = ((1/9397.28)s/12.5ns) - 1
= 8512

//period for E is 659.255
fs = 659.255*16
= 10548.08

RELOAD = ((1/10548.08)s/12.5ns) - 1
= 7583

//period for G is 783.991
fs = 783.991*16
= 12543.856

RELOAD = ((1/12543.856)s/12.5ns) - 1
= 6377

*/
