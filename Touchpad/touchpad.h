/**
 * @file indev.h
 * 
 */

#ifndef INDEV_H
#define INDEV_H

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
void Touch_Init(void);

void Touch_Get_XY(int16_t *x, int16_t *y);

/**********************
 *      MACROS
 **********************/

#endif
