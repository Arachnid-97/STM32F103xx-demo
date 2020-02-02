/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "bsp_adc.h"


/* Private functions ---------------------------------------------------------*/

const uint32_t Baudrate_1 = 115200;		// 波特率设置	支持的波特率：115200,19200,9600,38400,57600,1200,2400,4800
const uint32_t Baudrate_2 = 115200;		// 波特率设置	支持的波特率：115200,19200,9600,38400,57600,1200,2400,4800

/************************************************
函数名称 ： main
功    能 ： 主函数入口
参    数 ： 无
返 回 值 ： 无
*************************************************/
int main(void)
{	
	RCC_ClocksTypeDef Rcc_clock;

	/* Initial Configuration */
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SysTick_Init();
	Timer2_Config();
	UART1_Config(Baudrate_1);
//	UART2_Config(Baudrate_2);
	LED_Config();
	Key_Config();
	ADCx_Init();
	
	/* -------- End -------- */
	
	RCC_GetClocksFreq(&Rcc_clock);
	
		
    /* Infinite loop */
    while (1)
    {
		if(Usart1.Frame_flag)
		{
			Usart1.Frame_flag = 0;
			USART_SendString(USART1, Usart1.RxBuffer, Usart1.RxCounter);
			Usart1.RxCounter = 0;
		}		
    }
}

#if 0
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(EVAL_COM1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
    {}

    return ch;
}

#endif

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
