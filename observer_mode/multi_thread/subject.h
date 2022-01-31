#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>
#include <algorithm>
#include <mutex>
#include "base.h"

// 派生类(被观察者)
class Product : public Subject {
private:
    int price=0;
    std::mutex mutex_;
public:
    void registerObs(Observer* obs) override{
        std::lock_guard<std::mutex> guard(mutex_);
        observers.push_back(obs);
    }
    void deleteObs(Observer* obs) override{
        std::lock_guard<std::mutex> guard(mutex_);
        auto it = std::find(observers.begin(), observers.end(), obs);
        if(it != observers.end())
            observers.erase(it);
    }
    void notifyObs() override {
        std::lock_guard<std::mutex> guard(mutex_);
        for(auto obs : observers){
            obs->update(price);
        }
    }

    void setPrice(int p) {
        price = p;
    }
    int getPrice() {
        return price;
    }
};

#endif