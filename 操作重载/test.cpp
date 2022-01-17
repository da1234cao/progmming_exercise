#include <iostream>
#include <iomanip>
#include "self_complex.h"
 
int main()
{
    using namespace test_complex;
    std::cout << std::fixed << std::setprecision(1);
    
    complex z2 = 1.0;
    complex z1 = 2.0_i; // 使用字面量    
    complex z3 = 1.0 + 2.0_i; // 重载加号运算符
    complex z4 = z3; // 拷贝构造函数
    
    std::cout<<z4<<std::endl; // 重载输出运算符

    if(z3 == z4) // 重载相等运算符
        std::cout<<"z3 is equal to z4\n";
    else
        std::cout<<"z3 is not equal to z4\n";

    if(z1 != z2) // 对称的重载不想等运算符
        std::cout<<"z1 is not equal to z2\n";
}