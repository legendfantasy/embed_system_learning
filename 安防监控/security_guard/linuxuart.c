//实现串口通信
/*
函数：
	set_com_config  设置串口配置
	open_port 打开串口
	USB_UART_Config 配置USB串口
*/

#include "linuxuart.h"

int set_com_config(int fd, int baud_rate, int data_bits, char parity, int stop_bits)
{
	struct termios new_cfg, old_cfg;
	int speed;
	/*保存原有串口配置*/
	if (tcgetattr(fd, &old_cfg) != 0)  //tcgetattr 获取串口配置	
	{
		perror("tcgetattr");
		return -1;
	}
	new_cfg = old_cfg;

	/*配置为原始模式*/
	cfmakeraw(&new_cfg);  //cfmakeraw 配置为原始模式
	new_cfg.c_cflag &= ~CSIZE;  //清空数据位设置

	/*设置波特率*/
	switch (baud_rate)
	{
		case 2400:{
					  speed = B2400;
					  break; 
				  }
		case 4800:{
					  speed = B4800;
					  break;
				  }
		case 9600:{
					  speed = B9600;
					  break;
				  }
		case 19200:{
					   speed = B19200;
					   break;
				   }
		case 38400:{
					   speed = B38400;
					   break;
				   }

		default:
		case 115200:{
						speed = B115200;
						break;
					}
	}

	cfsetispeed(&new_cfg, speed);  //设置一下输入波特率
	cfsetospeed(&new_cfg, speed);  //设置一下输出波特率

	/*设置数据位*/
	switch (data_bits)
	{
		case 7:{
				   new_cfg.c_cflag |= CS7;
				   break;
			   }   
		default:	
		case 8:{
				   new_cfg.c_cflag |= CS8;  
				   break;
			   }
	}

	/*设置奇偶校验位*/
	switch (parity)
	{
		default:
		case 'n':
		case 'N':{
					 new_cfg.c_cflag &= ~PARENB;  //清除PARENB位
					 new_cfg.c_iflag &= ~INPCK;   //清除INPCK位
					 break;
				 }
		case 'o':
		case 'O':{
					 new_cfg.c_cflag |= (PARODD |PARENB);  //设置奇校验
					 new_cfg.c_iflag |= INPCK;  //设置输入校验
					 break;
				 }
		case 'e':
		case 'E':{
					 new_cfg.c_cflag |= PARENB;  //设置偶校验
					 new_cfg.c_cflag &= ~PARODD;  //清除PARODD位
					 new_cfg.c_iflag |= INPCK;  //设置输入校验
					 break;
				 }
		case 's':
		case 'S':{
					 new_cfg.c_cflag &= ~PARENB;  //清除PARENB位
					 new_cfg.c_cflag &= ~CSTOPB;  //清除CSTOPB位
					 break;
				 }
	}

	/*设置停止位*/
	switch (stop_bits)
	{
		default:
		case 1:{
				   new_cfg.c_cflag &= ~CSTOPB;
				   break;
			   }   	
		case 2:{
				   new_cfg.c_cflag |= CSTOPB;
				   break;
			   }
	}

	/*设置等待时间和最小接收字符*/
	new_cfg.c_cc[VTIME] = 0;
	new_cfg.c_cc[VMIN] = 1;
	tcflush(fd, TCIFLUSH);   //刷新了一下　接收和发出　缓冲区　
	if ((tcsetattr(fd, TCSANOW, &new_cfg)) != 0)   //用于设置终端参数    
	{
		perror("tcsetattr");
		return -1;
	}

	return 0;
}


int open_port(char *com_port)
{
	int fd;

	// 打开串口
	fd = open(com_port, O_RDWR|O_NOCTTY|O_NDELAY);
	if (fd < 0)
	{
		perror("open serial port");
		return -1;
	}
	// 恢复串口阻塞状态
	if (fcntl(fd, F_SETFL, 0) < 0)  //F_SETFL 改变文件描述符的阻塞模式
	{   //改变一下文件属性　
		perror("fcntl F_SETFL\n");
	}
	// 判断是否为终端设备
	if (isatty(fd) == 0)  //isatty()函数用于判断文件描述符fd是否为终端设备	
	{
		perror("This is not a terminal device");
	}

	return fd;
}


/*--------------------CH340Ƥ׃---------------------------*/
void USB_UART_Config(char* path, int baud_rate)
{
	int fd;
	// 打开串口
	fd = open_port(path);
	if(fd < 0)
	{
		// 打开失败，输出错误信息
		printf("open %s failed\n",path);
		return ;
	}
	// 设置串口配置
	if (set_com_config(fd, baud_rate, 8, 'N', 1) < 0)
	{
		// 设置失败，输出错误信息
		perror("set_com_config");
		return ;
	}
	// 关闭串口
	close(fd);
	return ;
}


