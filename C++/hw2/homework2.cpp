#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;
class Date
{
    private:
        int year;
        int month;
        int day;
    public:
        Date(int y, int m, int d)
        {   
            year = y;
            month = m;
            day = d;
        }
        Date()
        {
            year = 0;
            month = 0;
            day = 0;
        }
        explicit Date(int n)
        {
            year = n / 10000;
            month = (n % 10000) / 100;
            day = n % 100;
        }

        void print() const 
        {
            cout << year << month << day << endl;
        }
};

int main()
{
    
    class Date d1(2023, 03, 25);
    class Date d2 = Date(20230325);
    class Date d3;
    d1.print();
    d2.print();
    d3.print();
}