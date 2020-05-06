/**
  ******************************************************************************
  * @file    stmpe811.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    15-December-2014
  * @brief   This file contains all the functions prototypes for the
  *          stmpe811.c IO expander driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __XTP2046_H
#define __XTP2046_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f10x.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup ILI9341_Exported_Constants
  * @{
  */

/******************** XPT2046 触摸屏触摸信号指示引脚定义(不使用中断) ********************/
#define XPT2046_PENIRQ_GPIO_CLK                     RCC_APB2Periph_GPIOE
#define XPT2046_PENIRQ_GPIO_PORT                    GPIOE
#define XPT2046_PENIRQ_GPIO_PIN                     GPIO_Pin_4

//触屏信号有效电平
#define XPT2046_PENIRQ_ActiveLevel                  0
#define XPT2046_PENIRQ_Read()                       GPIO_ReadInputDataBit ( XPT2046_PENIRQ_GPIO_PORT, XPT2046_PENIRQ_GPIO_PIN )


/******************** XPT2046 触摸屏模拟SPI引脚定义 ********************/
#define XPT2046_SPI_GPIO_CLK                        RCC_APB2Periph_GPIOE| RCC_APB2Periph_GPIOD

#define XPT2046_SPI_CS_PIN		                    GPIO_Pin_13
#define XPT2046_SPI_CS_PORT		                    GPIOD

#define XPT2046_SPI_CLK_PIN	                        GPIO_Pin_0
#define XPT2046_SPI_CLK_PORT	                    GPIOE

#define XPT2046_SPI_MOSI_PIN	                    GPIO_Pin_2
#define XPT2046_SPI_MOSI_PORT	                    GPIOE

#define XPT2046_SPI_MISO_PIN	                    GPIO_Pin_3
#define XPT2046_SPI_MISO_PORT	                    GPIOE


#define XPT2046_CS_ENABLE()                         GPIO_SetBits ( XPT2046_SPI_CS_PORT, XPT2046_SPI_CS_PIN )
#define XPT2046_CS_DISABLE()                        GPIO_ResetBits ( XPT2046_SPI_CS_PORT, XPT2046_SPI_CS_PIN )

#define XPT2046_CLK_HIGH()                          GPIO_SetBits ( XPT2046_SPI_CLK_PORT, XPT2046_SPI_CLK_PIN )
#define XPT2046_CLK_LOW()                           GPIO_ResetBits ( XPT2046_SPI_CLK_PORT, XPT2046_SPI_CLK_PIN )

#define XPT2046_MOSI_1()                            GPIO_SetBits ( XPT2046_SPI_MOSI_PORT, XPT2046_SPI_MOSI_PIN )
#define XPT2046_MOSI_0()                            GPIO_ResetBits ( XPT2046_SPI_MOSI_PORT, XPT2046_SPI_MOSI_PIN )

#define XPT2046_MISO()                              GPIO_ReadInputDataBit ( XPT2046_SPI_MISO_PORT, XPT2046_SPI_MISO_PIN )


/******************************* XPT2046 触摸屏参数定义 ***************************/
//校准触摸屏时触摸坐标的AD值相差门限
#define XPT2046_THRESHOLD_CalDiff                   2

#define XPT2046_CHANNEL_X 	                        0x90 	          //通道Y+的选择控制字	
#define XPT2046_CHANNEL_Y 	                        0xD0	          //通道X+的选择控制字


/**
  * @}
  */

/** @defgroup XPT2046_Exported_Functions
  * @{
  */
void XPT2046_Init ( void );
void XPT2046_ReadAdc_XY ( volatile int16_t * sX_Ad, volatile int16_t * sY_Ad, uint8_t count_read );


#ifdef __cplusplus
}
#endif

#endif /* __XTP2046_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
