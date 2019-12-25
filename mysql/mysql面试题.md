---
？Title: mysql面试题整理
Keywords: mysql面试题
Description: 常见面试题看似简单，实际不知道咋回答，即使
Label: mysql面试题
Cover: mysql面试题-cover.jpeg
Date: 2019-12-23 13:56:00
LastEditTime: 2019-12-23 13:57:00
---

# 存储引擎相关

**innodb和myisam的区别**

- innodb支持事务，myisam不支持事务

- innodb支持外键，myisam不支持，innodb 外键表转myisam会报错

- innodb是聚集索引，myisam是非聚集索引，聚集索引的数据存放在主键的叶子节点上，非聚集索引的存放的是主键的id，需要查询两次。myisam是非聚集索引，数据文件是分离的，索引保存数据文件的指针

- innodb不保存具体行数，`select count(*) from user`需要扫面全表
- innodb最小粒度锁是行锁，myisam是表锁

**myisam static和myisam dynamic 有什么区别**

static所有字段都有固定宽度，dynqmic是变长

# 数据类型

**char 和 vachar 的区别**

1. char定长、vachar变长
2. varchar 255 需要多1个字节存储字符串长度 varchar (>255) 需要2个字节存储字符串长度
3. char一次性分配，varchar变更前后会有碎片产生

**列auto_increament达到最大值，会发生什么**

Duplicate entry '255' for key 'PRIMARY'

**date和timestamp区别**

timestamp：

-   客户端插入时间从当前时区转化为世界标准时间存储，查询时转化为客户端时区返回。
-   timestamp时间范围到2038年

datetime：

-   不做任何改变，原样输入输出
-   存储范围到9999

**如何获取最后一次插入时分配了哪个自动增量**

1.  last_insert_id返回表中自增字段的当前值
2.  一条insert语句插入多条，`insert into foo(val) VALUES(1),(2), (3), (4);` 在当前AUTO_INCREMENT值加1返回，当前AUTO_INCREMENT=1，执行上面语句后只会返回2
3.  `INSERT ... ON DUPLICATE KEY UPDATE` 
    1.  如果是insert，AUTO_INCREMENT + 1返回
    2.  如果是update，返回AUTO_INCREMENT，不是变更记录的id
4.  显示指定自增字段id的值，last_insert_id()返回的是AUTO_INCREMENT的值，不会加1，只有自增字段由mysql来分配，
5.  sql执行出错，last_insert_id值未定义，若事务回滚，last_insert_id的值不会恢复到事务执行前的那个值
6.  Last_insert_id的值是每个链接独立维护的。



# 日志

**binlog几种录入格式，有什么区别**

- statement ： 基于语句记录日志。

优点：不需要记录每一行的变化，减少binlog日志量，节约I/O，提高性能，和row相比：正常同一条记录修改或者插入row格式产生的日志量小于statement产生的，如果是带条件的update，delete，row会产生大量日志。

缺点：如果以一种不确定的方式修改数据的方式设计一条语句，则主服务器和从服务器上的数据可能会变得不同 。也就是说，它由查询优化器决定。例如function，触发器等。

- row（默认）：基于行记录日志

优点：基于行的格式将许多更改写入二进制日志，row level的日志内容会非常清楚的记录下每一行数据修改的细节，不会出现function，以及trigger的调用和触发无法被正确复制。所有DDL语句的仍然基于语句的格式

缺点：所有执行的语句当记录到日志中的时候，都会以行的形式记录，这样可能会产生大量的日志内容，如一条update操作修改多条记录，binlog中每一条修改都会有记录，造成binlog过大。

- mixed: 以上两种level的混合，一般的语句修改使用statment格式保存binlog，如一些函数，statement无法完成主从复制操作，则操作row格式保存binlog

# 索引

