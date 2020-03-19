#ifndef __SDCARD_BASE_H
#define __SDCARD_BASE_H


#include "stm32f10x.h"

/* 无硬件 IO检测则需要注释 */
//#define _SD_SPECCY


/**
  * @brief  SD FLASH SDIO Interface
  */
#define SD_SDIO_APBxClock_FUN(x, y)		RCC_APB2PeriphClockCmd(x, y)

#define SD_SDIO_Dx_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SD_SDIO_Dx_CLK					RCC_APB2Periph_GPIOC

#define SD_SDIO_D0_PORT					GPIOC
#define SD_SDIO_D0_PINS					GPIO_Pin_8
#define SD_SDIO_D1_PORT					GPIOC
#define SD_SDIO_D1_PINS					GPIO_Pin_9
#define SD_SDIO_D2_PORT					GPIOC
#define SD_SDIO_D2_PINS					GPIO_Pin_10
#define SD_SDIO_D3_PORT					GPIOC
#define SD_SDIO_D3_PINS					GPIO_Pin_11

#define SD_SDIO_CLK_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SD_SDIO_CLK_CLK					RCC_APB2Periph_GPIOC
#define SD_SDIO_CLK_PORT				GPIOC
#define SD_SDIO_CLK_PINS				GPIO_Pin_12

#define SD_SDIO_CMD_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SD_SDIO_CMD_CLK					RCC_APB2Periph_GPIOD
#define SD_SDIO_CMD_PORT				GPIOD
#define SD_SDIO_CMD_PINS				GPIO_Pin_2

/* 如果 IO的桥接线一样就直接用下面的宏，否则分开配置 */
#define SD_SDIO_IO_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define SD_SDIO_IO_PORT					GPIOC


/* 硬件检测 IO */
#ifdef _SD_SPECCY
	#define SD_DETECT_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
	#define SD_DETECT_PIN					GPIO_Pin_0                 /* PE.00 */
	#define SD_DETECT_GPIO_PORT				GPIOE                      /* GPIOE */
	#define SD_DETECT_GPIO_CLK				RCC_APB2Periph_GPIOE
	
#endif /* _SD_SPECCY */

#define SDIO_FIFO_ADDRESS				((uint32_t)0x40018080)
/** 
  * @brief  SDIO Intialization Frequency (400KHz max)
  */
#define SDIO_INIT_CLK_DIV				((uint8_t)0xB2)
/** 
  * @brief  SDIO Data Transfer Frequency (25MHz max) 
  */
#define SDIO_TRANSFER_CLK_DIV			((uint8_t)0x00) 

uint32_t SD_DMAEndOfTransferStatus(void);
void SD_LowLevel_DMA_TxConfig( uint32_t *BufferSRC, uint32_t BufferSize );
void SD_LowLevel_DMA_RxConfig( uint32_t *BufferDST, uint32_t BufferSize );
void SD_LowLevel_Init(void);
void SD_LowLevel_DeInit(void);


#endif	/* __SDCARD_BASE_H */


/*---------------------------- END OF FILE ----------------------------*/

