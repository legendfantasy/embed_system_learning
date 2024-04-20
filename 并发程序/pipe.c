#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

//#define ALLWRITEPCLOSE
#define ONEWRITEPCLOSE1
//#define ONEWRITEPCLOSE2

int main()
{
    int pfd[2];
    int re;
    char buf[20]={0};
    pid_t pid;
    /*创建无名管道*/
    re = pipe(pfd);
    if(re < 0)
    {
        perror("pipe");
        return 0;
    }

    printf("%d,%d\n",pfd[0],pfd[1]);  //pfd[0]=3  pfd[1]=4 
    /*创建进程*/
    pid = fork();
    if(pid < 0)  //创建进程失败
    {
        perror("fork");
        return 0;
    }
    else if(pid > 0)  //父进程
    {
        close(pfd[0]);  //关闭读

#ifdef ALLWRITEPCLOSE
        close(pfd[1]);  //关闭写
        //如果关闭 则程序无阻塞,会一直打印re=0
        //如果没有关闭,程序会一直打印read pipe=hhahahahah
#endif
#ifdef ONEWRITEPCLOSE1
        close(pfd[1]);  //关闭写
#endif
        while(1)
        {
            strcpy(buf, "hhahahahah");
            write(pfd[1], buf, strlen(buf));  //向写管道写入
            sleep(1);
        }

    }
    else   //子进程
    {

#ifdef ALLWRITEPCLOSE
        close(pfd[1]);   //关闭写
#endif
#ifdef ONEWRITEPCLOSE2
        close(pfd[1]);   //关闭写
#endif

        while(1)
        {
            re = read(pfd[0], buf, 20);

            if(re > 0)             //管道中有数据
            {
                printf("read pipe=%s\n",buf);
            }
            else if(re == 0)      //管道中没有数据
            {
                printf("re=0\n");
            }
        }
    }
}
