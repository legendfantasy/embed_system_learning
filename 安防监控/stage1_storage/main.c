#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "data_global.h"


void release_pthread_resource(int signo);

extern pthread_mutex_t mutex_client_request,
						mutex_refresh,
						mutex_sqlite,
						mutex_transfer,
						mutex_sms,
						mutex_buzzer,
						mutex_led;

extern pthread_cond_t  cond_client_request,
						cond_refresh,
						cond_sqlite,
						cond_transfer,
						cond_transfer,
						cond_sms,
						cond_buzzer,
						cond_led;

extern int msgid;
extern int shmid;
extern int semid;

pthread_t  	id_client_request,
		  	id_refresh,
		  	id_sqlite,
		  	id_transfer,
		  	id_sms,
		  	id_buzzer,
		  	id_led;		


int main(int argc, const char *argv[])
{
	//初使化线程互斥锁
	pthread_mutex_init(&mutex_client_request, 	NULL);  //客户请求
	pthread_mutex_init(&mutex_refresh, 			NULL);  //刷新
	pthread_mutex_init(&mutex_sqlite, 			NULL);  //sqlite
	pthread_mutex_init(&mutex_transfer, 		NULL);  //传输
	pthread_mutex_init(&mutex_sms, 				NULL);  //短信
	pthread_mutex_init(&mutex_buzzer, 			NULL);  //蜂鸣器
	pthread_mutex_init(&mutex_led, 				NULL);  //LED
	
	//注册信号处理函数
	signal(SIGINT, release_pthread_resource);  //ctrl+c 释放所有线程资源

	//初使化线程条件变量
	pthread_cond_init(&cond_client_request, 	NULL);  
	pthread_cond_init(&cond_refresh, 			NULL);
	pthread_cond_init(&cond_sqlite, 			NULL);
	pthread_cond_init(&cond_transfer, 			NULL);
	pthread_cond_init(&cond_sms, 				NULL);
	pthread_cond_init(&cond_buzzer, 			NULL);
	pthread_cond_init(&cond_led, 				NULL);

	//创建线程
	pthread_create(&id_client_request,  NULL,	pthread_client_request,	NULL);  //处理客户请求 
	pthread_create(&id_refresh,		   	NULL,  	pthread_refresh,		NULL);  //将传感器数据刷新到数据库			
	pthread_create(&id_sqlite,		   	NULL, 	pthread_sqlite,			NULL);  //数据库线程
	pthread_create(&id_transfer,		NULL, 	pthread_transfer,		NULL);  //数据传输线程
	pthread_create(&id_sms,				NULL, 	pthread_sms,			NULL); 	//短信通知线程
	pthread_create(&id_buzzer,			NULL, 	pthread_buzzer,			NULL);	//蜂鸣器线程
	pthread_create(&id_led,				NULL, 	pthread_led,			NULL);  //LED线程

	
	pthread_join(id_client_request, 	NULL);   printf ("pthread1\n");
	pthread_join(id_refresh,			NULL);   printf ("pthread2\n");
	pthread_join(id_sqlite,				NULL);	 printf ("pthread3\n");
	pthread_join(id_transfer,			NULL);	 printf ("pthread4\n");
	pthread_join(id_sms,				NULL);	 printf ("pthread5\n");
	pthread_join(id_buzzer,				NULL);	 printf ("pthread6\n");
	pthread_join(id_led,				NULL);	 printf ("pthread7\n");

	return 0;
}


void release_pthread_resource(int signo)
{

	pthread_mutex_destroy (&mutex_client_request);   
	pthread_mutex_destroy (&mutex_refresh);   
	pthread_mutex_destroy (&mutex_sqlite);    
	pthread_mutex_destroy (&mutex_transfer);   
	pthread_mutex_destroy (&mutex_sms);   
	pthread_mutex_destroy (&mutex_buzzer);   
	pthread_mutex_destroy (&mutex_led);   

 	pthread_cond_destroy (&cond_client_request);
 	pthread_cond_destroy (&cond_refresh);
 	pthread_cond_destroy (&cond_sqlite);
 	pthread_cond_destroy (&cond_transfer);
 	pthread_cond_destroy (&cond_sms);
 	pthread_cond_destroy (&cond_buzzer);
 	pthread_cond_destroy (&cond_led);

	pthread_detach(id_client_request);
	pthread_detach(id_refresh);
	pthread_detach(id_sqlite);
	pthread_detach(id_transfer);
	pthread_detach(id_sms);
	pthread_detach(id_buzzer);
	pthread_detach(id_led);

	printf("all pthread is detached\n");
	
	msgctl (msgid, IPC_RMID, NULL);  //删除消息队列
	shmctl (shmid, IPC_RMID, NULL);  //删除共享内存
	semctl (semid, 1, IPC_RMID, NULL);  //删除信号量

	exit(0);
}



