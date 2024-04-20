#include <iostream>
using namespace std;
class A
{
    public:
        int m_data;
        A(int data = 0)
        {
            cout << "A(int)" << endl;
            m_data = data;
        }
        A(const A& that)
        {
        //拷贝构造函数
            cout << "A(const A&)" << endl;
            m_data = that.m_data;
        }
};
class B
{
    public:
        int m_data;
        A m_a;
};

int main(void)
{
    B b1; //创建b1对象，调用B的缺省构造函数，由于有`成员对象`m_a, 要对其进行初始化，所以会调用A的构造函数
    b1.m_a.m_data = 98;
    b1.m_data = 97;
    B b2 = b1; //调用B的缺省拷贝构造函数，由于有成员对象m_a, 所以会调用A的拷贝构造函数
    cout << b2.m_a.m_data << endl;
    cout << b2.m_data << endl;
    return 0;
}