#ifndef __BSP_ADC_H
#define __BSP_ADC_H


#include "stm32f10x.h"

#define VDD_VALUE	3300

// 注意：用作 ADC采集的 IO必须没有复用，否则采集电压会有影响
/******************** ADC1输入通道（引脚）配置 **************************/
#define    ADCx                          ADC1
#define    ADC_APBxClock_FUN(x, y)       RCC_APB2PeriphClockCmd(x, y)
#define    ADC_CLK                       RCC_APB2Periph_ADC1

#define    ADC_GPIO_APBxClock_FUN(x, y)  RCC_APB2PeriphClockCmd(x, y)
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC
#define    ADC_PORT                      GPIOC

// PC1-通道11 独立IO
#define    ADC_PIN                       GPIO_Pin_1
#define    ADC_CHANNEL                   ADC_Channel_11
#define    ADC_DMA_CHANNEL               DMA1_Channel1

extern __IO uint16_t ADC_ConvertedValue;

float Get_ADC_Result(void);
void ADCx_Init(void);


#endif	/* __BSP_ADC_H */


/*---------------------------- END OF FILE ----------------------------*/

