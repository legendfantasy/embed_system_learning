#ifndef _NET_H_
#define _NET_H_

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#define BACKLOG 5

#define ErrExit(msg) do { \
	fprintf(stderr, "[%s:%d] ", __FUNCTION__, __LINE__); \ //打印出错的函数名和行号
	perror(msg); \  //打印错误信息
	exit(EXIT_FAILURE); } while(0)  //退出程序

typedef struct sockaddr Addr;  //通用地址结构体
typedef struct sockaddr_in Addr_in;  //IPv4地址结构体

void Argment(int argc, char *argv[]);  //检查参数
int SocketInit(char *addr, char *port, bool server);  //创建套接字并发起连接请求

ssize_t Read(int fd, void *buf, size_t len);  //读取数据
ssize_t Write(int fd, const void *buf, size_t len);  //发送数据

#endif