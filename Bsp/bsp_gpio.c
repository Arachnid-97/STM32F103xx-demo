#include "bsp_gpio.h"


/************************************************
函数名称 ： LED_Config
功    能 ： LED配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* Configure PB.14 pin as output push pull */
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
	LED_OFF;	
}

/************************************************
函数名称 ： Key_Config
功    能 ： 按键配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Key_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* Configure PA.00 pin as input pull up */
	GPIO_InitStructure.GPIO_Pin = KEY_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
	
	/* Connect EXTI1 Line to PA.00 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable the EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Generate software interrupt: simulate a falling edge applied on EXTI0 line */
	EXTI_GenerateSWInterrupt(EXTI_Line0);
}
	

/*---------------------------- END OF FILE ----------------------------*/


