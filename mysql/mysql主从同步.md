---
Title: mysql主从同步
Keywords: 主从同步,master-slave,bin_log,relay_log
Description: mysql主从同步配置一台内网服务器和一台外网服务器
Label: 主从同步
Author: douyacun
Date: 2019-02-01
LastEditTime: 2019-10-09 20:34:48
typora-root-url: ./assert
---

家里一台联想笔记本作为爬虫服务器，有一台公网阿里云服务器，目的爬虫跑完之后，公网服务器可以主从同步，家里的服务器和公网服务器通信的问题通过[内网穿透工具(frp)](https://github.com/fatedier/frp)解决, 优秀简单，具体可以看[frp内网穿透_mysql服务_ssh服务_http服务](/linux/frp内网穿透_mysql服务_ssh服务_http服务.md)

MYSQL主从同步是在MySQL主从复制(Master-Slave Replication)基础上实现的，通过设置在Master MySQL上的binlog(使其处于打开状态)，Slave MySQL上通过一个I/O线程从Master MySQL上读取binlog，然后传输到Slave MySQL的中继日志中，然后Slave MySQL的SQL线程从中继日志中读取中继日志，然后应用到Slave MySQL的数据库中。这样实现了主从数据同步功能。

# 配置

## 主库

1. 配置
```conf
# 日志文件名
log-bin = mysql-bin

# 主数据库端ID号
server-id = 1

# 不同步哪些数据库
binlog-ignore-db = mysql
binlog-ignore-db = test
binlog-ignore-db = information_schema

# 只同步哪些数据库，除此之外，其他不同步
binlog-do-db = game

# 日志保留时间
expire_logs_days = 10

# 控制binlog的写入频率。每执行多少次事务写入一次
# 这个参数性能消耗很大，但可减小MySQL崩溃造成的损失
sync_binlog = 5

# 日志格式，建议默认mixed
# statement 保存SQL语句
# row 保存影响记录数据
# mixed 前面两种的结合
binlog_format = mixed
```

2. 重启数据库，创建同步账户
```sql
-- 创建slave帐号slave，密码123456, 这里host 是 127.0.0.1，frp连接的内网，不需要开放，大家可以写成%
grant replication slave on *.* to 'slave'@'127.0.0.1' identified by '123456';


CREATE DATABASE videos DEFAULT CHARSET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 更新数据库权限
flush privileges;
```
> 可以去这里看[frp内网穿透_mysql服务_ssh服务_http服务](/linux/frp内网穿透_mysql服务_ssh服务_http服务.md)

1. 查询master的状态
```
mysql> show master status;
+------------------+----------+--------------+------------------+
| File             | Position | Binlog_Do_DB | Binlog_Ignore_DB |
+------------------+----------+--------------+------------------+
| mysql-bin.000001 |      196 |              |                  |
+------------------+----------+--------------+------------------+
1 row in set
```

## 从库

1. 配置
```conf
log-bin=mysql-bin
server-id=20
```

2. 执行同步命令
```sql
-- 执行同步命令，设置主数据库ip，同步帐号密码，同步位置
change master to master_host='127.0.0.1',master_port=3307, master_user='slave',master_password='123456',master_log_file='mysql-bin.000003',master_log_pos=151459295;

-- 开启同步功能
start slave;
```

3. 检查从库状态, 在执行几条sql看看sql同步情况
```
mysql> show slave status\G;
*************************** 1. row ***************************
               Slave_IO_State: Waiting for master to send event
                  Master_Host: 127.0.0.1
                  Master_User: slave
                  Master_Port: 3307
                Connect_Retry: 60
              Master_Log_File: mysql-bin.000001
          Read_Master_Log_Pos: 185295
               Relay_Log_File: 00h-relay-bin.000002
                Relay_Log_Pos: 756
        Relay_Master_Log_File: mysql-bin.000001
             Slave_IO_Running: Yes    -----------> 这里是连接成功
            Slave_SQL_Running: Yes    -----------> 这里sql执行成功
            ......
1 row in set (0.00 sec)
```

4. 常用
```sql
-- 停止主从同步
stop slave;

-- 连接断开时，重新连接超时时间
change master to master_connect_retry=50;

-- 开启主从同步
start slave;
```


# 关于`binlog`和`relaylog`

## 格式化

主库binlog
```
 mysqlbinlog --base64-output=decode-rows -v -v /var/lib/mysql/mysql-bin.000001 > binlog
```

从库relaylog
```
mysqlbinlog --base64-output=decode-rows -v -v mysql-relay-bin.000001 > relaylog
```

##  binlog和relaylog的rotate机制
binlog
- Rotate：每一条binary log写入完成后，都会判断当前文件是否超过 max_binlog_size(默认1g)，如果超过则自动生成一个binlog file
- delete: expire-logs-days 只在 实例启动时 和 flush logs 时以及文件文件是否超过 max_binlog_size时判断是否有过期的binlog文件，如果文件访问时间早于设定值，则purge file

relaylog
- Rotate：每从Master fetch一个events后，判断当前文件是否超过 max_relay_log_size（默认1g) 如果超过则自动生成一个新的relay-log-file
- Delete：purge-relay-log 在SQL Thread每执行完一个events时判断，如果该relay-log 已经不再需要则自动删除


## 配置解析

binlog 配置
```
mysql> show variables like "%bin%";
+--------------------------------------------+--------------------------------+
| Variable_name                              | Value                          |
+--------------------------------------------+--------------------------------+
| bind_address                               | *                              |
| binlog_cache_size                          | 32768                          |
| binlog_checksum                            | CRC32                          |
| binlog_direct_non_transactional_updates    | OFF                            |
| binlog_error_action                        | ABORT_SERVER                   |
| binlog_format                              | ROW                            |
| binlog_group_commit_sync_delay             | 0                              |
| binlog_group_commit_sync_no_delay_count    | 0                              |
| binlog_gtid_simple_recovery                | ON                             |
| binlog_max_flush_queue_time                | 0                              |
| binlog_order_commits                       | ON                             |
| binlog_row_image                           | FULL                           |
| binlog_rows_query_log_events               | OFF                            |
| binlog_stmt_cache_size                     | 32768                          |
| binlog_transaction_dependency_history_size | 25000                          |
| binlog_transaction_dependency_tracking     | COMMIT_ORDER                   |
| innodb_api_enable_binlog                   | OFF                            |
| innodb_locks_unsafe_for_binlog             | OFF                            |
| log_bin                                    | ON                             |
| log_bin_basename                           | /var/lib/mysql/mysql-bin       |
| log_bin_index                              | /var/lib/mysql/mysql-bin.index |
| log_bin_trust_function_creators            | OFF                            |
| log_bin_use_v1_row_events                  | OFF                            |
| log_statements_unsafe_for_binlog           | ON                             |
| max_binlog_cache_size                      | 18446744073709547520           |
| max_binlog_size                            | 1073741824                     |
| max_binlog_stmt_cache_size                 | 18446744073709547520           |
| sql_log_bin                                | ON                             |
| sync_binlog                                | 1                              |
+--------------------------------------------+--------------------------------+
29 rows in set (0.01 sec)
```

- log_bin
设置此参数表示启用binlog功能，并指定路径名称
- log_bin_index
设置此参数是指定二进制索引文件的路径与名称
- binlog_do_db
此参数表示只记录指定数据库的二进制日志
- binlog_ignore_db
此参数表示不记录指定的数据库的二进制日志
- max_binlog_cache_size
此参数表示binlog使用的内存最大的尺寸
- binlog_cache_size
此参数表示binlog使用的内存大小，可以通过状态变量binlog_cache_use和binlog_cache_disk_use来帮助测试。
- binlog_cache_use：使用二进制日志缓存的事务数量
- binlog_cache_disk_use:使用二进制日志缓存但超过binlog_cache_size值并使用临时文件来保存事务中的语句的事务数量
- max_binlog_size
Binlog最大值，最大和默认值是1GB，该设置并不能严格控制Binlog的大小，尤其是Binlog比较靠近最大值而又遇到一个比较大事务时，为了保证事务的完整性，不可能做切换日志的动作，只能将该事务的所有SQL都记录进当前日志，直到事务结束
- sync_binlog”：这个参数是对于MySQL系统来说是至关重要的，他不仅影响到Binlog对MySQL所带来的性能损耗，而且还影响到MySQL中数据的完整性。对于“sync_binlog”参数的各种设置的说明如下：
sync_binlog=0，当事务提交之后，MySQL不做fsync之类的磁盘同步指令刷新binlog_cache中的信息到磁盘，而让Filesystem自行决定什么时候来做同步，或者cache满了之后才同步到磁盘。
sync_binlog=n，当每进行n次事务提交之后，MySQL将进行一次fsync之类的磁盘同步指令来将binlog_cache中的数据强制写入磁盘。
在MySQL中系统默认的设置是sync_binlog=0，也就是不做任何强制性的磁盘刷新指令，这时候的性能是最好的，但是风险也是最大的。因为一旦系统Crash，在binlog_cache中的所有binlog信息都会被丢失。而当设置为“1”的时候，是最安全但是性能损耗最大的设置。因为当设置为1的时候，即使系统Crash，也最多丢失binlog_cache中未完成的一个事务，对实际数据没有任何实质性影响。
从以往经验和相关测试来看，对于高并发事务的系统来说，“sync_binlog”设置为0和设置为1的系统写入性能差距可能高达5倍甚至更多

relaylog 配置
```
mysql> show variables like '%relay%';
+---------------------------+------------------------------------+
| Variable_name             | Value                              |
+---------------------------+------------------------------------+
| max_relay_log_size        | 0                                  |
| relay_log                 |                                    |
| relay_log_basename        | /var/lib/mysql/00h-relay-bin       |
| relay_log_index           | /var/lib/mysql/00h-relay-bin.index |
| relay_log_info_file       | relay-log.info                     |
| relay_log_info_repository | FILE                               |
| relay_log_purge           | ON                                 |
| relay_log_recovery        | OFF                                |
| relay_log_space_limit     | 0                                  |
| sync_relay_log            | 10000                              |
| sync_relay_log_info       | 10000                              |
+---------------------------+------------------------------------+
11 rows in set (0.01 sec)
```

- max_relay_log_size：标记relay log 允许的最大值，如果该值为0，则默认值为max_binlog_size(1G)；如果不为0，则max_relay_log_size则为最大的relay_log文件大小；
- relay_log：定义relay_log的位置和名称，如果值为空，则默认位置在数据文件的目录，文件名为host_name-relay-bin.nnnnnn（By default, relay log file names have the form host_name-relay-bin.nnnnnn in the data directory）；
- relay_log_index：同relay_log，定义relay_log的位置和名称；
- relay_log_info_file：设置relay-log.info的位置和名称（relay-log.info记录MASTER的binary_log的恢复位置和relay_log的位置）
- relay_log_purge：是否自动清空不再需要中继日志时。默认值为1(启用)。
- relay_log_recovery：当slave从库宕机后，假如relay-log损坏了，导致一部分中继日志没有处理，则自动放弃所有未执行的relay-log，并且重新从master上获取日志，这样就保证了relay-log的完整性。默认情况下该功能是关闭的，将relay_log_recovery的值设置为 1时，可在slave从库上开启该功能，建议开启。
- relay_log_space_limit：防止中继日志写满磁盘，这里设置中继日志最大限额。但此设置存在主库崩溃，从库中继日志不全的情况，不到万不得已，不推荐使用；
- sync_relay_log：这个参数和sync_binlog是一样的，当设置为1时，slave的I/O线程每次接收到master发送过来的binlog日志都要写入系统缓冲区，然后刷入relay log中继日志里，这样是最安全的，因为在崩溃的时候，你最多会丢失一个事务，但会造成磁盘的大量I/O。当设置为0时，并不是马上就刷入中继日志里，而是由操作系统决定何时来写入，虽然安全性降低了，但减少了大量的磁盘I/O操作。这个值默认是0，可动态修改，建议采用默认值。
- sync_relay_log_info：这个参数和sync_relay_log参数一样，当设置为1时，slave的I/O线程每次接收到master发送过来的binlog日志都要写入系统缓冲区，然后刷入relay-log.info里，这样是最安全的，因为在崩溃的时候，你最多会丢失一个事务，但会造成磁盘的大量I/O。当设置为0时，并不是马上就刷入relay-log.info里，而是由操作系统决定何时来写入，虽然安全性降低了，但减少了大量的磁盘I/O操作。这个值默认是0，可动态修改，建议采用默认值。


# mysql 主从同步原理
主库将变更 -> binlog\
从库io线程 -> relay中继日志\
从库sql线程 realylog 读取 -> 执行一遍sql\

从库同步主库数据的过程是串行化的，主库上的并行操作在从库上串行执行的，导致的问题：
- 从库同步是有延迟的
- 如果主库宕机，恰好数据没有同步到从库，可能造成数据不一致的情况

mysql 解决机制：
- 半同步复制：解决数据丢失问题
- 并行复制：解决主从同步延时问题

半同步复制，主库写入binlog，强制将数据同步给从库，从库将数据写入relaylog后，返回ack给主库，主库收到至少一个ack之后，认为写操作完成

并行复制，从库开启多个线程，并行读取relaylog不同库的日志，并行重放不同库的日志，是库级别

# myaql主从同步延时问题

如果主从同步延时比较严重，解决方案：
- 分库，将一个主库拆分为多个主库，每个主库的写压力就小了几倍，此时延迟问题就会小了
- 并行复制，多个库并行复制如果某个库的写入特别高，并行复制还是没
- 重构代码