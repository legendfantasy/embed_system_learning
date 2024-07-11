#include "data_global.h"
#include "sem.h"

#define N 1024  //for share memory

// extern int shmid;  //share memory id
// extern int msgid;  //message queue id
// extern int semid;  //semaphore id

// extern key_t shm_key;  //share memory key
// extern key_t sem_key;  //semaphore key
// extern key_t key; //msg_key
struct IPC_id id_pjsg;
struct IPC_key key_pjsg;

extern sig_atomic_t should_exit;
extern void cleanup_handle(void *arg);

struct shm_addr  //记录共享内存的状态和地址
{
    char shm_status;    //shm_status可以等于home_id，用来区分共享内存数据
    struct env_info_client_addr  sm_all_env_info;
};

struct shm_addr *shm_buf;

int file_env_info_struct(struct env_info_client_addr *rt_status, int home_id);

void *pthread_data_refresh(void *arg)
{
	pthread_cleanup_push(cleanup_handle, NULL)
	//---------使用信号量来控制共享内存的访问----------
	// 使用 ftok 生成用于访问资源限制的信号量键值
	//semaphore for access to resource limits
	if((key_pjsg.sem_key = ftok("/tmp", 'i')) < 0)
	{
		perror("ftok failed .\n");
		exit(-1);
	}
	// 使用信号量键值创建或获取信号量
	id_pjsg.semid = semget(key_pjsg.sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if(id_pjsg.semid == -1)	
	{
		// 如果信号量已存在，则获取该信号量
		if(errno == EEXIST)
		{
			id_pjsg.semid = semget(key_pjsg.sem_key, 1, 0777);
		}
		else
		{
			perror("fail to semget");
			exit(1);
		}
	}
	else
	{
		// 初始化信号量
		init_sem(id_pjsg.semid, 0, 1);  //初始化一个信号量，编号为0初始值为1
	}
	
	//---------使用共享内存来刷新环境信息----------
	// 使用 ftok 生成用于 env_info 刷新配置的共享内存键值
	if((key_pjsg.shm_key = ftok("/tmp", 'i')) < 0)
	{
		perror("ftok failed .\n");
		exit(-1);
	}
	// 使用共享内存键值创建或获取共享内存
	id_pjsg.shmid = shmget(key_pjsg.shm_key, N, IPC_CREAT | IPC_EXCL | 0666);
	if(id_pjsg.shmid == -1)	
	{
		// 如果共享内存已存在，则获取该共享内存
		if(errno == EEXIST)
		{
			id_pjsg.shmid = shmget(key_pjsg.msg_key, N, 0777);  //获取N字节的共享内存
		}
		else
		{
			perror("fail to shmget");
			exit(1);
		}
	}

	//将共享内存附加到进程的地址空间
	//share memap
	if((shm_buf = (struct shm_addr *)shmat(id_pjsg.shmid, NULL, 0)) == (void *)-1)  
	// shmat(shmid, NULL, 0)将会把系统推荐的共享内存附加到进程的地址空间，并返回一个指向该共享内存的指针
	{
		perror("fail to shmat");
		exit(1);
	}

	printf("pthread_data_refresh start>>>>>>>\n");
	
	// 将共享内存缓冲区清零
	bzero (shm_buf, sizeof(struct shm_addr));
	// 循环执行以下操作
	while(!should_exit)
	{
		// 获取信号量
		sem_p(id_pjsg.semid, 0);  //sem_p(semid, 0)将会阻塞进程，直到信号量semid的值为1
		// 设置共享内存中的状态标志为1
		shm_buf->shm_status = 1;
		// 调用 file_env_info_struct 函数处理共享内存中的 env_info
		file_env_info_struct(&shm_buf->sm_all_env_info, shm_buf->shm_status);
		// 休眠1秒
		sleep(1);
		// 释放信号量
		sem_v(id_pjsg.semid, 0);  //sem_v(semid, 0)将会释放信号量semid的值为1
	}
	printf("pthread_web_client_handle exit\n");
    pthread_cleanup_pop(0);
    return NULL;
}



int file_env_info_struct(struct env_info_client_addr *rt_status, int home_id)
//该函数用于将环境信息填充到env_info_struct结构体中
{
	int env_info_size = sizeof(struct env_info_client_addr);
	//printf("env_info_size = %d.\n", env_info_size);

	rt_status->monitor_no[home_id].zigbee_info.temperature = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMIN = 2.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMAX = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidity  = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMIN  = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMAX  = 30.0;
	rt_status->monitor_no[home_id].zigbee_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].zigbee_info.reserved[1]  = -0.01;


	rt_status->monitor_no[home_id].a9_info.adc  = 9.0;
	rt_status->monitor_no[home_id].a9_info.gyrox  = -14.0;
	rt_status->monitor_no[home_id].a9_info.gyroy  = 20.0;
	rt_status->monitor_no[home_id].a9_info.gyroz  = 40.0;
	rt_status->monitor_no[home_id].a9_info.aacx  = 642.0;
	rt_status->monitor_no[home_id].a9_info.aacy  = -34.0;
	rt_status->monitor_no[home_id].a9_info.aacz  = 5002.0;
	rt_status->monitor_no[home_id].a9_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].a9_info.reserved[1]  = -0.01;
	
	return 0;
}

