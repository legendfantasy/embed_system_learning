#include <pthread.h>
#include <sched.h>
void *productor_f (void *arg);
void *consumer_f (void *arg);
int buffer_has_item = 0;
pthread_mutex_t mutex;
int running = 1;
int main(void)
{
    pthread_t consumer_t;
    pthread_t productor_t;

    pthread_mutex_init(&mutex, NULL);
    pthread_create(&consumer_t, NULL, (void *)consumer_f, NULL);
    pthread_create(&productor_t, NULL, (void *)productor_f, NULL);
    usleep(1);
    running = 0;
    pthread_join(consumer_t, NULL);
    pthread_join(productor_t, NULL);
    pthread_mutex_destroy(&mutex);
    return 0; 
}
void *productor_f (void *arg)
{
    while(running)
    {
        pthread_mutex_lock(&mutex);
        buffer_has_item ++;
        printf("生产，总数量： %d\n", buffer_has_item);
        pthread_mutex_unlock(&mutex);
    }
}
void *consumer_f (void *arg)
{
    while(running)
    {
        pthread_mutex_lock(&mutex);
        buffer_has_item --;
        printf("消费，总数量： %d\n", buffer_has_item);
        pthread_mutex_unlock(&mutex);
    }
}