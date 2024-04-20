#include <iostream>
#include <cstring>
using namespace std;
class Integer
{
    private:
        int m_i ;
    public:
        Integer(void)
        {
            cout << "Integer(void)" << endl;
            m_i = 0;
        }
        Integer(int n)
        { //类型转换构造函数
            cout << "Integer(int)" <<endl;
            m_i = n;
        }
        Integer(const char * c)
        {
            cout << "Integer(const char*)" <<endl;
            m_i = strlen(c);
        }
        void print(void)
        {
            cout << m_i << endl;
        }
};

int main(void)
{
    Integer a;
    a.print();

    Integer b = 1; //编译器会找参数为int类型的构造函数
    b.print();

    Integer c = "abc";
    c.print();
    return 0;
}