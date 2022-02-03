[toc]

# 前言&摘要
使用C++连接MySQL数据库，根据[官方手册](https://dev.mysql.com/doc/connectors/en/),可以使用[Connector/C++](https://dev.mysql.com/doc/connectors/en/connector-cpp.html)或者[Connector/ODBC](https://dev.mysql.com/doc/connectors/en/connector-odbc.html)。

关于Connector/C++，本文使用示例代码演示如何使用。
关于Connector/ODBC，暂未使用。

关于数据库的安装和表的创建准备工作，参见：[mysql环境准备](https://da1234cao.blog.csdn.net/article/details/105478479)

使用API编程有三个阶段：了解API对应事物的概念，了解API的使用，了解API的实现原理。数据库这部分内容，我还有所欠缺，待提升。本文只涉及C++连接MySQL数据库API的使用。

本文示例代码见仓库。

数据库内容，准备如下：

```sql
--  创建数据库
create database library;     
use library;
 
--  创建图书表
CREATE TABLE books (
    id INT NOT NULL AUTO_INCREMENT,
	title VARCHAR(1024) NOT NULL,
    author VARCHAR(1024) NOT NULL,
    price INT NOT NULL DEFAULT 0,
    quantity INT NOT NULL DEFAULT 1,
    PRIMARY KEY (id)
)DEFAULT CHARACTER SET=UTF8;
show tables;
-- drop table books;
-- truncate table books;

-- 向表格中插入内容
insert into books(title,author,price,quantity)
values("Linux多线程服务器编程","陈硕",50,1);

insert into books(title,author,price,quantity)
values("C++ primer 中文版","Lippman",60,1);

-- 查询表中内容
select * from books;
```

---
# 使用Connector/C++连接MySQL数据库

[Introduction to Connector/C++](https://dev.mysql.com/doc/connectors/en/connector-cpp-introduction.html)：MySQL Connector/C++ 8.0 是一个 MySQL 数据库连接器，用于连接到 MySQL 服务器的 C++ 应用程序。Connector/C++ 可用于访问实现文档存储的 MySQL 服务器，或以传统方式使用 SQL 语句。==Connector/C++ 8.0 的首选开发环境是支持使用 X DevAPI 开发 C++ 应用程序，或使用 X DevAPI for C 开发普通 C 应用程序，但 Connector/C++ 8.0 还支持开发使用旧版基于 JDBC 的 API 的 C++ 应用程序。Connector/C++ 1.1==。

[Installing Connector/C++ from a Binary Distribution](https://dev.mysql.com/doc/connectors/en/connector-cpp-installation-binary.html#connector-cpp-installation-binary-linux)：使用包管理器进行安装。

```shell
 sudo apt install libmysqlcppconn-dev
```

另外，[编译Connector/C++程序](https://dev.mysql.com/doc/connectors/en/connector-cpp-apps-general-considerations.html)需要指定动态库。

## Connector/C++ and JDBC 

[Connector/C++ and JDBC Compatibility](https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-introduction.html)：Connector/C++ 与 JDBC 4.0 API 兼容。Connector/C++ 没有实现整个 JDBC 4.0 API，但具有以下类：Connection, DatabaseMetaData, Driver, PreparedStatement, ResultSet, ResultSetMetaData, Savepoint, Statement。JDBC 4.0 API 为刚才提到的类定义了大约 450 个方法。Connector/C++ 实现了其中的大约 80%。

API文档：[doc](https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-getting-started-examples.html) | [code](https://github.com/mysql/mysql-connector-cpp/tree/33f2a7271a062dc5476984a712c4b2b9f53f05df)

官方文档的演示过程并不是一个完整的应用示例。这里有一个使用Connector/C++的JDBC API，用单例模式连接MySQL数据库，==图书管理系统demon==：[Linux MySQL Connector/C++ 编程实例](https://blog.csdn.net/fuyuande/article/details/104160311)

基本过程如下：

* [与数据库建立连接](https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-connecting.html)。
* [执行查询语句](https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-query.html)
* [获取结果](https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-results.html)


## Connector/C++ and X DevAPI

[X DevAPI Overview](https://dev.mysql.com/doc/x-devapi-userguide/en/devapi-users-introduction.html)：X DevAPI 由支持 X 协议的 MySQL Shell 和 MySQL 连接器实现。

首先是确保服务器端的MySQL安装了[X Plugin](https://dev.mysql.com/doc/refman/8.0/en/x-plugin.html)。

```shell
# 默认是安装的，我们检查下
mysql -u user -p -e "SHOW plugins"

+----------------------------+----------+--------------------+---------+---------+
| Name                       | Status   | Type               | Library | License |
+----------------------------+----------+--------------------+---------+---------+
...
| mysqlx                     | ACTIVE   | DAEMON             | NULL    | GPL     |
```

接着，确保远程应用程序可以与运行 X 插件的 MySQL 服务器实例[建立逻辑会话](https://dev.mysql.com/doc/x-devapi-userguide/en/database-connection-example.html)，将/etc/mysql/mysql.conf.d/mysqld.cnf中mysqlx-bind-address = 127.0.0.1注销掉。

X DevAPI的文档，我目前仅阅读了：[连接单个 MySQL 服务器](https://dev.mysql.com/doc/x-devapi-userguide/en/connecting-to-a-single-mysqld-node-setup.html)、[使用关系表](https://dev.mysql.com/doc/x-devapi-userguide/en/devapi-users-working-with-relational-tables.html)

```c++
#include <iostream>
#include <iomanip>
#include <list>
#include <mysql-cppconn-8/mysqlx/xdevapi.h>

using namespace std;

// Scope controls life-time of objects such as session or schema

int main(void) {
    mysqlx::Session sess("192.168.122.34",33060,"dacao","111111");
    mysqlx::Schema db = sess.getSchema("library");
    mysqlx::Table tb = db.getTable("books");
    mysqlx::RowResult result = tb.select("*").execute();
    list<mysqlx::Row> rows = result.fetchAll();

    cout<<"id "<<"title "<<"author "<<"price "<<"quantity "<<endl;
    for(auto row : rows)
        cout<<row[0]<<" "<<row[1]<<" "<<row[2]<<" "<<row[3]<<" "<<row[4]<<endl;
}

```

---
# 使用ODBC连接MySQL数据库-略

[Introduction to MySQL Connector/ODBC](https://dev.mysql.com/doc/connectors/en/connector-odbc-introduction.html)|[MyODBC介绍](https://www.mysqlzh.com/doc/205/511.html)：ODBC（开放式数据库连接性）为客户端程序提供了访问众多数据库或数据源的一种方式。ODBC是标准化的API，允许与SQL数据库服务器进行连接。它是根据SQL Access Group的规范开发的，它定义了一套函数调用、错误代码和数据类型，可将其用于开发独立于数据库的应用程序。通常情况下，当需要数据库独立或需要同时访问不同的数据源时，将用到ODBC。

这里有个windows环境下使用ODBC连接操作数据库的演示视频：[C/C++使用ODBC连接操作数据库](https://www.bilibili.com/video/BV1uf4y1Y7Kf)

配置有点麻烦，我还没使用过。

```shell
 sudo apt install mysql-connector-odbc
 
 ➜  ~ dpkg -L mysql-connector-odbc:amd64 
/.
/usr
/usr/bin
/usr/bin/myodbc-installer
/usr/lib
/usr/lib/x86_64-linux-gnu
/usr/lib/x86_64-linux-gnu/odbc
/usr/lib/x86_64-linux-gnu/odbc/libmyodbc8a.so
/usr/lib/x86_64-linux-gnu/odbc/libmyodbc8w.so
/usr/share
/usr/share/doc
/usr/share/doc/mysql-connector-odbc
/usr/share/doc/mysql-connector-odbc/ChangeLog
/usr/share/doc/mysql-connector-odbc/INFO_BIN
/usr/share/doc/mysql-connector-odbc/INFO_SRC
/usr/share/doc/mysql-connector-odbc/LICENSE.txt.gz
/usr/share/doc/mysql-connector-odbc/README.txt
/usr/share/doc/mysql-connector-odbc/changelog.Debian.gz
/usr/share/doc/mysql-connector-odbc/copyright

➜  ~ odbcinst -j
unixODBC 2.3.6
DRIVERS............: /etc/odbcinst.ini
SYSTEM DATA SOURCES: /etc/odbc.ini
FILE DATA SOURCES..: /etc/ODBCDataSources
USER DATA SOURCES..: /home/dacao/.odbc.ini
SQLULEN Size.......: 8
SQLLEN Size........: 8
SQLSETPOSIROW Size.: 8

```







