#include <stdio.h>
int inc(int a)
{
    return (++a);
}
int multi(int *a, int *b, int *c)
{
    return (*c = *a++**b+++*a);
}
typedef int(*FUNC1)(int in);  
typedef int(*FUNC2)(int*, int*, int*);  
void show(FUNC2 fun, int arg1, int *arg2)
{
    FUNC1 p = &inc;  
    int temp = p(arg1);
    fun(&temp, &arg1, arg2);
    printf("%d\n", *arg2);
}
int main()
{
    int a;
    show(multi, 9, &a);
    return 0;
}
