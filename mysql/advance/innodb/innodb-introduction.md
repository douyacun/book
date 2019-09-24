# innodb 介绍

# 优势
- 灾难恢复，重启即可
- 独立的缓冲区，使用的数据通常直接来自内存，专门的数据库服务器，80%的内存会交给 buffer pool
- 关联数据表字段可以使用外键，innodb会强制参照完整性，更新，删除数据关联表会自动更新,次表中如果插入主表没有的记录，会抛出异常
- 数据在设备或内存丢失损坏，checksum可以检验
- 适当的列作为自增主键，innodb会自动优化。在where,group by, order by,join中使用主键速度会非常快
- cache buffer会自动优化insert,update,delete, innodb允许读写并发访问，同样缓存修改来简化设备Io
- innodb能长时间执行大表查询，多次查询的row，Adaptive Hash Index 会接管这些查找，像hash表查找一样
- 可以压缩表和索引
- 可以添加或删除索引，对表的性能和可用性影响很小
- 磁盘布局使 内存表 和 长文本类型 动态格式的字段更高效
- information_schema表 可以监控网络状态
- performance_schema表 可以监控*存储引擎状态
- innodb可以和其他引擎的表混合使用，也可以关联查询，配置中声明默认也没有影响
- innodb专为大数据性能而生
- 操作系统限制文件大小2g,mysql也可以处理大数据

# innodb 实践
- 适合作为主键的列
    - 经常查询的一列或多个列
    - 自增的列
- 外键
    - join多表关联的列
    - 关键列设置外键
    - 关联列设置为相同的字段类型
    - 设置外键的列的主表的该列设置为索引
- 自动提交
    - 如果没有autocommit，每秒数百次的提交会被限制性能
- 事物
    - 事物要包裹在 transactions / commit 中，否则大量的修改会一直执行
    - innodb_rollback_on_timout 事物超时回滚，事物内sql报错，程序会中断，设置为on超时会自动回滚
- 表锁
    - 不要用 lock_tables 锁全表，innodb可以处理多个会话的同一个表的读写，可以使用行锁 select ... for update
锁住需要更新的一行
- innodb_file_per_table 
    - 每个表都有自己独立的空间
    - 数据和索引存在单独的自己的表空间
    - 默认启用
- 表压缩
    - 评估表压缩或页面压缩是否数据存储和访问的读写性能有提升
    - 适合i/o密集型，读多写少，不适合cpu密集型。

# 验证 innodb default storage engine

```
show engine

select * from information_schema
```


# innodb 测试

设置默认存储引擎为innodb ,验证各项服务是否正常

重启mysql进程命令行方式使用参数 `--default-storage-engine=innodb` 或者设置配置文件
```
[mysqld]
default-storage-engine=innodb
```
- 修改默认存储引擎，只会影响新建表，如果一些表依赖于其他存储引擎特性，会报错，可以添加选项 ENGINE=other_engine_name 避开这种错误

- 模拟真实环境工作量，测试mysql服务器性能，安装最后版本的mysql，运行基准测试

-测试整个应用的生命周期，安装，大量使用，服务重启，可以通过kill进程来模拟断电的情况，重启服务验证数据恢复正常

# turning off innodb

默认使用innodb作为默认存储引擎

- --skip-innodb 已经弃用

可以使用同样功能的选项，向前兼容
- --innodb=off
- --disable-innodb