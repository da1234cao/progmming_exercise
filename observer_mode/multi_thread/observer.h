#ifndef Observer_H
#define Observer_H

#include <iostream>
#include "base.h"
#include "subject.h"

// 派生类，东西在地摊的价格-随着成本的波动
class streetProduct : public Observer {
private:
    double price=0;
    Product& product;
public:
    streetProduct(Product& p): product(p) {
        product.registerObs(this);
    };

    void update(double cost) override {
        price = cost*1.5;
        std::cout<<"The procude's cost is "<<cost<<';'<<"When it appear in street stall, It's price is "<<price<<std::endl;
    }

    ~streetProduct() {
        product.deleteObs(this);
    }
};

// 派生类，东西在商场的价格-随着成本的波动
class marketProduct : public Observer {
private:
    double price=0;
    Product& product;
public:
    marketProduct(Product& p): product(p) {
        product.registerObs(this);
    };

    void update(double cost) override {
        price = cost*1.8;
        std::cout<<"The procude's cost is "<<cost<<';'<<"When it appear in market, It's price is "<<price<<std::endl;
    }

    ~marketProduct(){
        product.deleteObs(this);
    }
};

#endif