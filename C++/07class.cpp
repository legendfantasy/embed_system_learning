#include<iostream>
using namespace std;
class CTest
{
    public:
        CTest(int n){
            cout<<"CTest(int n)"<<endl;
        }
        CTest(int n, int m){
            cout<<"CTest(int n, int m)"<<endl;
        }
        CTest(){
            cout<<"CTest()"<<endl;
        }
};
int main()
{
    CTest array1[3] = {1, CTest(2)};
    CTest array2[3] = {CTest(2,3), CTest(1,2), 1};
    CTest *parray[3] = {new CTest(4), new CTest(1, 2)};
}