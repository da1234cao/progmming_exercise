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