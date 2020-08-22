/**
 * @file disp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../TFT/tft.h"
#include "../TFT/ili9341.h"
#include "stm32f10x.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
  * @brief  TFT 初始化
  * @param  None
  * @retval None
  */
void TFT_Init(void)
{
    ILI9341_Init();
}

/**
 * @brief  对 TFT 显示器的某一点以某种颜色进行填充
 * @param  usX ：在特定扫描方向下该点的X坐标
 * @param  usY ：在特定扫描方向下该点的Y坐标
 * @param  usColor ：颜色
 * @retval 无
 */
void TFT_SetPointPixel ( uint16_t usX, uint16_t usY, uint16_t usColor )
{
	/* Your driver's underlying interface */
    ILI9341_SetAddressWindow(usX, usY, usX + 1, usY + 1);
    ILI9341_FillColor(usColor, 1);
}

/**
  * @brief  TFT 区域填充
  * @param  x1: X起点
  * @param  y1: Y起点
  * @param  x2: X终点
  * @param  y2: Y终点
  * @param  color: 颜色
  * @param  point: 填充数量
  * @retval None
  */
void TFT_FillArea( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint32_t point )
{
    unsigned short int temp;
    if(x1>x2) {
        temp=x1;
        x1=x2;
        x2=temp;
    }
    if(y1>y2) {
        temp=y1;
        y1=y2;
        y2=temp;
    }
	/* Your driver's underlying interface */
    ILI9341_SetAddressWindow(x1, y2, x2, y2);
    ILI9341_FillColor(color, point);
}

/**
 * @brief  获取 TFT 显示器上某一个坐标点的像素数据
 * @param  usX ：在特定扫描方向下该点的X坐标
 * @param  usY ：在特定扫描方向下该点的Y坐标
 * @retval usPixelData ：像素数据
 */
uint16_t TFT_GetPointPixel ( uint16_t usX, uint16_t usY )
{
    uint16_t usPixelData;

	/* Your driver's underlying interface */
    ILI9341_SetAddressWindow(usX, usY, usX + 1, usY + 1);

    usPixelData = ILI9341_ReadPixelData ();

    return usPixelData;
}

/**
 * @brief  在 TFT 显示器上使用 Bresenham 算法画线段
 * @param  usX1 ：在特定扫描方向下线段的一个端点X坐标
 * @param  usY1 ：在特定扫描方向下线段的一个端点Y坐标
 * @param  usX2 ：在特定扫描方向下线段的另一个端点X坐标
 * @param  usY2 ：在特定扫描方向下线段的另一个端点Y坐标
 * @param  usColor ：颜色
 * @retval 无
 */
void TFT_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2, uint16_t usColor )
{
    uint16_t us;
    uint16_t usX_Current, usY_Current;

    int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance;
    int32_t lIncrease_X, lIncrease_Y;

    lDelta_X = usX2 - usX1; //计算坐标增量
    lDelta_Y = usY2 - usY1;

    usX_Current = usX1;
    usY_Current = usY1;

    if ( lDelta_X > 0 )
        lIncrease_X = 1; //设置单步方向
    else if ( lDelta_X == 0 )
        lIncrease_X = 0;//垂直线
    else
    {
        lIncrease_X = -1;
        lDelta_X = - lDelta_X;
    }

    if ( lDelta_Y > 0 )
        lIncrease_Y = 1;
    else if ( lDelta_Y == 0 )
        lIncrease_Y = 0;//水平线
    else
    {
        lIncrease_Y = -1;
        lDelta_Y = - lDelta_Y;
    }

    if ( lDelta_X > lDelta_Y )
        lDistance = lDelta_X; //选取基本增量坐标轴
    else
        lDistance = lDelta_Y;

    for ( us = 0; us <= lDistance + 1; us ++ )//画线输出
    {
        TFT_SetPointPixel ( usX_Current, usY_Current, usColor );//画点

        lError_X += lDelta_X ;
        lError_Y += lDelta_Y ;

        if ( lError_X > lDistance )
        {
            lError_X -= lDistance;
            usX_Current += lIncrease_X;
        }

        if ( lError_Y > lDistance )
        {
            lError_Y -= lDistance;
            usY_Current += lIncrease_Y;
        }
    }
}

/**
 * @brief  在 TFT 显示器上画一个矩形
 * @param  usX_Start ：在特定扫描方向下矩形的起始点X坐标
 * @param  usY_Start ：在特定扫描方向下矩形的起始点Y坐标
 * @param  usWidth：矩形的宽度（单位：像素）
 * @param  usHeight：矩形的高度（单位：像素）
 * @param  usColor ：颜色
 * @param  ucFilled ：选择是否填充该矩形
  *   该参数为以下值之一：
  *     @arg 0 :空心矩形
  *     @arg 1 :实心矩形
 * @retval 无
 */
void TFT_DrawRectangle ( uint16_t usX_Start, 
						 uint16_t usY_Start, 
						 uint16_t usWidth, 
						 uint16_t usHeight, 
						 uint16_t usColor, 
						 uint8_t ucFilled )
{
    if ( ucFilled )
    {
		TFT_FillArea( usX_Start, usY_Start, usX_Start + usWidth, usY_Start + usHeight, usColor, usWidth * usHeight );
    }
    else
    {
        TFT_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start, usColor );
        TFT_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );
        TFT_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1, usColor );
        TFT_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );
    }
}

/**
 * @brief  在 TFT 显示器上使用 Bresenham 算法画圆
 * @param  usX_Center ：在特定扫描方向下圆心的X坐标
 * @param  usY_Center ：在特定扫描方向下圆心的Y坐标
 * @param  usRadius：圆的半径（单位：像素）
 * @param  usColor ：颜色
 * @param  ucFilled ：选择是否填充该圆
  *   该参数为以下值之一：
  *     @arg 0 :空心圆
  *     @arg 1 :实心圆
 * @retval 无
 */
void TFT_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint16_t usColor, uint8_t ucFilled )
{
    int16_t sCurrentX, sCurrentY;
    int16_t sError;

    sCurrentX = 0;
    sCurrentY = usRadius;

    sError = 3 - ( usRadius << 1 );     //判断下个点位置的标志

    while ( sCurrentX <= sCurrentY )
    {
        int16_t sCountY;

        if ( ucFilled )
		{
            for ( sCountY = sCurrentX; sCountY <= sCurrentY; sCountY++ )
            {
                TFT_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCountY, usColor  );           //1，研究对象
                TFT_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCountY, usColor  );           //2
                TFT_SetPointPixel ( usX_Center - sCountY,   usY_Center + sCurrentX, usColor  );         //3
                TFT_SetPointPixel ( usX_Center - sCountY,   usY_Center - sCurrentX, usColor  );         //4
                TFT_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCountY, usColor  );           //5
                TFT_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCountY, usColor  );           //6
                TFT_SetPointPixel ( usX_Center + sCountY,   usY_Center - sCurrentX, usColor  );         //7
                TFT_SetPointPixel ( usX_Center + sCountY,   usY_Center + sCurrentX, usColor  );         //0
            }
		}
        else
        {
            TFT_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCurrentY, usColor  );             //1，研究对象
            TFT_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCurrentY, usColor  );             //2
            TFT_SetPointPixel ( usX_Center - sCurrentY, usY_Center + sCurrentX, usColor  );             //3
            TFT_SetPointPixel ( usX_Center - sCurrentY, usY_Center - sCurrentX, usColor  );             //4
            TFT_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCurrentY, usColor  );             //5
            TFT_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCurrentY, usColor  );             //6
            TFT_SetPointPixel ( usX_Center + sCurrentY, usY_Center - sCurrentX, usColor  );             //7
            TFT_SetPointPixel ( usX_Center + sCurrentY, usY_Center + sCurrentX, usColor  );             //0
        }

        sCurrentX++;

        if ( sError < 0 )
		{
            sError += 4 * sCurrentX + 6;
		}
        else
        {
            sError += 10 + 4 * ( sCurrentX - sCurrentY );
            sCurrentY --;
        }
    }
}

/**
 * @brief  Draw rectangule with round corner
 * @param  x1 ：Start X coordinate
 * @param  y1 ：Start Y coordinate
 * @param  x2 ：End   X coordinate
 * @param  y2 ：End   Y coordinate
 * @param  r ：Radius
 * @param  color ：Color
 * @retval None
 */
void TFT_DrawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color)
{
    int x;
    int y;
    int err;
    int old_err;
    unsigned short int temp;

    if(x1>x2) {
        temp=x1;
        x1=x2;
        x2=temp;
    }

    if(y1>y2) {
        temp=y1;
        y1=y2;
        y2=temp;
    }
    if (x2 - x1 < r) return; // Add 20190517
    if (y2 - y1 < r) return; // Add 20190517

    x=0;
    y=-r;
    err=2-2*r;

    do {
        if(x) {
            TFT_SetPointPixel(x1+r-x,y1+r+y,color);
            TFT_SetPointPixel(x2-r+x,y1+r+y,color);
            TFT_SetPointPixel(x1+r-x,y2-r-y,color);
            TFT_SetPointPixel(x2-r+x,y2-r-y,color);
        }
        if ((old_err=err)<=x)   err+=++x*2+1;
        if (old_err>y || err>x) err+=++y*2+1;
    } while(y<0);

    TFT_DrawLine(x1+r,y1  ,x2-r,y1  ,color);
    TFT_DrawLine(x1+r,y2  ,x2-r,y2  ,color);
    TFT_DrawLine(x1  ,y1+r,x1  ,y2-r,color);
    TFT_DrawLine(x2  ,y1+r,x2  ,y2-r,color);
}

static float Sqrt(float number) 
{
    volatile long i;
    volatile float x, y;
    volatile const float f = 1.5F;

    x = number * 0.5F;
    y = number;
    i = * (( long * ) &y);
    i = 0x5f375a86 - ( i >> 1 );
    y = * (( float * ) &i);
    y = y * ( f - ( x * y * y ) );
	
    return 1/y;
}

// Draw arrow
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End   X coordinate
// y2:End   Y coordinate
// w:Width of the botom
// fill:filling
// color:color
// Thanks http://k-hiura.cocolog-nifty.com/blog/2010/11/post-2a62.html
/**
 * @brief  Draw arrow
 * @param  x1 ：Start X coordinate
 * @param  y1 ：Start Y coordinate
 * @param  x2 ：End   X coordinate
 * @param  y2 ：End   Y coordinate
 * @param  w ：Width of the botom
 * @param  color ：Color
 * @param  fill ：Filling
 * @retval None
 * @other  Thanks http://k-hiura.cocolog-nifty.com/blog/2010/11/post-2a62.html
 */
void TFT_DrawArrow(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t w,uint16_t color,uint8_t fill)
{

   double Vx= x1 - x0;
   double Vy= y1 - y0;
   double v = Sqrt(Vx*Vx+Vy*Vy);
//   printf("v=%f\n",v);
   double Ux= Vx/v;
   double Uy= Vy/v;

   uint16_t L[2],R[2];
   L[0]= x1 - Uy*w - Ux*v;
   L[1]= y1 + Ux*w - Uy*v;
   R[0]= x1 + Uy*w - Ux*v;
   R[1]= y1 - Ux*w - Uy*v;
//   printf("L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);

//   lcdDrawLine(x0,y0,x1,y1,color);
   TFT_DrawLine(x1,y1,L[0],L[1],color);
   TFT_DrawLine(x1,y1,R[0],R[1],color);
   TFT_DrawLine(L[0],L[1],R[0],R[1],color);
	
	if(fill)
	{
	   int ww;
	   for(ww=w-1;ww>0;ww--) {
		 L[0]= x1 - Uy*ww - Ux*v;
		 L[1]= y1 + Ux*ww - Uy*v;
		 R[0]= x1 + Uy*ww - Ux*v;
		 R[1]= y1 - Ux*ww - Uy*v;
	//     printf("Fill>L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);
		 TFT_DrawLine(x1,y1,L[0],L[1],color);
		 TFT_DrawLine(x1,y1,R[0],R[1],color);
	   }
	}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
    while(1)
    {
    }
}

#endif
