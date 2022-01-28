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