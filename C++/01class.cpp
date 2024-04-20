#include<iostream>
#include<cstdio>
using namespace std;
class arry
{
    private:
            int a[8];
    public:
    void out(int *acode)
    {
        for(int i = 0; i < 8; i++)
        {
            cout << a[i] << endl;
        }
    }
    void in(int *a)
    {
        for(int i = 0; i < 8; i++)
        {
            cin >> a[i] >> endl;
        }
    }
    int sum(int *a)
    {
        int sum = 0;
        for(int i = 0; i < 8; i++)
        {
            sum += a[i];   
        }
        return sum;
    }
    int *sort(int *a)
    {

    }
}
int main()
{
    
}