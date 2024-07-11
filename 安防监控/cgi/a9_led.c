#include <stdio.h> 
#include "cgic.h" 
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

#define N 8

struct msg
{
	long type;   //用于判断的消息类型
	long msgtype;  //具体的消息类型
	unsigned char text[N];  //消息内容 1个字节
};


int cgiMain() 
{ 

	key_t key;
	char buf[N];  //接收表单中led的值
	char sto_no[2];  //接收表单中store的值

	int msgid;

	// 定义消息队列结构体msg_buf，并初始化为0
	struct msg msg_buf;
	memset(&msg_buf, 0, sizeof(msg_buf));  // 初始化消息队列

	// 从表单中获取名为"led"的字段值，存入buf中,最多接收N-1个字符
	cgiFormString("led", buf, N);
	// 从表单中获取名为"store"的字段值，存入sto_no中
	cgiFormString("store", sto_no, 2);  //表示仓库号

	// 使用ftok函数生成唯一的key值
	if((key = ftok("/tmp", 'g')) < 0)  // 'g' 用以生成一个唯一的key值
	{
		// 如果生成失败，输出错误信息并退出程序
		perror("ftok");
		exit(1);
	}
	// 使用msgget函数获取消息队列
	if((msgid = msgget(key, 0666)) < 0)  // 获取消息队列
	{
		// 如果获取失败，输出错误信息并退出程序
		perror("msgget");
		exit(1);
	}

	// 初始化消息队列中的text
	bzero (msg_buf.text, sizeof(msg_buf.text));  // 初始化消息队列中的text

	// 根据buf的值设置msg_buf.text[0]的值
	if (buf[0] == '1')  //html发来的是打开,总开关
	{
		msg_buf.text[0] = (1 << 6) | (0 << 4) | (1 << 0);    //0x41
		// 01 00 0001 ----- A9平台上的LED设备执行0001命令(全部打开)
	}
	else
	{
		msg_buf.text[0] = (1 << 6) | (0 << 4) | (0 << 0);    //0x40
		// 01 00 0000 ----- A9平台上的LED设备执行0000命令(全部关闭)
	}

	// 设置消息队列类型
	msg_buf.type = 1L;  //对应发给仓库1的消息
	// 设置消息类型
	msg_buf.msgtype = 1L;  //对应LED控制线程
	// 发送消息到消息队列
	msgsnd(msgid, &msg_buf, sizeof(msg_buf) - sizeof(long), 0);  //通常，消息缓冲区的最后一部分会用于存放消息类型，因此从总大小中减去一个 long 的大小

	// 将sto_no[0]的值减48（ASCII码转换）
	sto_no[0] -= 48;

	// 设置响应头信息
	cgiHeaderContentType("text/html\n\n"); 
	// 输出HTML头部信息
	fprintf(cgiOut, "<HTML><HEAD>\n"); 
	// 设置网页标题
	fprintf(cgiOut, "<TITLE>My CGI</TITLE></HEAD>\n"); 
	// 输出HTML主体部分
	fprintf(cgiOut, "<BODY>"); 
	// 显示发送成功的消息
	fprintf(cgiOut, "<H2>send sucess</H2>");
	// // 输出返回链接（该行代码被注释掉了）
	// fprintf(cgiOut, "<a href='.html'>返回</a>"); 
	// 设置1秒后自动跳转到指定URL
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"1;url=../a9_zigbee%d.html\">", sto_no[0]);
	// 输出HTML尾部信息
	fprintf(cgiOut, "</BODY>\n"); 
	fprintf(cgiOut, "</HTML>\n"); 

	return 0; 
} 
