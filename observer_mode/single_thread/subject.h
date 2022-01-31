#ifndef SUBJECT_H
#define SUBJECT_H

/**
 * 被观察者。
 * 1. 观察者需要在这里注册。
 * 2. 当被观察者发生变化时，需要通知观察者
*/

#include <vector>
#include <algorithm>
#include "observer.h"

// 基类(被观察者)
class Subject {
protected:
    double price=0;
    std::vector<Observer*> observers;
public:
    virtual void registerObs(Observer* ) = 0;
    virtual void deleteObs(Observer* ) = 0;
    virtual void notifyObs() = 0;
    virtual ~Subject() = default; 

    void setPrice(double p) {
        price = p;
    }
    double getPrice() {
        return price;
    }
};

// 派生类(被观察者)
class Product : public Subject {
public:
    void registerObs(Observer* obs) override{
        observers.push_back(obs);
    }
    void deleteObs(Observer* obs) override{
        auto it = std::find(observers.begin(), observers.end(), obs);
        if(it != observers.end())
            observers.erase(it);
    }
    void notifyObs() override {
        for(auto obs : observers){
            obs->update(price);
        }
    }
};

#endif