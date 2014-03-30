// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

//------------------------------------------
// east/west red light connected to PB5
// PB5 = EWRed

// east/west yellow light connected to PB4
// PB4 = EWYellow

// east/west green light connected to PB3
// PB3 = EWGreen
//------------------------------------------
// north/south facing red light connected to PB2
// PB3 = NSRed

// north/south facing yellow light connected to PB1
// PB3 = NSYellow

// north/south facing green light connected to PB0
// PB3 = NSGreen
//------------------------------------------
// pedestrian detector connected to PE2 (1=pedestrian present)
// PE2 = WalkDetect

// north/south car detector connected to PE1 (1=car present)
// PE1 = NSDetect

// east/west car detector connected to PE0 (1=car present)
// PE0 = EWDetect
//------------------------------------------
// "walk" light connected to PF3 (built-in green LED)
// PF3 = WalkGreen

// "don't walk" light connected to PF1 (built-in red LED)
// PF3 = WalkRed

//------------------------------------------


// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void Delay100ms(void);
void PortInit(void);
void LEDInit(void);
void SysTickDelayer(unsigned long delayLength);

// inputs
unsigned long WalkDetect = 0;
unsigned long NSDetect = 0;
unsigned long EWDetect = 0;

// outputs
// 6 lights

// pedestrian light
unsigned long WalkGreen = 0;
unsigned long WalkRed = 0;

#define PE0 (*((volatile unsigned long *)0x40024004))
#define PE1 (*((volatile unsigned long *)0x40024008))
#define PE2 (*((volatile unsigned long *)0x4002400C))




// ***** 3. Subroutines Section *****

int main(void)
{
	TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
	EnableInterrupts();
	PortInit();
	LEDInit();

	while (1)
	{
		SysTickDelayer(50);
	}
}


void PortInit(void)
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x32; // turn on the clock for ports B, E, and F,   0b: 0011 0010
	delay = SYSCTL_RCGC2_R;           // allow time for clock to start

	// port B
    GPIO_PORTB_AFSEL_R &= 0x00;// turn off alternate function
    GPIO_PORTB_AMSEL_R &= 0x00; // turn off analog function
    GPIO_PORTB_PCTL_R &= 0x00;   // GPIO clear bit PCTL

    GPIO_PORTB_DIR_R &= 0x00; // set all Port B input
    GPIO_PORTB_DIR_R |= 0x3F; // then set Port B Pin 0123 45 Output
    GPIO_PORTB_DEN_R |= 0x3F; // turn on digital function for B 0123 45


	// port E
    GPIO_PORTE_AFSEL_R &= 0x00;// turn off alternate function
    GPIO_PORTE_AMSEL_R &= 0x00; // turn off analog function
    GPIO_PORTE_PCTL_R &= 0x00;   // GPIO clear bit PCTL

    GPIO_PORTE_DIR_R &= 0x00; // set all Port E input
	GPIO_PORTE_DEN_R |= 0x07; // turn on digital function for E0 E1 E2
    
	// port F
	// don't forget you need to unlock port F
	GPIO_PORTF_LOCK_R |= 0x4C4F434B;
	GPIO_PORTF_CR_R = 0x1F;  // allow changes to PF4-0
	// done unlocking port F    
    GPIO_PORTF_AFSEL_R &= 0x00;// turn off alternate function
    GPIO_PORTF_AMSEL_R &= 0x00; // turn off analog function
    GPIO_PORTF_PCTL_R &= 0x00;   // GPIO clear bit PCTL
    // these LEDs need the Pull up resistor
	GPIO_PORTF_PUR_R |= 0x0A;

	GPIO_PORTF_DIR_R &= 0x00; // set all Port F input
    GPIO_PORTF_DIR_R |= 0x0A; // then set Port F Pin 1 and 3 (red and green) Output
	GPIO_PORTF_DEN_R |= 0x0A; // turn on digital function
	
}




void LEDInit(void) 
{
    // blank for now
}



//leftover from previous code, leave it alone for now until we write a new delay function
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

