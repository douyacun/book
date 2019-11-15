---
Title: mysql锁
Keywords: 共享锁,乐观锁,悲观锁，排他锁
Description: 深入理解一下mysql的这些常用锁
Author: douyacun
Cover: assert/mysql-locks.png
Label: mysql锁
Date: 2019-06-04 14:05:09
LastEditTime: 2019-11-15 18:09:17
---
# 共享锁
共享锁也叫读锁，简称S锁，原理：一个事务获取了一个数据行的共享锁，其他事务能获得该行对应的共享锁，但不能获得排他锁，即一个事务在读取一个数据行的时候，其他事务也可以读，但不能对该数据行进行增删改。
```sql
SELECT * FROM table_name WHERE ...
```

# 排他锁/悲观锁
排他锁也叫写锁，简称x锁，原理：一个事务获取了一个数据行的排他锁，其他事务就不能再获取该行的其他锁（排他锁或者共享锁），即一个事务在读取一个数据行的时候，其他事务不能对该数据行进行增删改查。

```sql
SELECT * FROM table_name WHERE ... FOR UPDATE。
```

注意点:
- 对于select语句，innodb不会加任何锁，也就是可以多个并发去进行select的操作，不会有任何的锁冲突，因为根本没有锁。
- 对于insert，update，delete操作，innodb会自动给涉及到的数据加排他锁，只有查询select需要我们手动设置排他锁。
- 共享锁和排他锁都是锁的行记录，意向共享锁和意向排他锁锁定的是表。

悲观锁，每次在拿数据都会上锁，其他session想拿这个数据就会被阻塞知道它拿到锁


# 乐观锁
[乐观锁应用场景](https://segmentfault.com/a/1190000008935924)

乐观锁，不会发生并发冲突，只在提交操作是检查是否违反数据完整性，实现方式有两种：
- 使用数据版本(version)记录机制实现
- 使用时间戳

```sql
SELECT * FROM table_name WHERE ... FOR UPDATE。
```

数据库： 行锁，表锁
排他锁（写锁），共享锁（读锁）

select： 读锁，共享锁
update \ insert \ delete : 每次都会自己加排他锁

一个商品A, 库存有10个， 一百人来抢，不要超卖
update table set num=num-1 where num=10;

select num from table;
update table set num=num-1 where num=9;

// 悲观锁: 锁的是索引， 如果没有索引，直接锁表。  主键锁，唯一键锁、非唯一建锁、间隙锁等
select num from table where goods = 'A' for update;
select num from table where user ='liuning' for update;

update table set num=num-1;



