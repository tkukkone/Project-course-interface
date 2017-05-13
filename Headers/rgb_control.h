#include "stm32f10x.h"

#ifndef RGB_CONTROL
#define RGB_CONTROL

void Red (void const *dutyCycle);
void Blue (void const *dutyCycle);
void Green (void const *dutyCycle);
void AutoReset (void const *resetDelay);


typedef struct Point{
    float x;
    float y;
} Point;

//Moves point target within triangle limited by red, green and blue. If point is
//not within triangle, vector towards the middle of the triangle is added until
//the point is inside the triangle. The colors have to be in counter-clockwise order.
void movePointWithinTriangle(Point* target, Point* red, Point* green, Point* blue);

//Checks if point p is within triangle limited by p0, p1 and p2
int PointInTriangle(Point* p, Point* p0, Point* p1, Point* p2);

//Calculates duty ratios for red, green and blue LEDs. Duty ratios are 
//global variables in rgb_control.c
void RGB_ratio(Point* red, Point* green, Point* blue, Point* target);

#endif //DELAY_MICROSECONDS

