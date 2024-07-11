#ifndef __DATA_GLOBAL__H__
#define __DATA_GLOBAL__H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>


/*********************************************************
  data_global.h : 

  全局的宏定义#define
  全局的线程函数声明
  全局的设备节点声明
  全局的消息队列发送函数外部extern声明
  全局的消息队列传递的结构体信息声明

 *********************************************************/

/*********************************************************
 全局的宏定义
 *********************************************************/

#define MONITOR_NUM   1   // 监控的个数
#define QUEUE_MSG_LEN  32  // 消息队列的长度

#define		GPRS_DEV   		 "/dev/ttyUSB0"  //GPRS设备
#define		ZIGBEE_DEV 		 "/dev/ttyUSB1"  //zigbee设备
#define		BEEPER_DEV 		 "/dev/fsbeeper0"  //蜂鸣器设备
#define		LED_DEV    		 "/dev/fsled0"  //LED设备


/*********************************************************
 全局的结构体声明
 *********************************************************/

typedef  unsigned char uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int uint32_t;

//考虑到内存对齐的问题
struct makeru_zigbee_info
{
	uint8_t head[3]; //标识位: 'm' 's' 'm'  makeru-security-monitor  
	uint8_t type;	 //数据类型  'z'---zigbee  'a'---a9

	float temperature; //温度
	
	float humidity;  //湿度
	
	float tempMIN;  //温度下限
	float tempMAX;  //温度上限
	
	float humidityMIN;   //湿度下限
	float humidityMAX;   //湿度上限
	
	uint32_t reserved[2]; //预留扩展位，默认0
};

struct makeru_a9_info
{
	uint8_t head[3]; //标识位: 'm' 's' 'm'  makeru-security-monitor  
	uint8_t type;	 //数据类型  'z'---zigbee  'a'---a9
	
	float adc;
	
	short gyrox;   //陀螺仪烃所
	short gyroy;
	short gyroz;
	
	short  aacx;  //加速度计数据
	short  aacy;
	short aacz;
	
	uint32_t reserved[2]; //保留扩展位，默认0
};

//环境信息分成两个部分，一个来自A9，一个来自zigbee
struct makeru_env_data
{
	struct makeru_a9_info       a9_info;    
	struct makeru_zigbee_info   zigbee_info;
	uint32_t reserved[2]; //保留扩展位，默认填充0
};

//封装所有监控区域信息的结构体
struct env_info_client_addr
{
	struct makeru_env_data  monitor_no[MONITOR_NUM];	//数组 两个区域
};


/*********************************************************
 全局的外部线程函数声明
 *********************************************************/

extern void *pthread_client_request (void *arg);      //接收CGI 等的请求
extern void *pthread_refresh(void *arg);              //刷新共享内存数据线程 从A9 zigbee到网页
extern void *pthread_sqlite(void *arg);               //数据库线程，保存数据库的数据
extern void *pthread_transfer(void *arg);             //接收ZigBee的数据并解析
extern void *pthread_sms(void *arg);                  //发送短信线程
extern void *pthread_buzzer(void *arg);               //蜂鸣器控制线程
extern void *pthread_led(void *arg);                  //led灯控制线程


extern int send_msg_queue(long type,unsigned char text);  //发送消息队列


/*********************************************************
  全局的消息队列传递的结构体声明
 *********************************************************/

//消息队列结构体
struct msg
{
	long type;       //从消息队列接收消息时用于判断的消息类型
	long msgtype;    //具体的消息类型
	unsigned char text[QUEUE_MSG_LEN]; //消息正文
};




#endif 


