#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//#define ALLREADCLOSE
//#define ONEREADCLOSE1
#define ONEREADCLOSE2

int main()
{
    int pfd[2];
    int re;
    char buf[20]={0};
    pid_t pid;
    
    re = pipe(pfd);
    if(re < 0)
    {
        perror("pipe");
        return 0;
    }
    printf("%d,%d\n", pfd[0], pfd[1]);
    
    pid = fork();
    if(pid < 0)
    {
        perror("fork");
        return 0;
    }
    else if(pid > 0)   //父进程
    {

#ifdef ALLREADCLOSE
        close(pfd[0]); 
#endif
#ifdef ONEREADCLOSE2
        close(pfd[0]); 
#endif

        while(1)
        {
            strcpy(buf,"hhahahahah");
            write(pfd[1], buf, strlen(buf));  //向写端写入数据
            sleep(1);
        }
    }
    else   //子进程
    {
        close(pfd[1]);  //关闭写端

#ifdef ALLREADCLOSE
        close(pfd[0]);  //关闭读端
#endif
#ifdef ONEREADCLOSE1
        close(pfd[0]); 
#endif
        while(1)
        {
            re = read(pfd[0], buf, 20);
            if(re > 0)
            {
                printf("read pipe=%s\n",buf);
            }
            else if(re == 0)
            {
                printf("re=0\n");
            }
        }
    }
}
