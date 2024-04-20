#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>

#define BIRD '@'
#define BLANK ' '
#define PIPE '+'

/*使用链表实现管道*/
typedef struct Pipe
{
	int x;              //列坐标
	int y;              //横坐标
	struct Pipe *next;
}Pipe_node, * Pipe_list;

Pipe_list head, tail;

void creat_list();      //创建链表
void show_pipe();       //显示管道
void clear_pipe();      //清除管道
void move_pipe();       //移动管道

int bird_y, bird_x;                                         //小鸟坐标
void show_bird();                                           //显示小鸟
void clear_bird();                                          //清除小鸟
void move_bird();                                           //移动小鸟

void init_curses();                                         //curses库初始化
int set_timer(int ms_t);                                    //设置定时器--ms
void handler(int sig);                                      //信号处理函数

int main(int argc, const char *argv[])
{
    /*初始小鸟的位置*/
	bird_y = 15;
	bird_x = 10;
	
    init_curses();
	signal(SIGALRM, handler);                               //系统捕捉定时器信号
	set_timer(500);                                         //500ms

	srand(time(0));                                         //随机种子
    creat_list();                                           //创建链表
	show_pipe();                                            //显示管道

	show_bird();                                            //显示小鸟
	move_bird();                                            //移动小鸟
	return 0;
}

/*curses库初始化*/
void init_curses()
{
	initscr();                                              //进入curses模式
	curs_set(0);                                            //禁止光标显示
	noecho();                                               //禁止输入字符显示
	keypad(stdscr,1);                                       //启动功能按键
	start_color();                                          //启动颜色机制
	init_pair(1, COLOR_WHITE, COLOR_RED);                    //小鸟颜色设置
	init_pair(2, COLOR_WHITE, COLOR_GREEN);                  //管道颜色设置
}

/*设置定时器--ms*/
int set_timer(int ms_t)
{
	struct itimerval timer;
	long t_sec,t_usec;
	int ret;

	t_sec = ms_t / 1000; //s
	t_usec = (ms_t % 1000) * 1000;//us

	timer.it_value.tv_sec = t_sec;
	timer.it_value.tv_usec = t_usec;//首次启动定时值

	timer.it_interval.tv_sec = t_sec;
	timer.it_interval.tv_usec = t_usec;//定时时间间隔

	ret = setitimer(ITIMER_REAL, &timer, NULL);
	return ret;
}

void handler(int sig)
{
	Pipe_list p, new;
	int i,j;

	/*小鸟下落*/
	clear_bird();
	bird_y++;
	show_bird();
	
    /*游戏结束判断*/
	if((char)inch() == PIPE)
	{
		set_timer(0);
		endwin();
		exit(1);
	}

	p = head->next;
	if(p->x == 0)
	{
		head->next = p->next;
		for(i = p->x; i < p->x+10; i++)
		{
			/*上半部分管道*/
			for(j=0; j<p->y; j++)
			{
				move(j,i);
				addch(BLANK);
			}
			/*下半部分管道创建*/
			for(j = p->y+5; j < 25; j++)
			{
				move(j,i);
				addch(BLANK);
			}
		refresh();
		}
		free(p);

		new = (Pipe_list)malloc(sizeof(Pipe_node));
		new->x = tail->x + 20;
		new->y = rand() % 11 + 5;
		new->next = NULL;
		tail->next = new;
		tail = new;
	}

	/*管道移动*/
	clear_pipe();
	move_pipe();
	show_pipe();
}

/*显示小鸟*/
void show_bird()
{
	attron(COLOR_PAIR(1));
	move(bird_y,bird_x);
	addch(BIRD);
	refresh();
	attroff(COLOR_PAIR(1));
}

void clear_bird()//清除小鸟
{
	move(bird_y,bird_x);
	addch(BLANK);
	refresh();
}

void move_bird()//移动小鸟
{
	char key;
	while(1)
	{
		key = getch();
		if(key == ' ')
		{
			clear_bird();
			bird_y--;
			show_bird();
			/*游戏结束判断*/
			if((char)inch() == PIPE)
			{
				set_timer(0);
				endwin();
				exit(1);
			}
		}
	}
}

void creat_list()//创建链表
{
	int i;
	Pipe_list p, new;

	head = (Pipe_list)malloc(sizeof(Pipe_node));    //申请内存，将首地址转化成pipelist类型再交给head
	head->next = NULL;								//初始化节点
	p = head;										//p指针指向开关准备操作

	for(i = 0; i < 5; i++)
	{
		new = (Pipe_list)malloc(sizeof(Pipe_node));	//申请新节点的内存
		new->x = (i + 1) * 20;						//20 40 60 80 100
		new->y = rand() % 11 + 5; // (5-15行)随机产生//0~11 + 5
		new->next = NULL;
		p->next = new;
		p = new;
	}
	tail = p;

}

void show_pipe()//显示管道  对链表进行复制
{
	Pipe_list p;
	int i,j;
	p = head->next;

	attron(COLOR_PAIR(2));   //设置颜色
	
	while(p)               
	{
		for(i = p->x; i < p->x + 10; i++)
		{
			/*上半部分管道*/
			for(j = 0; j < p->y; j++)
			{
				move(j,i);
				addch(PIPE);
			}
			/*下半部分管道创建*/
			for(j = p->y + 5; j < 25; j++)
			{
				move(j,i);
				addch(PIPE);
			}
		}
		refresh();
		p = p->next;
	}
	attroff(COLOR_PAIR(2));
}

/*清除管道*/
void clear_pipe()
{
	Pipe_list p;
	int i,j;
	p = head->next;
	while(p)
	{
		for(i = p->x; i < p->x+10; i++)
		{
			/*上半部分管道*/
			for(j=0; j<p->y; j++)
			{
				move(j,i);
				addch(BLANK);
			}
			/*下半部分管道创建*/
			for(j = p->y+5; j < 25; j++)
			{
				move(j,i);
				addch(BLANK);
			}
		}
		refresh();
		p = p->next;
	}

}

/*移动管道*/
void move_pipe()
{
	Pipe_list p;
	p = head->next;
	while(p)
	{
		p->x--;
		p = p->next;
	}

}