/**
 * @file disp.h
 * 
 */

#ifndef DISP_H
#define DISP_H

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void TFT_Init(void);
void TFT_SetPointPixel ( uint16_t usX, uint16_t usY, uint16_t usColor );
void TFT_FillArea( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint32_t point );
uint16_t TFT_GetPointPixel ( uint16_t usX, uint16_t usY );

void TFT_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2, uint16_t usColor );
void TFT_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint16_t usColor, uint8_t ucFilled );
void TFT_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint16_t usColor, uint8_t ucFilled );
void TFT_DrawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color);
void TFT_DrawArrow(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t w,uint16_t color,uint8_t fill);

/**********************
 *      MACROS
 **********************/

#endif
