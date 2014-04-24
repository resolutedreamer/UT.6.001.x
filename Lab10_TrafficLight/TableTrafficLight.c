// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

//------------------------------------------
// Outputs
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
// "walk" light connected to PF3 (built-in green LED)
// PF3 = WalkGreen

// "don't walk" light connected to PF1 (built-in red LED)
// PF3 = WalkRed
//------------------------------------------
// Inputs
//------------------------------------------
// pedestrian detector connected to PE2 (1=pedestrian present)
// PE2 = WalkDetect

// north/south car detector connected to PE1 (1=car present)
// PE1 = NSDetect

// east/west car detector connected to PE0 (1=car present)
// PE0 = EWDetect
//------------------------------------------

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "stdio.h"

//Shortcuts to refer to the various states in the FSM array
#define EWRedOn 1
#define EWRedOff 0
#define EWYellowOn 1
#define EWYellowOff 0
#define EWGreenOn 1
#define EWGreenOff 0

#define NSRedOn 1
#define NSRedOff 0
#define NSYellowOn 1
#define NSYellowOff 0
#define NSGreenOn 1
#define NSGreenOff 0

#define WalkGreenOn 1
#define WalkGreenOff 0
#define WalkRedOn 1
#define WalkRedOff 0



#define AllRed 0
#define GoWest 1
#define SlowWest 2
#define GoSouth 3 
#define SlowSouth 4
#define PedWalk 5 
#define FlashingDontWalk 6
#define GoSouth2 7
#define GoWest2 8



// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void Delay100ms(void);
void PortInit(void);

// inputs
unsigned long WalkDetect = 0;
unsigned long NSDetect = 0;
unsigned long EWDetect = 0;
unsigned long input = 0;

// Index of current FSM state
unsigned char cState;

// represents a State of the FSM 
struct TrafficStateType
{
	// Outputs
	/*unsigned char EWRed;
	unsigned char EWYellow;
	unsigned char EWGreen;
	unsigned char NSRed;
	unsigned char NSYellow;
	unsigned char NSGreen;
	unsigned char WalkGreen;
	unsigned char WalkRed;*/
	unsigned char allLights[8];
	
	// Delay, Time to wait when in this state
	unsigned short wait;
  // Next State
	unsigned char next[8];
};

typedef const struct TrafficStateType TrafficState;

//The data structure that captures the FSM state transition graph
TrafficState Fsm[9] =
{
	// AllRed
   { {EWRedOn, EWYellowOff, EWGreenOff,          NSRedOn, NSYellowOff, NSGreenOff,          WalkGreenOff, WalkRedOn}, 10, {AllRed,PedWalk,GoSouth,GoSouth,GoWest,GoWest,GoWest,GoWest}},
	// GoWest
	 { {EWRedOff, EWYellowOff, EWGreenOn,          NSRedOn, NSYellowOff, NSGreenOff,          WalkGreenOff, WalkRedOn}, 70, {GoWest,SlowWest,SlowWest,SlowWest,GoWest,SlowWest,SlowWest,SlowWest}},
	 // SlowWest
	 { {EWRedOff, EWYellowOn, EWGreenOff,          NSRedOn, NSYellowOff, NSGreenOff,          WalkGreenOff, WalkRedOn}, 20, {AllRed,PedWalk,GoSouth,GoSouth,AllRed,GoWest,GoSouth,GoSouth}},
	 // GoSouth
	 { {EWRedOn, EWYellowOff, EWGreenOff,          NSRedOff, NSYellowOff, NSGreenOn,          WalkGreenOff, WalkRedOn}, 70, {GoSouth,SlowSouth,GoSouth,SlowSouth,SlowSouth,SlowSouth,SlowSouth,SlowSouth}},
	 // SlowSouth
	 { {EWRedOn, EWYellowOff, EWGreenOff,          NSRedOff, NSYellowOn, NSGreenOff,          WalkGreenOff, WalkRedOn}, 20, {AllRed,PedWalk,AllRed,PedWalk,GoWest,GoWest,GoWest,PedWalk}},
	 // PedWalk
	 { {EWRedOn, EWYellowOff, EWGreenOff,          NSRedOn, NSYellowOff, NSGreenOff,          WalkGreenOn, WalkRedOff}, 50, {PedWalk,PedWalk,AllRed,AllRed,AllRed,AllRed,AllRed,FlashingDontWalk}},
	 // FlashingDontWalk
	 { {EWRedOn, EWYellowOff, EWGreenOff,          NSRedOn, NSYellowOff, NSGreenOff,          WalkGreenOff, WalkRedOff}, 50, {PedWalk,PedWalk,PedWalk,PedWalk,PedWalk,PedWalk,PedWalk,GoSouth2}},
	 // GoSouth2
	 { {EWRedOn, EWYellowOff, EWGreenOff,          NSRedOff, NSYellowOff, NSGreenOn,          WalkGreenOff, WalkRedOff}, 50, {0,0,0,0,0,0,0,GoWest2}},
	 // GoWest2
	 { {EWRedOff, EWYellowOff, EWGreenOn,          NSRedOn, NSYellowOff, NSGreenOff,          WalkGreenOff, WalkRedOff}, 50, {0,0,0,0,0,0,0,AllRed}}
	 
};


// ***** 3. Subroutines Section *****

int main(void)
{
	TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
	EnableInterrupts();
	PortInit();
	SysTick_Init();
	
	
	while (1)
	{
		//Activate different lights based on current state
		GPIO_PORTB_DATA_R = (Fsm[cState].allLights[0] << 5) + (Fsm[cState].allLights[1] << 4) + (Fsm[cState].allLights[2] << 3) + (Fsm[cState].allLights[3] << 2) + (Fsm[cState].allLights[4] << 1) + (Fsm[cState].allLights[5] << 0) ;  //Output to PB5, PB4, PB3, PB2, PB1, PB0
		GPIO_PORTF_DATA_R = (Fsm[cState].allLights[6] << 3) + (Fsm[cState].allLights[7] << 1) ; //Output to PF3 and PF1 in one line
		
		// wait for time relevant to state, different lights have different times
		SysTick_Wait10ms(Fsm[cState].wait);
		
		// get individual inputs, names present for debugging purposes
		WalkDetect = (GPIO_PORTE_DATA_R & 0x04)>>2; // Input 0/1: Sw1 NotPressed/Pressed
		NSDetect = (GPIO_PORTE_DATA_R & 0x02)>>1; // Input 0/1: Sw1 NotPressed/Pressed
		EWDetect = (GPIO_PORTE_DATA_R & 0x01)>>0; // Input 0/1: Sw1 NotPressed/Pressed
		
		// change state based on input and current state
		input = WalkDetect + (NSDetect << 1) + (EWDetect<< 2);
		//printf("text%d",input);
		cState = Fsm[cState].next[input] ; 

	}
}

//currently flashing walk is no good

unsigned long tempz;

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
    // No Pull up resistor
		GPIO_PORTF_PUR_R |= 0x00;

		tempz = GPIO_PORTF_PUR_R;

		GPIO_PORTF_DIR_R &= 0x00; // set all Port F input
    GPIO_PORTF_DIR_R |= 0x0A; // then set Port F Pin 1 and 3 (red and green) Output
		GPIO_PORTF_DEN_R |= 0x0A; // turn on digital function
	
}
