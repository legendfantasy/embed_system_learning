#include <iostream>
#include <algorithm>

class MyArray {
private:
    int arr[8];

public:
    void output();
    void input();
    int sum();
    void sort();
};

void MyArray::output() {
    for(int i = 0; i < 8; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

void MyArray::input() {
    std::cout << "请输入8个整数：" << std::endl;
    for(int i = 0; i < 8; i++) {
        std::cin >> arr[i];
    }
}

int MyArray::sum() {
    int total = 0;
    for(int i = 0; i < 8; i++) {
        total += arr[i];
    }
    return total;
}

void MyArray::sort() {
    std::sort(arr, arr + 8);
}