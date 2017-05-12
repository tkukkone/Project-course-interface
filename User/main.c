/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x.h"                  		// Device header
#include "delay.h"
#include "adc.h"
#include "rgb_control.h"






	/***********Defining Threads************/
osThreadDef(Red, osPriorityNormal, 1, 0); //Define Red PWM Thread
osThreadDef(Blue, osPriorityNormal, 1, 0); //Define Blue PWM Thread
osThreadDef(Green, osPriorityNormal, 1, 0); //Define Green PWM Thread
//osThreadDef(AutoReset, osPriorityNormal, 1, 0); //Define AutoReset thread

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
	


	//float resetDelay = 10000; // AutoReset delay (us)
	
	/**************** Duty cycles ****************/
	//float dutyCycles[3];
	 //SET TARGET COLOR HERE (disable potcontrol first!)
	Point target;
	Point red = {0.6907f, 0.3092f};
	Point green = {0.1516f, 0.7445f}; 
	Point blue = {0.1296f, 0.0627f};
	
	target.x = 0.7f * readADC1(1)/4095; //x (direction reversed)
	target.y = 0.8f * readADC1(2)/4095; //y (direction reversed)
	movePointWithinTriangle(&target, &red, &green, &blue);
	RGB_ratio(&red, &green, &blue, &target);
	

	//float dutyCycleRed = dutyCycles[0];
	//float dutyCycleGreen = dutyCycles[1];
	//float dutyCycleBlue = dutyCycles[2];
	
	/******************* Threads ******************/
	// create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	osThreadCreate(osThread(Red), NULL); 
	osThreadCreate(osThread(Green), NULL); 	
	osThreadCreate(osThread(Blue), NULL); 
	//osThreadCreate(osThread(AutoReset), NULL); //AutoReset thread
	
  osKernelStart ();                         // start thread execution 
	delayUS_DWT(10000);	//AutoReset delay
	NVIC_SystemReset(); //Resets microcontroller
}
