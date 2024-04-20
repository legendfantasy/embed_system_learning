#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <linux/input.h>
#include <aio.h>

#include "vser.h"

void aiow_completion_handler(sigval_t sigval)
{
	int ret;
	struct aiocb *req;

	req = (struct aiocb *)sigval.sival_ptr;

	if (aio_error(req) == 0) {
		ret = aio_return(req);
		printf("aio write %d bytes\n", ret);
	}

	return;
}

void aior_completion_handler(sigval_t sigval)
{
	int ret;
	struct aiocb *req;

	req = (struct aiocb *)sigval.sival_ptr;

	if (aio_error(req) == 0) {
		ret = aio_return(req);
		if (ret)
			printf("aio read: %s\n", (char *)req->aio_buf);
	}

	return;
}

int main(int argc, char *argv[])
{
	int ret;
	int fd;
	struct aiocb aiow, aior; //异步IO控制块，相当于定义两个操作读写的对象

	fd = open("/dev/vser0", O_RDWR);
	if (fd == -1) 
		goto fail;
	
	//初始化异步IO控制块
	//清空其中的内容
	memset(&aiow, 0, sizeof(aiow));  
	memset(&aior, 0, sizeof(aior));
	
	//给读异步IO控制块配置参数
	aiow.aio_fildes = fd;   // 传入文件描述符
	aiow.aio_buf = malloc(32);  // 传入申请32字节的空间的地址作为数据缓冲区
	strcpy((char *)aiow.aio_buf, "aio test");  //要把aio_buf的类型先转换为char*，然后再赋值
	aiow.aio_nbytes = strlen((char *)aiow.aio_buf) + 1; // 传入数据长度
	aiow.aio_offset = 0;  //偏移量
	aiow.aio_sigevent.sigev_notify = SIGEV_THREAD;  // 通知方式 SIGEV_THREAD表示使用线程
	aiow.aio_sigevent.sigev_notify_function = aiow_completion_handler;  // 通知的回调函数
	aiow.aio_sigevent.sigev_notify_attributes = NULL;  // 通知的属性
	aiow.aio_sigevent.sigev_value.sival_ptr = &aiow;  // 通知的参数sigval联合体的一个成员用于在发送信号时传递额外的数据
	// 给写异步IO控制块配置参数
	aior.aio_fildes = fd;  
	aior.aio_buf = malloc(32);
	aior.aio_nbytes = 32;
	aior.aio_offset = 0;
	aior.aio_sigevent.sigev_notify = SIGEV_THREAD;
	aior.aio_sigevent.sigev_notify_function = aior_completion_handler;
	aior.aio_sigevent.sigev_notify_attributes = NULL;
	aior.aio_sigevent.sigev_value.sival_ptr = &aior;

	while (1) 
	{
		if (aio_write(&aiow) == -1)
			goto fail;
		if (aio_read(&aior) == -1)
			goto fail;
		sleep(1);
	}

fail:
	perror("aio test");
	exit(EXIT_FAILURE);
}
