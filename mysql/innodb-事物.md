---
Title: mysql事务定义,常见问题
Keywords: ACID，事务含义
Description: 
Cover: 
Label: 
Date: 2019-02-25 23:02:12
LastEditTime: 2019-12-16 23:50:46
---

[toc]

# 隔离级别

**READ UNCOMMITTED (未提交读)**
事物的修改即使没有commit也可以被其他事物读到 - `脏读`

**READ COMMITTED (已提交读)**
一个事物从开始到提交对其他事物都是不可见的 - `不可重复读` 两次执行同样的查询读取的结果不一致

**REPEATABLE READ (可重复读)**
保证在同一个事物中多次读取到的结果是一致的，

**SERIALIZABLE (串行化)**
强制事物串行执行，在读取的每一行上加锁

# 事物的实现

隔离性由锁来实现，原子性、一致性、持久性是通过redo log和undo log来实现。redo log保证原子性和持久性，undo log保证一致性。

-   atomicity 原子性
-   consistency 一致性
-   isolation 隔离性
-   durability 持久性

## redo

innodb 在事物提交时，必须先将事务的所有事务写入到磁盘上的redo log file 和undo log file中进行持久化，为了确保每次日志都能写入到事务日志文件中，每次都会调用fsync，将内存中脏页同步到磁盘上

**mini transaction**是innodb对物理数据数据文件操作的最小事务单元，用于对page加锁，修改，释放以及日志提交到公共buffer等操作。mtr必须是原子性操作，一个事务可以包含多个mtr，每个mtr完成后需要将本地产生的日志拷贝到缓冲区，将修改的脏页放到flush list上

**LSN(log sequence number)** 用于记录日志序号，它是一个不断递增的 unsigned long long 类型整数。在 InnoDB 的日志系统中，LSN 无处不在，它既用于表示修改脏页时的日志序号，也用于记录checkpoint，通过LSN，可以具体的定位到其在redo log文件中的位置（注意: LSN不是自增+1，而是每次+日志的大小）

**Checkpoint**

-   sharp checkpoint

sharp checkpoint会把所有已提交事务相关的脏页刷到磁盘，并记录最新的已提交事务的LSN号。sharp checkpoint刷新到磁盘的脏页是某一时刻的一致性数据。
当数据库关闭时，会发生sharp checkpoint

-   fuzzy checkpoint

fuzzy checkpoint则复杂很多。fuzzy checkpoint会一点点的把脏页刷新到磁盘，直到与sharp checkpoint达到相同的目的（即所有的已提交事务相关的脏页到刷到磁盘）。fuzzy checkpoint会把两个LSN之间的脏页刷新到磁盘。但是并不能保障LSN之间的数据时一致性的。所以被称为fuzzy（失真） checkpoint。
innodb使用buffer pool来避免数据直接写入磁盘。这样数据可以再buffer pool中多次修改并最终写入磁盘，这样就减少了磁盘IO。buffer pool中维护了几个重要的list：free list、LRU list、flush list。当有新的数据读入buffer pool中时，会从free list中分配page。当free list中没有空闲page时，需要等待flush list中的数据刷到磁盘，这样很慢。所以innodb会定期的把flush list中的旧数据刷到磁盘。
再者，innodb redo log文件是循环使用的，所以必须保证日志文件在重写前，所有buffer pool中相关的脏数据刷新的磁盘，不然数据库宕机后这些数据就会丢失。因为日志是按照数据修改的时间记录的，所以旧的脏数据会被先刷到磁盘，这也就是fuzzy checkpoint的工作。因为日志中的旧数据已经刷新到磁盘，所以数据库宕机后，实例恢复会从fuzzy checkpoint后的LSN开始。
当数据库正常工作时，会进行fuzzy checkpoint

**innodb_flush_log_at_trx_commit** 控制重做日志刷新到磁盘的策略

- 1（默认）：事务提交时，必须调用一次fysnc同步到磁盘，宕机不会丢失事务
- 0：事务提交不用不会触发redo log写操作，而是依赖master thread每秒刷盘操作
- 2：每次提交只write写入到os buffer，并不保证写入到磁盘，crash不会丢失事务，宕机会丢失事
- 
日常场景设置为1,系统高峰期可以修改成2以应对大负载。

各个事务可以交叉拷贝到log bufeer中，一次事务commit触发的写redo(fysnc)到文件，可能隐式的帮别的线程也写了redo log，从而达到group commit操作。



**redo log 和 二进制文件的区别**

1. 二进制文件时存储引擎上层产生的，不管什么存储引擎都会产生二进制文件，redo log是innodb层产生的只记录存储引擎层表的修改，
2. 二进制日志是基于行格式修改，本质也是sql。redo log是物理日志记录每个page的修改(mini transaction)
3. 二进制是每次事务提交后写入缓存中的日志文件，而redo log在数据准备修改前写入缓存中的redo log中，写入完成后才会执行修改操作
4. 二进制日志只在提交时一次性写入，二进制记录顺序和提交顺序一致，redo log记录的物理页的修改，redo log文件同一个事务可能多次修改，
5. 二进制记录所有影响数据库的操作，记录的内容较多。eg. 插入记录一次，删除记录一次，而redo log记录的物理页的情况，插入一次删除一次前后状态一致
务

## MVCC 多版本并发控制

innodb 每个事务开始会 在 系统版本号（全局） 递增加一作为事务的版本号，用来和查询到的每行记录的版本号进行比较。在每行记录后面会保存两个隐藏的列，一：创建修改时系统版本号 二：过期时系统版本号

**select**: [](https://segmentfault.com/a/1190000012650596)



# 推荐阅读

-   http://mysql.taobao.org/monthly/2017/12/01/
-   https://segmentfault.com/a/1190000012650596