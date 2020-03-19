#ifndef __BSP_RTC_H
#define __BSP_RTC_H


#include "stm32f10x.h"

#define RTC_BKP_DATA         0xA5A5

struct rtc_time{
    uint8_t Seconds;	// ��
    uint8_t Minute;		// ��
    uint8_t Hour;		// ʱ
    uint8_t Day;		// ��
	uint8_t Week;		// ����
    uint8_t Month;		// ��
    uint16_t Year;		// ��
};

void RTC_Time_Scan( uint32_t Time );
void RTC_Init( struct rtc_time *Time );


#endif	/* __BSP_H */


/*---------------------------- END OF FILE ----------------------------*/

