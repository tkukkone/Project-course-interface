#include "stm32f10x.h"

#ifndef RGB_CONTROL
#define RGB_CONTROL

void Red (void const *dutyCycle);
void Blue (void const *dutyCycle);
void Green (void const *dutyCycle);
void AutoReset (void const *resetDelay) ;


typedef struct Point{
    float x;
    float y;
} Point;

//Moves point pointToMove to closest location that is within the triangle
//Modified from https://www.gamedev.net/topic/552906-closest-point-on-triangle/
//
//Example use:
//Point p0 = {2, 1};
//Point p1 = {3, 4};
//Point p2 = {6, 1};
//Point triangle[3] = {p0, p1, p2};
//Point sourcePos = {3,2.122};
//Point* pointToMove = &sourcePos;
//MovePointWithinTriangle(triangle, pointToMove);
void MovePointWithinTriangle( const Point* triangle, Point* pointToMove);

#endif //DELAY_MICROSECONDS

