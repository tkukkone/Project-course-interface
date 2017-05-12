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

//Moves point pointToMove to closest location that is within the triangle
void movePointWithinTriangle(Point* target, Point* red, Point* green, Point* blue);
int PointInTriangle(Point* p, Point* p0, Point* p1, Point* p2);
void RGB_ratio(Point* red, Point* green, Point* blue, Point* target);

#endif //DELAY_MICROSECONDS

