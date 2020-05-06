/**
  ******************************************************************************
  * @file    ili9341.h
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    02-December-2014
  * @brief   This file contains all the functions prototypes for the ili9341.c
  *          driver.
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
#ifndef __ILI9341_H
#define __ILI9341_H

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
  
/** @addtogroup ILI9341
  * @{
  */

/** @defgroup ILI9341_Exported_Types
  * @{
  */
/**
  * @}
  */ 

/** @defgroup ILI9341_Exported_Constants
  * @{
  */

/***************************************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM1 连接 TFT，地址范围为0X6000 0000 ~ 0X63FF FFFF
FSMC_A16 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X60000000
RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
当选择不同的地址线时，地址要重新计算  
****************************************************************************************/

/******************** ILI9341 显示屏的 FSMC 参数定义 ********************/
//FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define FSMC_Addr_ILI9341_CMD         ( ( uint32_t ) 0x60000000 )

//FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define FSMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x60020000 )

//由片选引脚决定的NOR/SRAM块
#define FSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM1

/******************** ILI9341 显示屏8080通讯引脚定义 ********************/
/* 控制信号线 */
//片选，选择NOR/SRAM块
#define ILI9341_CS_CLK                RCC_APB2Periph_GPIOD   
#define ILI9341_CS_PORT               GPIOD
#define ILI9341_CS_PIN                GPIO_Pin_7

//DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
//PD11为FSMC_A16
#define ILI9341_DC_CLK                RCC_APB2Periph_GPIOD   
#define ILI9341_DC_PORT               GPIOD
#define ILI9341_DC_PIN                GPIO_Pin_11

//写使能
#define ILI9341_WR_CLK                RCC_APB2Periph_GPIOD   
#define ILI9341_WR_PORT               GPIOD
#define ILI9341_WR_PIN                GPIO_Pin_5

//读使能
#define ILI9341_RD_CLK                RCC_APB2Periph_GPIOD   
#define ILI9341_RD_PORT               GPIOD
#define ILI9341_RD_PIN                GPIO_Pin_4

//复位引脚
#define ILI9341_RST_CLK               RCC_APB2Periph_GPIOE
#define ILI9341_RST_PORT              GPIOE
#define ILI9341_RST_PIN               GPIO_Pin_1

//背光引脚
#define ILI9341_BK_CLK                RCC_APB2Periph_GPIOD    
#define ILI9341_BK_PORT               GPIOD
#define ILI9341_BK_PIN                GPIO_Pin_12

/* 数据信号线 */
#define ILI9341_D0_CLK                RCC_APB2Periph_GPIOD   
#define ILI9341_D0_PORT               GPIOD
#define ILI9341_D0_PIN                GPIO_Pin_14

#define ILI9341_D1_CLK                RCC_APB2Periph_GPIOD   
#define ILI9341_D1_PORT               GPIOD
#define ILI9341_D1_PIN                GPIO_Pin_15

#define ILI9341_D2_CLK                RCC_APB2Periph_GPIOD   
#define ILI9341_D2_PORT               GPIOD
#define ILI9341_D2_PIN                GPIO_Pin_0

#define ILI9341_D3_CLK                RCC_APB2Periph_GPIOD  
#define ILI9341_D3_PORT               GPIOD
#define ILI9341_D3_PIN                GPIO_Pin_1

#define ILI9341_D4_CLK                RCC_APB2Periph_GPIOE   
#define ILI9341_D4_PORT               GPIOE
#define ILI9341_D4_PIN                GPIO_Pin_7

#define ILI9341_D5_CLK                RCC_APB2Periph_GPIOE   
#define ILI9341_D5_PORT               GPIOE
#define ILI9341_D5_PIN                GPIO_Pin_8

#define ILI9341_D6_CLK                RCC_APB2Periph_GPIOE   
#define ILI9341_D6_PORT               GPIOE
#define ILI9341_D6_PIN                GPIO_Pin_9

#define ILI9341_D7_CLK                RCC_APB2Periph_GPIOE  
#define ILI9341_D7_PORT               GPIOE
#define ILI9341_D7_PIN                GPIO_Pin_10

#define ILI9341_D8_CLK                RCC_APB2Periph_GPIOE   
#define ILI9341_D8_PORT               GPIOE
#define ILI9341_D8_PIN                GPIO_Pin_11

#define ILI9341_D9_CLK                RCC_APB2Periph_GPIOE   
#define ILI9341_D9_PORT               GPIOE
#define ILI9341_D9_PIN                GPIO_Pin_12

#define ILI9341_D10_CLK               RCC_APB2Periph_GPIOE   
#define ILI9341_D10_PORT              GPIOE
#define ILI9341_D10_PIN               GPIO_Pin_13

#define ILI9341_D11_CLK               RCC_APB2Periph_GPIOE   
#define ILI9341_D11_PORT              GPIOE
#define ILI9341_D11_PIN               GPIO_Pin_14

#define ILI9341_D12_CLK               RCC_APB2Periph_GPIOE   
#define ILI9341_D12_PORT              GPIOE
#define ILI9341_D12_PIN               GPIO_Pin_15

#define ILI9341_D13_CLK               RCC_APB2Periph_GPIOD   
#define ILI9341_D13_PORT              GPIOD
#define ILI9341_D13_PIN               GPIO_Pin_8

#define ILI9341_D14_CLK               RCC_APB2Periph_GPIOD   
#define ILI9341_D14_PORT              GPIOD
#define ILI9341_D14_PIN               GPIO_Pin_9

#define ILI9341_D15_CLK               RCC_APB2Periph_GPIOD   
#define ILI9341_D15_PORT              GPIOD
#define ILI9341_D15_PIN               GPIO_Pin_10

/** 
  * @brief ILI9341 chip IDs  
  */ 
#define ILI9341_ID                  		0x9341

/** 
  * @brief  ILI9341 Size  
  */  
#define ILI9341_LCD_PIXEL_MORE    			320
#define ILI9341_LCD_PIXEL_LESS   			240

/** 
  * @brief  ILI9341 Star Coords  
  */  
#define ILI9341_DispWindow_X_Star		    0     	//起始点的X坐标
#define ILI9341_DispWindow_Y_Star		    0    	//起始点的Y坐标

/** 
  * @brief  ILI9341 Timing  
  */     
/* Timing configuration  (Typical configuration from ILI9341 datasheet)
  HSYNC=10 (9+1)
  HBP=20 (29-10+1)
  ActiveW=240 (269-20-10+1)
  HFP=10 (279-240-20-10+1)

  VSYNC=2 (1+1)
  VBP=2 (3-2+1)
  ActiveH=320 (323-2-2+1)
  VFP=4 (327-320-2-2+1)
*/
#define  ILI9341_HSYNC            ((uint32_t)9)   /* Horizontal synchronization */
#define  ILI9341_HBP              ((uint32_t)29)    /* Horizontal back porch      */ 
#define  ILI9341_HFP              ((uint32_t)2)    /* Horizontal front porch     */
#define  ILI9341_VSYNC            ((uint32_t)1)   /* Vertical synchronization   */
#define  ILI9341_VBP              ((uint32_t)3)    /* Vertical back porch        */
#define  ILI9341_VFP              ((uint32_t)2)    /* Vertical front porch       */

/** 
  * @brief  ILI9341 Registers  
  */

/* Level 1 Commands */
#define LCD_SWRESET             0x01   /* Software Reset */
#define LCD_READ_DISPLAY_ID     0x04   /* Read display identification information */
#define LCD_RDDST               0x09   /* Read Display Status */
#define LCD_RDDPM               0x0A   /* Read Display Power Mode */
#define LCD_RDDMADCTL           0x0B   /* Read Display MADCTL */
#define LCD_RDDCOLMOD           0x0C   /* Read Display Pixel Format */
#define LCD_RDDIM               0x0D   /* Read Display Image Format */
#define LCD_RDDSM               0x0E   /* Read Display Signal Mode */
#define LCD_RDDSDR              0x0F   /* Read Display Self-Diagnostic Result */
#define LCD_SPLIN               0x10   /* Enter Sleep Mode */
#define LCD_SLEEP_OUT           0x11   /* Sleep out register */
#define LCD_PTLON               0x12   /* Partial Mode ON */
#define LCD_NORMAL_MODE_ON      0x13   /* Normal Display Mode ON */
#define LCD_DINVOFF             0x20   /* Display Inversion OFF */
#define LCD_DINVON              0x21   /* Display Inversion ON */
#define LCD_GAMMA               0x26   /* Gamma register */
#define LCD_DISPLAY_OFF         0x28   /* Display off register */
#define LCD_DISPLAY_ON          0x29   /* Display on register */
#define LCD_COLUMN_ADDR         0x2A   /* Column address register */ 
#define LCD_PAGE_ADDR           0x2B   /* Page address register */ 
#define LCD_GRAM                0x2C   /* GRAM register */   
#define LCD_RGBSET              0x2D   /* Color SET */   
#define LCD_RAMRD               0x2E   /* Memory Read */   
#define LCD_PLTAR               0x30   /* Partial Area */   
#define LCD_VSCRDEF             0x33   /* Vertical Scrolling Definition */   
#define LCD_TEOFF               0x34   /* Tearing Effect Line OFF */   
#define LCD_TEON                0x35   /* Tearing Effect Line ON */   
#define LCD_MAC                 0x36   /* Memory Access Control register*/
#define LCD_VSCRSADD            0x37   /* Vertical Scrolling Start Address */   
#define LCD_IDMOFF              0x38   /* Idle Mode OFF */   
#define LCD_IDMON               0x39   /* Idle Mode ON */   
#define LCD_PIXEL_FORMAT        0x3A   /* Pixel Format register */
#define LCD_WRITE_MEM_CONTINUE  0x3C   /* Write Memory Continue */   
#define LCD_READ_MEM_CONTINUE   0x3E   /* Read Memory Continue */   
#define LCD_SET_TEAR_SCANLINE   0x44   /* Set Tear Scanline */   
#define LCD_GET_SCANLINE        0x45   /* Get Scanline */   
#define LCD_WDB                 0x51   /* Write Brightness Display register */
#define LCD_RDDISBV             0x52   /* Read Display Brightness */   
#define LCD_WCD                 0x53   /* Write Control Display register*/
#define LCD_RDCTRLD             0x54   /* Read CTRL Display */   
#define LCD_WRCABC              0x55   /* Write Content Adaptive Brightness Control */   
#define LCD_RDCABC              0x56   /* Read Content Adaptive Brightness Control */   
#define LCD_WRITE_CABC          0x5E   /* Write CABC Minimum Brightness */   
#define LCD_READ_CABC           0x5F   /* Read CABC Minimum Brightness */   
#define LCD_READ_ID1            0xDA   /* Read ID1 */
#define LCD_READ_ID2            0xDB   /* Read ID2 */
#define LCD_READ_ID3            0xDC   /* Read ID3 */

/* Level 2 Commands */
#define LCD_RGB_INTERFACE       0xB0   /* RGB Interface Signal Control */
#define LCD_FRMCTR1             0xB1   /* Frame Rate Control (In Normal Mode) */
#define LCD_FRMCTR2             0xB2   /* Frame Rate Control (In Idle Mode) */
#define LCD_FRMCTR3             0xB3   /* Frame Rate Control (In Partial Mode) */
#define LCD_INVTR               0xB4   /* Display Inversion Control */
#define LCD_BPC                 0xB5   /* Blanking Porch Control register */
#define LCD_DFC                 0xB6   /* Display Function Control register */
#define LCD_ETMOD               0xB7   /* Entry Mode Set */
#define LCD_BACKLIGHT1          0xB8   /* Backlight Control 1 */
#define LCD_BACKLIGHT2          0xB9   /* Backlight Control 2 */
#define LCD_BACKLIGHT3          0xBA   /* Backlight Control 3 */
#define LCD_BACKLIGHT4          0xBB   /* Backlight Control 4 */
#define LCD_BACKLIGHT5          0xBC   /* Backlight Control 5 */
#define LCD_BACKLIGHT7          0xBE   /* Backlight Control 7 */
#define LCD_BACKLIGHT8          0xBF   /* Backlight Control 8 */
#define LCD_POWER1              0xC0   /* Power Control 1 register */
#define LCD_POWER2              0xC1   /* Power Control 2 register */
#define LCD_VCOM1               0xC5   /* VCOM Control 1 register */
#define LCD_VCOM2               0xC7   /* VCOM Control 2 register */
#define LCD_NVMWR               0xD0   /* NV Memory Write */
#define LCD_NVMPKEY             0xD1   /* NV Memory Protection Key */
#define LCD_RDNVM               0xD2   /* NV Memory Status Read */
#define LCD_READ_ID4            0xD3   /* Read ID4 */
#define LCD_PGAMMA              0xE0   /* Positive Gamma Correction register */
#define LCD_NGAMMA              0xE1   /* Negative Gamma Correction register */
#define LCD_DGAMCTRL1           0xE2   /* Digital Gamma Control 1 */
#define LCD_DGAMCTRL2           0xE3   /* Digital Gamma Control 2 */
#define LCD_INTERFACE           0xF6   /* Interface control register */

/* Extend register commands */
#define LCD_POWERA              0xCB   /* Power control A register */
#define LCD_POWERB              0xCF   /* Power control B register */
#define LCD_DTCA                0xE8   /* Driver timing control A */
#define LCD_DTCB                0xEA   /* Driver timing control B */
#define LCD_POWER_SEQ           0xED   /* Power on sequence register */
#define LCD_3GAMMA_EN           0xF2   /* 3 Gamma enable register */
#define LCD_PRC                 0xF7   /* Pump ratio control register */

/* Size of read registers */
#define LCD_READ_ID4_SIZE        3      /* Size of Read ID4 */

/**
  * @}
  */

extern uint16_t LCD_X_LENGTH;
extern uint16_t LCD_Y_LENGTH;

/** @defgroup ILI9341_Exported_Functions
  * @{
  */ 
extern __INLINE void ILI9341_SetAddressWindow( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 );
extern __INLINE void ILI9341_FillColor ( uint16_t usColor, uint32_t ulAmout_Point );
extern __INLINE uint16_t ILI9341_ReadPixelData ( void );

void ILI9341_Init(void);
void ILI9341_DisplayOn(void);
void ILI9341_DisplayOff(void);
void ILI9341_GramScan ( uint8_t ucOption );
void ILI9341_BackLed_Control ( FunctionalState enumState );
void ILI9341_Reset ( void );

      
#ifdef __cplusplus
}
#endif

#endif /* __ILI9341_H */

/**
  * @}
  */ 

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
