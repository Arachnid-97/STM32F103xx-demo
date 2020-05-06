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
//����Һ��ɨ�跽����仯��XY���ؿ��
//����ILI9341_GramScan�������÷���ʱ���Զ�����
uint16_t LCD_X_LENGTH = ILI9341_LCD_PIXEL_MORE;
uint16_t LCD_Y_LENGTH = ILI9341_LCD_PIXEL_LESS;

//Һ����ɨ��ģʽ����������Ҫ���ڷ���ѡ�������ļ������
//������ѡֵΪ0-7
//����ILI9341_GramScan�������÷���ʱ���Զ�����
//LCD�ճ�ʼ�����ʱ��ʹ�ñ�Ĭ��ֵ
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
 * @brief  �� ILI9341��ʾ���Ͽ���һ����������
 * @param  x1 �����ض�ɨ�跽���´��ڵ����X����
 * @param  y1 �����ض�ɨ�跽���´��ڵ����Y����
 * @param  x2 �����ض�ɨ�跽���´��ڵ��յ�X����
 * @param  y2 �����ض�ɨ�跽���´��ڵ��յ�Y����
 * @retval ��
 */
__INLINE void ILI9341_SetAddressWindow( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ) {
    ILI9341_WriteReg ( LCD_COLUMN_ADDR );				/* ����X���� */
    ILI9341_WriteData ( x1 >> 8 );	 					/* �ȸ�8λ��Ȼ���8λ */
    ILI9341_WriteData ( x1 & 0xff );					/* ������ʼ��ͽ�����*/
    ILI9341_WriteData ( (x2 - 1) >> 8 );
    ILI9341_WriteData ( (x2 - 1) & 0xff );

    ILI9341_WriteReg ( LCD_PAGE_ADDR );					/* ����Y����*/
    ILI9341_WriteData ( y1 >> 8 );
    ILI9341_WriteData ( y1 & 0xff );
    ILI9341_WriteData ( (y2 - 1) >> 8 );
    ILI9341_WriteData ( (y2 - 1) & 0xff );
}

/**
 * @brief  �� ILI9341��ʾ������ĳһ��ɫ������ص�
 * @param  usColor ����ɫ
 * @param  ulAmout_Point ��Ҫ�����ɫ�����ص������Ŀ
 * @retval ��
 */
__INLINE void ILI9341_FillColor ( uint16_t usColor, uint32_t ulAmout_Point ) {
    uint32_t i = 0;

    /* memory write */
    ILI9341_WriteReg ( LCD_GRAM );

    for ( i = 0; i < ulAmout_Point; i ++ )
        ILI9341_WriteData ( usColor );
}

/**
 * @brief  ��ȡ ILI9341 GRAN ��һ����������
 * @param  ��
 * @retval ��������
 */
__INLINE uint16_t ILI9341_ReadPixelData ( void ) {
    uint16_t usR=0, usG=0, usB=0 ;

    ILI9341_WriteReg ( LCD_RAMRD );   /* ������ */

    usR = ILI9341_ReadData (); 	/* FIRST READ OUT DUMMY DATA */

    usR = ILI9341_ReadData ();  	/* READ OUT RED DATA */
    usB = ILI9341_ReadData ();  	/* READ OUT BLUE DATA */
    usG = ILI9341_ReadData ();  	/* READ OUT GREEN DATA */

    return ( ( ( usR >> 11 ) << 11 ) | ( ( usG >> 10 ) << 5 ) | ( usB >> 11 ) );
}

#if 0
/**
 * @brief  �� ILI9341��ʾ����ĳһ����ĳ����ɫ�������
 * @param  usX �����ض�ɨ�跽���¸õ��X����
 * @param  usY �����ض�ɨ�跽���¸õ��Y����
 * @param  usColor ����ɫ
 * @retval ��
 */
__INLINE void ILI9341_SetPointPixel ( uint16_t usX, uint16_t usY, uint16_t usColor ) {
    ILI9341_WriteReg ( LCD_COLUMN_ADDR );				/* ����X���� */
    ILI9341_WriteData ( usX >> 8 );	 					/* �ȸ�8λ��Ȼ���8λ */
    ILI9341_WriteData ( usX & 0xff );					/* ������ʼ��ͽ�����*/
    ILI9341_WriteData ( usX >> 8 );
    ILI9341_WriteData ( usX & 0xff );

    ILI9341_WriteReg ( LCD_PAGE_ADDR );					/* ����Y����*/
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

    //����Ĭ��ɨ�跽������ 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����
    ILI9341_GramScan(5);
	
    ILI9341_BackLed_Control ( ENABLE );      //����LCD�����
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
 * @brief  ����ILI9341��GRAM��ɨ�跽��
 * @param  ucOption ��ѡ��GRAM��ɨ�跽��
 *     @arg 0-7 :������ѡֵΪ0-7��˸�����
 *
 *	����������0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 *				���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��
 *
 *	����0��2��4��6 ģʽ��X��������Ϊ240��Y��������Ϊ320
 *	����1��3��5��7 ģʽ��X��������Ϊ320��Y��������Ϊ240
 *
 *	���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����
 *	���� 3 ģʽΪ����ͷ����ʹ�õķ���
 *	���� 0 ģʽΪBMPͼƬ��ʾ����ʹ�õķ���
 *
 * @retval ��
 * @note  ����ͼ����A��ʾ���ϣ�V��ʾ���£�<��ʾ����>��ʾ����
					X��ʾX�ᣬY��ʾY��

------------------------------------------------------------
	ģʽ0��		.	ģʽ1��		.	ģʽ2��		.	ģʽ3��
			A	.			A	.	A			.	A
			|	.			|	.	|			.	|
			Y	.			X	.	Y			.	X
			0	.			1	.	2			.	3
	<--- X0 o	.	<----Y1	o	.	o 2X--->	.	o 3Y--->
------------------------------------------------------------
	ģʽ4��		.	ģʽ5��		.	ģʽ6��		.	ģʽ7��
	<--- X4 o	.	<--- Y5 o	.	o 6X--->	.	o 7Y--->
			4	.			5	.	6			.	7
			Y	.			X	.	Y			.	X
			|	.			|	.	|			.	|
			V	.			V	.	V			.	V
---------------------------------------------------------
				  LCD��ʾ��
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
		   ��Ļ���棨��240����320��

 *******************************************************/
void ILI9341_GramScan ( uint8_t ucOption ) {
    //������飬ֻ������0-7
    if(ucOption >7 )
        return;

    //����ģʽ����LCD_SCAN_MODE��ֵ����Ҫ���ڴ�����ѡ��������
//    LCD_SCAN_MODE = ucOption;

    //����ģʽ����XY��������ؿ��
#if 1
	if(ucOption%2 == 0)	
	{
		//0 2 4 6ģʽ��X����Ϊ����
		LCD_X_LENGTH = ILI9341_LCD_PIXEL_LESS;
		LCD_Y_LENGTH = ILI9341_LCD_PIXEL_MORE;
	}
	else				
	{
		//1 3 5 7ģʽ��X����Ϊ����
		LCD_X_LENGTH = ILI9341_LCD_PIXEL_MORE;
		LCD_Y_LENGTH = ILI9341_LCD_PIXEL_LESS; 
	}
#else
    if(ucOption < 4)
    {
        //0 1 2 3ģʽ��Ϊ����
        LCD_X_LENGTH = ILI9341_LCD_PIXEL_MORE;
        LCD_Y_LENGTH = ILI9341_LCD_PIXEL_LESS;
    }
    else
    {
        //4 5 6 7ģʽ��Ϊ����
        LCD_X_LENGTH = ILI9341_LCD_PIXEL_LESS;
        LCD_Y_LENGTH = ILI9341_LCD_PIXEL_MORE;
    }
#endif
	
    //0x36��������ĸ�3λ����������GRAMɨ�跽��
    ILI9341_WriteReg ( LCD_MAC );
    ILI9341_WriteData ( 0x08 |(ucOption<<5));//����ucOption��ֵ����LCD��������0-7��ģʽ
    ILI9341_WriteReg ( LCD_COLUMN_ADDR );
    ILI9341_WriteData ( ILI9341_DispWindow_X_Star );	/* x ��ʼ�����8λ */
    ILI9341_WriteData ( ILI9341_DispWindow_X_Star );	/* x ��ʼ�����8λ */
    ILI9341_WriteData ( ((LCD_X_LENGTH-1)>>8)&0xFF );	/* x ���������8λ */
    ILI9341_WriteData ( (LCD_X_LENGTH-1)&0xFF );		/* x ���������8λ */

    ILI9341_WriteReg ( LCD_PAGE_ADDR );
    ILI9341_WriteData ( ILI9341_DispWindow_Y_Star );	/* y ��ʼ�����8λ */
    ILI9341_WriteData ( ILI9341_DispWindow_Y_Star );	/* y ��ʼ�����8λ */
    ILI9341_WriteData ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y ���������8λ */
    ILI9341_WriteData ( (LCD_Y_LENGTH-1)&0xFF );		/* y ���������8λ */

    /* GRAM start writing */
    ILI9341_WriteReg ( LCD_GRAM );
}

/**
 * @brief  ILI9341����LED����
 * @param  enumState �������Ƿ�ʹ�ܱ���LED
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg ENABLE :ʹ�ܱ���LED
  *     @arg DISABLE :���ñ���LED
 * @retval ��
 */
void ILI9341_BackLed_Control ( FunctionalState enumState ) {
    if ( enumState )
        GPIO_ResetBits ( ILI9341_BK_PORT, ILI9341_BK_PIN );
    else
        GPIO_SetBits ( ILI9341_BK_PORT, ILI9341_BK_PIN );
}

/**
 * @brief  ILI9341 �����λ
 * @param  ��
 * @retval ��
 */
void ILI9341_Reset ( void ) {
    GPIO_ResetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );	 //�͵�ƽ��λ
    ILI9341_Delay ( 0xAFF );

    GPIO_SetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );
    ILI9341_Delay ( 0xAFF );
}

/** @defgroup ILI9341_Private_Functions
  * @{
  */

/**
  * @brief  ���� ILI9341 ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */
static void ILI9341_Delay ( __IO uint32_t nCount ) {
    for ( ; nCount != 0; nCount -- );
}

/**
 * @brief  ��ʼ��ILI9341�Ĵ���
 * @param  ��
 * @retval ��
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

	/* Ĭ��Ϊ mode 0 */
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
    ILI9341_WriteData ( 0xC8 );    /*����  ���Ͻǵ� (���)�����½� (�յ�)ɨ�跽ʽ*/
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
  * @brief  ��ʼ��ILI9341��IO����
  * @param  ��
  * @retval ��
  */
static void ILI9341_GPIO_Config ( void ) {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��FSMC��Ӧ��Ӧ�ܽ�ʱ��*/
    RCC_APB2PeriphClockCmd (
        /*�����ź�*/
        ILI9341_CS_CLK|ILI9341_DC_CLK|ILI9341_WR_CLK|
        ILI9341_RD_CLK	|ILI9341_BK_CLK|ILI9341_RST_CLK|
        /*�����ź�*/
        ILI9341_D0_CLK|ILI9341_D1_CLK|	ILI9341_D2_CLK |
        ILI9341_D3_CLK | ILI9341_D4_CLK|ILI9341_D5_CLK|
        ILI9341_D6_CLK | ILI9341_D7_CLK|ILI9341_D8_CLK|
        ILI9341_D9_CLK | ILI9341_D10_CLK|ILI9341_D11_CLK|
        ILI9341_D12_CLK | ILI9341_D13_CLK|ILI9341_D14_CLK|
        ILI9341_D15_CLK	, ENABLE );

    /* ����FSMC���Ӧ��������,FSMC-D0~D15 */
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

    /* ����FSMC���Ӧ�Ŀ�����
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

    /* ����LCD��λRST���ƹܽ�*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
    GPIO_Init ( ILI9341_RST_PORT, & GPIO_InitStructure );

    /* ����LCD������ƹܽ�BK*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN;
    GPIO_Init ( ILI9341_BK_PORT, & GPIO_InitStructure );
}


/**
 * @brief  LCD  FSMC ģʽ����
 * @param  ��
 * @retval ��
 */
static void ILI9341_FSMC_Config ( void ) {
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

    /* ʹ��FSMCʱ��*/
    RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

    //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 2/72M=28ns
    readWriteTiming.FSMC_AddressSetupTime      = 0x01;	 //��ַ����ʱ��
    //���ݱ���ʱ�䣨DATAST��+ 1��HCLK = 5/72M=70ns
    readWriteTiming.FSMC_DataSetupTime         = 0x04;	 //���ݽ���ʱ��
    //ѡ����Ƶ�ģʽ
    //ģʽB,�첽NOR FLASHģʽ����ILI9341��8080ʱ��ƥ��
    readWriteTiming.FSMC_AccessMode            = FSMC_AccessMode_B;

    /*����������ģʽB�޹�*/
    //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
    readWriteTiming.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
    //��������ת�����ڣ������ڸ���ģʽ��NOR����
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    //����ʱ�ӷ�Ƶ��������ͬ�����͵Ĵ洢��
    readWriteTiming.FSMC_CLKDivision           = 0x00;
    //���ݱ���ʱ�䣬������ͬ���͵�NOR
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

    /* ʹ�� FSMC_Bank1_NORSRAM4 */
    FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAMx, ENABLE );
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
