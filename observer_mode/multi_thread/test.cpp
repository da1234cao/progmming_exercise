#include <iostream>
#include <thread>
#include "subject.h"
#include "observer.h"

using namespace std;

void addObs1(Product& p){
    streetProduct obs1(p);
    p.setPrice(100);
    p.notifyObs();
}

int main(void){
    Product product; // 提供存储功能。当数据变动时，提供通知。通知的具体形式，由各自的观察者实现

    thread t1(addObs1,ref(product));
    
    marketProduct obs2(product); // 创建时候，构造函数将自己注册到被观察者对象中；消除的时候，析构函数将自己从被观察者对象中删除
    product.setPrice(100); // 成本设置为100
    product.notifyObs(); // 通知obs1和obs2

    t1.join();
}