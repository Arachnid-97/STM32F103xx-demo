#ifndef __BSP_RTC_H
#define __BSP_RTC_H


#include "stm32f10x.h"

#define RTC_BKP_DATA         0xA5A5

struct rtc_time{
    uint8_t Seconds;	// 秒
    uint8_t Minute;		// 分
    uint8_t Hour;		// 时
    uint8_t Day;		// 日
	uint8_t Week;		// 星期
    uint8_t Month;		// 月
    uint16_t Year;		// 年
};

void RTC_Time_Scan( uint32_t Time );
void RTC_Init( struct rtc_time *Time );


#endif	/* __BSP_H */


/*---------------------------- END OF FILE ----------------------------*/

