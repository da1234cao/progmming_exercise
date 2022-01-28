[toc]

## 前言

下面内容来自，《C++ primer》15.1 OPP概述，15.2 定义基类和派生类，15.3 虚函数。

这里以虚函数为出发点，简单修改了书上的代码，将继承关系下的动态绑定使用示例的方式展示出来。

完整代码见附录。

---
## 虚函数的基本概念

面向对象程序设计基于三个基本概念:数据抽象、继承和动态绑定。通过使用数据抽象,我们可以将类的接口与实现分离;使用继承,可以定义相似的类型并对其相似关系建模;使用动态绑定,可以在一定程度上忽略相似类型的区别,而以统一的方式使用它们的对象。

在C++语言中,基类将类型相关的函数与派生类不做改变直接继承的函数区分对待。一种是基类希望派生类直接继承而不要改变的函数。**另一种是基类希望它的派生类各自定义适合自身的版本,此时基类就将这些函数声明成虚函数(virtual function)**。

派生类必须在其内部对所有重新定义的虚函数进行声明。派生类可以在这样的函数之前加上`virtual`关键字,但是并不是非得这么做。C++11新标准允许派生类显式地注明它将使用哪个成员函数改写基类的虚函数,具体措施是在该函数的形参列表之后增加一个`override`关键字。

在C++语言中,**当我们使用基类的引用(或指针)调用一个虚函数时将发生动态绑定**。所谓动态绑定，就是在运行时，虚函数会根据绑定对象的实际类型，选择调用函数的版本。(对象的类型是确定不变的,我们无论如何都不可能令对象的动态类型与静态类型不一致。因此,通过对象进行的函数调用将在编译时绑定到该对象所属类中的函数版本上。当且仅当对通过指针或引用调用虚函数时,才会在运行时解析该调用,也只有在这种情况下对象的动态类型才有可能与静态类型不同。)

---
## 代码展示

在很多程序中都存在着一些相互关联但是有细微差别的概念。例如,书店中不同书籍的定价策略可能不同:有的书籍按原价销售,有的则打折销售。有时,我们给那些购买书籍超过一定数量的顾客打折;另一些时候,则只对前多少本销售的书籍打折,之后就调回原价,等等。面向对象的程序设计(OOP)适用于这类应用。

### 基类

```c++
class Book {
private:
    std::string bookNo;  // 图书编号
protected:
    double price = 0.0; // 图书价格  // (2.1.4)
public:
    Book() = default; // (2.1.1)
    Book(const std::string &book, double sales_price):
                    bookNo(book), price(sales_price) {}

    std::string isbn() const { 
        return bookNo; 
    }

    double get_price() const {
        return price;
    }

    virtual double net_price(std::size_t n) const { // (2.1.2)
        return n*price;
    }

    virtual ~Book()=default;  // (2.1.3)
};
```
(2.1.1)它是一个默认构造函数。我们定义这个构造函数的目的仅仅是因为我们既需要其他形式的构造函数,也需要默认的构造函数。我们希望这个函数的作用完全等同于，当没有构造函数时，编译器默认创建的构造函数。在C++11新标准中,如果我们需要默认的行为,那么可以通过在参数列表后面写上`= defau1t`来要求编译器生成构造函数。其中,`= default`既可以和声明一起出现在类的内部,也可以作为定义出现在类的外部。和其他函数一样,如果`= default`在类的内部,则默认构造函数是内联的;如果它在类的外部,则该成员默认情况下不是内联的。

(2.1.2)我们在net_price成员函数之前，添加了`virtual`关键字。因为我们希望派生类覆盖该方法，使用其自己的计算总价的方法。

(2.1.3)基类通常都应该定义一个虛析构函数,即使该函数不执行任何实际操作也是如此。

(2.1.4)和其他使用基类的代码一样,派生类能访问公有成员,而不能访问私有成员。不过在某些时候基类中还有这样一种成员,基类希望它的派生类有权访问该成员,同时禁止其他用户访问。我们用受保护的(protected)访问运算符说明这样的成员。(书籍可以使用不同的策略销售，但只需要一个价格。成员变量不应该设置成public而暴露在外。如果price成员变量设置为私有，也可以，但子类中不需要再创建一个price变量，它应使用基类的price的get/set方法。这里设置成protect，子类可以直接访问，比较方便。)

### 派生类

```c++
class Discount_Book : public Book {  // (2.2.4)
private:
    std::size_t min_qty = 0; // 使用折购策略的最低购买量
    double discount = 0.0; // 折扣额
public:
    Discount_Book() = default;
    Discount_Book(const std::string &book, double sales_price, std::size_t min_qty, double discount):
                    Book(book,sales_price),min_qty(min_qty),discount(discount) {}  // (2.2.1)
    
    double net_price(std::size_t n) const override { // (2.2.2)
        if(n < min_qty){ // 数量没有达到最低采购价，按照标准价出售
            return Book::net_price(n); // (2.2.3)
        }else{
            return n*get_price() - discount*(n/min_qty);
        }
    }
};
```

(2.2.1)一个派生类对象包含多个组成部分:一个含有派生类自己定义的(非静态)成员的子对象,以及一个与该派生类继承的基类对应的子对象,如果有多个基类,那么这样的子对象也有多个。和其他创建了基类对象的代码一样,派生类也必须使用基类的构造函数来初始化它的基类部分。

(2.2.2)派生类如果定义了一个函数与基类中虚函数的名字相同但是形参列表不同,这仍然是合法的行为。编译器将认为新定义的这个函数与基类中原有的函数是相互独立的。这时,派生类的函数并没有覆盖掉基类中的版本。就实际的编程习惯而言,这种声明往往意味着发生了错误,因为我们可能原本希望派生类能覆盖掉基类中的虚函数,但是一不小心把形参列表弄错了。要想调试并发现这样的错误显然非常困难。在C++11新标准中我们可以使用`override`关键字来说明派生类中的虚函数。这么做的好处是在使得程序员的意图更加清晰的同时让编译器可以为我们发现一些错误,后者在编程实践中显得更加重要。如果我们使用`override`标记了某个函数,但该函数并没有覆盖已存在的虚函数,此时编译器将报错。

(2.2.3)有时候我们需要回避虚函数的默认机制。通常是当一个派生类的虚函数调用它覆盖的基类的虚函数版本时。使用作用域运算符可以实现这一目的。

(2.2.4)如果一个派生是公有的,则基类的公有成员也是派生类接口的组成部分。

### 动态绑定

```c++
int get_spend(Book &book, int n){ // (2.3.1)
    return book.net_price(n);
}

int main(void){
    // 传入基类对象
    Book b1("6927610320",55); // 55元一本书
    cout<<b1.get_price()<<" RMB every book. Buy two books, need "<<get_spend(b1,2)<<" RMB"<<endl; //买两本

    // 传入派生类对象
    Discount_Book b2("6927610321",50,2,10);// 50元一本，每买两本，减少10元
    cout<<b2.get_price()<<" RMB every book. Buy two books, need "<<get_spend(b2,4)<<" RMB"<<endl; //买四本

    return 0;
} 
```

(2.3.1)**我们能将公有派生类型的对象绑定到基类的引用或指针上。因为在派生类对象中含有与其基类对应的组成部分,所以我们能把派生类的对象当成基类对象来使用**。(这种转换通常称为派生类到基类的(derived--to-base)类型转换。)

这个程序的输出如下：

```shell
55 RMB every book. Buy two books, need 110 RMB
50 RMB every book. Buy two books, need 180 RMB
```

有两点我没有串到上面程序中，这里单独说下：我们还能把某个函数指定为`final`,如果我们已经把函数定义成`final`了,则之后任何尝试覆盖该函数的操作都将引发错误；如果虚函数使用默认实参,则基类和派生类中定义的默认实参最好一致。

---
## 附录

### 虚函数表-略

[C++虚函数表（多态的实现原理）](http://c.biancheng.net/view/267.html)

### 完整代码

```shell
.
├── book.h
├── CMakeLists.txt
└── test.cpp
```

头文件。

```c++
#ifndef BOOK_H
#define BOOK_H

#include <string>

// 基类
class Book {
private:
    std::string bookNo;  // 图书编号
protected:
    double price = 0.0; // 图书价格
public:
    Book() = default;
    Book(const std::string &book, double sales_price):
                    bookNo(book), price(sales_price) {}

    std::string isbn() const { 
        return bookNo; 
    }

    double get_price() const {
        return price;
    }

    virtual double net_price(std::size_t n) const {
        return n*price;
    }

    virtual ~Book()=default;
};


// 派生类
class Discount_Book : public Book {
private:
    std::size_t min_qty = 0; // 使用折购策略的最低购买量
    double discount = 0.0; // 折扣额
public:
    Discount_Book() = default;
    Discount_Book(const std::string &book, double sales_price, std::size_t min_qty, double discount):
                    Book(book,sales_price),min_qty(min_qty),discount(discount) {}
    
    double net_price(std::size_t n) const override {
        if(n < min_qty){ // 数量没有达到最低采购价，按照标准价出售
            return Book::net_price(n);
        }else{
            return n*get_price() - discount*(n/min_qty);
        }
    }
};

#endif
```

测试函数。

```c++
#include <iostream>
#include "book.h"

using namespace std;

int get_spend(Book &book, int n){
    return book.net_price(n);
}

int main(void){
    // 传入基类
    Book b1("6927610320",55); // 55元一本书
    cout<<b1.get_price()<<" RMB every book. Buy two books, need "<<get_spend(b1,2)<<" RMB"<<endl; //买两本

    // 传入派生类
    Discount_Book b2("6927610321",50,2,10);// 50元一本，每买两本，减少10元
    cout<<b2.get_price()<<" RMB every book. Buy two books, need "<<get_spend(b2,4)<<" RMB"<<endl; //买四本

    return 0;
} 
```

编译过程。看下[cmake-examples](https://github.com/ttroy50/cmake-examples)，照葫芦画瓢写个就好。

```cmake
cmake_minimum_required(VERSION 3.16)

project(test)

add_executable(${PROJECT_NAME} test.cpp)
```

编译运行。

```shell
mkdir build
cd build
cmake ..
make
./test
```

