---
title: mysql复制_常用复制方式_mysql复制配置
keywords: mysql复制，复制方式，配置
desc: 
author: douyacun
date: 2019-01-20
---

- mysql复制方式
- 复制解决的问题，常见用途
- 配置复制-新的服务器
- 从另一个服务器开始复制
- 复制拓扑

# mysql复制方式
- 基于行的复制
- 基于语句的复制

# 复制解决的问题及常见用途

==数据分布==

mysql复制通常不会给带宽造成很大压力，即使在不稳定的网络环境中，也可以正常工作

==负载均衡==

通过mysql复制可以读操作分布到多个服务器上

==备份==

对于备份来说，复制即使不是备份也能取代备份

==高可用性和故障切换==

能够帮组应用程序避免mysql单点失败

==mysql升级测试==

使用更高版本的mysql作为备库，保证升级全部实例前，mysql能在备库按照预期执行

# 配置复制
一般步骤：
1. 在每台服务器上创建复制账号
2. 配置主库和备库
3. 通知备库连接到主库并从主库复制数据

## 在每台服务器上创建主账号

```
grant replication slave, replication client 
on *.* 
to slave@"192.168.100.2"
identified by "password"
```
帐号限制在本地，复制账户只需要主库的`replication slave`权限，并不需要每一断服务器`replication client权限`目的是为了：
- 用来监控和管理的复制账号都需要`replication client`权限，两种目的使用同一个账号更方便一点
- 两端都设置方便主库和备库的切换

## 配置主库和备库

**主库** 
```
log_bin: mysql-bin
server_id: 10
```

使用`show master status`检查主库设置

**备库**
```
log_bin=mysql-bin
server_id=11
# 指定中继日志的位置和命名
relay_log=/var/lib/mysql/mysql-relay-log
# 允许将其重放的事件也记录到自身的二进制日志中
log_slave_updates=1
read_only=1
```

# 从另一个服务器开始复制

让主库和备库保持同步的三个条件：
- 某个时间点的主库的数据快照
- 主库当前的二进制文件，获得数据快照时该二进制日志文件中的偏移量
- 从快照时间到现在的二进制日志

从其他服务器克隆备库的方法：
- 冷备份，最基本方法是关闭主库，把数据复制到备库，重启主库后，会使用一个新的二进制文件，在备库执行`change master to`指向这个文件的起始处
- 热备份，如果仅使用了myisam表，可以在主库使用`mysqlhotcopy` 或者 `rsync` 来复制数据
- mysqldump,`mysqldump --single-transaction --all-databases --master-data=1 --host=serve1 | mysql -host=server2`


# 复制拓扑

基本原则：
- 一个mysql备库实例只能有一个主库
- 每个备库必须有一个唯一的id
- 一个主库可以有多个备库
- 如果打开了`log_slave_updates`,一个备库可以把其主库上的数据变化传播到其他备库

## 一主库多备库
用途：
- 为不同的角色使用不同的备库(例如使用不同的索引使用不同的存储引擎)
- 把一台主库当作待用的主库，除了复制没有其他数据传输
- 一台备库放到远程数据中心，用作灾难恢复
- 延迟一个或多个备库，以备灾难恢复
- 使用一个备库，作为备份，培训，开发，或者测试使用的服务器

## 主动-被动模式下的主-主复制
这种方式使得反复切换主动被动服务器非常方便，因为服务器是对称的，可以在不关闭服务器的情况下执行维护，优化表，升级操作系统或其他任务

例如，执行`alter table`语句，可能会锁住整个表,阻塞对表的读和写，可能会花费很长时间并导致服务中断，在主-主配置下，可以先停止主动服务器上的备库复制线程，这样就不会在被动服务器执行任何更新操作，然后在被动服务器上执行alter操作，交换角色，最后在先前的主动服务器上启动复制线程，这个服务器将会读取中级日志执行相同的alter语句

配置：
1. 确保两台服务器有相同的数据
2. 启用二进制日志，选择唯一的服务器id，并创建复制账号
3. 启用备库更新的日志记录，这是故障转移和故障恢复的关键
4. 把被动服务器设置为只读，防止可能与主动服务器上的更新产生冲突
5. 启动每个服务器的mysql实例
6. 将每个主库设置为对方的备库，使用新创建的二进制日志开启工作

切换主主的角色，所需要的步骤：
1. 停止主动服务器上的所有写入
2. 在主动服务器上执行`set global read_only=1`,同时在配置文件中也设置`read_only`,防止重启后失效，这个不会阻止root的写入，如果阻止所有人更改数据，`flush tables with read lock`
3. 在主动服务器上`show master status`,并记录二进制日志坐标
4. 使用主动服务器上的二进制日志坐标，在被动服务器上执行`select master_pos_wait()`,该语句将阻塞住，直到被动服务器跟上
5. 在被动服务器上执行`set global read_only=0`,这样就变成主动服务器
6. 修改应用的配置，使其写入到新的主动服务器中