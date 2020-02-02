#include "bsp.h"


static __IO u32 TimingDelay = 0;

/* Setup SysTick Timer for 1 msec interrupts.
 ------------------------------------------
1. The SysTick_Config() function is a CMSIS function which configure:
   - The SysTick Reload register with value passed as function parameter.
   - Configure the SysTick IRQ priority to the lowest value (0x0F).
   - Reset the SysTick Counter register.
   - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
   - Enable the SysTick Interrupt.
   - Start the SysTick Counter.

2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
   SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
   SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
   inside the misc.c file.

3. You can change the SysTick IRQ priority by calling the
   NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function 
   call. The NVIC_SetPriority() is defined inside the core_cm3.h file.

4. To adjust the SysTick time base, use the following formula:
						
	 Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)

   - Reload Value is the parameter to be passed for SysTick_Config() function
   - Reload Value should not exceed 0xFFFFFF
*/

/************************************************
函数名称 ： SysTick_Init
功    能 ： 启动系统滴答定时器 SysTick
参    数 ： 无
返 回 值 ： 无
*************************************************/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (SysTick_Config(SystemCoreClock / 1000000))	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		while(1);
	}
	
	// 关闭滴答定时器  
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	
}

/************************************************
函数名称 ： Delay_us
功    能 ： us延时程序,1us为一个单位
参    数 ： nTime ---- 时间次数
返 回 值 ： 无
*************************************************/
void Delay_us( __IO u32 nTime )
{ 
	TimingDelay = nTime;	

	// 使能滴答定时器  
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while( TimingDelay != 0 );
	
	// 关闭滴答定时器  
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	
}

/************************************************
函数名称 ： TimingDelay_Decrement
功    能 ： 获取节拍程序
参    数 ： 无
返 回 值 ： 无
注    意 ： 在 SysTick 中断函数 SysTick_Handler()调用
*************************************************/
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	
}

/**
 * @brief  Reset the mcu by software
 *
 * @param  none
 *
 * @note   use the 3.5 version of the firmware library. 
 */
void SystemSoftReset(void)
{
    __set_FAULTMASK(1); 	// 关闭所有中断
    NVIC_SystemReset(); 	// 复位
	while(1);
}


/*---------------------------- END OF FILE ----------------------------*/


