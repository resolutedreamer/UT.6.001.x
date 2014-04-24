// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 11, 2014

// Lab 8
//      Jon Valvano and Ramesh Yerraballi
//      November 21, 2013

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void Delay100ms(void);
void PortInit(void);
void LEDOn(void);
unsigned long input, output, currentoutput, temp;
#define PE0 (*((volatile unsigned long *)0x40024004))
#define PE1 (*((volatile unsigned long *)0x40024008))

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10 k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).

int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
    PortInit();    
    LEDOn();
    
    TExaS_Init(SW_PIN_PE0, LED_PIN_PE1);  // activate grader and set system clock to 80 MHz
 
    EnableInterrupts();           // enable interrupts for the grader
    

    
    while (1)
    {
        Delay100ms();
        input = PE0 & 0x01;
        if (input == 0)
        {
            LEDOn();
        }
        else if (input == 1)
        {
            currentoutput = (PE1 & 0x02);
            // we get the current output
            if (currentoutput == 0x00) // if it is low, make it high
            {
                output = 0x02;
            }
            else if (currentoutput == 0x02) // if it is high, make it low
            {
                output = 0x00;
            }            
            PE1 = output;
            // GPIO_PORTE_DATA_R = output;            
        }

    }
  
}

void PortInit(void)
{
    // port E0 input
    // port E1 output
 
    SYSCTL_RCGC2_R |= 0x10; // turn on the clock for port E    
    Delay100ms();                   // wait for the port to be on
    
    GPIO_PORTE_DEN_R |= 0x03; // turn on digital function    
    GPIO_PORTE_AFSEL_R &= 0x00;// turn off alternate function
    GPIO_PORTE_AMSEL_R &= 0x00; // turn off analog function
    GPIO_PORTE_PCTL_R &= 0x00;   // 4) GPIO clear bit PCTL
    

    GPIO_PORTE_DIR_R &= 0x00; //set all Port E input
    GPIO_PORTE_DIR_R |= 0x02; //then set Port E Pin 1 Output
    // ok
}


void LEDOn(void) 
{
    GPIO_PORTE_DATA_R |= 0x02; // LED starts ON
}



void Delay100ms(void)
{
    unsigned long halfsecs = 1;
    unsigned long count;
  
  while(halfsecs > 0 ) { // repeat while there are still halfsecs to delay
    count = 1538460; // 400000*0.5/0.13 that it takes 0.13 sec to count down to zero
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    halfsecs--;
  }
}
