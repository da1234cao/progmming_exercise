 /*
 *  Desc: Linux c++ MySQL 示例，图书馆管理系统，图书类
 *  Author: mason
 *  Date: 20200203
 */
 
#pragma once
#include <string>
#include <iostream>
 
using namespace std;
 
class Book
{
    public:
        int id;                      // 图书编号
        string book_name;            // 书名
        string author;               // 作者
        unsigned int price;          // 价格
        unsigned int quantity;       // 数量
 
    public:
        // 默认构造函数
        Book(){}
 
        // 含参构造函数
        Book(string book_name, string author, int price, int quantity, int book_id = 0):book_name(book_name),author(author),
            price(price),quantity(quantity), id(book_id){}
 
        // 析构函数    
        ~Book(){}
};