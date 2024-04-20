#include <iostream>
#include <string>
using namespace std;
class Student
{
    private:
        string m_name;
        int m_age;
        float m_score;
    public:
        Student();  //构造函数的重载
        Student(const string& name, int age, float score);
        void setname(const string& name);
        void setage(int age);
        void setscore(float score);
        void show();
};
Student::Student()  //构造函数，完成清零操作
{
    m_name = "";
    m_age = 0;
    m_score = 0.0;
}
Student::Student(const string& name, int age, float score)
{
    m_name = name;
    m_age = age;
    m_score = score;
}
void Student::setname(const string& name)
{
    m_name = name;
}
void Student::setage(int age)
{
    m_age = age;
}
void Student::setscore(float score)
{
    m_score = score;
}
void Student::show()
{
    if(m_name.empty() || m_age <= 0)
    {
        cout << "Invalid data!" << endl;
    }
    else
    {
        cout << "Name: " << m_name << endl;
        cout << "Age: " << m_age << endl;
        cout << "Score: " << m_score << endl;
    }
}
int main()
{
    Student stu("Tom", 18, 90.5);  //
    stu.show();

    Student *pstu = new Student();  // 使用构造函数Student()，只要是创建了构造函数就必须使用
    pstu->show();

    delete pstu;

    return 0;
}