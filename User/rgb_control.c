#include "rgb_control.h"
#include "delay.h"
void Red (void const *dutyCycle) {
	unsigned onDelayRed = *(float*)dutyCycle * 1000;
	unsigned offDelayRed = 1000-onDelayRed;
	while (1) {
		GPIOB->BSRR = (1<<13);			//LED on: set PortB.13
		delayUS_DWT(onDelayRed);					//pause
		GPIOB->BRR = (1<<13);				//LED off: clear PortB.13
		
		delayUS_DWT(offDelayRed);					//pause
	}
}

void Blue (void const *dutyCycle) {
	unsigned onDelayBlue = *(float*)dutyCycle * 1000;
	unsigned offDelayBlue = 1000-onDelayBlue;
	while (1) {
		GPIOB->BSRR = (1<<11);			//LED on: set PortB.11
		delayUS_DWT(onDelayBlue);	
		GPIOB->BRR = (1<<11);				//LED off: clear PortB.11
		delayUS_DWT(offDelayBlue);				
	}
}

void Green (void const *dutyCycle) {
	unsigned onDelayGreen = *(float*)dutyCycle * 1000;
	unsigned offDelayGreen = 1000-onDelayGreen;
	while (1) {
		GPIOB->BSRR = (1<<15);			//LED on: set PortB.15
		delayUS_DWT(onDelayGreen);
		GPIOB->BRR = (1<<15);				//LED off: clear PortB.15
		delayUS_DWT(offDelayGreen);
	}
}

void AutoReset (void const *resetDelay) {
	delayUS_DWT(*(float*)resetDelay);	//AutoReset delay
	NVIC_SystemReset(); //Resets microcontroller
}


//Clamps number within lower and upper limit
float clamp(float number, float lower_limit, float upper_limit) {
    if (number < lower_limit) {
        return lower_limit;
    } else if (number > upper_limit) {
        return upper_limit;
    } else {
        return number;
    }
}

//Moves point pointToMove within area limited by the corners of the triangle
void MovePointWithinTriangle( const Point* triangle, Point* pointToMove ) {
    Point edge0 = {(triangle[1].x - triangle[0].x), ((triangle[1].y - triangle[0].y))};
    Point edge1 = {(triangle[2].x - triangle[0].x), ((triangle[2].y - triangle[0].y))};
    Point v0 = {(triangle[0].x - pointToMove->x), (triangle[0].y - pointToMove->y)};

    float a = edge0.x*edge0.x + edge0.y*edge0.y;
    float b = edge0.x*edge1.x + edge0.y*edge1.y;
    float c = edge1.x*edge1.x + edge1.y*edge1.y;
    float d = edge0.x*v0.x +  edge0.y*v0.y;
    float e = edge1.x*v0.x + edge1.y*v0.y;

    float det = a*c - b*b;
    float s = b*e - c*d;
    float t = b*d - a*e;

    if ( s + t < det ){
        if ( s < 0.f ){
            if ( t < 0.f ){
                if ( d < 0.f ){
                    s = clamp( -d/a, 0.f, 1.f );
                    t = 0.f;
                }
                else {
                    s = 0.f;
                    t = clamp( -e/c, 0.f, 1.f );
                }
            }
            else{
                s = 0.f;
                t = clamp( -e/c, 0.f, 1.f );
            }
        }
        else if ( t < 0.f ){
            s = clamp( -d/a, 0.f, 1.f );
            t = 0.f;
        }
        else {
            float invDet = 1.f / det;
            s *= invDet;
            t *= invDet;
        }
    }
    else {
        if ( s < 0.f ) {
            float tmp0 = b+d;
            float tmp1 = c+e;
            if ( tmp1 > tmp0 ) {
                float numer = tmp1 - tmp0;
                float denom = a-2*b+c;
                s = clamp( numer/denom, 0.f, 1.f );
                t = 1-s;
            }
            else {
                t = clamp( -e/c, 0.f, 1.f );
                s = 0.f;
            }
        }
        else if ( t < 0.f ) {
            if ( a+d > b+e ) {
                float numer = c+e-b-d;
                float denom = a-2*b+c;
                s = clamp( numer/denom, 0.f, 1.f );
                t = 1-s;
            }
            else {
                s = clamp( -e/c, 0.f, 1.f );
                t = 0.f;
            }
        }
        else {
            float numer = c+e-b-d;
            float denom = a-2*b+c;
            s = clamp( numer/denom, 0.f, 1.f );
            t = 1.f - s;
        }
    }
    //pointToMove->x = triangle[0].x + s * edge0.x + t * edge1.x;
    //pointToMove->y = triangle[0].y + s * edge0.y + t * edge1.y;
		//pointToMove[0].x = 0.5;
		//pointToMove[0].y = 0.5;
		
}





