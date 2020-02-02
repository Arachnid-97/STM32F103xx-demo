#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H


#include "stm32f10x.h"

#define LED_GPIO_PORT			GPIOB
#define LED_GPIO_PIN			GPIO_Pin_0

#define LED_ON					GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define LED_OFF					GPIO_SetBits(GPIOB, GPIO_Pin_0)

#define KEY_GPIO_PORT			GPIOA
#define KEY_GPIO_PIN			GPIO_Pin_0

void LED_Config(void);
void Key_Config(void);


#endif	/* __BSP_GPIO_H */


/*---------------------------- END OF FILE ----------------------------*/


