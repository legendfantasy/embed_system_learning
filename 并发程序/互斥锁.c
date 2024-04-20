#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //静态方式创建一个互斥锁
FILE *fp;


void *write_func(void *arg)
{
    //分离线程
    pthread_detach(pthread_self());
    printf("This is write thread\n");
    
    char str[]="xxxxxxxxxxxxxxxxxxxxxxxx\n";

    pthread_mutex_lock(&mutex);//加锁
    for(int i=0; i<strlen(str); i++)
    {
        //将str中的内容写入文件
        fputc(str[i], fp);
    }
    pthread_mutex_unlock(&mutex);//解锁
    //线程挂起休眠5s
    sleep(5);
    pthread_exit("write exit");
}

void *read_func(void *arg)
{
    pthread_detach(pthread_self());
    printf("This is read thread\n");
    
    char str[] = {0};

    pthread_mutex_lock(&mutex);//加锁
    //将文件中的内容读取到str中
    while(fgets(str, sizeof(str), fp) != NULL)
    {
        printf("%s", str);
    }
    pthread_mutex_unlock(&mutex);//解锁
    //线程挂起休眠5s
    sleep(5);
    pthread_exit("read exit");
}



int main()
{
    //打开一个文件，进行文件IO操作
    fp = fopen("test.txt", "a+");
    if(fp == NULL)
    {
        perror("fopen error");
        return -1;
    }
    //创建两个线程来处理读和写
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, write_func, NULL);
    pthread_create(&tid2, NULL, read_func, NULL);

    while(1)
    {
        sleep(1);
    }

    return 0;
}