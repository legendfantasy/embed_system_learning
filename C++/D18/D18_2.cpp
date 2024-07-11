
#include <iostream>
#include <vector>
 
using namespace std;
 
int main()
{
	vector<int> ivec;
	int data;
	while (cin >> data)
	{
		ivec.push_back(data);
	}
	for (vector<int>::size_type index = 0; index != ivec.size() / 2; ++index)
	{
		cout << ivec[index] + ivec[ivec.size() - 1 - index] << " ";
	}
	if (ivec.size() % 2)
	{
		cout << endl;
		cout << "有奇数个元素，中间的元素为：" << ivec[ivec.size() / 2];
	}
	cout << endl;
	getchar();
	return 0;
 
 
}