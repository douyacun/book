innodb关键特性包括：
- 插入缓冲 (insert buffer)
- 两次写(double write)
- 自适应hash索引(adaptive hash index)
- 异步I/O (async i/o)
- 刷新临界页 (flush neighbor page)

# 插入缓冲
## insert buffer

每张表都有一个主键索引(没有明确指明，会默认生成一个不可见的主键索引)，主键索引(聚集索引)的插入顺序是非常快的，但是不可能每张表只有一个聚集索引，更多情况下一张表会有很多辅助索引，对于主键索引的插入式顺序存放的，但是对于辅助索引的叶子节点的插入就不是顺序的了。

innodb 对于非聚集索引的插入和更新操作，不是每一次直接插入到索引页，而是先判断索引页是否在缓冲池中,若有直接插入，否则，先放入到insert buffer中。再以一定频率合并insert buffer和索引子节点。通常能将多个插入操作合并到一个操作中，因为在一个索引页中。使用insert buffer需要同时满足两个条件：
- 索引是辅助索引
- 索引不是唯一索引

`show engine innodb status\G;`可以查看insert buffer的使用情况

- seg_size 代表insert buffer大小为 n*16k
- free list len代表空闲列表的长度
- inserts代表插入记录数
- merged recs代表了合并后的插入记录数
- merges 代表合并的次数，实际读取页的次数

> 缓冲池: 是为了通过内存速度来弥补磁盘速度较慢对于性能的影响，包括insert buffer、data pag、index page、lock info、adaptive hash index

**IBUF_POOL_SIZE_PER_MAX_SIZE**可以调整insert_buffer_pool可以占用缓冲池的大小

## change buffer
change buffer可以对DML操作(insert,delete,update)进行缓冲操作,适用对象仍然是非唯一的辅助索引， 对一条记录update的步骤：
- 将记录标记为已删除
- 真正将记录删除

**innodb_change_buffer_max_size**控制change buffer最大使用内存的数量，25表示最多使用1/4缓冲池空间

# double write
插入缓冲给innodb存储引擎带来的是性能上的提升，double write提高了innodb存储引擎的数据页的可靠性

那么double write做了什么？\
数据库宕机时，可能对数据页只写入部分，这种情况称为部分写失效。可以通过redo log来恢复，在写入失效时，先通过页的副本还原该页，在进行重做，这就是double write

double write有一段2m的内存空间，在磁盘上还有2m*的共享表空间，数据先从内存空间写入到共享空间，立即调用fsync同步，此时是顺序写入速度快，在完成写入后，再将共享空间的数据写入到各个表空间文件中。此时写入是离散的。double write写入时会合并写入操作，消耗要小于两次io的消耗

```shell
mysql> show global status like 'innodb_dblwr%\G'
```

- innodb_dblwr_pages_writen: inndob一共写了多少页
- innodb_dblwr_writes: 实际写入次数

`skip_innodb_doublewrite`可以禁止使用doublewrite功能

# 自适应hash索引
hash索引的查找复杂度O(1),B+树的查找取决于B+树的高度，生产环境一般是3～4,需要3～4次

innodb会监控对表上各索引的查询，如果二级索引被频繁访问，成为热点数据，建立hash索引可以带来，速度的提升

hash索引有一个要求，查询模式必须是一样的，必须是等值查询

**`innodb_adaptive_hash_index`**: 是否开启自适应hash索引

# 异步I/O - AIO
AIO优势:
1. 多个I/O异步执行
2. 合并多个I/O为同步操作

mysql read、脏页刷新，磁盘写入都是通过AIO来实现的

# 刷新邻接页
原理是：innodb刷新脏页时，会检测该页所在区的所有页，如果有脏页AIO就会将多个I/O操作合并为一个操作。
**innodb_flush_neighbors** 可以控制是否启用，传统机械键盘建议启用，固态硬盘建议关闭