#include <iostream>
#include <cstring>
using namespace std;
class String
{
    public:
        String(const char *str)    //类型转换构造函数
        {
            this->str = new char[strlen(str) + 1];
            strcpy(this->str, str);
        }
        String(const String &that)  //拷贝构造函数
        {
            str = new char[strlen(that.str) + 1];
            strcpy(str, that.str);
        }
        void print(void)
        {
            cout << str << endl;
        }
        String &operator=(const String &that)  //拷贝赋值
        {
            if(this != &that)
            {
                /*释放掉this对象原来的堆空间*/
                delete [] str;
                str = new char[strlen(that.str) + 1];
                strcpy(str, that.str);
            }
            return *this;
        }
        ~String()   //析构函数
        {
            delete []str;
        }
        const char *c_str()  //string::c_str
        {
            return str;
        }
    private:
        char *str;
};

int main(void)
{
    String s1 = "hello"; //类型转换构造函数,接受字符串
    s1.print();
    String s2 = s1; //拷贝构造函数
    s2.print();
    String s3 = "world"; //类型转换构造函数
    s2 = s3; //拷贝赋值
    s2.print();
    cout << s3.c_str() << endl; //string:: c_str
    return 0;
}