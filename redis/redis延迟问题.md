---
Title: redis_延迟高问题解决思路_'latency_monitor_threshold'
Keywords: redis,延迟高,latency_monitor_threshold
Description: redis源码阅读
Label: redis延迟
Author: douyacun
Cover: 
Date: 2019-06-13 14:37:52
LastEditTime: 2019-10-14 12:46:40
---

# redis延迟高问题解决思路
延迟是 客户端从发出命令与客户端接收命令的回复之间的最大延迟，通常redis的处理时间是非常低的，但是存在高延迟的某些条件

解决思路：
1. 确保没有慢执行的命令，通过redis慢查询日志查看
2. 对于EC2用户，EC2是基于HVM，否则fork太慢了
3. 禁用transparent_hugepage，`echo never > /sys/kernel/mm/transparent_hugepage/enabled`, 然后重启redis
4. 可能存在与redis无关的内在延迟，`redis-cli --intrinsic-latency 100`, 需要redis-server服务器上运行
5. aof/rdb 持久化 性能/延迟的权衡
   1. AOF+fsync 总是很慢，不推荐使用
   2. AOF+fsync+everysec 每秒执行一次，推荐(也可能会造成阻塞的问题,[当执行bgrewrite对aof进程归并优化时，redis出现被阻塞的问题](./aof.md#1))
   3. AOF+fsync+everysec+no-appendfsync-no-write 主进程不调用fsync()操作，最长不超过30秒，推荐
   4. AOF+fsync never
   5. RDB 配置触发save条件

## 网络和通信引起的延迟
1Gbit/s的网络延迟大约为200us，uninx域套接字的延迟低至30us，如果在同一个服务器可以用unix_socket加速，redis启用unix_socket

`redis.conf`
```conf
unixsocket /var/run/redis/redis.sock
unixsocketperm 777
```
php 连接redis
```php
$redis->connect('/var/run/redis/redis.sock');
```
解决方案：
- 优先选择物理机托管服务器
- 不要系统的连接/断开服务器
- 如果客户端与服务器位于同一主机上，使用域套接字
- 尽量使用MSET/MGET批量操作
- redis不适合绑定在单个cpu上,bgsave/aof重写会抢占主进程的时间

## redis单线程特性
redis主要使用单线程设计，多路复用为客户端的请求提供服务，所有请求都是安装顺序提供

## 慢查询导致的延迟
单线程的结果是，当有慢查询时所有其他客户端将等待该请求完成

如果有慢查询时必须的可以放在salve上执行所有慢查询

查看慢查询日志
```shell
slowlog get 2
```
sort\lrem\sunion\keys 这些都慢查询命令，用scan代替keys

## fork生成的延迟
开启RDB和AOF重写，redis必须fork后台进程，fork操作可能会阻塞主进程，fork涉及到复制大量对象，尤其对于使用虚拟内存机制的页表
linux/64，内存为4kb每页，每个进程存储一个页表，对于24G内存的redis实例需要(24GB/4kb)*8=48M,每次fork都会分配和复制48M内存

## transparent_hugepage 引起的延迟
transparent_hugepage linux内核尽可能的尝试分配2M的页大小给程序使用，内核空间在内存中自身是2M对齐的，目的是减少内核TLB的压力(cpu使用的一小块关联内存)，如果内存没有2M的连续大小的空间可分配，内核会回退到分配4kB

禁用transparent_hugepage[](/linux/页缓存.md#pdflush)
```shell
echo never > /sys/kernel/mm/transparent_hugepage/enabled
```
strace只显示慢速系统调用:
```shell
sudo strace -f -p $(pidof redis-server) -T -e trace=fdatasync,write 2>&1 | grep -v '0.0' | grep -v unfinished
```

## swap内存交换引起的延迟
- redis使用的内存比可用内存多
- 某些进程产生了大量的读取和写入I/O，因为文件是会被缓存的，会增加内核压力增大文件内存缓存，产生内存交换，包括RDB和AOF

[vmstat](/linux/vmstat.md)
```shell
[root@00h ~]# vmstat 5
procs -----------memory---------- ---swap-- -----io---- -system-- ------cpu-----
 r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st
 2  0      0  93016  16576 212056    0    0    81   151    1   11  2  0 98  0  0
 0  0      0  92892  16588 212116    0    0    44     0  288  591  1  0 98  1  0
 0  0      0  92892  16616 212068    0    0    28     0  322  663  0  1 99  0  0
 0  0      0  92892  16628 212084    0    0    12     0  326  650  1  1 98  0  0
^C
```
swap si/so 为0,说明swap没有内存交换活动

[iostat](/linux/iostat.md)
```shell
[www@00h www.00h.tv]$ iostat -xk 1
Linux 3.10.0-957.1.3.el7.x86_64 (00h) 	06/15/2019 	_x86_64_	(1 CPU)

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
           1.54    0.00    0.43    0.20    0.00   97.83

Device:         rrqm/s   wrqm/s     r/s     w/s    rkB/s    wkB/s avgrq-sz avgqu-sz   await r_await w_await  svctm  %util
vda               0.16     1.14    1.58    1.71    81.23   150.43   140.57     0.04   10.98   14.13    8.08   0.97   0.32
```
如果延迟问题是由于redis内存在磁盘上交换造成，需要降低系统内存压力，增加更多的内存

## AOF和磁盘I/O造成的延迟
AOF有两个系统调用：

write：将数据写入aof文件，产生阻塞的情况：1. 当系统缓冲区已满 2. 系统正在将缓冲区的内容写入到文件

fdatasync: 刷新内核文件缓冲区的文件到磁盘上，相比fsync要[高效](/linux/sync_fsync_fdatasync.md#性能)

redis可以配置3种fsync：
- appendfysnc=no redis不执行fsync，再次配置中，唯一的延迟源是write
- appendfsync=everysec redis每秒执行一次，使用不同的线程，如果fsync仍在进行中，redis使用缓冲区将write调用延迟两秒（如果fsync正在针对同一文件进行，write会在Linux上阻塞），如果fsync占用时间太长，redis最终也会执行write
- appendfsync=always 每次写入都执行fsync，这种情况下性能一般非常低

可以使用[strace](/linux/strace.md)查看进程对fdatasync/write的调用
```
sudo strace -p $(pidof redis-server) -T -e trace=fdatasync，write
```

## redis过期产生的延迟
redis使key过期的方式：
- 惰性淘汰：在key请求时，判断key是否失效，失效就淘汰
- 定时策略：每隔100毫秒检测一次

redis没100毫秒会启动一个[serverCron](/redis/serverCron.md)来执行定期淘汰策略
- `ACTIVE_EXPIRE_CYCLE_LOOKUPS_PER_LOOP` 移除所有已过期的键
- 如果发现超过25%的键过期，会重复执行

正常情况下,`ACTIVE_EXPIRE_CYCLE_LOOKUPS_PER_LOOP`默认情况下设置为20，每秒执行10次，通常每秒只有200个密钥主动过期，每秒200个key过期对redis实例是没有影响的，
然而算法是自适应的，如果发现该组采样key中发现超过25%的key已经过期，该算法会循环，如果数据库有超过25%的key可以在同一秒过期，并且这些key占当前key的25%

# 如何启用延迟监控
设置延迟阀值(单位：毫秒)，只有超过阀值的事件才会被记录成为延迟峰值，根据需要设置
```conf
config set latency-monitor-threshold 100
```

# redis测量延迟工具
```shell
[work@cpc-dev01 ~]$ redis-cli -p 6379 --intrinsic-latency 100
Max latency so far: 1 microseconds.
Max latency so far: 3 microseconds.
Max latency so far: 4 microseconds.
Max latency so far: 5 microseconds.
Max latency so far: 9 microseconds.
Max latency so far: 13 microseconds.
Max latency so far: 17 microseconds.
Max latency so far: 28 microseconds.
Max latency so far: 110 microseconds.
Max latency so far: 159 microseconds.
Max latency so far: 548 microseconds.
^C
685295282 total runs (avg latency: 0.1459 microseconds / 145.92 nanoseconds per run).
Worst run took 3755x longer than the average latency.
```
这个命令要在服务器上运行

# latency 查看延迟信息
- `latency latest` 最新延迟事件
- `latency history command` 从事件时间序列中获取原始数据非常有用
- `latency reset` 不带参数，重置所有事件
- `latency graph command` 生成ASCII图形
```shell
#
|
|
|_

11
77
mm
```
这个图要竖着看，17m 代表17分钟前发生

# 延迟诊断 
`latency history`

# 推荐阅读
- [Redis latency problems troubleshooting](https://redis.io/topics/latency)


# 总结
1. 有时候也需要注意一下redis服务器的流量负载情况，由于redis本身的高性能服务，可能会出现直接把服务器带宽打满的情况
2. 注意请求的复杂度，建议将O(n)的甚至是更复杂的操作，排序，多键操作等指令转化放在redis slave上
   - 案例1: 项目推送模块redis优化，将ZREVRANGE(O(log(N)+M)， N 为有序集的基数，而 M 为结果集的基数)操作转换为list(O(1))的操作，QPS从5000上升到6500
   - 在线用户统计模块，包含了大量的keys的遍历操作，取消keys操作以后，吞吐量回升
3. 不必要的操作,`EXISTS`、`DEL`这样的操作，很多场景下是不需要的，QPS不必要的情况，取消这些可以提升有效用户请求的数量
   - XX项目点播项目redis实例，平均存在37%的EXISTS请求，每次GET、ZRANG、HMGET前都会执行，用于判断是否存在，redis对这些不存在的key都会提示，可以直接取消EXISTS请求
   - XX项目推送业务redis实例中存在25.5%的DEL操作，占据40%的时间，后将key直接设置过期时间
   - 具体案例具体分析，存在必有其道理
4. IO优化，主要是redis快照和AOF产生的，根据数据的主要成读来设置快照频率，或者快照值在slave上发生
5. CPU的使用频率，redis是单线程作业，除了分离业务、读写分离。`keepalive + twemproxy` 也能有效提升redis的CPU利用率