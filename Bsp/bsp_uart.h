#ifndef __BSP_UART_H
#define __BSP_UART_H


#include <stdio.h>
#include "stm32f10x.h"

#define DEBUG_UART			USART1
#define EVAL_COM1			USART1
#define EVAL_COM2			USART2

#define TxBUFFER_SIZE   	100
#define RxBUFFER_SIZE   	0xFF

#define _DEBUG				1
#define DEBUG_PRINTF(fmt,arg...)			do{\
											if(_DEBUG)\
												printf("<<-DEBUG DATA->> "fmt"\n",##arg);\
											}while(0)

typedef struct
{
	uint8_t RxBuffer[RxBUFFER_SIZE];		// 接收暂存缓冲区
	__IO uint8_t RxCounter;					// 接收数据个数
	uint8_t Receiving_Time;					// 接收时间
	uint8_t Frame_flag;						// 一帧完成标志
}EVAL_COMx_TypeDef;
extern EVAL_COMx_TypeDef Usart1,Usart2;

extern uint8_t g_TxCounter;
extern uint8_t TxBuffer[TxBUFFER_SIZE];

void UART1_Config(void);
void UART2_Config(void);
void USART_SendByte( USART_TypeDef* USARTx, uint8_t c );
void USART_SendString( USART_TypeDef* USARTx, const uint8_t *pData, uint16_t Length );
void USART_Printf( USART_TypeDef* USARTx, char *String );


#endif	/* __BSP_UART_H */


/*---------------------------- END OF FILE ----------------------------*/


