#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"linklist.h"

linklist linklist_create(void)
{
	linklist H = (linklist)malloc(sizeof(listnode));
	if(H == NULL)
	{
		printf("malloc failed!\n");
		return NULL;
	}
	//赋初值
	H->data = 0;
	H->next = NULL;
	return H;
}
int list_head_insert(linklist H, data_t value)
{
	if(H == NULL)
	{
		printf("no linklist\n");
	}
	linklist N = (linklist)malloc(sizeof(listnode));
	if(N == NULL)
	{
		printf("malloc failed\n");
		return -1;
	}
	N->data = value;
	p->next = H->next;
	H->next = p;
	return 0;
}
int list_tail_insert(linklist H, data_t value)
{
	if(H == NULL)
	{
		printf("no linklist\n");
	}
	linklist N = (linklist)malloc(sizeof(listnode));
	if(N == NULL)
	{
		printf("malloc failed!\n");
	}
	N->data = value;
	N->next = NULL;

	//找到尾结点
	linklist L = H;
	while(L->next != NULL)
	{
		L = L->next;
	}
	L->next = N;
	return 0
}
linklist list_get(linklist H, int pos)
{
	if(H == NULL)
	{
		printf("H is NULL\n");
		return NULL;
	}
	if(pos == -1)
	{
		return H;
	}
	linklist p = H;
	for(int i = 0; i < pos + 1; i++)
	{
		p = p->next;
		if(p == NULL)
		{
			printf("pos is invalid\n");
			return  NULL;
		}
	}
	return p;
}
int list_insert(linklist H, data_t value, int pos)
{
	if(H == NULL)
	{
		printf("H is NULL\n");
		return -1;
	}
	p = list_get(H, pos - 1);
	if(p == NULL)
	{
		return -1;
	}
	linklist q = (linklist)mallioc(sizeof(

