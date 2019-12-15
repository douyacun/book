---
Title: mysql ACID事务含义
Keywords: ACID，事务含义
Description: 
Cover: 
Label: 
Date: 2019-02-25 23:02:12
LastEditTime: 2019-11-29 11:36:34
---


# ACID

- ACID 是注重可靠性的设计原则
- mysql 引入 innodb components 并紧密结合ACID model,因此数据不会因为异常条件损坏丢失，同时具有innodb的灾难恢复和故障重启特性

## atomicity 原子性

原子性主要与mysql的事物相关联

- autocommit setting
- commit statement
- rollback statement
- information_schema 中的操作数据


## consistency 一致性

与 innodb 进程关联，防止进程崩溃数据损坏

- innodb doublewrite buffer
- innodb crash recover

## isolation 隔离性

隔离性和 myaql 的事物关联，设置 isolation level，适应每个事物的隔离级别

- autocommit
- set isolation level statement
- 低级别的事物系列 && 索引性能优化 都可以查看information_schema


## durability 持久性

mysql 软件特性与特定的硬件配置交互,大多数情况依赖于cpu, 网络，存储的性能

- innob doublewrite buffer,innodb_doublrwrite 配置项是否开启
- innodb_flush_log_at_trx_commit 控制每次事物提交写入flush_log的时机，评估数据安全和io性能上的平衡
- sync_binlog 控制mysql server 
- innodb_file_per_table
- writer buffer
- 电池支持的磁盘缓冲
- 支持fsync的操作系统
- 用不断电的电源保护mysql服务进程和数据存储
- 备份策略，
    - 周期
    - 备份方式
    - 备份保留时间
- 对于分布式和托管的应用，mysql server的位置和网络连通性很重要


> doublewrite buffer \
dowblwrite buffer 是一种 file flush技术，innodb 将 pages 写入数据文件之前,会首先写入到 double buffer 中，只有写入和刷新double bufffer完成后，才会将他们写入对应的数据文件位置，如果在pages写入过程中发生操作系统或mysql进程崩溃，innodb可以从 double buffer 拷贝数据，虽然写入需要两次，但是double buffer 不需要两倍i/o开销，

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

## redo

innodb 在事物提交时，必须先将事务的所有事务写入到磁盘上的redo log file 和undo log file中进行持久化，为了确保每次日志都能写入到事务日志文件中，每次都会调用fsync，将内存中脏页同步到磁盘上

**innodb_flush_log_at_trx_commit** 控制重做日志刷新到磁盘的策略

- 1（默认）：事务提交时，必须调用一次fysnc同步到磁盘
- 0：事务提交不用讲log buffer写入的os buffer中，而是依赖master thread每秒将log buffer写入到os buffer并调用fsync
- 2：每次提交都仅写入到os buffer，然后是每秒调用fsync()将os buffer中的日志写入到log file on disk。