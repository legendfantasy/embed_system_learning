#include <iostream>
using namespace std;

class Complex
{
    private:
        double r;
        double i;
    public:
        Complex(double r, double i)
        {
            this->r = r;
            this->i = i;
        }
        void print(void)
        {
            cout << r << " + " << i << "i" << endl;
        }
        
        const Complex operator+(const Complex &c)  //对+运算符进行重载，传入参数为对象c的引用
        {
            Complex tmp(r+c.r, i+c.i);  //返回一个对象, 对象的r为调用类的r+传入参数的r
            return tmp;  //返回一个对象
        }
        friend const Complex operator-(const Complex &l , const Complex &r);  //友元函数，对-运算符进行重载
};
const Complex operator-(const Complex &l, const Complex &r)  //传入两个对象的引用
{
    Complex tmp(l.r - r.r, l.i - r.i);
    return tmp;
}
int main(void)
{
    Complex a(1,2);
    Complex b(3,4);
    a.print();
    b.print();
    
    Complex c = a + b;// a.operator+(b);
    // a和b都是对象
    //对象a的operator+方法，传入参数为对象b
    c.print();
    Complex d = c - a; // operator-(c, a);
    d.print();
    
    return 0;
}