#ifndef __BSP_TIME_H
#define __BSP_TIME_H


#include "stm32f10x.h"

typedef struct
{
	uint16_t Time_ms;
	uint16_t Time_s;
}Time_TypeDef;
extern Time_TypeDef Time2;

void Timer2_Config(void);


#endif	/* __BSP_TIME_H */


/*---------------------------- END OF FILE ----------------------------*/

