#include "bsp_adc.h"
#include "bsp_uart.h"


// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
__IO uint16_t ADC_ConvertedValue;

/************************************************
�������� �� Get_ADCx_Result
��    �� �� ADC���ݻ�ȡ
��    �� �� Ch ---- ͨ��
�� �� ֵ �� temp ---- ת�������ĵ�ѹֵ���Ŵ� 1000����
*************************************************/
float Get_ADC_Result(void)
{
	float temp;

	temp =(float) ADC_ConvertedValue / 4096*VDD_VALUE;	// ��ȡת���� ADֵ

#if 1
	printf(">>>>> ��ѹֵ:%d\r\n",(int)temp);
	
#endif

    return temp;                 
}

/************************************************
�������� �� ADCx_GPIO_Config
��    �� �� ADC GPIO����
��    �� �� ��
�� �� ֵ �� ��
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
�������� �� ADCx_Mode_Config
��    �� �� ADCģʽ����
��    �� �� ��
�� �� ֵ �� ��
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
	/* ӳ�䵽 ADC���ݼĴ�����ַ 0x4001244C */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR));	
	/* �洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı��� */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;	
	/* ����Դ�������� */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	
	/* ��������СΪ 1���������Ĵ�СӦ�õ��ڴ洢���Ĵ�С */
	DMA_InitStructure.DMA_BufferSize = 1;	
	/* ����Ĵ���ֻ��һ������ַ���õ��� */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/* �洢����ַ�̶� */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 	
	/* �������ݴ�СΪ���֣��������ֽ� */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
	/* �洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	/* ѭ������ģʽ */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	/* DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ�� DMAͨ��ʱ�����ȼ����ò�Ӱ�� */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	
	/* ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢�� */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	/* ��ʼ�� DMA */
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	
	/* Enable DMA ADC channel */
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	
	/* ADC init structure parameters values */
	/* ֻʹ��һ��ADC�����ڵ�ģʽ */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
	/* ��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 
	/* ����ת��ģʽ */
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	/* �����ⲿ����ת��������������� */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/* ת������Ҷ��� */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	/* ת��ͨ��1�� */
	ADC_InitStructure.ADC_NbrOfChannel = 1;			
	/* ��ʼ��ADC */
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
�������� �� ADCx_Init
��    �� �� �� ADC�ڳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}


/*---------------------------- END OF FILE ----------------------------*/


