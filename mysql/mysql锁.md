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

# 乐观锁
[乐观锁应用场景](https://segmentfault.com/a/1190000008935924)

总是假设最好的情况，每次去拿数据的时候都认为别人不会修改，不会上锁，但是更新时会判断在此期间有没有人更新这个数据。实现方式有两种：
- 使用数据版本(version)记录机制实现
- 使用时间戳

```sql
SELECT * FROM table_name WHERE ... FOR UPDATE。
```

select： 读锁，共享锁
update \ insert \ delete : 每次都会自己加排他锁

一个商品A, 库存有10个， 一百人来抢，不要超卖
update table set num=num-1 where num=10;

select num from table;
update table set num=num-1 where num=9;

# 悲观锁: 
锁的是索引， 如果没有索引，直接锁表。

总是假设最坏的情况，每次去拿数据时总会假设别人会修改，所以每次在拿数据时都会加锁。别人想拿这条数据就会阻塞直到他拿到锁，

## 共享锁
确认该行记录存在，保证其他人不会修改该记录，但是如果该事物记录，很有可能会造成死锁

一个事务在修改一行记录时，
```sql
SELECT * FROM table_name WHERE ... lock in share mode
```

## 排他锁
排他锁也叫写锁，简称x锁，原理：一个事务获取了一个数据行的排他锁，其他事务就不能再获取该行的其他锁（排他锁或者共享锁），即一个事务在读取一个数据行的时候，其他事务不能对该数据行进行增删改查。

```sql
SELECT * FROM table_name WHERE ... FOR UPDATE。
```

注意点:
- 对于select语句，innodb不会加任何锁，也就是可以多个并发去进行select的操作，不会有任何的锁冲突，因为根本没有锁。
- 对于insert，update，delete操作，innodb会自动给涉及到的数据加排他锁，只有查询select需要我们手动设置排他锁。
- 共享锁和排他锁都是锁的行记录，意向共享锁和意向排他锁锁定的是表。

悲观锁，每次在拿数据都会上锁，其他session想拿这个数据就会被阻塞知道它拿到锁


# 行锁3种算法

## Record Lock

单个行记录上锁，总是锁住索引记录。如果表在建立是并没有建立索引，innodb会使用隐式的主键来锁

索引可以是主键、唯一、普通索引

## Gap lock

间隙锁，锁定一个范围，但不包含记录本身。

## Next key lock

gap lock + record lock 锁定范围并锁定记录本身。

innodb对于行的查询采用的是这种方式，eg. 有一个索引，有10、11、13、20四个值，那么该索引被Next key lock的区间是
- (-∞, 10)
- [10, 11)
- [11, 13)
- [13, 20)
- [20, +∞)

**幻像读**

innodb事物默认的隔离级别是REPEATBLE RRAD， 

幻想读是指在同一事物下，连续执行两次相同的sql，会有不同的结果，第二次sql可能会返回之前不存在的行。

next key lock是如何解决的?
```sql
select * from t where key > 20 for update;
```
next key lock会锁住[20, ∞)区间，在此事物未提交期间，key > 20记录都是不能插入的。

# 死锁

两个事物互相持有资源不释放。

发生死锁以后，innodb是能检测到并使其中一个事物释放锁并回退，另一个事物获得锁继续完成事物

在涉及到外部锁，或涉及表锁时，innodb并不能完全检测到这时需要`innodb_lock_wait_timeout`来解决。