# mysql表压缩


# 压缩需求场景
 1. 客户端和服务端数据量传输量大，需要压缩节约带宽
 2. 某个列数据量大，只针对某个列压缩
 3. 某个表或几个表太大需要压缩节省空间

场景一 使用mysql的压缩协议

场景二 使用mysql的压缩函数

场景三 使用mysql存储引擎压缩


# mysql压缩协议

## 适用场景
- 查询大量数据，比如数据导出
- 复制时binlog量大，启用slave_compressed_protocol参数进行日志压缩复制

## 压缩协议

客户端和服务端都支持 zlib 算法，会导致服务器cpu上升

```
mysql -u root -p --compress
```

```
mysqldump -u root -p --compress db table > db.sql
```

主从复制中启用压缩传输，在丛机开启 slave_compressed_protocol=1


# MySQL列压缩

mysql压缩函数compress一个字符串然后返回一个二进制串，需要mysql服务端支持压缩，否则返回null，压缩的字段采用varbinary或blob字段类型保存，使用uncompress进行解压缩

```
inert into test (content) valued (compress(repeat(a, 1000));

select from uncompress(content) from test

select uncompress_length(content) as length, length(content) as compress_length from test
```

# MySQL 表压缩解决方案


## 为什么需要压缩


- 数据库几乎都是io负载，cpu负载不高
- 尤其是日志型和监控型的数据库，会有大量数据插入
- 如果大部分都是字符串，并且重复数据比较多，压缩效果比较好

## innodb压缩

前提
- innodb_file_per_table=on
- innodb_file_format=Barracuda

可以设置 `key_block_size=n` 开启压缩，n=1,2,4,8,16
单位是k, n越小 压缩比越高

可以调整 innodb_compression_level 设置压缩的级别，默认6，越小压缩越高

## 压缩算法

zlib压缩库，l777无损压缩算法，l777 原理查找数据重复序列号然后进行压缩

## 压缩表在buffer_pool中的处理
在 buffer_pool 中，压缩的数据通过 key_block_size 大小的也页来保存数据，如果要保存压缩的数据或更新压缩数据对应的列，则会创建一个未压缩页来解压缩数据

## 评估 key_block_size 大小

information_schema 库中 innodb_cmp_per_index 会收集表和索引的压缩情况，必须开启innodb_cmp_per_index_enable,生产环境不要开启

- 如果失败比较多，需要开大一点
- 一般建议设置为8

## 实践

```
set global innodb_file_per_table=1
set global innodb_file_format=Barracuda

create table test (
    id int primary key auto_increament;
    content text；
)engine=innodb row_format=compressed key_block_size=8;
```