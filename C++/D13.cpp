#include <iostream>

class Integer {
public:
    int value;

    Integer(int value) : value(value) {}

    // 使用友元函数重载逻辑非运算符
    friend bool operator!(const Integer& obj);
};

bool operator!(const Integer& obj) {
    return obj.value == 0;
}

int main() {
    Integer obj(0);
    if (!obj) {
        std::cout << "obj的值为0" << std::endl;
    } else {
        std::cout << "obj的值不为0" << std::endl;
    }

    return 0;
}
