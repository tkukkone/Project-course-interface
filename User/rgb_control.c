#include "rgb_control.h"
#include "delay.h"
#include "adc.h"
#include "stdlib.h"

//Change brightnesses in RGB_ratio function
//float red_lm = 1.315; 		// maximum brightness of red, measured in lab.
//float green_lm	= 2.553;	// maximum brightness of green, measured in lab.
//float blue_lm = 0.466;  // maximum brightness of blue, measured in lab.
//Point target = {0.2, 0.2}; //SET TARGET COLOR HERE (disable potcontrol first!)

unsigned onDelayRed;
unsigned onDelayGreen;
unsigned onDelayBlue;

void Red (void const *dutyCycle) {
	//unsigned onDelayRed = *(float*)dutyCycle * 1000;
	//unsigned offDelayRed = 1000-onDelayRed;
	while (1) {
		GPIOB->BSRR = (1<<13);			//LED on: set PortB.13
		delayUS_DWT(onDelayRed);					//pause
		GPIOB->BRR = (1<<13);				//LED off: clear PortB.13
		delayUS_DWT(1000-onDelayRed);			//pause
	}
}

void Blue (void const *dutyCycle) {
	//unsigned onDelayBlue = *(float*)dutyCycle * 1000;
	//unsigned offDelayBlue = 1000-onDelayBlue;
	while (1) {
		GPIOB->BSRR = (1<<11);			//LED on: set PortB.11
		delayUS_DWT(onDelayBlue);	
		GPIOB->BRR = (1<<11);				//LED off: clear PortB.11
		delayUS_DWT(1000-onDelayBlue);				
	}
}

void Green (void const *dutyCycle) {
	//unsigned onDelayGreen = *(float*)dutyCycle * 1000;
	//unsigned offDelayGreen = 1000-onDelayGreen;
	while (1) {
		GPIOB->BSRR = (1<<15);			//LED on: set PortB.15
		delayUS_DWT(onDelayGreen);
		GPIOB->BRR = (1<<15);				//LED off: clear PortB.15
		delayUS_DWT(1000-onDelayGreen);
	}
}

void RGB_ratio(Point* red, Point* green, Point* blue, Point* target){
	// For derivation of ratios see 
	// http://www.ledsmagazine.com/articles/print/volume-10/issue-6/features/understand-rgb-led-mixing-ratios-to-realize-optimal-color-in-signs-and-displays-magazine.html
	// Variable are being recycled to save memory.
	float mRB = (red->y - blue->y)/(red->x-blue->x);
	float cRB = blue->y - mRB*blue->x;

	float mGD = (green->y-target->y)/(green->x-target->x);
	float cGD = green->y-mGD*green->x;
	/*
	float xP = (cGD-cRB)/(mRB-mGD);
	float yP = mRB * xP + cRB;
	*/
	
	mGD = (cGD-cRB)/(mRB-mGD);
	mRB = mRB * mGD + cRB;

	/*
	float R_RB = -(red.y/blue.y)*(blue.y-yP) / (red.y-yP);
	float R_GP = -(green.y/yP)*(yP-target.y) / (green.y-target.y);
	float ratio_R = (R_RB)/(R_RB+1); 	//Red
	float ratio_G = R_GP;							//Green
	float ratio_B = 1/(R_RB+1);				//Blue
	float total_ratio = ratio_R + ratio_G + ratio_B;
	*/
	
	cGD = -(red->y/blue->y)*(blue->y-mRB) / (red->y-mRB);
	mGD = -(green->y/mRB)*(mRB-target->y) / (green->y-target->y);
	
	mRB = (cGD)/(cGD+1); 	//Red
	//ratio_G = mGD;							//Green
	cRB = 1/(cGD+1);				//Blue
	cGD = mRB + mGD + cRB;
	
	
	/*
	dutyCycles[0] = brightness * ((0.466f / total_ratio * ratio_R)/1.315f); 	//Red duty cycle
	dutyCycles[1] = brightness * ((0.466f / total_ratio * ratio_G)/2.553f); //Green duty cycle
	dutyCycles[2] = brightness * ((0.466f / total_ratio * ratio_B)/0.466f);	//Blue duty cycle
	*/
	
	//change brightnesses here
	//float brightness; //brightness between 0 and 1
	float brightness = 1.0f * readADC1(0)/4095; //Brightness (direction reversed)
	onDelayRed = (brightness * ((0.466f / cGD * mRB)/1.315f)) * 1000; //Red duty cycle
	onDelayGreen = (brightness * ((0.466f / cGD * mGD)/2.553f)) * 1000; //Green duty cycle
	onDelayBlue = (brightness  * ((0.466f / cGD * cRB)/0.466f)) * 1000;	//Blue duty cycle
}

int PointInTriangle(Point* p, Point* p0, Point* p1, Point* p2){
	//area has to be precalculated, causes stack overflow otherwise
	//float area = 0.5 *(-p1->y*p2->x + p0->y*(-p1->x + p2->x) + p0->x*(p1->y - p2->y) + p1->x*p2->y);
	float s = p0->y*p2->x - p0->x*p2->y + (p2->y - p0->y)*p->x + (p0->x - p2->x)*p->y;
	float t = p0->x*p1->y - p0->y*p1->x + (p0->y - p1->y)*p->x + (p1->x - p0->x)*p->y;
	//return s > 0 && t > 0 && s + t < 2 * area;
	return s > 0 && t > 0 && s + t < 2 * 0.188567;
}

void movePointWithinTriangle(Point* target, Point* red, Point* green, Point* blue) {
	Point vectorTowardsMiddle;
	vectorTowardsMiddle.x = (((red->x + green->x + blue->x) / 3) - target->x ) / 100;
	vectorTowardsMiddle.y = (((red->y + green->y + blue->y) / 3) - target->y ) / 100;
	while (!PointInTriangle(target, red, green, blue)) {
		target->x += vectorTowardsMiddle.x;
		target->y += vectorTowardsMiddle.y;
	}
}





