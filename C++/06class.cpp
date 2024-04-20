#include <iostream>
using namespace std;
class A
{
    private:
        int m_data;
    public:
        A(int data)
        {
            cout <<"A(int)" << endl;
            m_data = data;
        }
};
class B
{
    private:
        A m_a;
    public:
        B(void):m_a(123)
        {
            cout <<"B(void)" << endl;
        }
};
int main(void)
{
    B b; //一定会去构造成员对象m_a , 未指定如何构造，系统去调用m_a的无参构造函数
    return 0;
}