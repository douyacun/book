---
Title: 排序模式
Keywords: order by,全字段排序,rowid排序
Description: 如何高效的随机取值
Label: mysql排序
Cover:
Date: 2020-01-22 10:43:00
LastEditTime: 2020-01-22 17:08:12
---


```sql
CREATE TABLE `t` (
  `id` int(11) NOT NULL,
  `city` varchar(16) NOT NULL,
  `name` varchar(16) NOT NULL,
  `age` int(11) NOT NULL,
  `addr` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `city` (`city`)
) ENGINE=InnoDB;

select city,name,age from t where city='杭州' order by name limit 1000;
```



# 全字段排序

排序可能在内存中完成，也可能使用使用外部排序

`sort_buffer_size`

-   如果排序的数据量小于sort buffer size，排序在内存中完成，

-   如果排序数据量大，内存放不下时，就要使用外部排序，一般是归并排序， 将需要排序的数据分成12份，每一份单独排序后存在这些临时文件中。然后把这12个有序文件再合并成一个有序的大文件。

执行过程

1.  初始化sort_buffer，确定放入name、city、age这三个字段
2.  从索引city找到第一个满足city='杭州’条件的主键id
3.  到主键id索引取出整行，取name、city、age三个字段的值，存入sort_buffer中
4.  从索引city取下一个记录的主键id
5.  重复步骤3、4直到city的值不满足查询条件为止
6.  对sort_buffer中的数据按照字段name做快速排序
7.  按照排序结果取前1000行返回给客户端

# rowid排序

如果字段太多，排序的单行长度太大，mysql回采用row id来排序。只把需要排序的字段放入sort buffer，然后根据id的值回到原表取其他的字段

`SET max_length_for_sort_data = 16;` 可以控制排序字段的长度

执行过程

1.  初始化sort_buffer，确定放入两个字段，即name和id
2.  从索引city找到第一个满足city='杭州’条件的主键id
3.  到主键id索引取出整行，取name、id这两个字段，存入sort_buffer中
4.  从索引city取下一个记录的主键id
5.  重复步骤3、4直到不满足city='杭州’条件为止
6.  对sort_buffer中的数据按照字段name进行排序
7.  遍历排序结果，取前1000行，并按照id的值回到原表中取出city、name和age三个字段返回给客户

# 排序优化

**联合索引**: city取出来的数据是天然排好序的，就不需要在排序了, 创建一个city,name的联合索引

```sql
alter table t add index city_user(city, name);
```

执行过程：

1.  从索引(city,name)找到第一个满足city='杭州’条件的主键id；
2.  到主键id索引取出整行，取name、city、age三个字段的值，作为结果集的一部分直接返回；
3.  从索引(city,name)取下一个记录主键id；
4.  重复步骤2、3，直到查到第1000条记录，或者是不满足city='杭州’条件时循环结束

**索引覆盖**: age，就不需要回主键索引查age了

```sql
alter table t add index city_user(city, name, age);
```

执行过程：

1.  从索引(city,name,age)找到第一个满足city='杭州’条件的记录，取出其中的city、name和age这三个字段的值，作为结果集的一部分直接返回；
2.  从索引(city,name,age)取下一个记录，同样取出这三个字段的值，作为结果集的一部分直接返回；
3.  重复执行步骤2，直到查到第1000条记录，或者是不满足city='杭州’条件时循环结束。

# 取随机值

总行数：

```sql
select count(*) as c from t;
```

随机数：

```php
$offset = mt_rand(1, c)
```

跳过offset取1条

```sql
select * from t limit :offset, 1;
```

这样要比`order by rand()` 效率高，且随机性也比较好；