// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// April 8, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE1 and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value


void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10];
unsigned long Distance; // units 0.001 cm
unsigned long ADCdata;  // 12-bit 0 to 4095 sample
unsigned long Flag;     // 1 means valid Distance, 0 means Distance is empty


//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample)
{
	// sample value of 0 means initial position, no distance 0 cm
	// sample value of 4096 means final position, max distance 2.0 cm
	// max distance (2.0 cm) / 4096 (unit)  = (0.00048828125 cm)/(1 unit)
	
	// if the linearity isn't high enough the real board will give error.
	
	float distance;
	distance = sample * 2000.0/4095.0;
	//2000.0 instead of 2.0 cm due to the format of the return value
	distance = distance + 1.0;
	return distance;  // replace this line with real code
}

//T = period = 12.5ns*(RELOAD+1)

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period)
{
	//SysTick
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
	NVIC_ST_RELOAD_R = 2000000-1;  // proper reload value
	NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
	
	// enable SysTick with core clock
	NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN;
}

// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void)
{
	// call the function to get the raw information value
	ADCdata = ADC0_In();
	// process the raw information
	Distance = Convert(ADCdata);
	// store the string in the mailbox
	Flag = 1;
}



//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n)
{
	// as part of Lab 11 you implemented this function
    unsigned long numberOfDigits;
    String[1] = '.';
    String[5] = ' ';
    String[6] = 'c';
    String[7] = 'm';
    String[8] = '\0';

    
    if ( n > 9999)
    {
        numberOfDigits = 5;
    }
    else if ( n > 999 && n < 10000 )
    {
        numberOfDigits = 4;
    }
    else if ( n > 99 && n < 1000)
    {
        numberOfDigits = 3;
    }
    else if ( n > 9 && n < 100)
    {
        numberOfDigits = 2;
    }
    else if (n < 10)
    {
        numberOfDigits = 1;
    }
    
    
    switch (numberOfDigits)
    {
        case 5:
            String[0] = '*';
            String[2] = '*';
            String[3] = '*';
            String[4] = '*';
            break;
        case 4:
            String[0] = 48 + (n/1000)%10;
            String[2] = 48 + (n/100)%10;
            String[3] = 48 + (n/10)%10;
            String[4] = 48 + (n)%10;
            break;
        case 3:
            String[0] = '0';
            String[2] = 48 + (n/100)%10;
            String[3] = 48 + (n/10)%10;
            String[4] = 48 + (n)%10;
            break;
        case 2:
            String[0] = '0';
            String[2] = '0';
            String[3] = 48 + (n/10)%10;
            String[4] = 48 + (n)%10;
            break;
        case 1:
            String[0] = '0';
            String[2] = '0';
            String[3] = '0';
            String[4] = 48 + n;
            break;
        default:
            String[0] = '*';
            String[2] = '*';
            String[3] = '*';
            String[4] = '*';
            break;
    }
}
int main(void)
{ 
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
	// initialize ADC0, channel 1, sequencer 3
	ADC0_Init();
	// initialize Nokia5110 LCD (optional)
	Nokia5110_Init();
	// initialize SysTick for 40 Hz interrupts
	SysTick_Init(0);
	// initialize profiling on PF1 (optional)
                                    //    wait for clock to stabilize

  EnableInterrupts();
// print a welcome message  (optional)
  while(1)
	{ 
			// read mailbox
			// convert the value to a string
			UART_ConvertDistance(Distance);

			// output to Nokia5110 LCD (optional)
		
  }
}

int main1(void)
{ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
  }
}
int main2(void)
{
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    
  }
}

