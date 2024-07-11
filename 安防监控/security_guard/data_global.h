#ifndef __DATA_GLOABL_H__
#define __DATA_GLOABL_H__

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


#define MONITOR_NUM   1   // 监控的个数
#define QUEUE_MSG_LEN  32  // 消息队列的长度

#define LED_DEV "/dev/fsled"

struct IPC_id
{
	int msgid;
	int semid;
	int shmid;
};
struct IPC_key
{
	key_t msg_key;
	key_t shm_key;
	key_t sem_key;
};
struct thread_pid
{
    pthread_t pid_web;
    pthread_t pid_msqsend;
    pthread_t pid_led;
    pthread_t pid_buzzer;
    pthread_t pid_data_refresh;
    pthread_t pid_data_zigbee;
	pthread_t pid_fan;
};

struct thread_mutex
{
    pthread_mutex_t mtx_web;
    pthread_mutex_t mtx_msqsend;
    pthread_mutex_t mtx_led;
    pthread_mutex_t mtx_buzzer;
	pthread_mutex_t mtx_fan;
};

struct thread_cond
{
    pthread_cond_t cond_web;
    pthread_cond_t cond_msqsend;
    pthread_cond_t cond_led;
    pthread_cond_t cond_buzzer;
	pthread_cond_t cond_fan;
};

// extern void *pthread_message_queue_send(void *arg);
// extern void *pthread_led_control(void *arg);
// extern void *pthread_buzzer_control(void *arg);
extern void *pthread_web_client_handle(void *arg);
extern void *pthread_message_queue_send(void *arg);
extern void *pthread_led_control(void *arg);
extern void *pthread_buzzer_control(void *arg);
extern void *pthread_data_refresh(void *arg);
extern void *pthread_fan_control(void *arg);

//-------------------------------------------------
//定义通信协议 ----- 通信的格式--- 结构体
typedef  unsigned char uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int uint32_t;
//-------------------------------------------------

//考虑到内存对齐的问题
//zigbee数据结构体
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
//A9数据结构体
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

//消息队列结构体
struct msg
{
	long type;       //从消息队列接收消息时用于判断的消息类型
	long msgtype;    //具体的消息类型
	unsigned char text[QUEUE_MSG_LEN]; //消息正文
};

#endif