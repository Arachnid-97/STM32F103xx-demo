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
�������� �� SysTick_Init
��    �� �� ����ϵͳ�δ�ʱ�� SysTick
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemCoreClock / 1000000))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while(1);
	}
	
	// �رյδ�ʱ��  
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	
}

/************************************************
�������� �� Delay_us
��    �� �� us��ʱ����,1usΪһ����λ
��    �� �� nTime ---- ʱ�����
�� �� ֵ �� ��
*************************************************/
void Delay_us( __IO u32 nTime )
{ 
	TimingDelay = nTime;	

	// ʹ�ܵδ�ʱ��  
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while( TimingDelay != 0 );
	
	// �رյδ�ʱ��  
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	
}

/************************************************
�������� �� TimingDelay_Decrement
��    �� �� ��ȡ���ĳ���
��    �� �� ��
�� �� ֵ �� ��
ע    �� �� �� SysTick �жϺ��� SysTick_Handler()����
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
    __set_FAULTMASK(1); 	// �ر������ж�
    NVIC_SystemReset(); 	// ��λ
	while(1);
}


/*---------------------------- END OF FILE ----------------------------*/


