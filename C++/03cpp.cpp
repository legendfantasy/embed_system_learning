#include <iostream>
using namespace std;
#include <cstdio>

int main(void)
{
    int x, y, z;
#if 0
    scanf("%d %d %d", &x, &y, &z);
    printf("%d %d %d\n", x, y, z);
#endif
    cin >> x >> y >> z;
    cout << x << " " <<
    y <<
    " " << z << endl;
    return 0;
}