mysql中常见的日志文件：
- 错误日志(error log)
- 二进制日志(binlog)
- 慢查询日志(slow log)
- 查询日志(log)

# 错误日志
查看错误日志的位置
```shell
mysql> show variables like 'log_error'\G;
```

# 慢查询日志

开启慢查询日志`log_slow_queries`
```shell
mysql> show variables like 'log_slowqueries'\G;
```
慢查询时间`long_query_time`
```shell
mysql> show variables like 'long_query_time'\G
```

sql没有用到索引也会记录到慢查询日志
```shell
mysql> show variables like 'long_throttle_queries_not_using_indexes'\G;
```

mysqldumpslow查看执行时间最长的10条sql
```shell
$ mysqldumpslow -s -al -n 10 mysqlslow.log
```

mysql支持将慢查询日志写入到slow_log表，**log_output**指定慢查询输出格式，默认为file，设置为table会写入到slow_log表

```shell
mysql> show create table mysql.slow_log;
```

# 查询日志

genrea_log

# 二进制日志
- max_binlog_size: 二进制文件最大值，超过该值产生新的文件，后缀加1
- binlog_cache_size: innodb所有未提交的事物都会被记录缓存中去，事物提交时，在写二进制日志中。当一个session开启一个事物时，会自动初始化一个大小为binlog_cache_size大小的缓存，当事物的记录大于binlog_cache_size的时候，会把缓冲的日志写入一个临时文件
    - binlog_cache_use 缓冲写二进制日志次数 
    - binlog_cache_disk_use 临时文件写二进制日志次数

查看binlog文件的内容：
```shell
mysql> mysqlbinlog --start-position=203 douyacun.00007
```

# 套接字文件
本地mysql可以使用套接字文件来连接
```shell
mysql> show variables like 'socket';
```

# pid文件
进程pid，可以用pid文件来重启进程
```shell
mysql> show variables like "";
```

