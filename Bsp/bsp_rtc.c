#include "bsp_rtc.h"
#include "bsp_uart.h"


// 默认使用LSE外部时钟
#define RTC_CLOCK_SOURCE_LSI


/* 秒中断标志，进入秒中断时置 1，当时间被刷新之后清 0 */
__IO uint32_t TimeDisplay = 0;

/************************************************
函数名称 ： RTC_Time_Scan
功    能 ： 时间扫描显示
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RTC_Time_Scan( uint32_t Time )
{
	if(TimeDisplay)
	{
		TimeDisplay = 0;
	}
}

/************************************************
函数名称 ： USART_Scanf
功    能 ： 从超级终端获取数值
参    数 ： 无
返 回 值 ： 无
*************************************************/
static uint8_t USART_Scanf( uint32_t value )
{
    uint32_t index = 0;
//    uint32_t tmp[2] = {0, 0};

//    while (index < 2)
//    {
//        /* Loop until RXNE = 1 */
//        while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) == RESET)
//        {}
//        tmp[index++] = (USART_ReceiveData(EVAL_COM1));
//        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
//        {
//            printf("\n\rPlease enter valid number between 0 and 9");
//            index--;
//        }
//    }
//    /* Calculate the Corresponding value */
//    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
	
	do
	{
		printf("\n\rPlease enter valid number between 0 and %d and press enter to confirm",value);
		scanf("%d",&index);
		/* Checks */
		if (index > value)
		{
			printf("\n\r[error] Please enter valid number between 0 and %d", value);
			return 0xFF;
		}
		else
		{
			printf("\n\rCurrently configured");
			break;
		}
	}while(1);
	
    return index;
}

/************************************************
函数名称 ： Time_Regulate
功    能 ： 返回用户使用超级终端输入的时间
参    数 ： 无
返 回 值 ： 当前时间RTC计数器值
*************************************************/
static uint32_t Time_Regulate(void)
{
    uint32_t Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;

    printf("\r\n============== Time Settings ==============");

    printf("\r\n  Please Set Hours");
    while (Tmp_HH == 0xFF)
    {
        Tmp_HH = USART_Scanf(23);
    }
    printf(":  %d", Tmp_HH);

    printf("\r\n  Please Set Minutes");
    while (Tmp_MM == 0xFF)
    {
        Tmp_MM = USART_Scanf(59);
    }
    printf(":  %d", Tmp_MM);

    printf("\r\n  Please Set Seconds");
    while (Tmp_SS == 0xFF)
    {
        Tmp_SS = USART_Scanf(59);
    }
    printf(":  %d", Tmp_SS);

    /* Return the value to store in RTC counter register */
    return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/************************************************
函数名称 ： RTC_SetDate
功    能 ： RTC日期修改
参    数 ： Time ---- 读取RTC时间的结构体指针
返 回 值 ： 无
*************************************************/
void RTC_SetDate( struct rtc_time *Time )
{

}

/************************************************
函数名称 ： RTC_Time_Adjust
功    能 ： RTC时间调节
参    数 ： Time ---- 读取RTC时间的结构体指针
返 回 值 ： 无
*************************************************/
void RTC_Time_Adjust( struct rtc_time *Time )
{
	/* Disable the Usart RX Interrupt */
	USART_ITConfig(EVAL_COM1, USART_IT_RXNE, DISABLE);
	
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Change the current date */
    RTC_SetDate(Time);
    /* Change the current time */
    RTC_SetCounter(Time_Regulate());
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
	
	/* Clean Usart RX flag and Enable Interrupt */
	USART_ClearFlag(EVAL_COM1, USART_IT_RXNE);
	USART_ITConfig(EVAL_COM1, USART_IT_RXNE, ENABLE);
}

/************************************************
函数名称 ： RTC_Config
功    能 ： W25Qxx配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/************************************************
函数名称 ： RTC_Config
功    能 ： W25Qxx配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RTC_Config(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

#ifndef RTC_CLOCK_SOURCE_LSI
    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

#else

    /* Enable LSI */
    RCC_LSICmd(ENABLE);
    /* Wait till LSI is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {}

    /* Select LSI as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#endif /* RTC_CLOCK_SOURCE_LSI */

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

/************************************************
函数名称 ： W25Qxx_Init
功    能 ： W25Qxx初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RTC_Init( struct rtc_time *Time )
{
    NVIC_Config();

    if (BKP_ReadBackupRegister(BKP_DR1) != RTC_BKP_DATA)
    {
        /* Backup data register value is not correct or not yet programmed (when
           the first time the program is executed) */

        printf("\r\n\n RTC not yet configured....");

        /* RTC Configuration */
        RTC_Config();

        printf("\r\n RTC configured....");

        /* Adjust time by values entered by the user on the hyperterminal */
        RTC_Time_Adjust(Time);

        BKP_WriteBackupRegister(BKP_DR1, RTC_BKP_DATA);
    }
    else
    {
        /* Enable PWR and BKP clocks */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

#ifdef RTC_CLOCK_SOURCE_LSI
        /* Enable LSI */
        RCC_LSICmd(ENABLE);

        /* Wait till LSI is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        {}

#endif /* RTC_CLOCK_SOURCE_LSI */

        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            printf("\r\n\n Power On Reset occurred....");
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            printf("\r\n\n External Reset occurred....");
        }

        printf("\r\n No need to configure RTC....");
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();

        /* Enable the RTC Second */
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
}

/************************************************************************/
/*            STM32F10x RTC Interrupt Handlers                        */
/************************************************************************/

/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);

        /* Enable time update */
        TimeDisplay = 1;

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
}



/*---------------------------- END OF FILE ----------------------------*/


