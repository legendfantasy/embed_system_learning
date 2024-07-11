#include "data_global.h"
#include "linuxuart.h"

extern void cleanup_handle(void *arg);
extern unsigned char fan_cmd[QUEUE_MSG_LEN];

extern sig_atomic_t should_exit;
extern struct thread_mutex mtx_pjsg;
extern struct thread_cond cond_pjsg;

void *pthread_fan_control(void *arg)
{
    printf("-------%s-------\n", __FUNCTION__);
    pthread_cleanup_push(cleanup_handle, NULL);
    int fd = open_port("/dev/ttyUSB0");  //打开串口
    if(fd < 0)
    {
        printf("open failed\n");
        return NULL;
    }
    set_com_config(fd, 115200, 8, 'N', 1);  //设置串口参数
    while(!should_exit)  //如果没被信号中断
    {
        printf("//////fan//////\n");
        pthread_mutex_lock(&mtx_pjsg.mtx_fan);
		write(fd, fan_cmd, sizeof(fan_cmd));  //向串口发送命令
		pthread_mutex_unlock(&mtx_pjsg.mtx_fan);
		pthread_cond_wait(&cond_pjsg.cond_fan, &mtx_pjsg.mtx_fan);
    }
    printf("pthread_led_control exit\n");
    pthread_cleanup_pop(0);
    
    return NULL;
}

