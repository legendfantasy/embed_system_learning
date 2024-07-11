#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <errno.h>

#include "leddrv.h"


#define LED_DEVICE "/dev/mydev"
#define MAX_BUFFER_SIZE PIPE_BUF

int main(int argc, const char *argv[])
{
	int i = 0, j = 3;
	int nread;
	int led_control,led_state;
	int led_fd;
	led_desc_t led;
	char *data;
/*******************CGI先打开LED设备***************/
	led_fd = open(LED_DEVICE, O_RDWR);
	if(led_fd < 0)
	{
		printf("open failed !\n");
	}
	printf("open device success! led_fd: %d\n", led_fd);  //输出设备文件描述符

/******************向网页中输出内容****************/
	printf("Content-type: text/html;charset=utf-8\n\n"); //这行代码告诉浏览器，我们输出的内容是html格式的
	printf("<html>\n");//输出html的开始标签
	printf("<head><title>cgi control led web</title></head>\n");//输出html的head标签
	printf("<body>\n");//输出html的body标签
	printf("<p>led is setted successful! you can watch the led's change</p>\n");//输出html的body标签
	printf("<a href=\"/led.html\">go back led control page </a>");//输出html的body标签
	printf("</body>\n");//输出html的body标签
/**************CGI从网页中获取数据****************/
    data = getenv("QUERY_STRING");   //getenv()读取环境变量的当前值的函数，QUERY_STRING是环境变量，
    if(sscanf(data, "led_control=%d&led_state=%d", &led_control, &led_state) != 2)
    {   //利用sscnaf（）函数的特点将环境变量分别提取出led_control和led_state这两个值
        printf("<p>please input right"); 
        printf("</p>");
    } 
    printf("<p>led_control = %d,led_state =  %d</p>", led_control, led_state);
    if(led_control < 2 || led_control > 5) 
	{ 
        printf("<p>Please input 2<=led_control<=5!"); 
        printf("</p>");
    } 
    if(led_state>1) 
	{
        printf("<p>Please input 0<=led_state<=1!"); 
        printf("</p>"); 
    }

	led.led_num   = led_control;
	led.led_state = led_state;
/***************CGI控制LED***************/
/****相当于驱动的应用程序*****/
	if(led.led_state == 0)  //如果led_state为0，则关闭LED
	{
		ioctl(led_fd, MY_LED_OFF, &led);
	}
	else if(led.led_state == 1)  //如果led_state为1，则打开LED
	{
		ioctl(led_fd, MY_LED_ON, &led);
	}
	else if(led.led_state == 2)  //如果led_state为2，则闪烁LED
	{
		while(j --)  //循环5次
		{
			for(i = 2; i <= 5; i ++ )
			{
				led.led_num = i;   //2-5号LED闪烁
				led.led_state = 0;
				ioctl(led_fd, MY_LED_OFF, &led);
				usleep(500000);  //延时50ms
				led.led_state = 1;
				ioctl(led_fd, MY_LED_ON, &led);
				usleep(500000);
			}
		}
	}

	close(led_fd);
    printf("</html>\n");
	return 0;
}
