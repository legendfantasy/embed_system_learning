#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;
class A
{
    private:
        int array[8];
    public:
        A()
        {
            bzero(array, sizeof(array));
        }
        void output(void);
        void input(void);
        int sum(void);
        void sort(void);
};
void A::input()
{
    for(int i = 0; i < 8; i++)
    {
        cin >> array[i];
    }
}
void A::output()
{
    for(int i = 0; i < 8; i++)
    {
        cout << array[i] << endl;
    }
}
int A::sum(void)
{
    int sum = 0;
    for(int i = 0; i < 8; i++)
    {
        sum += array[i];
    }
    return sum;
}
void A::sort(void)
{
    std::sort(array, array + 8);
    for(int i = 0; i < 8; i++)
    {
        cout << array[i] << endl;
    }
}
int main()
{
    class A aa;
    aa.input();
    aa.output();
    cout << aa.sum() << endl;
    aa.sort();
}