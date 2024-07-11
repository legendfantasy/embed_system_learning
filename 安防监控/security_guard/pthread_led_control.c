#include <pthread.h>
#include <stdio.h>

#include "data_global.h"
#include "leddrv.h"

extern sig_atomic_t should_exit;
extern void cleanup_handle(void *arg);
extern struct thread_mutex mtx_pjsg;
extern struct thread_cond cond_pjsg;

extern char cmd_led;

void All_led_open(int led_fd)
{
	int i = 0;
	led_desc_t led;	

    for(i = 2; i < 6; i ++)
    {
        led.led_num = i;
        ioctl(led_fd, FS_LED_ON, &led);
    }
}
void All_led_close(int led_fd)
{
	int i = 0;
	led_desc_t led;	
    for(i = 2; i < 6; i ++)
    {
        led.led_num = i;
        ioctl(led_fd, FS_LED_OFF, &led);
    }
}
void *pthread_led_control(void *arg)
{
    int i = 0;
    int led_fd;
    led_desc_t led_desc;

    pthread_cleanup_push(cleanup_handle, NULL)

        led_fd = open(LED_DEV, O_RDWR);  //打开led设备
        if(led_fd == -1)
        {
            perror("open led device error");
            perror("pthread_led_control exit");
            pthread_exit(NULL);
        }
        while(!should_exit)  //如果没被信号中断
        {
            // 给线程加锁，保护临界区
            pthread_mutex_lock(&mtx_pjsg.mtx_led);
            // 阻塞等待条件变量
            pthread_cond_wait(&cond_pjsg.cond_led, &mtx_pjsg.mtx_led);
            if(cmd_led == 0x41)   // 应该执行打开LED开关的指令
            {
                All_led_open(led_fd);
            }
            else if(cmd_led == 0x40)  
            {
                All_led_close(led_fd);
            }
            pthread_mutex_unlock(&mtx_pjsg.mtx_led);
        }
        printf("pthread_led_control exit\n");
 
    pthread_cleanup_pop(0);
    
    return NULL;
}