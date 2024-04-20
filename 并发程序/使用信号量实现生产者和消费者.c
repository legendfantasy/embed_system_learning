#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
void *productor_f(void *arg);
void *consumer_f(void *arg);
sem_t sem;
int running = 1;
int main(void)
{
    pthread_t consumer_t;
    pthread_t productor_t;

    sem_init(&sem, 0, 16);
    pthread_create(&productor_t, NULL, (void *)productor_f, NULL);
    pthread_create(&consumer_t, NULL, (void *)consumer_f, NULL);
    sleep(1);
    running = 0;
    pthread_join(productor_t, NULL);
    pthread_join(consumer_t, NULL);
    sem_destroy(&sem);
    return 0;
}
void *productor_f(void *arg)
{
    int semval = 0;
    while (running)
    {
        usleep(1);
        sem_post(&sem);
        sem_getvalue(&sem, &semval);
        printf("productor: %d\n", semval);
    }
}
void *consumer_f(void *arg)
{
    int semval = 0;
    while (running)
    {
        usleep(1);
        sem_wait(&sem);
        sem_getvalue(&sem, &semval);
        printf("consumer: %d\n", semval);
    }
}