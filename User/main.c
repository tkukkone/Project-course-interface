/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x.h"                  		// Device header
#include "rgb_control.h"
#include "delay.h"
#include "adc.h"


	/************ Luminous fluxes ***********/
float red_lm = 1.315; 		// maximum brightness of red, measured in lab.
float green_lm	= 2.553;	// maximum brightness of green, measured in lab.
float blue_lm = 0.466;  // maximum brightness of blue, measured in lab.
Point target = {0.2, 0.2}; //SET TARGET COLOR HERE (disable potcontrol first!)
float brightness = 1.0; //brightness between 0 and 1


void RGB_ratio(Point target, float dutyCycles[]){
	Point red = {0.6907, 0.3902};
	Point green = {0.1516, 0.7445};
	Point blue = {0.1296, 0.0627};
	Point triangle[] = {red, green, blue};
	MovePointWithinTriangle(triangle, &target);
	
	
	// For derivation of ratios see 
	// http://www.ledsmagazine.com/articles/print/volume-10/issue-6/features/understand-rgb-led-mixing-ratios-to-realize-optimal-color-in-signs-and-displays-magazine.html
	float mRB = (red.y - blue.y)/(red.x-blue.x);
	float cRB = blue.y - mRB*blue.x;

	float mGD = (green.y-target.y)/(green.x-target.x);
	float cGD = green.y-mGD*green.x;

	float xP = (cGD-cRB)/(mRB-mGD);
	float yP = mRB * xP + cRB;

	float R_RB = -(red.y/blue.y)*(blue.y-yP) / (red.y-yP);
	float R_GP = -(green.y/yP)*(yP-target.y) / (green.y-target.y);
	float ratio_R = (R_RB)/(R_RB+1); 	//Red
	float ratio_G = R_GP;							//Green
	float ratio_B = 1/(R_RB+1);				//Blue
	float total_ratio = ratio_R + ratio_G + ratio_B;
	dutyCycles[0] = (blue_lm / total_ratio * ratio_R)/red_lm; 	//Red duty cycle
	dutyCycles[1] = (blue_lm / total_ratio * ratio_G)/green_lm; //Green duty cycle
	dutyCycles[2] = (blue_lm / total_ratio * ratio_B)/blue_lm;	//Blue duty cycle
}

	/***********Defining Threads************/
osThreadDef(Red, osPriorityNormal, 1, 0); //Define Red PWM Thread
osThreadDef(Blue, osPriorityNormal, 1, 0); //Define Blue PWM Thread
osThreadDef(Green, osPriorityNormal, 1, 0); //Define Green PWM Thread
osThreadDef(AutoReset, osPriorityNormal, 1, 0); //Define AutoReset thread

// main: initialize and start the system
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here
	RCC->APB2ENR |= (1UL << 3);                /* Enable GPIOB clock            */
	GPIOB->CRH    =  0x33333333;               /* PB.8..16 defined as Outputs   */
	
	volatile uint32_t *DWT_CONTROL = (uint32_t *) 0xE0001000;
	volatile uint32_t *DWT_CYCCNT = (uint32_t *) 0xE0001004;
	volatile uint32_t *DEMCR = (uint32_t *) 0xE000EDFC;
	volatile uint32_t *LAR  = (uint32_t *) 0xE0001FB0;   // <-- added lock access register

	*DEMCR = *DEMCR | 0x01000000;     // enable trace
	*LAR = 0xC5ACCE55;                // <-- added unlock access to DWT (ITM, etc.)registers 
	*DWT_CYCCNT = 0;                  // clear DWT cycle counter
	*DWT_CONTROL = *DWT_CONTROL | 1;  // enable DWT cycle counter
   
	/******************* ADC ******************/
	// Comment these lines to disable potcontrol
	ADC_Configuration(); //	12-bit ADC potentiometer reading config
	brightness = 1.000 - (1.100 * readADC1(0))/4095; //Brightness (direction reversed)
	target.x = 0.750 - (0.800 * readADC1(1))/4095; //x (direction reversed)
	target.y = 0.800 - (0.900 * readADC1(2))/4095; //y (direction reversed)
	
	/**************** Duty cycles ****************/
	float dutyCycles[3];
	RGB_ratio(target, dutyCycles);
	
	float dutyCycleRed = dutyCycles[0] * brightness;
	float dutyCycleGreen = dutyCycles[1] * brightness;
	float dutyCycleBlue = dutyCycles[2] * brightness;
	
	/******************* Threads ******************/
	// create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	osThreadCreate(osThread(Green), &dutyCycleGreen); 	
	osThreadCreate(osThread(Red), &dutyCycleRed); 	
	osThreadCreate(osThread(Blue), &dutyCycleBlue); 
	osThreadCreate(osThread(AutoReset), NULL); //AutoReset thread
	
  osKernelStart ();                         // start thread execution 
}
