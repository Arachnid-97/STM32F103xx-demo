#ifndef __BSP_H
#define __BSP_H


#include "stm32f10x.h"

void SysTick_Init(void);
void Delay_us( __IO u32 nTime );
void TimingDelay_Decrement(void);
void SystemSoftReset(void);
void Get_ChipID(void);


#endif	/* __BSP_H */


/*---------------------------- END OF FILE ----------------------------*/

