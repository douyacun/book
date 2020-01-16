---
Title: mysql锁
Keywords: 共享锁,乐观锁,悲观锁,排他锁,行锁,死锁,避免死锁
Description: 深入理解一下mysql的行表锁、行锁，如果正确对待死锁。
Author: douyacun
Cover: assert/mysql-locks.png
Label: mysql锁
Date: 2019-06-04 14:05:09
LastEditTime: 2020-01-16 20:16:35
---

[toc]

# 全局锁

对整个数据实例加锁，FTWRL

```sql
flush table with read lock
```

使用这个语句后其他线程的语句会阻塞：数据更新语句，数据表定义语句和更新事务的提交语句

**使用场景**：全库逻辑备份，mysqldump导出的时候使用MVCC的一致性保证innodb导出数据，但是对于MyISAM来说是不支持的，备份只能通过FTWRL来保证

**为什么不用 set global readonly = true**

1.  readonly的值，一般被用作判断一个库是主库还是从库，修改global变量的方式影响面大，不建议使用
2.  异常处理机制上，如果执行FTWRT由于客户端发生异常断开，mysql会自动释放这个全局锁，而设置全局readonly后，如果客户端发生异常，则数据库会一直保持readonly状态，导致整个库事件处于不可写状态，风险较高

# 表级锁

mysql表级锁有两种: 

**表锁**

与FTWRL类似，可以用unlock tables主动释放锁，也可以在客户端断开的时候自动释放

```sql
lock tables t1 read, t2 write;
```

其他线程写t1，读写t2都会被阻塞，该线程也只能读t1，读写t2

不推荐使用，粒度太大，目前很少见到了

**MDL（元数据锁，meta data lock）**

MDL不需要显示使用，在访问一个表的时候会自动加上，MDL的作用是，保证读写的正确性，防止在一个查询在执行期间另一线程对这个表结构做变更，删除了一列，导致查询线程的结果和表结构对不上。

对表做CURD时，加MDL读锁；对表结构做变更操作时，加MDL写锁；

-   MDL读锁之间不互斥，可以并发读写
-   MDL写锁互斥，两个线程同时修改，只能等拿到锁的线程执行完成

| 时间点 | 会话A                                | 会话B                        | 会话C                             | 会话D                                                |
| ------ | ------------------------------------ | ---------------------------- | --------------------------------- | ---------------------------------------------------- |
| 1      | begin; <br />Select count(*) from t; |                              |                                   |                                                      |
| 2      |                                      | Select * from t where id =2; |                                   |                                                      |
| 3      |                                      |                              | alter  table t add column c3 int; |                                                      |
| 4      |                                      |                              |                                   | Show  processlist；B：copy to tmp table              |
| 5      |                                      |                              | 阻塞                              | Show  processlist;B：Waiting for table metadata lock |
| 6      | A：执行完毕                          |                              |                                   |                                                      |
| 7      |                                      | B: 执行完成                  |                                   | Show  processlist; B：rename table                   |
| 8      | Select  count(*) from t;             |                              |                                   |                                                      |
| 9      |                                      |                              | C：执行完毕                       |                                                      |
| 10     |                                      |                              |                                   | Show  processlist; A: Sending data                   |
| 11     | A:执行完毕                           |                              |                                   |                                                      |

session A先启动，这是会对t加一个MDL读锁，session B需要的也是加MDL读锁。读锁不互斥可以正常执行

而session C需要MDL写锁，而之后所有要在表t上申请MDL读锁的请求也会被session C阻塞。等于现在这个表t完全不可读写了，而客户端超时会有重试机制，新的session请求，这个库的线程很快就爆满。

**如何安全的进行表结构变更**

如果是热表的话，就麻烦了，虽然数据量不大，但是请求频繁。这时候kill掉未必管用，因为新的请求马上就来了，一般能遇到这种情况的公司都是有点实力的了，必定会有DBA的支撑了，很幸运和DBA聊了一下

一般表结构变更直接执行就好了，如果遇到一直拿不到MDL锁的情况, 考虑

-   处理慢查询，商量能不能kill
-   解决长事务，会一直占着MDL锁，[长事物解决方案](innodb事务#长事务)
-   对于热表的话，因为查询很频繁，会一直拿不到MDL写锁，考虑晚上执行，到这就很头疼了。
-   如果是上亿数据的大表的话，而且晚上也拿不到MDL写锁的话，推荐方案：
    -   [pt-online-schema-change](https://www.percona.com/doc/percona-toolkit/LATEST/pt-online-schema-change.html)
    -   [gh-ost](https://github.com/github/gh-ost)
    -   [阿里云-不锁表结构变更](https://help.aliyun.com/document_detail/98373.html)

主要原理：

-   1、创建临时表：CREATE TABLE tmp_table_table LIKE table_name
-   2、变更临时表结构： ALTER TABLE tmp_table_table XXXX
-   3、全量拷贝数据：INSERT IGNORE INTO tmp_table_table (SELECT %s FROM table_name FORCE INDEX (%s) WHERE xxx
-   4、增量数据binlog同步： UPDATA/INSRT/DELETE tmp_table_name
-   5、切换新旧表： RENAME TABLE table_name to old_tmp_table_table, tmp_table_name to table_name



# 行锁

**两阶段锁协议**

在innodb事务中，行锁是在需要的时候才加上的，但并不是不需要了就立即释放，而是要等到事务结束时才释放，这个就是两阶段锁协议

## 三种算法

**幻像读**

innodb事物默认的隔离级别是REPEATBLE RRAD， 

幻想读是指在同一事物下，连续执行两次相同的sql，会有不同的结果，第二次sql可能会返回之前不存在的行。

next key lock是如何解决的?

```sql
select * from t where key > 20 for update;
```

next key lock会锁住[20, ∞)区间，在此事物未提交期间，key > 20记录都是不能插入的。

- Record Lock

单个行记录上锁，总是锁住索引记录。如果表在建立是并没有建立索引，innodb会使用隐式的主键来锁

索引可以是主键、唯一、普通索引

- Gap lock

间隙锁，锁定一个范围，但不包含记录本身。

- Next key lock

gap lock + record lock 锁定范围并锁定记录本身。

innodb对于行的查询采用的是这种方式，eg. 有一个索引，有10、11、13、20四个值，那么该索引被Next key lock的区间是

- (-∞, 10)
- [10, 11)
- [11, 13)
- [13, 20)
- [20, +∞)

## 死锁

并发线程出现循环资源依赖，涉及的线程都在等待别的线程释放资源时，就会导致这几个线程都进入无限等待的状态。

**死锁处理策略**：

- 超时策略：`innodb_lock_wait_timeout` 默认50s
- 死锁检测：`innodb_deadlock_detect` 默认时开启的

**避免死锁的建议**：

- 通过两阶段锁协议我们知道：行锁是在事务提交的时候才会释放锁，要把最可能造成锁冲突、最可能影响并发度的锁尽量往后放
- 控制访问相同资源的并发事务量，比如：抢购商品分成10条记录，或放入队列

## 乐观锁

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

## 悲观锁: 

锁的是索引， 如果没有索引，直接锁表。

总是假设最坏的情况，每次去拿数据时总会假设别人会修改，所以每次在拿数据时都会加锁。别人想拿这条数据就会阻塞直到他拿到锁，

### 共享锁

读取的任何行上设置共享模式锁定，其他会话可以读取行，但是在事务提交之前不能修改它们，如果这些行中的任何一个被尚未提交的另一个事务修改，则查询等待直到该事务结束，然后使用最新职

```sql
-- 5.7
SELECT * FROM table_name WHERE ... lock in share mode
-- 8.0
SELECT * FROM table_name WHERE ... for share
```

### 排他锁 

排他锁也叫写锁，简称x锁，原理：一个事务获取了一个数据行的排他锁，其他事务就不能再获取该行的其他锁（排他锁或者共享锁），即一个事务在读取一个数据行的时候，其他事务不能对该数据行进行增删改查。

子查询中的行不会被锁定，除非在子查询中也指定了锁

```sql
SELECT * FROM table_name WHERE ... FOR UPDATE。
```

注意点:
- 对于select语句，innodb不会加任何锁，也就是可以多个并发去进行select的操作，不会有任何的锁冲突，因为根本没有锁。
- 对于insert，update，delete操作，innodb会自动给涉及到的数据加排他锁，只有查询select需要我们手动设置排他锁。
- 共享锁和排他锁都是锁的行记录，意向共享锁和意向排他锁锁定的是表。

悲观锁，每次在拿数据都会上锁，其他session想拿这个数据就会被阻塞知道它拿到锁

**脏读**

脏数据：未提交的数据

脏读：一个事物读取到了另一个事物未提交的数据

脏读违背了事物的隔离性，关注一下innodb是如何隔离事务的。

现在不会见到脏读了，数据隔离级别要设置成 READ UNCOMMITTED级别才可以，现在默认级别是READ REAPEATABLE

那么READ UNCOMMITTED只有特殊场景才会遇到，如：slave

**不可重复读**

不可重读读是指：在一个事物内多次读取同一数据集合，在这个事物还没有结束时，另一个事物对该数据集合做了一些DML操作。这样就发生了同一事物内两次读取同一数据集合，数据不一致。

不可重复读和脏读的区别是，脏读是读未提交，而不可重复读的已经提交

next key lock算法下，不仅锁住扫描到的索引，还锁住了这些索引覆盖的范围，这个范围内的插入是不允许的，这样就避免了另外一个事物在这个范围内插入数据，导致多次读取数据不一致。

**阻塞**

一个事物中的锁需要等待另一个事物中的锁释放他所占的资源，这就是阻塞

**innodb_lock_wait_timeout** 用来控制等待的时间默认50秒

**innodb_rollback_on_timeout** 用来设置在等待超时是否对进行中的事物进行回滚。

