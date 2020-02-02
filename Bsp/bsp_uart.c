#include "bsp_uart.h"


uint8_t g_TxCounter = 0; 				// �������ݸ���
uint8_t TxBuffer[TxBUFFER_SIZE] = {0};	// �����ݴ滺����

EVAL_COMx_TypeDef Usart1,Usart2;

/************************************************
�������� �� UART1_Config
��    �� �� UART1�˿�����
��    �� �� Baudrate ---- ������
�� �� ֵ �� ��
*************************************************/
void UART1_Config( uint32_t Baudrate )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	/* config GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = Baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(EVAL_COM1, &USART_InitStructure);

	USART_ITConfig(EVAL_COM1, USART_IT_RXNE, ENABLE);
    USART_Cmd(EVAL_COM1, ENABLE);
}

/************************************************
�������� �� UART2_Config
��    �� �� UART2�˿�����
��    �� �� Baudrate ---- ������
�� �� ֵ �� ��
*************************************************/
void UART2_Config( uint32_t Baudrate )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	/* config GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* config USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* USART2 GPIO config */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* USART2 mode config */
    USART_InitStructure.USART_BaudRate = Baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(EVAL_COM2, &USART_InitStructure);

	USART_ITConfig(EVAL_COM2, USART_IT_RXNE, ENABLE);
    USART_Cmd(EVAL_COM2, ENABLE);
}

/************************************************
�������� �� USART_SendByte
��    �� �� �����ַ�����
��    �� �� c ---- ���͵�����
�� �� ֵ �� ��
*************************************************/
void USART_SendByte( USART_TypeDef* USARTx, uint8_t c )
{     
	USART_SendData(USARTx, c);
	
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/************************************************
�������� �� USART_SendString
��    �� �� �����ַ�������
��    �� �� USARTx ---- ����
			pData ---- �ַ���
			Length ---- ����
�� �� ֵ �� ��
*************************************************/
void USART_SendString( USART_TypeDef* USARTx, const uint8_t *pData, uint16_t Length )
{
    while(Length--)
    {
        USART_SendByte(USARTx, *pData);
        pData++;
    }
}

/************************************************
�������� �� USART_Printf
��    �� �� ���ڴ�ӡ���
��    �� �� USARTx ---- ����
			String	---- �ַ���
�� �� ֵ �� ��
*************************************************/
void USART_Printf( USART_TypeDef* USARTx, char *String )
{
    do
    {
        USART_SendByte(USARTx, *String);
        String++;
    }while((*String) != '\0');
}


/************************************************
�������� �� fputc
��    �� �� �ض��� c�⺯�� printf�� DEBUG_UART
��    �� �� ch
�� �� ֵ �� ��
*************************************************/
int fputc(int ch, FILE *f)
{
    /* ����һ���ֽ����ݵ� DEBUG_UART */
    USART_SendData(DEBUG_UART, (uint8_t) ch);

    /* �ȴ�������� */
    while (USART_GetFlagStatus(DEBUG_UART, USART_FLAG_TXE) == RESET);

    return (ch);
}

/************************************************
�������� �� fgetc
��    �� �� �ض��� c�⺯�� scanf�� DEBUG_UART
��    �� �� f ---- �ļ�
�� �� ֵ �� ��
*************************************************/
int fgetc(FILE *f)
{
    /* �ȴ� DEBUG_UART�������� */
    while (USART_GetFlagStatus(DEBUG_UART, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_UART);
}


/************************************************************************/
/*            STM32F10x USART Interrupt Handlers                        */
/************************************************************************/

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(EVAL_COM1, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        Usart1.RxBuffer[Usart1.RxCounter++] = (USART_ReceiveData(EVAL_COM1) & 0x7F);

		if(Usart1.RxCounter >= RxBUFFER_SIZE)
		{
//			/* Disable the EVAL_COM1 Receive interrupt */
//			USART_ITConfig(EVAL_COM1, USART_IT_RXNE, DISABLE);

			Usart1.Frame_flag = 0;
			Usart1.RxCounter = 0;
			Usart1.Receiving_Time = 0;
		}
		Usart1.Receiving_Time = 2;
    }

//  if(USART_GetITStatus(EVAL_COM1, USART_IT_TXE) != RESET)
//  {
//    /* Write one byte to the transmit data register */
//    USART_SendData(EVAL_COM1, TxBuffer[TxCounter++]);

//    if(TxCounter == RxBUFFER_SIZE)
//    {
//      /* Disable the EVAL_COM1 Transmit interrupt */
//      USART_ITConfig(EVAL_COM1, USART_IT_TXE, DISABLE);
//    }
//  }
}

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(EVAL_COM2, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        Usart2.RxBuffer[Usart2.RxCounter++] = (USART_ReceiveData(EVAL_COM2) & 0x7F);
		
        if(Usart2.RxCounter >= RxBUFFER_SIZE)
        {
//			/* Disable the EVAL_COM1 Receive interrupt */
//			USART_ITConfig(EVAL_COM2, USART_IT_RXNE, DISABLE);

			Usart2.Frame_flag = 0;
            Usart2.RxCounter = 0;
			Usart2.Receiving_Time = 0;
        }
		Usart2.Receiving_Time = 2;
    }

//  if(USART_GetITStatus(EVAL_COM2, USART_IT_TXE) != RESET)
//  {
//    /* Write one byte to the transmit data register */
//    USART_SendData(EVAL_COM2, TxBuffer[TxCounter++]);

//    if(TxCounter == RxBUFFER_SIZE)
//    {
//      /* Disable the EVAL_COM2 Transmit interrupt */
//      USART_ITConfig(EVAL_COM2, USART_IT_TXE, DISABLE);
//    }
//  }
}


/*---------------------------- END OF FILE ----------------------------*/


