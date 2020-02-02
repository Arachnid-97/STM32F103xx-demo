#include "bsp_adc.h"
#include "bsp_uart.h"


// ADC1转换的电压值通过MDA方式传到SRAM
__IO uint16_t ADC_ConvertedValue;

/************************************************
函数名称 ： Get_ADCx_Result
功    能 ： ADC数据获取
参    数 ： Ch ---- 通道
返 回 值 ： temp ---- 转换计算后的电压值（放大 1000倍）
*************************************************/
float Get_ADC_Result(void)
{
	float temp;

	temp =(float) ADC_ConvertedValue / 4096*VDD_VALUE;	// 读取转换的 AD值

#if 1
	printf(">>>>> 电压值:%d\r\n",(int)temp);
	
#endif

    return temp;                 
}

/************************************************
函数名称 ： ADCx_GPIO_Config
功    能 ： ADC GPIO配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable ADCxGPIO clock */
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);
	
	/* Configure PB.00 (ADC Channel8) as analog input */
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);				
}

/************************************************
函数名称 ： ADCx_Mode_Config
功    能 ： ADC模式配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* Enable ADCx clock */
	ADC_APBxClock_FUN(ADC_CLK, ENABLE);
	
	/* Restoration DMA ADC Channel*/
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	/* DMA init structure parameters values */
	/* 映射到 ADC数据寄存器地址 0x4001244C */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR));	
	/* 存储器地址，实际上就是一个内部SRAM的变量 */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;	
	/* 数据源来自外设 */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	
	/* 缓冲区大小为 1，缓冲区的大小应该等于存储器的大小 */
	DMA_InitStructure.DMA_BufferSize = 1;	
	/* 外设寄存器只有一个，地址不用递增 */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/* 存储器地址固定 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 	
	/* 外设数据大小为半字，即两个字节 */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
	/* 存储器数据大小也为半字，跟外设数据大小相同 */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	/* 循环传输模式 */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	/* DMA 传输通道优先级为高，当使用一个 DMA通道时，优先级设置不影响 */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	
	/* 禁止存储器到存储器模式，因为是从外设到存储器 */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	/* 初始化 DMA */
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	
	/* Enable DMA ADC channel */
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	
	/* ADC init structure parameters values */
	/* 只使用一个ADC，属于单模式 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
	/* 禁止扫描模式，多通道才要，单通道不需要 */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 
	/* 连续转换模式 */
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	/* 不用外部触发转换，软件开启即可 */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/* 转换结果右对齐 */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	/* 转换通道1个 */
	ADC_InitStructure.ADC_NbrOfChannel = 1;			
	/* 初始化ADC */
	ADC_Init(ADCx, &ADC_InitStructure);
	
	/* ADCCLK = PCLK2/8 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	/* ADCx regular channel configuration */
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
	/* Enable ADCx DMA */
	ADC_DMACmd(ADCx, ENABLE);
	
	/* Enable ADCx */
	ADC_Cmd(ADCx, ENABLE);
	
	/* Enable ADCx reset calibration register */  
	ADC_ResetCalibration(ADCx);
	/* Check the end of ADCx reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADCx));
	
	/* Start ADCx calibration */
	ADC_StartCalibration(ADCx);
	/* Check the end of ADCx calibration */
	while(ADC_GetCalibrationStatus(ADCx));
	
	/* Start ADCx Software Conversion */
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

/************************************************
函数名称 ： ADCx_Init
功    能 ： 各 ADC口初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}


/*---------------------------- END OF FILE ----------------------------*/


