#include "delay.h"
#include "rgb_control.h"

Point red = {0.6907, 0.3092};
Point green = {0.1516, 0.7445};
Point blue = {0.1296, 0.0627};
Point target;
float brightness;
unsigned onDelayRed = 500;
unsigned offDelayRed = 500;
unsigned onDelayGreen = 500;
unsigned offDelayGreen = 500;
unsigned onDelayBlue = 500;
unsigned offDelayBlue = 500;
