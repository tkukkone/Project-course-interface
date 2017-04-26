#include "stm32f10x.h"

#ifndef ADC_H
#define ADC_H

void ADC_Configuration(void);
	
u16 readADC1(u8 channel);

#endif //ADC_H
