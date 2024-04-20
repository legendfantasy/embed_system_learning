#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

typedef int sem_t;
static char msg[] = "Hello!\n";
// 定义信号量共用体，可以保存各种不同种类的信号量的值
union semun
{
    int val;  //整型变量
    struct semid_ds *buf;  //结构体指针， 指向semid_ds结构体,保存信号量的各种信息
/*
struct semid_ds
{
    struct ipc_perm sem_perm;  //权限
    time_t sem_otime;         //上一次操作时间
    time_t sem_ctime;         //创建时间
    union semun sem_u;        //信号量值
};
*/
    unsigned short *array; //无符号短整型数组指针
    struct seminfo *__buf; //结构体指针，也保存了信号量的信息
};

int Sem_P(sem_t semid)
{
    struct sembuf buf = {0, +1, IPC_NOWAIT};  //sembuf 结构体保存semop函数所需要的操作信息
    return semop(semid, &buf, 1);
}
int Sem_V(sem_t semid)
{
    struct sembuf buf = {0, -1, IPC_NOWAIT};
    return semop(semid, &buf, 1);
}
sem_t CreateSem(key_t key, int value)
{
    union semun sem;
    sem.val = value;  //初始化信号量值
    sem_t semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);  //创建新的信号量集合
    if(semid == -1)
    {
        perror("semget");
        return -1;
    }
    semctl(semid, 0, SETVAL, sem);  //设置信号量值 semctl(semid, 0, SETVAL, sem);  
    // SETVAL：这是 semctl 的命令参数，表示我们要设置信号量的值。
    return semid;
}
void SetvalueSem(sem_t semid, int value)
{
    union semun sem;
    sem.val = value;
    semctl(semid, 0, SETVAL, sem);
}
int GetvalueSem(sem_t semid)
{
    union semun sem;
    return semctl(semid, 0, GETVAL, &sem);
}
void DestroySem(sem_t semid)
{
    union semun sem;
    sem.val = 0;
    semctl(semid, 0, IPC_RMID, &sem);
}
int main(void)
{
    key_t key = ftok("./ipc/sem", 'a');
    int shmid = shmget(key, 1024, IPC_CREAT | IPC_EXCL | 0666);
    int semid = CreateSem(key, 1);
    
    pid_t pid = fork();
    if(pid > 0)
    {
        char *shms = (char *)shmat(shmid, 0, 0);
        memcpy(shms, msg, strlen(msg) + 1);
        sleep(10);
        Sem_P(semid);
        shmdt(shms);
        DestroySem(semid)
    }
    else if(P == 0)
    {
        char *shmc = (char *)shmat(shmid, 0, 0);
        Sem_V(semid);
        printf("共享内存的值为：%s\n", shmc);
        shmdt(shmc);
    }
    return 0;
}