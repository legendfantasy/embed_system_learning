//该文件用于定义信号量的相关函数
/* 
init_sem 初始化一个值为val的信号量
sem_p 执行信号量的P操作
sem_v 执行信号量的V操作

*/
#ifndef __MONITOR_SEM_H__
#define __MONITOR_SEM_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun 
{
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};

/**
 * @brief 初始化信号量
 *
 * 使用semctl函数设置信号量的值。
 *
 * @param semid 信号量标识符
 * @param num 信号量编号
 * @param val 信号量初始值
 *
 * @return 初始化成功返回0，否则返回-1
 */
int init_sem(int semid, int num, int val)

{
	// 定义一个联合体semun
	union semun myun;
	// 设置联合体的val成员为传入的val值
	myun.val = val;
	// 调用semctl函数设置信号量的值
	if(semctl(semid, num, SETVAL, myun) < 0)
	{
		// 如果设置失败，打印错误信息
		perror("semctl");
		// 退出程序
		exit(1);
	}
	// 返回0表示设置成功
	return 0;
}

/**
 * @brief 信号量P操作
 *
 * 执行信号量的P操作，即对指定的信号量进行减一操作。
 *
 * @param semid 信号量标识符
 * @param num 信号量编号
 *
 * @return 成功返回0，失败返回-1
 */
int sem_p(int semid, int num)
{
	// 定义一个信号量缓冲区
	struct sembuf mybuf;
	// 设置信号量的编号
	mybuf.sem_num = num;
	// 设置信号量的操作值，减1表示等待信号量
	mybuf.sem_op = -1;
	// 设置信号量的标志位，SEM_UNDO表示当进程结束时自动释放资源
	mybuf.sem_flg = SEM_UNDO;

	// 调用semop函数进行信号量操作
	if(semop(semid, &mybuf, 1) < 0)
	{
		// 如果操作失败，则打印错误信息
		perror("semop");
		// 退出程序
		exit(1);
	}

	// 返回成功标志
	return 0;
}

/**
 * @brief 增加信号量值
 *
 * 增加指定信号量的值。
 *
 * @param semid 信号量标识符
 * @param num 信号量编号
 *
 * @return 成功返回0，失败返回-1
 */
int sem_v(int semid, int num)
{
	// 定义一个信号量操作结构体
	struct sembuf mybuf;
	// 设置信号量的编号
	mybuf.sem_num = num;
	// 设置信号量的操作数，加1操作
	mybuf.sem_op = 1;
	// 设置信号量操作的标志位，表示在进程结束时自动撤销对信号量的操作
	mybuf.sem_flg = SEM_UNDO;

	// 调用semop函数执行信号量操作
	if(semop(semid, &mybuf, 1) < 0)
	{
		// 如果执行失败，则打印错误信息
		perror("semop");
		// 终止程序
		exit(1);
	}

	// 返回0表示执行成功
	return 0;
}


#endif

