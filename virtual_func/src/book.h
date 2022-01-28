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