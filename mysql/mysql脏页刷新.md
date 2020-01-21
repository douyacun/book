---
Title: mysql脏页刷新
Keywords: dirty_page,脏页
Description: 
Label:
Cover:
Date: 2020-01-20 11:48:00
LastEditTime: 2020-01-20 12:52:32
---

[toc]

# InnoDB刷脏页的控制策略

[脏页](../linux/页缓存.md#dirty_page)：**当内存数据页跟磁盘数据页内容不一致的时候，我们称这个内存页为“脏页”**



首先告诉innodb刷脏页的I/O能力，建议设置磁盘IOPS。可以使用FIO工具来测试：

**安装FIO**

```
yum install -y fio
```

**测试iops**

```
fio -filename=iotest -direct=1 -iodepth 1 -thread -rw=randrw -ioengine=psync -bs=16k -size=500M -numjobs=10 -runtime=10 -group_reporting -name=mytest
```

-   filename=iotest：测试文件名称，通常选择需要测试的盘的data目录
-   direct=1：测试过程绕过机器自带的buffer。使测试结果更真实
-   rw：测试类型
    -   randwrite：测试随机写的I/O
    -   randrw：测试随机写和读的I/O
    -   andread：测试随机读的I/O
-   bs=4k：单次io的块文件大小为4k
-   bsrange=512-2048：同上，提定数据块的大小范围
-   size=60g：本次的测试文件大小为60g，以每次4k的io进行测试
-   numjobs=10：本次的测试线程为10
-   runtime=10：测试时间为10秒，如果不写则一直将5g文件分4k每次写完为止
-   ioengine=psync：io引擎使用pync方式
-   rwmixwrite=30：在混合读写的模式下，写占30%
-   group_reporting：关于显示结果的，汇总每个进程的信息。
-   lockmem=1g：只使用1g内存进行测试
-   zero_buffers：用0初始化系统buffer
-   nrfiles=1：每个进程生成文件的数量。

如果是MySQL的写入速度很慢，TPS很低，但是数据库主机的IO压力并不大, 就要考虑一下IOPS设置是否合理了：

```conf
# 默认是300
innodb_io_capacity = 300
```

**脏页比例**

脏页页比例，不要让它经常接近75%。

```sql
show global status like 'Innodb_buffer_pool_pages_dirty';
show global status like 'Innodb_buffer_pool_pages_total';
select :Innodb_buffer_pool_pages_dirty/:Innodb_buffer_pool_pages_total;
```

