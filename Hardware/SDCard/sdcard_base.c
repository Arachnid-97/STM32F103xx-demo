#include "./SDCard/sdcard_base.h"
#include "./SDCard/stm32_eval_sdio_sd.h"


/************************************************
函数名称 ： SD_DMAEndOfTransferStatus
功    能 ： SD DMA结束传输状态
参    数 ： 无
返 回 值 ： 无
*************************************************/
uint32_t SD_DMAEndOfTransferStatus(void)
{
    return (uint32_t)DMA_GetFlagStatus(DMA2_FLAG_TC4);
}

/************************************************
函数名称 ： SD_LowLevel_DMA_TxConfig
功    能 ： 配置 SDIO DMA发送请求
参    数 ： BufferDST ---- 指向目标缓冲区的指针
			BufferSize ---- 缓冲区大小
返 回 值 ： 无
*************************************************/
void SD_LowLevel_DMA_TxConfig( uint32_t *BufferSRC, uint32_t BufferSize )
{

    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

    /*!< DMA2 Channel4 disable */
    DMA_Cmd(DMA2_Channel4, DISABLE);

    /*!< DMA2 Channel4 Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferSRC;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    /*!< DMA2 Channel4 enable */
    DMA_Cmd(DMA2_Channel4, ENABLE);
}

/************************************************
函数名称 ： SD_LowLevel_DMA_RxConfig
功    能 ： 配置 SDIO DMA接收请求
参    数 ： BufferDST ---- 指向目标缓冲区的指针
			BufferSize ---- 缓冲区大小
返 回 值 ： 无
*************************************************/
void SD_LowLevel_DMA_RxConfig( uint32_t *BufferDST, uint32_t BufferSize )
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

    /*!< DMA2 Channel4 disable */
    DMA_Cmd(DMA2_Channel4, DISABLE);

    /*!< DMA2 Channel4 Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferDST;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    /*!< DMA2 Channel4 enable */
    DMA_Cmd(DMA2_Channel4, ENABLE);
}

/************************************************
函数名称 ： SD_LowLevel_Init
功    能 ： SD底层初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void SD_LowLevel_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    /*!< Dx /CLK and CMD clock enable */
    SD_SDIO_IO_APBxClock_FUN(SD_SDIO_Dx_CLK | SD_SDIO_CLK_CLK | SD_SDIO_CMD_CLK, ENABLE);
	
#if _SD_SPECCY
	/*!< sd detect clock enable */
	SD_DETECT_APBxClock_FUN(SD_DETECT_GPIO_CLK, ENABLE);

#endif /* _SD_SPECCY */
	
    /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
    GPIO_InitStructure.GPIO_Pin = SD_SDIO_D0_PINS | SD_SDIO_D1_PINS | SD_SDIO_D2_PINS \
									| SD_SDIO_D3_PINS | SD_SDIO_CLK_PINS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SD_SDIO_IO_PORT, &GPIO_InitStructure);

    /*!< Configure PD.02 CMD line */
    GPIO_InitStructure.GPIO_Pin = SD_SDIO_CMD_PINS;
    GPIO_Init(SD_SDIO_CMD_PORT, &GPIO_InitStructure);

#if _SD_SPECCY
    /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
    GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

#endif /* _SD_SPECCY */

    /* Enable the SDIO AHB Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);

    /* Enable the DMA2 Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/************************************************
函数名称 ： SD_LowLevel_DeInit
功    能 ： SD底层复位
参    数 ： 无
返 回 值 ： 无
*************************************************/
void SD_LowLevel_DeInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /*!< Disable SDIO Clock */
    SDIO_ClockCmd(DISABLE);

    /*!< Set Power State to OFF */
    SDIO_SetPowerState(SDIO_PowerState_OFF);

    /*!< DeInitializes the SDIO peripheral */
    SDIO_DeInit();

    /*!< Disable the SDIO AHB Clock */
    SD_SDIO_APBxClock_FUN(RCC_AHBPeriph_SDIO, DISABLE);

    /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
    GPIO_InitStructure.GPIO_Pin = SD_SDIO_D0_PINS | SD_SDIO_D1_PINS | SD_SDIO_D2_PINS \
									| SD_SDIO_D3_PINS | SD_SDIO_CLK_PINS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SD_SDIO_IO_PORT, &GPIO_InitStructure);

    /*!< Configure PD.02 CMD line */
    GPIO_InitStructure.GPIO_Pin = SD_SDIO_CMD_PINS;
    GPIO_Init(SD_SDIO_CMD_PORT, &GPIO_InitStructure);
}


/************************************************************************/
/*            STM32F10x SDIO Interrupt Handlers                         */
/************************************************************************/

/**
  * @brief  This function handles SDIO global interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}


/*---------------------------- END OF FILE ----------------------------*/


