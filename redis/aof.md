---
Title: redis AOF增量同步
Keywords: aof
Description: bgsave全量同步，aof增量同步
Author: douyacun
Label: redis aof
Date: 2019-05-27 16:13:13
LastEditTime: 2019-12-27 22:52:55
---
# aof 文件增量

当appendfsync选项的值直接决定AOF持久化功能的效率和安全性
- always: 每个时间循环都要将aof_buf缓冲区的内容写入到AOF文件，并且同步AOF文件，效率也是最安全的
- everysec: 每个事件循环都要将AOF缓冲区的所有内容写入到AOF文件，并且每隔一秒就要在子线程中对AOF文件进行一次同步
- no: 服务器在每个时间玄幻都要将aof_buf缓冲区中的所有内容写入到AOF文件，何时将内存的AOF文件同步到磁盘中，由操作系统决定

- 当AOF持久化功能打开状态时，服务器在执行完一个写命令，会以协议格式将被将被执行的写命令追加到服务器状态的AOF_buf缓冲区的末尾，记录服务器状态
- 命令请求会先写入到AOF缓存里面，之后再定期写入到AOF文件
- 服务器只要载入并重新执行保存在AOF文件中的命令，可以还原数据库本来的状态
- AOF重写可以产生一个新的AOF文件，这个新的AOF文件和原有的AOF文件所保存额数据库状态一样
- AOF，程序无需对现有AOF文件进行读入，分析或者写入操作
- BGREWAITERAOF,redis服务器会维护一个AOF重写缓冲区，该缓冲区会在子进程创建AOF文件的工作之后


# 在一个采用aof模式的服务器上，当执行bgrewrite对aof进程归并优化时，redis出现被阻塞的问题#1
redis会fork一个子进程，处理bgwriteaof进行归并优化，不应该影响redis的正常服务，按照正常逻辑在一台多核的机器上，即使子进程占慢了CPU和磁盘，不应该导致redis服务阻塞

redis服务设置了`appendfsync everysec`，每秒会调用`fsync`，要求刷新内核文件缓冲的内容到磁盘上，由于子进程同时也在写硬盘，从而导致主进程fsync()/write()操作被阻塞，导致主进程被阻塞

简便的方法是设置: `no-appendfsync-no-write yes` 在子进程处理和写磁盘时，主进程不调用fsync操作，即使进程不调用fsync(), 内核也会在适当的时机将数据写到硬盘