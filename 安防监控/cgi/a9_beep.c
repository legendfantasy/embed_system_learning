#include <stdio.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "cgic.h" 

#define N 8

struct msg    //消息队列结构体
{
	long type;  //消息类型,区分是哪个仓库
	long msgtype;  //消息类型，区分是哪个控制指令
	unsigned char text[N];  //消息内容，控制命令
};


int cgiMain() 
{ 
	key_t key;
	char buf[N];
	char sto_no[2];
	int msgid;

	struct msg msg_buf;
	memset(&msg_buf,0,sizeof(msg_buf));
	
	cgiFormString("beep", buf, N);  //获取表单数据，放于buf中，表示控制指令
	cgiFormString("store", sto_no, 2);  //获取表单数据  表示是哪个仓库，最后一个参数表示最大长度

	if((key = ftok("/tmp", 'g')) < 0)
	{
		perror("ftok");
		exit(1);
	}

	if((msgid = msgget(key, 0666)) < 0)
	{
		perror("msgget");
		exit(1);
	}
	//从网页获取控制的指令

	switch (buf[0])  
	{
		case '0':  
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (0 << 0);
				break;
			}
		case '1':
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (1 << 0);
				break;
			}
		case '2':
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (2 << 0);
				break;
			}
		case '3':
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (3 << 0);
				break;
			}
	}
	//填充消息内容
	msg_buf.type = 1L;  // 对应仓库1
	msg_buf.msgtype = 4L;  // 对应风扇控制
	//发送消息
	msgsnd(msgid, &msg_buf, sizeof(msg_buf) - sizeof(long), 0);  //发送消息

	sto_no[0] -= 48; //将字符转换为数字

	cgiHeaderContentType("text/html\n\n"); 
	fprintf(cgiOut, "<HTML><HEAD>\n"); 
	fprintf(cgiOut, "<TITLE>My CGI</TITLE></HEAD>\n"); 
	fprintf(cgiOut, "<BODY>"); 

	fprintf(cgiOut, "<H2>send sucess</H2>");

	fprintf(cgiOut, "<a href='.html'>返回</a>"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"1;url=../a9_zigbee%d.html\">", sto_no[0]);
	fprintf(cgiOut, "</BODY>\n"); 
	fprintf(cgiOut, "</HTML>\n"); 


	return 0; 
} 
