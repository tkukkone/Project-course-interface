#include "stm32f10x.h"

#ifndef RGB_CONTROL
#define RGB_CONTROL

void Red (void const *dutyCycle);
void Blue (void const *dutyCycle);
void Green (void const *dutyCycle);
void AutoReset (void) ;
#endif //DELAY_MICROSECONDS

