#include "data_global.h"
#include <pthread.h>

volatile sig_atomic_t should_exit = 0;
struct thread_pid pid_pjsg;
struct thread_mutex mtx_pjsg;
struct thread_cond cond_pjsg;

struct IPC_id id_pjsg;
struct IPC_key key_pjsg;


void cleanup_handle(void *arg)
{
    printf("pthread_cleanup_pop\n");
    pthread_detach(pthread_self());
    printf("pthread %d is detached\n", (int)pthread_self());
}
void init_thread_resources(void)
{

	pthread_mutex_init(&mtx_pjsg.mtx_web, 	NULL);  //客户请求
    // pthread_mutex_init(&mtx_pjsg.mtx_msqsend, NULL);  //消息队列
    pthread_mutex_init(&mtx_pjsg.mtx_led, 	NULL);  //LED
    // pthread_mutex_init(&mtx_pjsg.mtx_buzzer, NULL);  //蜂鸣器
    // pthread_mutex_init(&mtx_pjsg.mtx_fan, 	NULL);  //风扇
    
    pthread_cond_init(&cond_pjsg.cond_web, 	NULL);  //客户请求
    // pthread_cond_init(&cond_pjsg.cond_msqsend, NULL);  //消息队列
    pthread_cond_init(&cond_pjsg.cond_led, 	NULL);  //LED
    // pthread_cond_init(&cond_pjsg.cond_buzzer, NULL);  //蜂鸣器
    // pthread_cond_init(&cond_pjsg.cond_fan, NULL);
}
void thread_exit_handler(int signum)
{
    should_exit = 1;
}
int main(void)
{
/***************** 主进程 *******************************/
    //如果需要提前使用信号来结束主进程
    struct sigaction act;
    act.sa_handler = thread_exit_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("sigaction");
        exit(-1);
    }
    
    /*********** 创建线程 ***********/
    //多个任务处理需要使用多线程
    int ret = -1;
    
    ///////////////////////////网页客户端请求处理线程/////////////////////////////////////////
    if((ret = pthread_create(&pid_pjsg.pid_web, NULL, pthread_web_client_handle, NULL)) != 0)
    {
        perror("pthread_create");
        exit(-1);
    }
    printf(">>> pthread_create: %d\n", (int)pid_pjsg.pid_web);
    
    // if((ret = pthread_create(&pid_pjsg.pid_msqsend, NULL, pthread_message_queue_send, NULL)) != 0)
    // {
    //     perror("pthread_create");
    //     exit(-1);
    // }
    // printf(">>> pthread_create: %d\n", (int)pid_pjsg.pid_msqsend);
    ////////////////////////////LED控制线程////////////////////////////////////////////////
    if((ret = pthread_create(&pid_pjsg.pid_led, NULL, pthread_led_control, NULL)) != 0)
    {
        perror("pthread_create");
        exit(-1);
    }
    printf(">>> pthread_create: %d\n", (int)pid_pjsg.pid_led);
    //////////////////////////蜂鸣器控制线程///////////////////////////////////////////////
    // if((ret = pthread_create(&pid_pjsg.pid_buzzer, NULL, pthread_buzzer_control, NULL)) != 0)
    // {
    //     perror("pthread_create");
    //     exit(-1);
    // }
    // printf(">>> pthread_create: %d\n", (int)pid_pjsg.pid_buzzer);

    // if((ret = pthread_create(&pid_pjsg.pid_data_refresh, NULL, pthread_data_refresh, NULL)) != 0)
    // {
    //     perror("pthread_create");
    //     exit(-1);
    // }
    // printf(">>> pthread_create: %d\n", (int)pid_pjsg.pid_data_refresh);
    
    // if((ret = pthread_create(&pid_pjsg.pid_fan, NULL, pthread_fan_control, NULL)) != 0)
    // {
    //     perror("pthread_create");
    //     exit(-1);
    // }
    // printf(">>> pthread_create: %d\n", (int)pid_pjsg.pid_data_refresh);

    init_thread_resources();
    // 阻塞主进程等待所有线程退出
    pthread_join(pid_pjsg.pid_web, NULL);
    printf(">>> pthread_join: %d\n", (int)pid_pjsg.pid_web);
    // pthread_join(pid_pjsg.pid_msqsend, NULL);
    // printf(">>> pthread_join: %d\n", (int)pid_pjsg.pid_msqsend);
    pthread_join(pid_pjsg.pid_led, NULL);
    printf(">>> pthread_join: %d\n", (int)pid_pjsg.pid_led);
    // pthread_join(pid_pjsg.pid_buzzer, NULL);
    // printf(">>> pthread_join: %d\n", (int)pid_pjsg.pid_buzzer);
    // pthread_join(pid_pjsg.pid_data_refresh, NULL);
    // printf(">>> pthread_join: %d\n", (int)pid_pjsg.pid_data_refresh);
    // pthread_join(pid_pjsg.pid_fan, NULL);
    // printf(">>> pthread_join: %d\n", (int)pid_pjsg.pid_fan);
    //程序正常结束
    return 0;
}
