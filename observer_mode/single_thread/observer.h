#ifndef Observer_H
#define Observer_H

/**
 * 观察者
*/

#include <iostream>

// 观察者基类
class Observer {
protected:
    double price=0;
public:
    virtual void update(double) = 0;
    virtual ~Observer() = default;
};

// 派生类，东西在地摊的价格-随着成本的波动
class streetProduct : public Observer {
public:
    void update(double cost) override {
        price = cost*1.5;
        std::cout<<"The procude's cost is "<<cost<<';'<<"When it appear in street stall, It's price is "<<price<<std::endl;
    }
};

// 派生类，东西在商场的价格-随着成本的波动
class marketProduct : public Observer {
public:
    void update(double cost) override {
        price = cost*1.8;
        std::cout<<"The procude's cost is "<<cost<<';'<<"When it appear in market, It's price is "<<price<<std::endl;
    }
};

#endif