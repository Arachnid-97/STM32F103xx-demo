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
#include "w25qxx.h"
#include "at24cxx.h"
#include "user_sdcard.h"
#include "bsp_rtc.h"
#include "user_fatfs.h"

#include "../lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "../demo/demo.h"
#include "../lv_test_theme/lv_test_theme_1.h"

/* Private variables ---------------------------------------------------------*/
struct rtc_time RTC_Time = {
0,38,20,8,6,2,2020
};

/* Private functions ---------------------------------------------------------*/

/************************************************
函数名称 ： System_Starts
功    能 ： 系统启动
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void System_Starts(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SysTick_Init();
	Timer2_Config();
	UART1_Config();
//	UART2_Config();
	LED_Config();
//	Key_Config();
//	ADCx_Init();
	W25Qxx_Init();
	AT24Cxx_Init();
//	RTC_Init(&RTC_Time);
}

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
	System_Starts();
	
	/* -------- End -------- */

	RCC_GetClocksFreq(&Rcc_clock);

//	SD_test();
//	FF_Test();

#if 0
    ILI9341_Init();         //LCD 初始化
	
	LCD_SetColors(RED,BLACK);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */

//	ILI9341_DrawRectangle(100, 50, 30, 80, 0);

#endif

	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();

	demo_create();

//	lv_theme_t * th = lv_theme_night_init(210, NULL);     //Set a HUE value and a Font for the Night Theme
//	lv_theme_t * th = lv_theme_material_init(210, NULL);

//	lv_test_theme_1(th);		//Load the test GUI
		
	/* Infinite loop */
    while (1)
    {
//		RTC_Time_Scan(RTC_GetCounter());
		
		lv_task_handler();
		Delay(8);

		if(Usart1.Frame_flag)
		{
			Usart1.Frame_flag = 0;
			USART_SendString(USART1, Usart1.RxBuffer, Usart1.RxCounter);
			Usart1.RxCounter = 0;
		}
    }
}

#if 0

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
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

	printf(">>>>> Wrong parameters value: file %s on line %d\r\n", file, line);
	
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
