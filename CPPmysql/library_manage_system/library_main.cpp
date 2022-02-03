/*
 *  Desc: Linux c++ MySQL 示例，图书馆管理系统，主程序
 *  Author: mason
 *  Date: 20200203
 */
 
#include "library_book.h"
#include "library_sql.h"
 
#define LIBRARY_ADD_BOOK    1   // 添加
#define LIBRARY_DELETE_BOOK 2   // 删除
#define LIBRARY_UPDATE_BOOK 3   // 修改
#define LIBRARY_QUERY_BOOK  4   // 查询
#define LIBRARY_QUIT        5   // 退出
 
// MySQL单例类静态变量初始化
sql::Driver* sql_loader::driver = nullptr;
sql::Connection* sql_loader::con = nullptr;
sql::Statement* sql_loader::stmt = nullptr;
sql::ResultSet* sql_loader::res = nullptr;
bool sql_loader::init = false;
 
// 系统使用说明
void Usage()
{
    cout<<"\n欢迎使用图书管理系统\n"
          "1:添加图书\n"
          "2:删除图书\n"
          "3:修改图书\n"
          "4:查询图书\n"
          "5:退出系统\n"
          <<endl;
    return ;
}
 
// 读取图书信息
void ReadBook(Book &new_book)
{
    cout<<"请输入添加的书名：";
    cin>>new_book.book_name;
    
    cout<<"请输入作者：";
    cin>>new_book.author;
    //cout<<"[DEBUG]"<<author<<endl;
    cout<<"请输入价格：";
    cin>>new_book.price;
 
    cout<<"请输入数量：";
    cin>>new_book.quantity;
 
    return ;
}
 
int main()
{
    bool is_quit = false;
    int op_code = -1, book_id;
    string book_name, author;
    unsigned int price, quantity;
    Book new_book;
    vector<Book> book_list;
    
    // 主循环
    while (!is_quit)
    {
        Usage();
        cout<<"请输入你的选择:";
        cin>>op_code;
        switch (op_code)
        {
        // 添加操作
        case LIBRARY_ADD_BOOK:
            // 读取图书信息
            ReadBook(new_book);
            sql_loader::add(new_book);
            break;
 
        // 删除操作
        case LIBRARY_DELETE_BOOK:    
            sql_loader::delete_book();
            break;
 
        // 修改操作
        case LIBRARY_UPDATE_BOOK:    
            cout<<"请输入待修改的编号：";
            cin>>new_book.id;
            ReadBook(new_book);
            sql_loader::update(new_book);
            break;        
        
        // 查询操作
        case LIBRARY_QUERY_BOOK:            
            sql_loader::query(book_list);
 
            // 打印查询结果
            cout << "------------- 查询开始 ----------------" <<endl;
            for (auto &iter : book_list)
            {   
                cout <<"编号： "<< iter.id << endl;
                cout <<"书名： "<< iter.book_name << endl;
                cout <<"作者： "<< iter.author << endl;
                cout <<"数量： "<< iter.quantity << endl;
                cout <<"价格： "<< iter.price << endl;
                cout << "-----------------------------" <<endl;
            }            
            cout << "------------- 查询结束 ----------------" <<endl;
 
            // 清空上次查询结果
            book_list.erase(book_list.begin(), book_list.end());
            break;
        
        // 退出系统        
        case LIBRARY_QUIT:
            is_quit = true;
            break;      
 
        default:
            cout<<"未识别的操作："<<op_code<<endl;
            break;
        }
    }
    cout<<"感谢使用"<<endl;
 
    return 0;
}