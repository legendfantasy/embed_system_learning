#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"sqlist.h"

pnode list_create(void)
{
	pnode L = (pnode)malloc(sizeof(node));
	if(L == NULL)
	{
		printf("申请空间失败")
		retrurn -1;
	}
	memset(L, 0, sizeof(node));
	L->last = -1;
	return L;
}
int list_clear(pnode L)
{
	if(L == NULL)
	{
		printf("顺序表未创建\n");
		return -1;
	}
	memset(L, 0, sizeof(sqlist));
	L->last = -1;
	return 0;
}
int list_free(pnode L)
{
	if(L == NULL)
	{
		printf("顺序表未创建\n");
		return -1;
	}
	free(L);
	L = NULL;
	return 0;

}
int list_empty(pnode L)
{
	if(L == NULL)
	{
		printf("顺序表未创建\n");
		return -1;
	}
	if (L->last == -1)
		return 1;
	else 
		return 0;
}
int list_length(pnode L)
{
	if(L == NULL)
	{
		return -1;
	}
	return L->last+1;
}
int list_locate(pnode L, data_t value)
{
	if(L == NULL)
	{
		return -1;
	}
	for(int i = 0; i <= L->last; i++)
	{
		if(value == L->data[i])
		{
			pos = i + 1;
			return pos;
		}
		else
		{
			return -1;
		}
	}

}
int list_insert(pnode L, data_t value, int p)
{
	if(L == NULL)
		return -1;
	//check wheather the list is full 
	if((L->last +1) == N)
	{
		printf("the list is full\n");
		return -1;
	}
//要对输入的参数进行检查
	if(pos < 0 || pos > L->last+1)
	{
		printf("Pos is invalid\n");
		retun -1;
	}
//要对表中的元素进行调整
	for(int i = L->last; i >= pos; i--)
	{
		
		L->data[i+1] = L->data[i];
	}
	L->data[pos] = value;
	L->last = L->last + 1;
	
	return 0; 
}
int list_show(pnode L)
{
	if(L == NULL)
		retrurn -1;
	if(L->last == -1)
		return -1;
	for(int i = 0; i <= N; i++)
	{
		printf("item %d | value %d \n", i, L->data[i]);
	}
	return 0;
}
int list_itemdelete(pnode L, int pos)
{
	if(L->last == -1)
	{
		printf("list is empty\n");
		return -1;
	}
	for(int i = pos + 1; i <= L->last; i++)
	{
		L->data[i-1] = L->data[i];
	}
	L->last--;
	return 0;
}
int list_merge(pnode L1, pnode L2)
{
	for(int i = 0; i <= L2->last, i++)
	{
		int ret = list_locate(L1, L2->data[i]);
		if(ret == -1)
		{
			if(list_insert(L1, L2->data[i], L->last + 1) == -1)
			return -1;
		}
	}
	return 0;
}
int list_purge(pnode L)
{
	if(L->last == 0)
		return 0;
	for(int i = 1; i <= L->last; i++)
	{
		for(int j = i - 1; j >= 0; j--)
		{
			if(L->data[i] == L->data[j])
			{
				list_delete(L, i);
				break;
			}
		}
	}


}



