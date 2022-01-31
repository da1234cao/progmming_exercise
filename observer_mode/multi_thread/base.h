#ifndef BASE_H
#define BASE_H

#include <vector>

// 观察者基类
class Observer {
public:
    virtual void update(double) = 0;
    virtual ~Observer() = default;
};

/**
 * 被观察者。
 * 1. 观察者需要在这里注册。
 * 2. 当被观察者发生变化时，需要通知观察者
*/
// 基类(被观察者)
class Subject {
protected:
    std::vector<Observer*> observers;
public:
    virtual void registerObs(Observer* ) = 0;
    virtual void deleteObs(Observer* ) = 0;
    virtual void notifyObs() = 0;
    virtual ~Subject() = default; 
};


#endif
