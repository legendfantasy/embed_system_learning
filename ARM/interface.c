#include "exynos_4412.h"




int main()
{
    //存储读取结果
	unsigned char zvalue_h,zvalue_l,xvalue_h,xvalue_l,yvalue_h,yvalue_l;						
	short int zvalue,xvalue,yvalue;
 
	/*设置GPB_2引脚和GPB_3引脚功能为I2C传输引脚*/
	GPB.CON = (GPB.CON & ~(0xF<<12)) | 0x3<<12;			 		//设置GPB_3引脚功能为I2C_5_SCL
	GPB.CON = (GPB.CON & ~(0xF<<8))  | 0x3<<8;					//设置GPB_2引脚功能为I2C_5_SDA
 
	uart_init(); 												//初始化串口
	MPU6050_Init();												//初始化MPU6050
	RTC_Init();													//RTC初始化
	BEEP_Init();												//BEEP初始化
 
	printf("\n********** I2C test!! ***********\n");
	PWM.TCON = PWM.TCON & (~(1));								//关闭BEEP
	while(1)
	{
		zvalue_h = iic_read(SlaveAddress, ACCEL_ZOUT_H);		//获取MPU6050-Z轴加速度高字节
		zvalue_l = iic_read(SlaveAddress, ACCEL_ZOUT_L);		//获取MPU6050-Z轴加速度低字节
		zvalue  =  (zvalue_h<<8)|zvalue_l;						//获取MPU6050-Z轴加速度
 
		xvalue_h = iic_read(SlaveAddress, ACCEL_XOUT_H);		//获取MPU6050-Z轴加速度高字节
		xvalue_l = iic_read(SlaveAddress, ACCEL_XOUT_L);		//获取MPU6050-Z轴加速度低字节
		xvalue  =  (xvalue_h<<8)|xvalue_l;						//获取MPU6050-Z轴加速度
 
		yvalue_h = iic_read(SlaveAddress, ACCEL_YOUT_H);		//获取MPU6050-Z轴加速度高字节
		yvalue_l = iic_read(SlaveAddress, ACCEL_YOUT_L);		//获取MPU6050-Z轴加速度低字节
		yvalue  =  (yvalue_h<<8)|yvalue_l;						//获取MPU6050-Z轴加速度
    }

}