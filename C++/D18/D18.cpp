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
	for (vector<int>::size_type index = 0; index <ivec.size()-1; index+=2)
	{
		cout << ivec[index] + ivec[index + 1] << " ";
	}
	if (ivec.size() % 2)
	{
		cout << "有奇数个元素，最后一个元素为：" << ivec[ivec.size() - 1];
	}
	cout << endl;
	getchar();
	return 0;
}