#include "exynos_4412.h"

void BEEP_Init()
{
    //GPD0_0设置PWM0输出功能
    GPD0.CON = GPD0.CON & (~(0xF)) | (0x2);
    //PWM0一级分频  100倍
    PWM.TCFG0 = PWM.TCFG0 & (~(0xFF)) | (99);
    //PWM0二级分频  1倍  PCLk/(99+1)/(0+1) = 1MHz
    PWM.TCFG1 = PWM.TCFG1 & (~(0xF));
    //设置PWM0自动重装 
    PWM.TCON = PWM.TCON | (1<<3);
    //设置周期 1000
    PWM.TCNTB0 = 1000;
    //占空比 60%
    PWM.TCMPB0 = 600;
    //手动装载
    PWM.TCON = PWM.TCON | (1 << 1);
    //关闭手动装载
    PWM.TCON = PWM.TCON & (~(1 << 1));
    //使能PWM0
    PWM.TCON = PWM.TCON | 1;
}