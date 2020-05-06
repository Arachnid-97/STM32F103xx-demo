/**
 * @file indev.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "touchpad.h"
#include "stm32f10x.h"
#include "xpt2046.h"

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
 * Initialize your input devices here
 */
void Touch_Init(void)
{
	XPT2046_Init();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief  获取 xy坐标位置
 * @param  x ：端点X坐标
 * @param  y ：端点Y坐标
 * @retval 无
 */
#define XPT2046_HOR_RES     320
#define XPT2046_VER_RES     240
#define XPT2046_X_MIN       300
#define XPT2046_Y_MIN       200
#define XPT2046_X_MAX       3800
#define XPT2046_Y_MAX       3800
#define XPT2046_XY_SWAP  	0
#define XPT2046_X_INV   	1
#define XPT2046_Y_INV   	1

void Touch_Get_XY(int16_t *x, int16_t *y)
{
	XPT2046_ReadAdc_XY(x, y, 1);
	
#if XPT2046_XY_SWAP
    int16_t swap_tmp;
    swap_tmp = *x;
    *x = *y;
    *y = swap_tmp;
#endif

    if((*x) > XPT2046_X_MIN)(*x) -= XPT2046_X_MIN;
    else(*x) = 0;

    if((*y) > XPT2046_Y_MIN)(*y) -= XPT2046_Y_MIN;
    else(*y) = 0;

    (*x) = (uint32_t)((uint32_t)(*x) * XPT2046_HOR_RES) /
           (XPT2046_X_MAX - XPT2046_X_MIN);

    (*y) = (uint32_t)((uint32_t)(*y) * XPT2046_VER_RES) /
           (XPT2046_Y_MAX - XPT2046_Y_MIN);

#if XPT2046_X_INV
    (*x) =  XPT2046_HOR_RES - (*x);
#endif

#if XPT2046_Y_INV
    (*y) =  XPT2046_VER_RES - (*y);
#endif
}
