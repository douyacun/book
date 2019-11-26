与innodb存储引擎有关的有表空间文件和重做日志文件(redo log)

# 表空间文件
innodb数据是按表来存储数据的，默认配置下会有初始大小为10M，文件名为ibdata1的文件。

**innodb_data_file_path=/data/ibdata1:2048M;/otherdir/ibdata2:2048M:autoextend**

用两个文件用来组成表空间，若两个文件位于不同的磁盘上，磁盘负载可能被均衡，可以提高数据库的整体性能。文件2G，autoextend，该文件可以自动增长。

**innodb_file_per_table=on**

设置之后会生成独立表空间文件(.ibd)，仅存储该表的数据、索引、插入缓冲的bitmap信息。其余还是存储在默认的表空间里

# 重做日志文件(redo_log)
重做日志记录了innodb存储引擎的事物日志
**innodb_log_file_size**: 指定了重做日志文件大小
**innodb_log_files_in_group**: 日志文件组重做日志个数，默认2，a写满了写入b，b写满了在写入a
**innodb_mirrored_log_group**: 日志镜像文件组的数量，默认为1，只有1个文件没有镜像。

**innodb_log_group_home_dir**:日志文件组所在路径

redo log写入过程：

先写入缓冲区(redo log buffer), 再按一定条件写入redo log，然后写入磁盘，每次512字节(扇区大小)保证写入的是成功的

写入redo log的条件有：
- 主线程每秒会将redo log内存缓存写入磁盘缓存，不论事物是否提交
- **innodb_flush_log_at_trx_commit**:     
    - 1 commit时将redo log写入磁盘， fsync调用
    - 2 写入缓冲区，不确保写入redo log
    - 0 不将事物写入redo log，等待主线程每秒的刷新


