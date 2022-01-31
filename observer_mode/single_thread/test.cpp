#include <iostream>
#include "subject.h"
#include "observer.h"

int main(void){
    Product product;
    streetProduct obs1;
    marketProduct obs2;
    product.registerObs(&obs1);
    product.registerObs(&obs2);

    product.setPrice(100); // 成本设置为100
    product.notifyObs(); // 通知obs1和obs2
}