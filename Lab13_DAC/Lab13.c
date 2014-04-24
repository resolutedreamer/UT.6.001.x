// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

//#include "helpers.h"
//#include "DAC.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);


int z = 0;
unsigned long SW0, SW1, SW2, SW3;
unsigned long data, beenpressed = 0, risingedge, newswitch = 0, lastdata = 0, go = 0;


int main(void)
{
	// Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
	
	
	// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
  while(1)
	{
		// input from keys to select tone
		data = Piano_In();
	
		SW0 = data & 0x01;
		SW1 = data & 0x02;
		SW2 = data & 0x04;
		SW3 = data & 0x08;
		
		risingedge = (beenpressed == 0);
		if (risingedge == 1)
		{
			risingedge = risingedge;
		}
		
		if (data == 0)
		{
			Sound_Off();
			continue;
		}
		
		if (data != lastdata)
		{
			newswitch = 1;
		}
		else
		{
			newswitch = 0;
		}
		
		go = (risingedge || newswitch);
		
		if (SW0 && (go == 1))
		{
			//play C
			// 32 element sine table
			Sound_Tone(4777); 
			// 16 element sine table
			//Sound_Tone(9555);
		}
		if (SW1 && (go == 1))
		{
			//play D
			// 32 element sine table
			Sound_Tone(4256);
			// 16 element sine table
			//Sound_Tone(8512);
		}
		if (SW2 && (go == 1))
		{
			//play E
			// 32 element sine table
			Sound_Tone(3791);
			// 16 element sine table
			//Sound_Tone(7583);			
		}
		if (SW3 && (go == 1))
		{
			//play G
			// 32 element sine table
			Sound_Tone(3188);
			// 16 element sine table
			//Sound_Tone(6377);
		}
		delay(10);
		beenpressed = data;
		lastdata = data;
	}
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


