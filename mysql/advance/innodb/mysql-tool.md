# mysql 工具

# `show profiles`

[推荐站点](http://www.ywnds.com/?p=8677)

设置全局

`show variables like '%profil%'`
```
profiling 开启关闭sql语句分析功能
profiling_history_size 保留数目，缺省值15
```

启用session级别

`set profiling = 1`

查看session已经产生的profile

`show profiles`

查看指定sql的所有开销，

`show profile all for query 2`

- all 所有
- cpu
- memory
- block

`show profile`后续版本会被记录到 `infomation_schema.profiling`，即将被移除

# pt-query-digest

Analyze MySQL queries from logs, processlist, and tcpdump.

# performance schema

# show global status

统计 每秒查询数，线程连接数，正在执行查询的线程数

```
mysqladmin ext -i1 | awk '
/Queries/{q=$4-qp;qp=$4}
/Threads_connected/{tc=$4}
/Threads_running/{printf "%5d %5d %5d\n",q,tc,tr=$4}'
```
对服务器压力小，可以长时间运行

# show process list
观察线程状态
```
mysql -e 'show processlist\G' | grep State: | sort | uniq -c | sort -rn
```