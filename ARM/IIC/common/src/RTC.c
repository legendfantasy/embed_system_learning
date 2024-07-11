#include "exynos_4412.h"

void RTC_Init()
{
	//使能RTC控制
    RTCCON = RTCCON | 1;
    //校准时间
    RTC.BCDYEAR = 0x024; //2024
    RTC.BCDMON  = 0x3;   //3
    RTC.BCDDAY  = 0x5;   //4
    RTC.BCDWEEK = 0x2;   //星期
    RTC.BCDHOUR = 0x13;  //13
    RTC.BCDMIN  = 0x00;  //00
    RTC.BCDSEC  = 0x00;  //00s
 
    //关闭RTC控制
    RTCCON = RTCCON & 0;
 
}