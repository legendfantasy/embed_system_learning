#include<stdio.h>
int main()
{
	int m = 100;
	int *p;
	p = &m;
	printf("p的值是:%p,p+1的值是:%p", p, p+1);
	printf("p指向的变量的值是:%d,p+1指向的变量的值是:%d", *p, *(p+1));
	return 0;

}
