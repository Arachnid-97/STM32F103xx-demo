/**
  ******************************************************************************
  * @file    stmpe811.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    15-December-2014
  * @brief   This file provides a set of functions needed to manage the STMPE811
  *          IO Expander devices.
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
#include "../Touchpad/xpt2046.h"


/**
  * @brief  XPT2046 初始化函数
  * @param  无
  * @retval 无
  */
void XPT2046_Init ( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* 开启GPIO时钟 */
    RCC_APB2PeriphClockCmd ( XPT2046_SPI_GPIO_CLK|XPT2046_PENIRQ_GPIO_CLK, ENABLE );

    /* 模拟SPI GPIO初始化 */
    GPIO_InitStructure.GPIO_Pin=XPT2046_SPI_CLK_PIN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(XPT2046_SPI_CLK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MOSI_PIN;
    GPIO_Init(XPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(XPT2046_SPI_MISO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(XPT2046_SPI_CS_PORT, &GPIO_InitStructure);

    /* 拉低片选，选择XPT2046 */
    XPT2046_CS_DISABLE();

    //触摸屏触摸信号指示引脚，不使用中断
    GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
    GPIO_Init(XPT2046_PENIRQ_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  用于 XPT2046 的简单微秒级延时函数
  * @param  nCount ：延时计数值，单位为微妙
  * @retval 无
  */
static void XPT2046_Delay_us ( __IO uint32_t ulCount )
{
    uint32_t i;

    for ( i = 0; i < ulCount; i ++ )
    {
        uint8_t uc = 8;
        while ( uc -- );     //延1微秒
    }
}

/**
  * @brief  XPT2046 的写入命令
  * @param  ucCmd ：命令
  *   该参数为以下值之一：
  *     @arg 0x90 :通道Y+的选择控制字
  *     @arg 0xd0 :通道X+的选择控制字
  * @retval 无
  */
static void XPT2046_WriteCMD ( uint8_t ucCmd )
{
    uint8_t i;

    XPT2046_MOSI_0();

    XPT2046_CLK_LOW();

    for ( i = 0; i < 8; i ++ )
    {
        ( ( ucCmd >> ( 7 - i ) ) & 0x01 ) ? XPT2046_MOSI_1() : XPT2046_MOSI_0();

        XPT2046_Delay_us ( 5 );
        XPT2046_CLK_HIGH();

        XPT2046_Delay_us ( 5 );
        XPT2046_CLK_LOW();
    }
}


/**
  * @brief  XPT2046 的读取命令
  * @param  无
  * @retval 读取到的数据
  */
static uint16_t XPT2046_ReadCMD ( void )
{
    uint8_t i;
    uint16_t usBuf=0, usTemp;

    XPT2046_MOSI_0();

    XPT2046_CLK_HIGH();

    for ( i=0; i<12; i++ )
    {
        XPT2046_CLK_LOW();

        usTemp = XPT2046_MISO();

        usBuf |= usTemp << ( 11 - i );

        XPT2046_CLK_HIGH();

    }
    return usBuf;
}

/**
  * @brief  对 XPT2046 选择一个模拟通道后，启动ADC，并返回ADC采样结果
  * @param  ucChannel
  *   该参数为以下值之一：
  *     @arg 0x90 :通道Y+的选择控制字
  *     @arg 0xd0 :通道X+的选择控制字
  * @retval 该通道的ADC采样结果
  */
static uint16_t XPT2046_ReadAdc ( uint8_t ucChannel )
{
    XPT2046_WriteCMD ( ucChannel );

    return 	XPT2046_ReadCMD ();
}

/**
  * @brief  读取 XPT2046 的X通道和Y通道的AD值（12 bit，最大是4096）
  * @param  sX_Ad ：存放X通道AD值的地址
  * @param  sY_Ad ：存放Y通道AD值的地址
  * @param  count_read ：读的次数
  * @retval 无
  */
void XPT2046_ReadAdc_XY ( volatile int16_t * sX_Ad, volatile int16_t * sY_Ad, uint8_t count_read )
{
	uint8_t i;
    int16_t tmpx, tmpy, touch_x, touch_y;

    touch_x = XPT2046_ReadAdc ( XPT2046_CHANNEL_X );

    XPT2046_Delay_us ( 1 );

    touch_y = XPT2046_ReadAdc ( XPT2046_CHANNEL_Y );

    for (i = 0; i < count_read; i++) {
        tmpx = XPT2046_ReadAdc ( XPT2046_CHANNEL_X );
        XPT2046_Delay_us( 1 );
        tmpy = XPT2046_ReadAdc ( XPT2046_CHANNEL_Y );

        if (tmpx == 0)  tmpy = 0;
        else if (tmpy == 0)  tmpx = 0;
        else
        {
            touch_x = (touch_x + tmpx) / 2;
            touch_y = (touch_y + tmpy) / 2;
        }
    }

    * sX_Ad = touch_x;
    * sY_Ad = touch_y;
}

#if 0
/**
  * @brief  在触摸 XPT2046 屏幕时获取一组坐标的AD值，并对该坐标进行滤波
  * @param  无
  * @retval 滤波之后的坐标AD值
  */
static uint8_t XPT2046_ReadAdc_Smooth_XY ( strType_XPT2046_Coordinate * pScreenCoordinate )
{
	
#if   0                 //注意：校正较精准，但是相对复杂，速度较慢

    uint8_t ucCount = 0;

    int16_t sAD_X, sAD_Y;
    int16_t sBufferArray [ 2 ] [ 9 ] = { { 0 }, { 0 } };  //坐标X和Y进行9次采样

    int32_t lAverage  [ 3 ], lDifference [ 3 ];


    do
    {
        XPT2046_ReadAdc_XY ( & sAD_X, & sAD_Y );

        sBufferArray [ 0 ] [ ucCount ] = sAD_X;
        sBufferArray [ 1 ] [ ucCount ] = sAD_Y;

        ucCount ++;

    } while ( ( XPT2046_EXTI_Read() == XPT2046_EXTI_ActiveLevel ) && ( ucCount < 9 ) ); 	//用户点击触摸屏时即TP_INT_IN信号为低 并且 ucCount<9*/


    /*如果触笔弹起*/
    if ( XPT2046_EXTI_Read() != XPT2046_EXTI_ActiveLevel )
        ucXPT2046_TouchFlag = 0;			//触摸中断标志复位


    /* 如果成功采样9次,进行滤波 */
    if ( ucCount == 9 )
    {
        /* 为减少运算量,分别分3组取平均值 */
        lAverage  [ 0 ] = ( sBufferArray [ 0 ] [ 0 ] + sBufferArray [ 0 ] [ 1 ] + sBufferArray [ 0 ] [ 2 ] ) / 3;
        lAverage  [ 1 ] = ( sBufferArray [ 0 ] [ 3 ] + sBufferArray [ 0 ] [ 4 ] + sBufferArray [ 0 ] [ 5 ] ) / 3;
        lAverage  [ 2 ] = ( sBufferArray [ 0 ] [ 6 ] + sBufferArray [ 0 ] [ 7 ] + sBufferArray [ 0 ] [ 8 ] ) / 3;

        /* 计算3组数据的差值 */
        lDifference [ 0 ] = lAverage  [ 0 ]-lAverage  [ 1 ];
        lDifference [ 1 ] = lAverage  [ 1 ]-lAverage  [ 2 ];
        lDifference [ 2 ] = lAverage  [ 2 ]-lAverage  [ 0 ];

        /* 对上述差值取绝对值 */
        lDifference [ 0 ] = lDifference [ 0 ]>0?lDifference [ 0 ]: ( -lDifference [ 0 ] );
        lDifference [ 1 ] = lDifference [ 1 ]>0?lDifference [ 1 ]: ( -lDifference [ 1 ] );
        lDifference [ 2 ] = lDifference [ 2 ]>0?lDifference [ 2 ]: ( -lDifference [ 2 ] );


        /* 判断绝对差值是否都超过差值门限，如果这3个绝对差值都超过门限值，则判定这次采样点为野点,抛弃采样点，差值门限取为2 */
        if (  lDifference [ 0 ] > XPT2046_THRESHOLD_CalDiff  &&  lDifference [ 1 ] > XPT2046_THRESHOLD_CalDiff  &&  lDifference [ 2 ] > XPT2046_THRESHOLD_CalDiff  )
            return 0;


        /* 计算它们的平均值，同时赋值给strScreenCoordinate */
        if ( lDifference [ 0 ] < lDifference [ 1 ] )
        {
            if ( lDifference [ 2 ] < lDifference [ 0 ] )
                pScreenCoordinate ->x = ( lAverage  [ 0 ] + lAverage  [ 2 ] ) / 2;
            else
                pScreenCoordinate ->x = ( lAverage  [ 0 ] + lAverage  [ 1 ] ) / 2;
        }

        else if ( lDifference [ 2 ] < lDifference [ 1 ] )
            pScreenCoordinate -> x = ( lAverage  [ 0 ] + lAverage  [ 2 ] ) / 2;

        else
            pScreenCoordinate ->x = ( lAverage  [ 1 ] + lAverage  [ 2 ] ) / 2;


        /* 同上，计算Y的平均值 */
        lAverage  [ 0 ] = ( sBufferArray [ 1 ] [ 0 ] + sBufferArray [ 1 ] [ 1 ] + sBufferArray [ 1 ] [ 2 ] ) / 3;
        lAverage  [ 1 ] = ( sBufferArray [ 1 ] [ 3 ] + sBufferArray [ 1 ] [ 4 ] + sBufferArray [ 1 ] [ 5 ] ) / 3;
        lAverage  [ 2 ] = ( sBufferArray [ 1 ] [ 6 ] + sBufferArray [ 1 ] [ 7 ] + sBufferArray [ 1 ] [ 8 ] ) / 3;

        lDifference [ 0 ] = lAverage  [ 0 ] - lAverage  [ 1 ];
        lDifference [ 1 ] = lAverage  [ 1 ] - lAverage  [ 2 ];
        lDifference [ 2 ] = lAverage  [ 2 ] - lAverage  [ 0 ];

        /* 取绝对值 */
        lDifference [ 0 ] = lDifference [ 0 ] > 0 ? lDifference [ 0 ] : ( - lDifference [ 0 ] );
        lDifference [ 1 ] = lDifference [ 1 ] > 0 ? lDifference [ 1 ] : ( - lDifference [ 1 ] );
        lDifference [ 2 ] = lDifference [ 2 ] > 0 ? lDifference [ 2 ] : ( - lDifference [ 2 ] );


        if ( lDifference [ 0 ] > XPT2046_THRESHOLD_CalDiff && lDifference [ 1 ] > XPT2046_THRESHOLD_CalDiff && lDifference [ 2 ] > XPT2046_THRESHOLD_CalDiff )
            return 0;

        if ( lDifference [ 0 ] < lDifference [ 1 ] )
        {
            if ( lDifference [ 2 ] < lDifference [ 0 ] )
                pScreenCoordinate ->y =  ( lAverage  [ 0 ] + lAverage  [ 2 ] ) / 2;
            else
                pScreenCoordinate ->y =  ( lAverage  [ 0 ] + lAverage  [ 1 ] ) / 2;
        }
        else if ( lDifference [ 2 ] < lDifference [ 1 ] )
            pScreenCoordinate ->y =  ( lAverage  [ 0 ] + lAverage  [ 2 ] ) / 2;
        else
            pScreenCoordinate ->y =  ( lAverage  [ 1 ] + lAverage  [ 2 ] ) / 2;

        return 1;
    }

    else if ( ucCount > 1 )
    {
        pScreenCoordinate ->x = sBufferArray [ 0 ] [ 0 ];
        pScreenCoordinate ->y = sBufferArray [ 1 ] [ 0 ];
        return 0;
    }
    return 0;
	
#else     //注意：画板应用实例专用,不是很精准，但是简单，速度比较快   
    uint8_t ucCount = 0, i;
    int16_t sAD_X, sAD_Y;
    int16_t sBufferArray [ 2 ] [ 10 ] = { { 0 },{ 0 } };  //坐标X和Y进行多次采样

    //存储采样中的最小值、最大值
    int32_t lX_Min, lX_Max, lY_Min, lY_Max;

    /* 循环采样10次 */
    do
    {
        XPT2046_ReadAdc_XY ( & sAD_X, & sAD_Y, 0 );

        sBufferArray [ 0 ] [ ucCount ] = sAD_X;
        sBufferArray [ 1 ] [ ucCount ] = sAD_Y;

        ucCount ++;

    }while ( ( XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel ) && ( ucCount < 10 ) );//用户点击触摸屏时即TP_INT_IN信号为低 并且 ucCount<10

    /*如果触笔弹起*/
    if ( XPT2046_PENIRQ_Read() != XPT2046_PENIRQ_ActiveLevel )
        ucXPT2046_TouchFlag = 0;			//中断标志复位

    /*如果成功采样10个样本*/
    if ( ucCount ==10 )
    {
        lX_Max = lX_Min = sBufferArray [ 0 ] [ 0 ];
        lY_Max = lY_Min = sBufferArray [ 1 ] [ 0 ];

        for ( i = 1; i < 10; i ++ )
        {
            if ( sBufferArray[ 0 ] [ i ] < lX_Min )
                lX_Min = sBufferArray [ 0 ] [ i ];

            else if ( sBufferArray [ 0 ] [ i ] > lX_Max )
                lX_Max = sBufferArray [ 0 ] [ i ];

        }

        for ( i = 1; i < 10; i ++ )
        {
            if ( sBufferArray [ 1 ] [ i ] < lY_Min )
                lY_Min = sBufferArray [ 1 ] [ i ];

            else if ( sBufferArray [ 1 ] [ i ] > lY_Max )
                lY_Max = sBufferArray [ 1 ] [ i ];

        }

        /*去除最小值和最大值之后求平均值*/
        pScreenCoordinate ->x =  ( sBufferArray [ 0 ] [ 0 ] + sBufferArray [ 0 ] [ 1 ] + sBufferArray [ 0 ] [ 2 ] + sBufferArray [ 0 ] [ 3 ] + sBufferArray [ 0 ] [ 4 ] +
                                   sBufferArray [ 0 ] [ 5 ] + sBufferArray [ 0 ] [ 6 ] + sBufferArray [ 0 ] [ 7 ] + sBufferArray [ 0 ] [ 8 ] + sBufferArray [ 0 ] [ 9 ] - lX_Min-lX_Max ) >> 3;

        pScreenCoordinate ->y =  ( sBufferArray [ 1 ] [ 0 ] + sBufferArray [ 1 ] [ 1 ] + sBufferArray [ 1 ] [ 2 ] + sBufferArray [ 1 ] [ 3 ] + sBufferArray [ 1 ] [ 4 ] +
                                   sBufferArray [ 1 ] [ 5 ] + sBufferArray [ 1 ] [ 6 ] + sBufferArray [ 1 ] [ 7 ] + sBufferArray [ 1 ] [ 8 ] + sBufferArray [ 1 ] [ 9 ] - lY_Min-lY_Max ) >> 3;

        return 1;
    }
    return 0;
	
#endif

}

#endif


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
