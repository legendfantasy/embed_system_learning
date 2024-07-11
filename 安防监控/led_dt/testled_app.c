#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
 
#include <stdio.h>
 
#include "led.h"
 
int main(int argc,char *argv[])
{
	int fd = -1;
	int onoff = 0;
	int no = 0;
 
	if(argc < 4)
	{
		printf("The argument is too few\n");
		return 1;
	}
//-------------从参数中获取LED的开关状态和LED编号-------------------
	sscanf(argv[2], "%d", &onoff);
	sscanf(argv[3], "%d", &no);
 
	if(no < 2 || no > 5)
	{
		printf("len-no is invalid\n");
		return 2;
	}
 
	fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		printf("open %s failed\n",argv[1]);
		return 3;
	}
 
	if(onoff)
	{
		ioctl(fd, MY_LED_ON, no);
	}
	else
	{
		ioctl(fd, MY_LED_OFF, no);
	}
 
	close(fd);
	fd = -1;
	return 0;
}
