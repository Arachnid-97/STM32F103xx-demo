/**
  ******************************************************************************
  * @file    ili9341.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    02-December-2014
  * @brief   This file includes the LCD driver for ILI9341 LCD.
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

/* Includes ------------------------------------------------------------------*/
#include "ili9341.h"

/** @defgroup ILI9341_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup ILI9341_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup ILI9341_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup ILI9341_Private_Variables
  * @{
  */
//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
uint16_t LCD_X_LENGTH = ILI9341_LCD_PIXEL_MORE;
uint16_t LCD_Y_LENGTH = ILI9341_LCD_PIXEL_LESS;

//液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
//参数可选值为0-7
//调用ILI9341_GramScan函数设置方向时会自动更改
//LCD刚初始化完成时会使用本默认值
uint8_t LCD_SCAN_MODE = 6;
/**
  * @}
  */

/** @defgroup ILI9341_Private_FunctionPrototypes
  * @{
  */
static void ILI9341_Register_Config ( void );
static void ILI9341_GPIO_Config ( void );
static void ILI9341_FSMC_Config ( void );
static void ILI9341_Delay ( __IO uint32_t nCount );

static __INLINE void ILI9341_WriteReg(uint16_t LCD_Reg);
static __INLINE void ILI9341_WriteData(uint16_t RegValue);
static __INLINE uint16_t ILI9341_ReadData(void);

/**
  * @}
  */

/**
 * @brief  在 ILI9341显示器上开辟一个操作区域
 * @param  x1 ：在特定扫描方向下窗口的起点X坐标
 * @param  y1 ：在特定扫描方向下窗口的起点Y坐标
 * @param  x2 ：在特定扫描方向下窗口的终点X坐标
 * @param  y2 ：在特定扫描方向下窗口的终点Y坐标
 * @retval 无
 */
__INLINE void ILI9341_SetAddressWindow( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ) {
    ILI9341_WriteReg ( LCD_COLUMN_ADDR );				/* 设置X坐标 */
    ILI9341_WriteData ( x1 >> 8 );	 					/* 先高8位，然后低8位 */
    ILI9341_WriteData ( x1 & 0xff );					/* 设置起始点和结束点*/
    ILI9341_WriteData ( (x2 - 1) >> 8 );
    ILI9341_WriteData ( (x2 - 1) & 0xff );

    ILI9341_WriteReg ( LCD_PAGE_ADDR );					/* 设置Y坐标*/
    ILI9341_WriteData ( y1 >> 8 );
    ILI9341_WriteData ( y1 & 0xff );
    ILI9341_WriteData ( (y2 - 1) >> 8 );
    ILI9341_WriteData ( (y2 - 1) & 0xff );
}

/**
 * @brief  在 ILI9341显示器上以某一颜色填充像素点
 * @param  usColor ：颜色
 * @param  ulAmout_Point ：要填充颜色的像素点的总数目
 * @retval 无
 */
__INLINE void ILI9341_FillColor ( uint16_t usColor, uint32_t ulAmout_Point ) {
    uint32_t i = 0;

    /* memory write */
    ILI9341_WriteReg ( LCD_GRAM );

    for ( i = 0; i < ulAmout_Point; i ++ )
        ILI9341_WriteData ( usColor );
}

/**
 * @brief  读取 ILI9341 GRAN 的一个像素数据
 * @param  无
 * @retval 像素数据
 */
__INLINE uint16_t ILI9341_ReadPixelData ( void ) {
    uint16_t usR=0, usG=0, usB=0 ;

    ILI9341_WriteReg ( LCD_RAMRD );   /* 读数据 */

    usR = ILI9341_ReadData (); 	/* FIRST READ OUT DUMMY DATA */

    usR = ILI9341_ReadData ();  	/* READ OUT RED DATA */
    usB = ILI9341_ReadData ();  	/* READ OUT BLUE DATA */
    usG = ILI9341_ReadData ();  	/* READ OUT GREEN DATA */

    return ( ( ( usR >> 11 ) << 11 ) | ( ( usG >> 10 ) << 5 ) | ( usB >> 11 ) );
}

#if 0
/**
 * @brief  对 ILI9341显示器的某一点以某种颜色进行填充
 * @param  usX ：在特定扫描方向下该点的X坐标
 * @param  usY ：在特定扫描方向下该点的Y坐标
 * @param  usColor ：颜色
 * @retval 无
 */
__INLINE void ILI9341_SetPointPixel ( uint16_t usX, uint16_t usY, uint16_t usColor ) {
    ILI9341_WriteReg ( LCD_COLUMN_ADDR );				/* 设置X坐标 */
    ILI9341_WriteData ( usX >> 8 );	 					/* 先高8位，然后低8位 */
    ILI9341_WriteData ( usX & 0xff );					/* 设置起始点和结束点*/
    ILI9341_WriteData ( usX >> 8 );
    ILI9341_WriteData ( usX & 0xff );

    ILI9341_WriteReg ( LCD_PAGE_ADDR );					/* 设置Y坐标*/
    ILI9341_WriteData ( usY >> 8 );
    ILI9341_WriteData ( usY & 0xff );
    ILI9341_WriteData ( usY >> 8 );
    ILI9341_WriteData ( usY & 0xff );
	
	ILI9341_WriteReg ( LCD_GRAM );
	ILI9341_WriteData ( usColor );
}

#endif

/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void ILI9341_Init(void) {
	/* Initialize ILI9341 low level bus layer */
    ILI9341_GPIO_Config ();
    ILI9341_FSMC_Config ();

    ILI9341_Reset ();
    ILI9341_Register_Config ();

    //设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向
    ILI9341_GramScan(5);
	
    ILI9341_BackLed_Control ( ENABLE );      //点亮LCD背光灯
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void ILI9341_DisplayOn(void) {
	/* Display On */
	ILI9341_WriteReg(LCD_DISPLAY_ON);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void ILI9341_DisplayOff(void) {
	/* Display Off */
	ILI9341_WriteReg(LCD_DISPLAY_OFF);
}

/**
  * @brief  Writes  to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void ILI9341_WriteReg(uint16_t LCD_Reg) {
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_CMD ) = LCD_Reg;
}

/**
  * @brief  Writes data to the selected LCD register.
  * @param  DataValue: data of the selected register.
  * @retval None
  */
void ILI9341_WriteData(uint16_t DataValue) {
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) = DataValue;
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  None
  * @retval LCD Register Value.
  */
uint16_t ILI9341_ReadData(void) {
	return ( * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) );
}

/**
 * @brief  设置ILI9341的GRAM的扫描方向
 * @param  ucOption ：选择GRAM的扫描方向
 *     @arg 0-7 :参数可选值为0-7这八个方向
 *
 *	！！！其中0、3、5、6 模式适合从左至右显示文字，
 *				不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果
 *
 *	其中0、2、4、6 模式的X方向像素为240，Y方向像素为320
 *	其中1、3、5、7 模式下X方向像素为320，Y方向像素为240
 *
 *	其中 6 模式为大部分液晶例程的默认显示方向
 *	其中 3 模式为摄像头例程使用的方向
 *	其中 0 模式为BMP图片显示例程使用的方向
 *
 * @retval 无
 * @note  坐标图例：A表示向上，V表示向下，<表示向左，>表示向右
					X表示X轴，Y表示Y轴

------------------------------------------------------------
	模式0：		.	模式1：		.	模式2：		.	模式3：
			A	.			A	.	A			.	A
			|	.			|	.	|			.	|
			Y	.			X	.	Y			.	X
			0	.			1	.	2			.	3
	<--- X0 o	.	<----Y1	o	.	o 2X--->	.	o 3Y--->
------------------------------------------------------------
	模式4：		.	模式5：		.	模式6：		.	模式7：
	<--- X4 o	.	<--- Y5 o	.	o 6X--->	.	o 7Y--->
			4	.			5	.	6			.	7
			Y	.			X	.	Y			.	X
			|	.			|	.	|			.	|
			V	.			V	.	V			.	V
---------------------------------------------------------
				  LCD屏示例
			|-------------------|
			|	   			  	|
			|					|
			|					|
			|					|
			|					|
			|					|
			|					|
			|					|
			|					|
			|-------------------|
		   屏幕正面（宽240，高320）

 *******************************************************/
void ILI9341_GramScan ( uint8_t ucOption ) {
    //参数检查，只可输入0-7
    if(ucOption >7 )
        return;

    //根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
//    LCD_SCAN_MODE = ucOption;

    //根据模式更新XY方向的像素宽度
#if 1
	if(ucOption%2 == 0)	
	{
		//0 2 4 6模式下X方向为竖屏
		LCD_X_LENGTH = ILI9341_LCD_PIXEL_LESS;
		LCD_Y_LENGTH = ILI9341_LCD_PIXEL_MORE;
	}
	else				
	{
		//1 3 5 7模式下X方向为横屏
		LCD_X_LENGTH = ILI9341_LCD_PIXEL_MORE;
		LCD_Y_LENGTH = ILI9341_LCD_PIXEL_LESS; 
	}
#else
    if(ucOption < 4)
    {
        //0 1 2 3模式下为横屏
        LCD_X_LENGTH = ILI9341_LCD_PIXEL_MORE;
        LCD_Y_LENGTH = ILI9341_LCD_PIXEL_LESS;
    }
    else
    {
        //4 5 6 7模式下为竖屏
        LCD_X_LENGTH = ILI9341_LCD_PIXEL_LESS;
        LCD_Y_LENGTH = ILI9341_LCD_PIXEL_MORE;
    }
#endif
	
    //0x36命令参数的高3位可用于设置GRAM扫描方向
    ILI9341_WriteReg ( LCD_MAC );
    ILI9341_WriteData ( 0x08 |(ucOption<<5));//根据ucOption的值设置LCD参数，共0-7种模式
    ILI9341_WriteReg ( LCD_COLUMN_ADDR );
    ILI9341_WriteData ( ILI9341_DispWindow_X_Star );	/* x 起始坐标高8位 */
    ILI9341_WriteData ( ILI9341_DispWindow_X_Star );	/* x 起始坐标低8位 */
    ILI9341_WriteData ( ((LCD_X_LENGTH-1)>>8)&0xFF );	/* x 结束坐标高8位 */
    ILI9341_WriteData ( (LCD_X_LENGTH-1)&0xFF );		/* x 结束坐标低8位 */

    ILI9341_WriteReg ( LCD_PAGE_ADDR );
    ILI9341_WriteData ( ILI9341_DispWindow_Y_Star );	/* y 起始坐标高8位 */
    ILI9341_WriteData ( ILI9341_DispWindow_Y_Star );	/* y 起始坐标低8位 */
    ILI9341_WriteData ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y 结束坐标高8位 */
    ILI9341_WriteData ( (LCD_Y_LENGTH-1)&0xFF );		/* y 结束坐标低8位 */

    /* GRAM start writing */
    ILI9341_WriteReg ( LCD_GRAM );
}

/**
 * @brief  ILI9341背光LED控制
 * @param  enumState ：决定是否使能背光LED
  *   该参数为以下值之一：
  *     @arg ENABLE :使能背光LED
  *     @arg DISABLE :禁用背光LED
 * @retval 无
 */
void ILI9341_BackLed_Control ( FunctionalState enumState ) {
    if ( enumState )
        GPIO_ResetBits ( ILI9341_BK_PORT, ILI9341_BK_PIN );
    else
        GPIO_SetBits ( ILI9341_BK_PORT, ILI9341_BK_PIN );
}

/**
 * @brief  ILI9341 软件复位
 * @param  无
 * @retval 无
 */
void ILI9341_Reset ( void ) {
    GPIO_ResetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );	 //低电平复位
    ILI9341_Delay ( 0xAFF );

    GPIO_SetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );
    ILI9341_Delay ( 0xAFF );
}

/** @defgroup ILI9341_Private_Functions
  * @{
  */

/**
  * @brief  用于 ILI9341 简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */
static void ILI9341_Delay ( __IO uint32_t nCount ) {
    for ( ; nCount != 0; nCount -- );
}

/**
 * @brief  初始化ILI9341寄存器
 * @param  无
 * @retval 无
 */
static void ILI9341_Register_Config ( void ) {
	
#if 1
	/* Configure LCD */
//	ILI9341_WriteReg(0xCA);
//	ILI9341_WriteData(0xC3);
//	ILI9341_WriteData(0x08);
//	ILI9341_WriteData(0x50);

	/*  Power control B (CFh)  */
	ILI9341_WriteReg(LCD_POWERB);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x81);
	ILI9341_WriteData(0x30);
	/*  Power on sequence control (EDh) */
	ILI9341_WriteReg(LCD_POWER_SEQ);
	ILI9341_WriteData(0x64);
	ILI9341_WriteData(0x03);
	ILI9341_WriteData(0x12);
	ILI9341_WriteData(0x81);
	/*  Driver timing control A (E8h) */
	ILI9341_WriteReg(LCD_DTCA);
	ILI9341_WriteData(0x85);
	ILI9341_WriteData(0x10);
	ILI9341_WriteData(0x78);
	/*  Power control A (CBh) */
	ILI9341_WriteReg(LCD_POWERA);
	ILI9341_WriteData(0x39);
	ILI9341_WriteData(0x2C);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x34);
	ILI9341_WriteData(0x02);
	/* Pump ratio control (F7h) */
	ILI9341_WriteReg(LCD_PRC);
	ILI9341_WriteData(0x20);
	/* Driver timing control B (EAh) */
	ILI9341_WriteReg(LCD_DTCB);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x00);
	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	ILI9341_WriteReg(LCD_FRMCTR1);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x1B);
	/*  Display Function Control (B6h) */
	ILI9341_WriteReg(LCD_DFC);
	ILI9341_WriteData(0x0A);
	ILI9341_WriteData(0xA2);
	/* Power Control 1 (C0h) */
	ILI9341_WriteReg(LCD_POWER1);
	ILI9341_WriteData(0x35);
	/* Power Control 2 (C1h) */
	ILI9341_WriteReg(LCD_POWER2);
	ILI9341_WriteData(0x11);
	/* VCOM Control 1 (C5h) */
	ILI9341_WriteReg(LCD_VCOM1);
	ILI9341_WriteData(0x45);
	ILI9341_WriteData(0x45);
	/* VCOM Control 2 (C7h)  */
	ILI9341_WriteReg(LCD_VCOM2);
	ILI9341_WriteData(0xA2);
	/* Enable 3G (F2h) */
	ILI9341_WriteReg(LCD_3GAMMA_EN);
	ILI9341_WriteData(0x00);
	/* Gamma Set (26h) */
	ILI9341_WriteReg(LCD_GAMMA);
	ILI9341_WriteData(0x01);

	/* Positive Gamma Correction (E0h) */
	ILI9341_WriteReg(LCD_PGAMMA);
	ILI9341_WriteData(0x0F);
	ILI9341_WriteData(0x26);
	ILI9341_WriteData(0x24);
	ILI9341_WriteData(0x0B);
	ILI9341_WriteData(0x0E);
	ILI9341_WriteData(0x09);
	ILI9341_WriteData(0x54);
	ILI9341_WriteData(0xA8);
	ILI9341_WriteData(0x46);
	ILI9341_WriteData(0x0C);
	ILI9341_WriteData(0x17);
	ILI9341_WriteData(0x09);
	ILI9341_WriteData(0x0F);
	ILI9341_WriteData(0x07);
	ILI9341_WriteData(0x00);
	/* Negative Gamma Correction (E1h) */
	ILI9341_WriteReg(LCD_NGAMMA);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x19);
	ILI9341_WriteData(0x1B);
	ILI9341_WriteData(0x04);
	ILI9341_WriteData(0x10);
	ILI9341_WriteData(0x07);
	ILI9341_WriteData(0x2A);
	ILI9341_WriteData(0x47);
	ILI9341_WriteData(0x39);
	ILI9341_WriteData(0x03);
	ILI9341_WriteData(0x06);
	ILI9341_WriteData(0x06);
	ILI9341_WriteData(0x30);
	ILI9341_WriteData(0x38);
	ILI9341_WriteData(0x0F);

	/* 默认为 mode 0 */
	/* memory access control (36h) */
	ILI9341_WriteReg(LCD_MAC);
	ILI9341_WriteData(0x08);

	/* Column address set */
	ILI9341_WriteReg(LCD_COLUMN_ADDR);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(((ILI9341_LCD_PIXEL_MORE-1)>>8)&0xFF);
	ILI9341_WriteData((ILI9341_LCD_PIXEL_MORE-1)&0xFF);
	/* Page address set */
	ILI9341_WriteReg(LCD_PAGE_ADDR);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(((ILI9341_LCD_PIXEL_LESS-1)>>8)&0xFF);
	ILI9341_WriteData((ILI9341_LCD_PIXEL_LESS-1)&0xFF);
	/* END */

	/*  Pixel Format Set (3Ah)  */
	ILI9341_WriteReg(LCD_PIXEL_FORMAT);
	ILI9341_WriteData(0x55);

	/* Sleep Out (11h)  */
	ILI9341_WriteReg(LCD_SLEEP_OUT);
	ILI9341_Delay(0xAFFF<<2);
	/* Display ON (29h) */
	ILI9341_WriteReg(LCD_DISPLAY_ON);

#else
    /*  Power control B (CFh)  */
    ILI9341_WriteReg ( 0xCF );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x81 );
    ILI9341_WriteData ( 0x30 );

    /*  Power on sequence control (EDh) */
     ;
    ILI9341_WriteReg ( 0xED );
    ILI9341_WriteData ( 0x64 );
    ILI9341_WriteData ( 0x03 );
    ILI9341_WriteData ( 0x12 );
    ILI9341_WriteData ( 0x81 );

    /*  Driver timing control A (E8h) */
     ;
    ILI9341_WriteReg ( 0xE8 );
    ILI9341_WriteData ( 0x85 );
    ILI9341_WriteData ( 0x10 );
    ILI9341_WriteData ( 0x78 );

    /*  Power control A (CBh) */
     ;
    ILI9341_WriteReg ( 0xCB );
    ILI9341_WriteData ( 0x39 );
    ILI9341_WriteData ( 0x2C );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x34 );
    ILI9341_WriteData ( 0x02 );

    /* Pump ratio control (F7h) */
     ;
    ILI9341_WriteReg ( 0xF7 );
    ILI9341_WriteData ( 0x20 );

    /* Driver timing control B */
     ;
    ILI9341_WriteReg ( 0xEA );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x00 );

    /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
     ;
    ILI9341_WriteReg ( 0xB1 );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x1B );

    /*  Display Function Control (B6h) */
     ;
    ILI9341_WriteReg ( 0xB6 );
    ILI9341_WriteData ( 0x0A );
    ILI9341_WriteData ( 0xA2 );

    /* Power Control 1 (C0h) */
     ;
    ILI9341_WriteReg ( 0xC0 );
    ILI9341_WriteData ( 0x35 );

    /* Power Control 2 (C1h) */
     ;
    ILI9341_WriteReg ( 0xC1 );
    ILI9341_WriteData ( 0x11 );

    /* VCOM Control 1 (C5h) */
    ILI9341_WriteReg ( 0xC5 );
    ILI9341_WriteData ( 0x45 );
    ILI9341_WriteData ( 0x45 );

    /*  VCOM Control 2 (C7h)  */
    ILI9341_WriteReg ( 0xC7 );
    ILI9341_WriteData ( 0xA2 );

    /* Enable 3G (F2h) */
    ILI9341_WriteReg ( 0xF2 );
    ILI9341_WriteData ( 0x00 );

    /* Gamma Set (26h) */
    ILI9341_WriteReg ( 0x26 );
    ILI9341_WriteData ( 0x01 );
     ;

    /* Positive Gamma Correction */
    ILI9341_WriteReg ( 0xE0 ); 	//Set Gamma
    ILI9341_WriteData ( 0x0F );
    ILI9341_WriteData ( 0x26 );
    ILI9341_WriteData ( 0x24 );
    ILI9341_WriteData ( 0x0B );
    ILI9341_WriteData ( 0x0E );
    ILI9341_WriteData ( 0x09 );
    ILI9341_WriteData ( 0x54 );
    ILI9341_WriteData ( 0xA8 );
    ILI9341_WriteData ( 0x46 );
    ILI9341_WriteData ( 0x0C );
    ILI9341_WriteData ( 0x17 );
    ILI9341_WriteData ( 0x09 );
    ILI9341_WriteData ( 0x0F );
    ILI9341_WriteData ( 0x07 );
    ILI9341_WriteData ( 0x00 );

    /* Negative Gamma Correction (E1h) */
    ILI9341_WriteReg ( 0XE1 ); 	//Set Gamma
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x19 );
    ILI9341_WriteData ( 0x1B );
    ILI9341_WriteData ( 0x04 );
    ILI9341_WriteData ( 0x10 );
    ILI9341_WriteData ( 0x07 );
    ILI9341_WriteData ( 0x2A );
    ILI9341_WriteData ( 0x47 );
    ILI9341_WriteData ( 0x39 );
    ILI9341_WriteData ( 0x03 );
    ILI9341_WriteData ( 0x06 );
    ILI9341_WriteData ( 0x06 );
    ILI9341_WriteData ( 0x30 );
    ILI9341_WriteData ( 0x38 );
    ILI9341_WriteData ( 0x0F );

    /* memory access control set */
     ;
    ILI9341_WriteReg ( 0x36 );
    ILI9341_WriteData ( 0xC8 );    /*竖屏  左上角到 (起点)到右下角 (终点)扫描方式*/
     ;

    /* column address control set */
    ILI9341_WriteReg ( LCD_COLUMN_ADDR );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0xEF );

    /* page address control set */
     ;
    ILI9341_WriteReg ( LCD_PAGE_ADDR );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x00 );
    ILI9341_WriteData ( 0x01 );
    ILI9341_WriteData ( 0x3F );

    /*  Pixel Format Set (3Ah)  */
     ;
    ILI9341_WriteReg ( 0x3a );
    ILI9341_WriteData ( 0x55 );

    /* Sleep Out (11h)  */
    ILI9341_WriteReg ( 0x11 );
    ILI9341_Delay ( 0xAFFf<<2 );
     ;

    /* Display ON (29h) */
    ILI9341_WriteReg ( 0x29 );

#endif
}

/**
  * @brief  初始化ILI9341的IO引脚
  * @param  无
  * @retval 无
  */
static void ILI9341_GPIO_Config ( void ) {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能FSMC对应相应管脚时钟*/
    RCC_APB2PeriphClockCmd (
        /*控制信号*/
        ILI9341_CS_CLK|ILI9341_DC_CLK|ILI9341_WR_CLK|
        ILI9341_RD_CLK	|ILI9341_BK_CLK|ILI9341_RST_CLK|
        /*数据信号*/
        ILI9341_D0_CLK|ILI9341_D1_CLK|	ILI9341_D2_CLK |
        ILI9341_D3_CLK | ILI9341_D4_CLK|ILI9341_D5_CLK|
        ILI9341_D6_CLK | ILI9341_D7_CLK|ILI9341_D8_CLK|
        ILI9341_D9_CLK | ILI9341_D10_CLK|ILI9341_D11_CLK|
        ILI9341_D12_CLK | ILI9341_D13_CLK|ILI9341_D14_CLK|
        ILI9341_D15_CLK	, ENABLE );

    /* 配置FSMC相对应的数据线,FSMC-D0~D15 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = ILI9341_D0_PIN;
    GPIO_Init ( ILI9341_D0_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D1_PIN;
    GPIO_Init ( ILI9341_D1_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D2_PIN;
    GPIO_Init ( ILI9341_D2_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D3_PIN;
    GPIO_Init ( ILI9341_D3_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D4_PIN;
    GPIO_Init ( ILI9341_D4_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D5_PIN;
    GPIO_Init ( ILI9341_D5_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D6_PIN;
    GPIO_Init ( ILI9341_D6_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D7_PIN;
    GPIO_Init ( ILI9341_D7_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D8_PIN;
    GPIO_Init ( ILI9341_D8_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D9_PIN;
    GPIO_Init ( ILI9341_D9_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D10_PIN;
    GPIO_Init ( ILI9341_D10_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D11_PIN;
    GPIO_Init ( ILI9341_D11_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D12_PIN;
    GPIO_Init ( ILI9341_D12_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D13_PIN;
    GPIO_Init ( ILI9341_D13_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D14_PIN;
    GPIO_Init ( ILI9341_D14_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_D15_PIN;
    GPIO_Init ( ILI9341_D15_PORT, & GPIO_InitStructure );

    /* 配置FSMC相对应的控制线
     * FSMC_NOE   :LCD-RD
     * FSMC_NWE   :LCD-WR
     * FSMC_NE1   :LCD-CS
     * FSMC_A16   :LCD-DC
     */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN;
    GPIO_Init (ILI9341_RD_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN;
    GPIO_Init (ILI9341_WR_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN;
    GPIO_Init ( ILI9341_CS_PORT, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN;
    GPIO_Init ( ILI9341_DC_PORT, & GPIO_InitStructure );

    /* 配置LCD复位RST控制管脚*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
    GPIO_Init ( ILI9341_RST_PORT, & GPIO_InitStructure );

    /* 配置LCD背光控制管脚BK*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN;
    GPIO_Init ( ILI9341_BK_PORT, & GPIO_InitStructure );
}


/**
 * @brief  LCD  FSMC 模式配置
 * @param  无
 * @retval 无
 */
static void ILI9341_FSMC_Config ( void ) {
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

    /* 使能FSMC时钟*/
    RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

    //地址建立时间（ADDSET）为1个HCLK 2/72M=28ns
    readWriteTiming.FSMC_AddressSetupTime      = 0x01;	 //地址建立时间
    //数据保持时间（DATAST）+ 1个HCLK = 5/72M=70ns
    readWriteTiming.FSMC_DataSetupTime         = 0x04;	 //数据建立时间
    //选择控制的模式
    //模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配
    readWriteTiming.FSMC_AccessMode            = FSMC_AccessMode_B;

    /*以下配置与模式B无关*/
    //地址保持时间（ADDHLD）模式A未用到
    readWriteTiming.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
    //设置总线转换周期，仅用于复用模式的NOR操作
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    //设置时钟分频，仅用于同步类型的存储器
    readWriteTiming.FSMC_CLKDivision           = 0x00;
    //数据保持时间，仅用于同步型的NOR
    readWriteTiming.FSMC_DataLatency           = 0x00;

    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait      = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming;

    FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure );

    /* 使能 FSMC_Bank1_NORSRAM4 */
    FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAMx, ENABLE );
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
