#include "data_global.h"

#define QUEUE_MSG_LEN 32
extern sig_atomic_t should_exit;
extern struct thread_mutex mtx_pjsg;
extern struct thread_cond cond_pjsg;

struct IPC_id id_pjsg;
struct IPC_key key_pjsg;
struct msg msgbuf;  // 消息结构体


unsigned char cmd_fan; //定义字符串保存风扇控制命令
unsigned char cmd_led; //定义字符串保存LED控制命令



extern void cleanup_handle(void *arg);
void *pthread_web_client_handle(void *arg)
{
    pthread_cleanup_push(cleanup_handle, NULL)
    if((key_pjsg.msg_key = ftok("/tmp",'g')) < 0)  // 获取一个key
	{
		perror("ftok failed .\n");
		exit(-1);
	}

	id_pjsg.msgid = msgget(key_pjsg.msg_key, IPC_CREAT|IPC_EXCL|0666);  // 创建消息队列
	if(id_pjsg.msgid == -1)	
	{
		if(errno == EEXIST)  // 消息队列已经存在
		{
			id_pjsg.msgid = msgget(key_pjsg.msg_key, 0777);  // 获取消息队列的id
		}
		else
		{
			perror("fail to msgget");
			exit(1);
		}
	}
	printf("process web_client_request begin >>>>>>>\n");
	
	while(!should_exit)
	{
		bzero(&msgbuf, sizeof(msgbuf));  // 清空消息结构体
		printf("wait form client request...\n");
		
		msgrcv(id_pjsg.msgid, &msgbuf, sizeof(msgbuf) - sizeof(long), 1L, 0);  // 接收消息只接收类型为 1 的消息
		
		printf("Get %ldL msg\n", msgbuf.msgtype);
		printf("text[0] = %#x\n", msgbuf.text[0]);

		switch(msgbuf.msgtype)
		{
			case 1L:
					pthread_mutex_lock(&mtx_pjsg.mtx_led);
					printf("//////led////////\n");
					cmd_led = msgbuf.text[0];  //将网页发送过来的消息队列中的text段存入cmd_led变量中
					pthread_mutex_unlock(&mtx_pjsg.mtx_led);
					pthread_cond_signal(&cond_pjsg.cond_led);
					break;
			case 2L:
					printf("hello beep\n");
					break;
			case 3L:
					printf("hello seg\n");
					break;
			case 4L:
			  		pthread_mutex_lock(&mtx_pjsg.mtx_fan);
					printf("/////fan start//////\n");
					//memcpy(cmd_fan, msgbuf.text, QUEUE_MSG_LEN);  //将网页发送过来的消息队列中的text段存入fan_cmd变量中
					pthread_mutex_unlock(&mtx_pjsg.mtx_fan);
					pthread_cond_signal(&cond_pjsg.cond_fan);
					break;
			case 5L:
			case 6L:
			case 7L:
			case 8L:
			case 9L:
			case 10L:
			default:
				break;
				
		}
	}
    printf("pthread_web_client_handle exit\n");
    pthread_cleanup_pop(0);
    
    return NULL;
}