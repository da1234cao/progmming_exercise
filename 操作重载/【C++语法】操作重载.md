[toc]

## 摘要

搬运了“《C++ Primer》第十四章 操作重载与类型转换 ”中的部分内容。

**运算符重载的实现有三个要点：重载的运算符选择作为成员或非成员，参数列表，返回类型**。

本文实现了一个复数运算的demo，用于演示运算符重载。

完整代码见仓库。

---

## 操作重载的基本概念

**本节来源：《C++ Primer》第十四章 操作重载与类型转换**

C++语言定义了大量的运算符。当运算符被用于类类型的对象时，C++语言允许我们，通过运算符重载，为其指定新的含义。

### 基本概念

**重载的运算符是具有特殊名字的函数：它们的名字由关键字`operator`和其后要定义的运算符号共同组成。和其他函样，重载的运算符也包含返回类型、参数列表以及函数体**。

重载运算符函数的参数数量与该运算符作用的运算对象数量一样多。一元运算符有一个参数，二元运算符有两个。对于二元运算符来说，左侧运算对象传递给第一个参数，而右侧运算对象传递给第二个参数。除了重载的函数调用运算符 operator()之外，其他重载运算符不能含有默认实参。

如果一个运算符函数是成员函数，则它的第第一个（左侧）运算对象绑定到隐式的`this`指针上。因此，成员运算符函数的（显示）参数数量比运算符的运算对象总数少一个。

每个运算符在用于内置类型时都有比较明确的含义。以二元`+`运算符为例，它明显执行的是加法操作。因此，把二元`+`运算符映射到类类型的一个类似操作上可以简化记忆。例如对于标准库类型 `string`来说，我们就会使用`+`把一个`string`对象连接到另一个后面，很多编程语言都有类似的用法。

### 选择作为成员与非成员

当我们定义重载的运算符时，必须首先决定是将其声明为类的成员函数还是声明为个普通的非成员函数。在某些时候我们别无选择，因为有的运算符必须作为成员:另一些情况下，运算符作为普通函数比作为成员更好。

下面的准则有助于我们在将运算符定义为成员函数还是普通的非成员函数做出抉择:

* 赋值（=）、下标（[]）、调用（()）和成员访问箭头（->）运算符必须是成员。

* ·复合赋值运算符一般来说应该是成员，但并非必须，这一点与赋值运算符略有不同。

* 改变对象状态的运算符或者与给定类型密切相关的运算符，如递增、递减和解引用运算符，通常应该是成员。

* 具有对称性的运算符可能转换任意一端的运算对象，例如算术、相等性、关系和位运算符等，因此它们通常应该是普通的非成员函数。

程序员希望能在含有混合类型的表达式中使用对称性运算符。例如，我们能求一个`int`和一个 `double`的和，因为它们中的任意一个都可以是左侧运算对象或右侧运算对象，所以加法是对称的。如果我们想提供含有类对象的混合类型表达式，则运算符必须定义成非成员函数。

当我们把运算符定义成成员函数时，它的左侧运算对象必须是运算符所属类的一个对象。例如:

```c++
string s = "world";
string t = s + "!"; // 正确：我们能把一个const char* 加到一个string对象中
string u = "hi" + s; // 错误：如果+是string的成员则产生错误
```

如果。 `operator+`是 `string`类的成员，则上面的第一个加法等价于`s.operator+("!")`。同样的，`"hi"+s`等价于`"hi".operator+(s)`。显然`"hi"`的类型是 `const char*`，这是一种内置类型，根本就没有成员函数。

因为 `string`将`+`定义成了普通的非成员函数，所以`"hi"+s`等价于`operator+("hi"，s)`。和任何其他函数调用一样，每个实参都能被转换成形参类型。唯一的要求是至少有一个运算对象是类类型，并且两个运算对象都能准确无误地转换成`string`。

### 小结

重载的运算符有多种，如，输入输出运算符，算术与关系运算符，赋值运算符，下标运算符，递增递减运算符，成员访问运算符，函数调用运算符。

<font color=red>这些运算符的重载都遵循着同一套规则（三个要点）：重载的运算符选择作为成员或非成员，参数列表，返回类型。</font>

**下面我们使用复数运算的demo实现，来展示运算符重载的使用。每次运算符重载的时候，我们都按照上面三点，组建运算符重载函数**。

---

## 代码展示

### 代码展示前言

完整代码见仓库。

C++标准库中，已经存在复数实现：[std::complex](https://zh.cppreference.com/w/cpp/numeric/complex)。

下面实现的复数运算demo，主要用于演示运算符重载。为了简单期间，没有使用模板，假定复数的实部和虚部都为`double`类型。演示代码仅仅实现了复数的加法运算，相等与不相等判断，标准输出。

### 复数类基本结构

`complex`类包含两个成员变量，分别表示复数的实部和虚部。成员变量有自己的`const`获取函数。另外，该类包含两个构造函数。

```c++
    class complex {
        private:
            double _M_real;
            double _M_imag;

        public:
            complex(double re=0.0, double im=0.0) : _M_real(re),_M_imag(im) {};

            complex(const complex &_M){
                _M_real = _M.getReal();
                _M_imag = _M.getImag();
            }

            double getReal() const{
                return _M_real;
            }

            double getImag() const{
                return _M_imag;
            }
    };
```

### 自定义字面量

我们希望代码中，复数的虚部可以包含`i`，如下所示：

```c
// 实部为0.0，虚部为2.0的复数
complex z1 = 2.0_i;
```

C++11开始支持，[用户定义字面量](https://zh.cppreference.com/w/cpp/language/user_literal)。

当匹配到`2.0_i`时，下面代码返回`complex(0.0，2.0)`。

 用户定义后缀必须以下划线字符 `_` 开始。所以我们使用`operator""_i`，而非`operator""i`。`std::complex`中使用的则是`operator""i`。

字面量运算符允许特定的形参，其允许`long double`，但不允许`double`。所以，我们的参数类型设置为 `long double`。

```c++
    // 使用自定义字面量
    complex 
    operator""_i(long double _num){
        return complex(0.0,_num);
    }
```

### 重载加号运算符(`+`)

复数加法：实部加实部，虚部加虚部。

加法运算，通常是将相加结果赋予到其他地方，不改变运算对象，所以均加上`const`修饰。

加号运算符是具有对称性的运算符，所以我们将其放在`complex`类外，而非作为`complex`的成员函数。

```c++
    // 重定义+
    complex
    operator+(const complex &m1, const complex &m2){
        double real = m1.getReal() + m2.getReal();
        double imag = m1.getImag() + m2.getImag();
        return complex(real,imag);
    }
```

### 重定义输出运算符(`<<`)

复数输出：输出实部和虚部。

通常情况下，输出运算符的第一个形参是一个非常量 `ostream`对象的引用。之所以`ostream`是非常量是因为向流写入内容会改变其状态;而该形参是引用是因为我们无法直接复制一个`ostream`对象。

第二个形参一般来说是一个常量的引用，该常量是我们想要打印的类类型。第二个形参是引用的原因是我们希望避免复制实参:而之所以该形参可以是常量是因为(通常情况下)打印对象不会改变对象的内容。

为了与其他输出运算符保持一致， `operator<<`一般要返回它的 `ostream`形参。

与 `iostream`标准库兼容的输入输出运算符必须是普通的非成员函数，而不能是类的成员函数。否则，它们的左侧运算对象将是我们的类的一个对象:

```c++
sales_data data;
data << cout; // 如果operator<<是sales_data的成员
```

根据上面限定，复数的输出运算符实现如下：

```c++
    // 重定义<<
    std::ostream& operator<<(std::ostream &os, const complex &_M_complex){
        os<<_M_complex.getReal()<<'+'<<_M_complex.getImag()<<'i';
        return os;
    }  
```

另外，用于内置类型的输出运算符不太考虑格式化操作，尤其不会打印换行符，用户希望类的输出运算符也像如此行事。如果运算符打印了换行符，则用户就无法在对象的同一行内接着打印一些描述性的文本了。相反，令输出运算符尽量减少格式化操作可以使用户有权控制输出的细节。

### 重定义相等运算符(`==`)

在重载相等运算符之前，我们先考虑下，如何判断两个`double`类型数值的相等与否。

这里我们借用下，[std::numeric_limits\<T\>::epsilon](https://zh.cppreference.com/w/cpp/types/numeric_limits/epsilon)。下面代码来自该链接。

```c++
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer， bool>::type
    almost_equal(T x, T y, int ulp=2)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
        // unless the result is subnormal
        || std::abs(x-y) < std::numeric_limits<T>::min();
}
```

通常情况下，我们把算术和关系运算符定义成非成员函数以允许对左侧或右侧的运算对象进行转换。因为这些运算符一般不需要改变运算对象的状态，所以形参都是常量的引用。

```c++
    // 重定义==
    bool
    operator==(const complex &m1, const complex &m2){
        bool real_eq = almost_equal(m1.getReal(), m2.getReal());
        bool imag_eq = almost_equal(m1.getImag(), m2.getImag());
        return real_eq && imag_eq;
    }
```

如果类定义了。 `operator==`，则这个类也应该定义 `operator!=`。对于用户来说，当他们能使用`==`时肯定也希望能使用`!=`，反之亦然。

```c++
    // 对称的，需要重定义!=
    bool
    operator!=(const complex &m1, const complex &m2){
        return !operator==(m1,m2);
    }
```

---

## 附录

### 完整代码

`complex`头文件，保存为`self_complex.h`。

```c++
#ifndef SELF_COMLPEX_H
#define SELF_COMPLEX_H

#include <limits>

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp=2)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
        // unless the result is subnormal
        || std::abs(x-y) < std::numeric_limits<T>::min();
}

namespace test_complex{
    class complex {
        private:
            double _M_real;
            double _M_imag;

        public:
            complex(double re=0.0, double im=0.0) : _M_real(re),_M_imag(im) {};

            complex(const complex &_M){
                _M_real = _M.getReal();
                _M_imag = _M.getImag();
            }

            double getReal() const{
                return _M_real;
            }

            double getImag() const{
                return _M_imag;
            }
    };

    // 使用自定义字面量
    complex 
    operator""_i(long double _num){
        return complex(0.0,_num);
    }

    // 重定义<<
    std::ostream& operator<<(std::ostream &os, const complex &_M_complex){
        os<<_M_complex.getReal()<<'+'<<_M_complex.getImag()<<'i';
        return os;
    }  

    // 重定义+
    complex
    operator+(const complex &m1, const complex &m2){
        double real = m1.getReal() + m2.getReal();
        double imag = m1.getImag() + m2.getImag();
        return complex(real,imag);
    }

    // 重定义==
    bool
    operator==(const complex &m1, const complex &m2){
        bool real_eq = almost_equal(m1.getReal(), m2.getReal());
        bool imag_eq = almost_equal(m1.getImag(), m2.getImag());
        return real_eq && imag_eq;
    }

    // 对称的，需要重定义!=
    bool
    operator!=(const complex &m1, const complex &m2){
        return !operator==(m1,m2);
    }

}

#endif
```

测试`complex`的测试代码，保存为`test.cpp`。

```c++
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
```

运行结果：

```shell
1.0+2.0i
z3 is equal to z4
z1 is not equal to z2
```











