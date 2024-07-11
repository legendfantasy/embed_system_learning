#include "data_global.h"


extern sig_atomic_t should_exit;
extern void cleanup_handle(void *arg);

void *pthread_message_queue_send(void *arg)
{
    pthread_cleanup_push(cleanup_handle, NULL)
    while(!should_exit)  //如果没被信号中断
    {
        printf("deal with message_queue\n");
        sleep(1);
    }
    printf("pthread_message_queue_send exit\n");
    pthread_cleanup_pop(0);
    
    return NULL;
}