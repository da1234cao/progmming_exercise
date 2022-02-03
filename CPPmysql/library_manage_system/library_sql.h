/*
 *  Desc: Linux c++ MySQL 示例，图书馆管理系统，MySQL单例
 *  Author: mason
 *  Date: 20200203
 */
 
 
#pragma once
 
#include <vector>
#include "library_book.h"
#include <mysql-cppconn-8/jdbc/mysql_connection.h>
#include <mysql-cppconn-8/jdbc/cppconn/driver.h>
#include <mysql-cppconn-8/jdbc/cppconn/exception.h>
#include <mysql-cppconn-8/jdbc/cppconn/resultset.h>
#include <mysql-cppconn-8/jdbc/cppconn/statement.h>
 
#define SQL_QUERY_ALL           0   // 查询所有
#define SQL_QUERY_BY_AUTHOR     1   // 查询作者
#define SQL_QUERY_BY_ID         2   // 查询编号
#define SQL_QUERY_BY_BOOK_NAME  3   // 查询书名
 
#define SQL_DELETE_ALL           0   // 删除所有
#define SQL_DELETE_BY_AUTHOR     1   // 删除作者
#define SQL_DELETE_BY_ID         2   // 删除编号
#define SQL_DELETE_BY_BOOK_NAME  3   // 删除书名
 
using namespace std;
 
class sql_loader
{
    public:
        static sql::Driver *driver;
        static sql::Connection *con;
        static sql::Statement *stmt;
        static sql::ResultSet *res;
        static bool init;
 
        // sql 初始化
        static void sql_loader_init()
        {
            if (sql_loader::init)
            {
                cout<<"sql 已经完成初始化"<<endl;
                return ;
            }
            
            /* Create a connection */
            driver = get_driver_instance();
 
            // 密码和用户名
            con = driver->connect("tcp://192.168.122.34:3306", "dacao", "111111");
 
            /* Connect to the MySQL test database ，数据库名*/
            con->setSchema("library");
 
            init = true;
            cout<<"sql 初始化成功"<<endl;
            return ;
        }
 
        // 添加图书
        static void add(Book &new_book)
        {
            // SQL初始化
            sql_loader_init();
 
            // 创建并构造sql语句，用完后手动删除
            stmt = con->createStatement();
            string sql_cmd = "INSERT INTO books(name, author, price, quantity) VALUES ('";
            sql_cmd.append(new_book.book_name);
            sql_cmd.append("','");
            sql_cmd.append(new_book.author);
            sql_cmd.append("', ");
            sql_cmd.append(to_string(new_book.price));
            sql_cmd.append(", ");
            sql_cmd.append(to_string(new_book.quantity));
            sql_cmd.append(")");
 
            cout<<"insert cmd : "<<sql_cmd<<endl;
            try{
                stmt->execute(sql_cmd);
            } catch (sql::SQLException &e) {
                cout << "# ERR: SQLException in " << __FILE__;
                cout << "(" << __FUNCTION__ << ") on line "
                    << __LINE__ << endl;
                cout << "# ERR: " << e.what();
                cout << " (MySQL error code: " << e.getErrorCode();
                cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            }
 
            // 删除资源
            delete stmt;
            return ;
        }
 
        // 修改图书
        static void update(Book& new_book)
        {
            // SQL初始化
            sql_loader_init();
 
            // 创建并构造sql语句，用完后手动删除            
            stmt = con->createStatement();
            string sql_cmd = "update books set name='";
            sql_cmd.append(new_book.book_name);
            sql_cmd.append("', author='");
            sql_cmd.append(new_book.author);
            sql_cmd.append("', price=");
            sql_cmd.append(to_string(new_book.price));
            sql_cmd.append(", quantity=");
            sql_cmd.append(to_string(new_book.quantity));
            sql_cmd.append(" where id=");
            sql_cmd.append(to_string(new_book.id));
 
            cout<<"update cmd : "<<sql_cmd<<endl;
            
            try{
                stmt->execute(sql_cmd);
            } catch (sql::SQLException &e) {
                cout << "# ERR: SQLException in " << __FILE__;
                cout << "(" << __FUNCTION__ << ") on line "
                    << __LINE__ << endl;
                cout << "# ERR: " << e.what();
                cout << " (MySQL error code: " << e.getErrorCode();
                cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            }
 
            // 释放资源
            delete stmt;
            return ;            
        }
        // 删除图书
        static void delete_book()
        {
            int op_code = -1, book_id;
            string author, book_name;
            string sql_cmd = "delete from books";
 
            cout<<"请输入删除方式：\n"
                "0: 删除所有图书\n"
                "1: 删除指定作者\n"
                "2: 删除指定编号\n"
                "3: 删除指定图书"<<endl;
            
            cin>>op_code;
            switch (op_code)
            {
                case SQL_DELETE_ALL:
                break;
 
                case SQL_DELETE_BY_AUTHOR:
                cout<<"请输入要删除的作者：";
                cin>>author;
                sql_cmd.append(" where author='");
                sql_cmd.append(author);
                sql_cmd.append("'");
                break;
 
                case SQL_DELETE_BY_ID:
                cout<<"请输入要删除的编号：";
                cin>>book_id;
                sql_cmd.append(" where id=");
                sql_cmd.append(to_string(book_id));             
                break;
 
                case SQL_DELETE_BY_BOOK_NAME:
                cout<<"请输入要删除的书名：";
                cin>>book_name;
                sql_cmd.append(" where name='");
                sql_cmd.append(book_name);
                sql_cmd.append("'");                
                break;
                default:
                cout<<"不存在 "<<op_code<<" 对应的删除方式，删除失败"<<endl;
                return ;
            }
            cout<<"delete cmd : "<<sql_cmd<<endl;
            
            // 初始化SQL
            sql_loader_init();
 
            // 创建并构造sql语句，用完后手动删除            
            stmt = con->createStatement();
 
            try{
                // 执行SQL语句
                stmt->execute(sql_cmd);
 
                // 释放资源
                delete stmt;
            } catch (sql::SQLException &e) {
                cout << "# ERR: SQLException in " << __FILE__;
                cout << "(" << __FUNCTION__ << ") on line "
                    << __LINE__ << endl;
                cout << "# ERR: " << e.what();
                cout << " (MySQL error code: " << e.getErrorCode();
                cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            }
 
            return ;            
        }
 
        // 查询图书
        static void query(vector<Book>& book_list)
        {
            int op_code = -1, book_id;
            string author, book_name;
            string sql_cmd = "select id, name, author, quantity, price from books";
 
            cout<<"请输入查询方式：\n"
                "0: 查询所有\n"
                "1: 查询作者\n"
                "2: 查询编号\n"
                "3: 查询书名"<<endl;
            
            cin>>op_code;
            switch (op_code)
            {
                case SQL_QUERY_ALL:
                break;
 
                case SQL_QUERY_BY_AUTHOR:
                cout<<"请输入要查询的作者：";
                cin>>author;
                sql_cmd.append(" where author='");
                sql_cmd.append(author);
                sql_cmd.append("'");
                break;
 
                case SQL_QUERY_BY_ID:
                cout<<"请输入要查询的编号：";
                cin>>book_id;
                sql_cmd.append(" where id=");
                sql_cmd.append(to_string(book_id));             
                break;
 
                case SQL_QUERY_BY_BOOK_NAME:
                cout<<"请输入要查询的书名：";
                cin>>book_name;
                sql_cmd.append(" where name='");
                sql_cmd.append(book_name);
                sql_cmd.append("'");                
                break;
                default:
                cout<<"不存在 "<<op_code<<" 对应的查询方式，查询失败"<<endl;
                return ;
            }
            cout<<"query cmd : "<<sql_cmd<<endl;
            
            // 初始化SQL            
            sql_loader_init();
            
            // 创建并构造sql语句，用完后手动删除             
            stmt = con->createStatement();
 
            try{
                // 执行查询
                res = stmt->executeQuery(sql_cmd);
                while (res->next()) {
                    /* Access column data by alias or column name */
                    cout << res->getInt("id") << res->getString("name") << res->getString("author") << res->getInt("quantity") << endl;
 
                    book_list.push_back(Book(res->getString("name"), res->getString("author"), res->getInt("price"), res->getInt("quantity"), res->getInt("id")));
                    /* Access column data by numeric offset, 1 is the first column */
                    //cout << res->getString(1) << endl;
                }
 
                // 资源释放
                delete res;
                delete stmt;
            } catch (sql::SQLException &e) {
                cout << "# ERR: SQLException in " << __FILE__;
                cout << "(" << __FUNCTION__ << ") on line "
                    << __LINE__ << endl;
                cout << "# ERR: " << e.what();
                cout << " (MySQL error code: " << e.getErrorCode();
                cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            }
 
            return ;
        }
 
 
    private:
        sql_loader(){}
};