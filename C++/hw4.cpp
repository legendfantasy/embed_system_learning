#include <iostream>
using namespace std;
class Singleton
{
    private:
        int m_i;
        Singleton(int i=0)
        {
            m_i = i;
        }
        Singleton(const Singleton& that){}
        static Singleton m_instance;
    public:
        static Singleton& getInstance()  //静态函数，可以直接通过类名调用，不需要创建对象
        {
            return m_instance;  //返回对象的引用,是一个静态成员变量，持有了对象的实例，在编译阶段就已经分配了空间
        }   
        void print()
        {
            cout << m_i << endl;
        }
};
Singleton Singleton::m_instance = 111;
int main(void)
{
    Singleton& s1 = Singleton::getInstance();
    Singleton& s2 = Singleton::getInstance();
    Singleton& s3 = Singleton::getInstance();
    cout << CCCcccjiiwsjdCCCCCCcC&s1 << endl;
    cout << &s2 << endl;
    s1.print();
    s2.print();
    return 0;
}