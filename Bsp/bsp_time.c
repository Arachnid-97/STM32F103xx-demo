#include "bsp_time.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"


Time_TypeDef Time2;

/************************************************
函数名称 ： Timer2_Config
功    能 ： Timer2配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Timer2_Config(void)
{
	uint16_t PrescalerValue = 0;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t)(SystemCoreClock / 1000000) - 1;		// us
	
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* Clear TIM2 update flag */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	/* TIM2 IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/* Time2 counter enable */
	TIM_Cmd(TIM2, ENABLE);
}

/************************************************************************/
/*            STM32F10x Time Interrupt Handlers                         */
/************************************************************************/

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		
		Time2.Time_ms++;
		if(1000 == Time2.Time_ms)
		{
			Time2.Time_ms = 0;
			Time2.Time_s++;
			if(Time2.Time_s > 60)
			{
				Time2.Time_s = 0;
			}
		}
		else if(0 == Time2.Time_ms % 50)
		{
			if(Usart1.Receiving_Time)
			{
				Usart1.Receiving_Time--;
				if(!Usart1.Receiving_Time)
					Usart1.Frame_flag = 1;
			}
			if(Usart2.Receiving_Time)
			{
				Usart2.Receiving_Time--;
				if(!Usart2.Receiving_Time)
					Usart2.Frame_flag = 1;
			}
		}
	}		 	
}


/*---------------------------- END OF FILE ----------------------------*/


