---
Title: mysql索引
Keywords: 索引,index,联合索引，覆盖索引
Description: 记录一下mysql索引的概念，随时补充
Author: douyacun
Cover: 
Label: 索引
Date: 2019-11-26 10:24:07
LastEditTime: 2019-11-26 23:07:12
---

innodb是索引存储表，表数据按主键顺序存放

# 聚集索引
聚集索引按照每张表的主键构造一棵B+树，叶子节点存放的是整张表的行记录数据，叶子节点被称为数据页。

每张表只能按照一颗B+树排序，每张表只能有一个聚集索引

聚集索引对于主键的排序查找和范围查找速度非常快

没有主键或唯一索引，innodb默认会生成一个行号(不可见)，作为聚集索引的主键

# 辅助索引

辅助索引叶子节点不会包含行记录的所有数据。叶子节点除了包含键值以外还会包含主键，来查找叶子节点以外的其它数据。

辅助索引通过主键id来关联记录

# 联合索引
对表上的多个列进行索引

对a,b创建索引

可以用到索引
- where a = 1;
- where a = 1 and b = 2;
- where a = 1 order by b;

用不到索引
- where b = 2;

# 覆盖索引

从索引即可查询到所需字段，不需要查询聚集索引中的记录，减少io操作。

# multi-range read

目的： 减少磁盘的随机访问，将随机访问变为顺序的数据访问。

MRR好处：
- 使数据访问变得较为顺序，在查询辅助索引时，首先根据得到的结果，按照主键排序，
- 减少缓冲池中页被替换的次数
- 批量处理对键值的查询

explain 可以看到extra列 using MRR

# ICP

mysql 5.6开始支持，将where的过滤部分放到存储层

```sql
select * from salaries
where (from_date between "2019-01-10" and "2019-02-10") and (salaries between 3800 and 4000)
```
不启用ICP，则数据需要先取出from_date between 2019-01-10 and 2019-02-10的数据，然后再过滤 3800-4000的数据

启用ICP，则会在索引取出时就会进行where过滤，前提是where条件被索引覆盖

explain 可以看到extra列 using index condition
