---
Title: mysql基础架构
Keywords: 执行器,优化器,索引基数
Description: 优化器存在选错索引的可能性，索引统计信息不准确导致的问题，你可以用analyze table来解决
Cover: 
Label: mysql基础架构
Date: 2020-01-19 11:35:00
LastEditTiem: Date: 2020-01-19 11:35:00
---

![](assert/mysql基础架构.png)

mysql 可以分为Server和存储引擎层

server层包括：链接器、查询缓存、分析器、优化器、执行器等，涵盖mysql大多数核心服务功能以及所有的内置函数（如日期、时间、数学和加密函数等），所有跨存储引擎的功能都在这一层实现，比如存储过程、触发器、视图等。

存储引擎层：插件式，支持InnoDB、MyISAM、Memory等多个存储引擎。现在最常用的存储引擎是InnoDB。

# 连接器

连接器负责跟客户端建立连接、获取权限、维持和管理连接。

客户端连接超时时间默认是8小时。

数据库连接池默认用的长连接，持续请求使用同一个连接，每次执行完成一个比较大的操作后，通过执行`mysql_reset_connection`来重新初始化连接资源。不需要重连和重新做权限验证

# 查询缓存

MySQL 8.0版本直接将查询缓存的整块功能删掉了，也就是说8.0开始彻底没有这个功能了

# 分析器

分析器先会做“词法分析”，“语法分析”

常见的错误：

```error
You have an error in your SQL syntax
```

# 优化器

优化器在多个索引时，决定使用哪个索引。一个语句在多表关联时，决定各表的执行顺序;

优化选择索引的目的，是找到一个最优执行方案，用最小的代价去执行语句。关联因素有：

-   扫描行数，
-   是否使用临时表
-   是否排序

**扫描行数如何判断的？**

innodb并不能准确的知道这个条件的记录有多少条，只能根据统计信息来估算行数，指的是索引的区分度， 一个索引上不同的值越多，这个索引的区分度越好；一般索引上不同值的个数成为基数（Cardinality）。

```sql
mysql> show index from t\G;
*************************** 1. row ***************************
        Table: t
   Non_unique: 0
     Key_name: PRIMARY
 Seq_in_index: 1
  Column_name: id
    Collation: A
  Cardinality: 100238
     Sub_part: NULL
       Packed: NULL
         Null:
   Index_type: BTREE
      Comment:
Index_comment:
*************************** 2. row ***************************
        Table: t
   Non_unique: 1
     Key_name: a
 Seq_in_index: 1
  Column_name: a
    Collation: A
  Cardinality: 100238
     Sub_part: NULL
       Packed: NULL
         Null: YES
   Index_type: BTREE
      Comment:
Index_comment:
*************************** 3. row ***************************
        Table: t
   Non_unique: 1
     Key_name: b
 Seq_in_index: 1
  Column_name: b
    Collation: A
  Cardinality: 100238
     Sub_part: NULL
       Packed: NULL
         Null: YES
   Index_type: BTREE
      Comment:
Index_comment:
3 rows in set (0.00 sec)
```

**MySQL是怎样得到索引的基数的呢?**

整张表取出来一行行统计，虽然可以得到精确的结果，但是代价太高了，所以只能选择“采样统计”。

采样统计: InnoDB默认会选择N个数据页，统计这些页面上的不同值，得到一个平均值，然后乘以这个索引的页面数，就得到了这个索引的基数。



# 执行器

执行的时候，要先判断一下你对这个表T有没有执行查询的权限