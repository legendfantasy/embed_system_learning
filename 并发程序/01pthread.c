//使用多线程编程实现A线程和B线程轮流打印a和b共打印50次
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


void printA(void *arg)
{
    for(int i = 0; i < 50; i++)
    {
        printf("a");
        sleep(1);
        printf("\n");
    }
}
void printB(void *arg)
{
    for(int i = 0; i < 50; i++)
    {
        printf("b");
        sleep(1);
        printf("\n");
    }
}
int main(void)
{
    pthread_t tid1,tid2;
    //创建线程
    int thread_a = pthread_create(&tid1, NULL, (void *)printA, NULL);
    if(thread_a != 0)
    {
        printf("创建a线程失败\n");
        exit(-1);
    }
    int thread_b = pthread_create(&tid2, NULL, (void *)printB, NULL);
    if (thread_b != 0)
    {
        printf("创建b线程失败\n");
        exit(-1);
    }
    //等待线程结束
    pthread_join(tid1, NULL);
    printf("a线程结束\n");
    pthread_join(tid2, NULL);
    printf("b线程结束\n");
    return 0;
}