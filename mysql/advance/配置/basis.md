---
title: mysql_配置调优_buffer配置优化_配置详解
desc: mysql配置,配置优化，buffer配置，配置详解
keywords: mysql,配置，调优，优化, 缓冲配置，buffer,配置
author: douyacun
date: 2019-01-14
---

Mysql没有最好的配置，只能不断调优适应当前环境

- mysql 基础配置
- 缓冲(buffer)配置


## mysql基础配置

```
[mysqld]
datadir=/var/lib/mysql
socket=/var/lib/mysql/mysql.sock
pid_file=/var/lib/mysql/mysql.pid
user=mysql
port=3306
default_storage_engine=innodb
max_allow_packet=<value>
sql_mode=STRICT_TRANS_TABLES

# INNODB
innodb_buffer_pool_size=<value>
innodb_log_file_size=<value>
innodb_file_per_table=1
innodb_flush_method=o_direct
innodb_log_buffer_size=1m
innodb_flush_log_at_trx_commit=1

# Myisam
key_buffer_size=<value>

# LOG
log_error=/var/lib/mysql/mysql-error.log
slow_query_log=/var/lib/mysql/mysql-slow.log

# other
tmp_table_size=32M
max_heap_table_size=32M
query_cache_type=0
query_cache_size=0
max_connextions=<value>
thread_cache=<value>
table_cache=<value>
table_cache_size=<value>
open_files_limit=65535

# client
socket=/var/lib/mysql/mysql.sock
port=3306
```
数据文件`datadir`放在`/var/lib`下面，这是unix常用目录，也是共识

`pid`和`socket`文件最好指定目录，升级时就不会改变位置，造成不必要的麻烦

指定系统用户`mysql`，需要mysql有对数据文件位置`/var/lib/mysql`读写权限

`port`默认端口，共识，有时安全考虑会更改

`default_storage_engine`: innob作为默认存储引擎，支持事物，mvvc，也是官方推荐的引擎，最好显式的指定

`max_allow_packet`: 控制mysql可以接受多大的包，太小： 备库不能接收主库发送过来的数据，可能导致数据被截断或变为null，可以增加到16m

`sysdate_is_now`: 系统时间一致，保证主库和从库的时间一致

`sql_mod`: sql的安全模式
- `ONLY_FULL_GROUP_BY` 对于GROUP BY聚合操作，如果在SELECT中的列，没有在GROUP BY中出现，那么这个SQL是不合法的，因为列不在GROUP BY从句中, 会报错
  ```
  Expression #3 of SELECT list is not in GROUP BY clause and contains nonaggregated column 'jojotoo_test.s.id' which is not functionally dependent on columns in GROUP BY clause; this is incompatible with sql_mode=only_full_group_by
  ```
- `NO_AUTO_VALUE_ON_ZERO` 该值影响自增长列的插入。默认设置下，插入0或NULL代表生成下一个自增长值。如果用户 希望插入的值为0，而该列又是自增长的，那么这个选项就有用了。$$
- `STRICT_TRANS_TABLES`: 在该模式下，如果一个值不能插入到一个事务表中，则中断当前的操作，对非事务表不做限制
- `NO_ZERO_IN_DATE`: 在严格模式下，不允许日期和月份为零
- `ERROR_FOR_DIVISION_BY_ZERO`: 在INSERT或UPDATE过程中，如果数据被零除，则产生错误而非警告。如 果未给出该模式，那么数据被零除时MySQL返回NULL
- `NO_AUTO_CREATE_USER`: 禁止GRANT创建密码为空的用户
- `NO_ENGINE_SUBSTITUTION`: 如果需要的存储引擎被禁用或未编译，那么抛出错误。不设置此值时，用默认的存储引擎替代，并抛出一个异常
- `PIPES_AS_CONCAT`: 将"||"视为字符串的连接操作符而非或运算符，这和Oracle数据库是一样的，也和字符串的拼接函数Concat相类似
- `ANSI_QUOTES`: 启用ANSI_QUOTES后，不能用双引号来引用字符串，因为它被解释为识别符

# innodb & 缓冲日志

`innodb`: 设置为FORCE,只有innodb存储引擎可以启动时，mysql才可以启动

`innodb_autoinc_lock_mode`: 控制innodb如何生成自增主键值，高并发插入时，自增主键值可能是个瓶颈，如果有很多事物在等待自增锁(`show_engine_innodb_status`可以查看)

`innodb_io_capacity`: 告诉innodb 服务器有多大io能力，默认每秒100个io操作，ssd磁盘可以设为上万io

`innodb_read_io_thread` & `innodb_write_io_thread`: 控制后台可以有多少后台线程可以被io使用，默认4个线程读4个线程写

`innodb_strict_mod`: 让mysql在某些条件下把警告改成抛错



整体的日志文件大小受限于`innodb_log_file_size` 和 `innodb_log_files_in_group`


`innodb_log_bufersize`：可以控制日志缓冲区的大小，推荐值是1-8m,

 缓冲(buffer)配置
1. 从服务器内存总量开始
2. 减去操作系统内存占用，如果mysql不是唯一占用内存的程序需要扣掉其他进程占用的内存
3. 减去mysql自身需要的内存，每个查询操作分配的一些缓冲
4. 减少足够让操作系统缓存innodb日志文件的内存，至少是足够缓存最近经常访问的部分，留一些内部至少可以缓存二进制日志的最后一部分，备库读取主库可能会给主库造成压力
5. 减去其他配置的myslq缓存和缓存需要的内存，如Myisam的 `key cache` 或 `query_cache`
6. 除以`105%`,差太多接近缓冲池增加的自身管理开销
7. 结果四舍五入，向下取，少了不太会影响结果，多了可能会导致很严重的问题

配置缓冲大小时，宁可谨慎，而不是把他们配置的过大，如果把缓冲池设的比他小了20%，很可能只对 性能有小的影响，也可能只影响几个百分点，如果大了20%,很可能造成严重的问题：内存交换，磁盘抖动，甚至内存耗尽和硬件死机

`innodb_flush_log_at_trx_commit`：
- 0：日志缓冲写到日志文件，每秒钟刷新一次
- 1：日志缓冲写到日志文件，每次事物提交都刷新到缓冲区，这是默认设置，保证不会丢失已提交的事物
- 2：每次提交时把日志写到缓冲文件，但并不会刷新，innodb每秒中刷新一次

todo:: 需进一步分析\
`innodb_file_per_table`: 为每张表都使用一个单独的文件，

`innodb_max_purge_lag`: innodb开启延迟后面的语句更新之前，可以等待被清除的最大事物数量，也许会影响性能，但伤害很少

`innodb_doublewrite`: innodb用双写缓冲来避免页没写完整导致的数据损坏，当innodb从缓冲池刷新到磁盘时，会首先刷新`doublewrite buffer`,然后再写到专属区域，保证每个页面写入都是原子并且持久化的

# 二进制日志配置
`sync_binlog`: 控制mysql怎么刷新二进制日志到磁盘，默认为0，一般为0或1,设置为1可能比`innodb_flush_log_at_trx_commit`对性能的影响大，每次写入都需要更新元信息
- `0`：mysql并不刷新，由操作系统决定什么时候刷新二进制日志到磁盘
- `>0`： 表示两次刷新到磁盘的动作之间间隔多少次二进制日志写操作

`autocommit`: 配置`autocommit`之后每个独立的语句都是一次写，否则就是一个事物一次写刷新二进制日志到磁盘

`expire_log_days`: 自动清理二进制文件，不要用 `rm`去删除, 建议留下7-14天的二进制文件，即使每天都做备份，你会感谢写1－2个星期的二进制文件，给自己留一些呼吸空间

# innodb并发配置
`innodb_thread_concurrency`: 限制一次性可以有多少线程进入内核访问数据

`innodb_commit_concurrency`: 控制有多少个线程可以在同一时间提交

`max_connection`：保证服务器不会因为应用程序突增链接导致崩溃

`thread_cache_size`: 线程缓冲大小，观察状态变量`thread_connected`，看下一般情况的最大值和最小值

`max_connection_errors`: 客户端链接最大尝试次数

# 主从数据库相关
`read_only`: 只接受主库传输过来的变更，禁止没有特权的用户在备库做变更，强烈建议把把从库设置为只读

`skip_slave_start`: myaql试图自动启动复制，在崩溃之后，启动复制是不安全的，手动检查之后再启动

`slave_net_start`: 与主库网络链接失败之后，重新连接之前等待的时间，默认一个小时，太长了1分钟就可以

# 总结

innodb最重要的两个配置
- `innodb_buffer_pool_size`
- `innodb_log_file_size`