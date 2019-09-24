# mysql分库分区

建议看下：
- [淘宝DB团队](http://mysql.taobao.org/monthly/2017/11/09/)
- [淘宝中间件团队](http://jm.taobao.org/2017/01/19/20170119/)

1. 为什么分库分表
2. 分区类型及限制
- 范围分区 `RANGE`
- 列表分区 `LIST`
- 哈希分区 `HASH`
- KEY分区
3. 常见问题
4. 总结

# 为什么分库分表

innodb使用`B+tree`结构存储数据和索引，单表数据量大时时，会出现以下几个情况:
1. 单机硬件限制
    - 单台数据库的存储能力不够，单个DB的磁盘不足以存储大量数据
    - 单机网络和cpu有瓶颈
2. 单机服务效率问题
    - 数据读取效率低，`Btree`深度加深，效率降低
    - 事物并发效率降低，并发更新、并发新增可能会锁表，
        > 事物并发，需要依赖加锁，来解决并发的一致性问题 
        > 1. 自增长计数器：auto-increment counter 每个包含子增加id的表都会维护一个
        > 2. 表锁：对自增长计数器锁定，auto-inc Locking，是一种个特殊锁，不是在事物提交后释放，而是在获取自增id后就释放
        > 3. 优化：使用互斥量(mutex)， 替代表锁(auto-inc locking)
    - 索引数据量大时，维护索引的效率也会降低
3. 单表过大，限制了表结构调整，限制了业务升级


# RANGE分区

基于给定连续区间，把多行分配给分区

常见的是基于时间字段，最好是整型，可以使用函数
- `year()`
- `to_seconds()`
- `unix_timestamp()`

`range`
```
create table access_log
(
    id         int auto_increment,
    ip         int unsigned,
    created_at timestamp,
    primary key (id)
) engine = innodb default charset utf8mb4 collate utf8mb4_unicode_ci
partition by range (id)(
    partition p0 values less than (10),
    partition p1 values less than (20)
);
```

`date`
```sql
create table my_range_datetime(
id int unsigned primary_key autoincrement,
hired_at datetime
)
partition by range (year(hired_at)) (
partition p2017 values less than (2017),
partition p2018 values less than (2018),
partition p2019 values less than (2019)
)
```

增加分区,  最大值分区 `maxvalue`
```
alter table access_log add partition (partition p2 values less than (maxvalue))
```

删除2017年的数据，直接删除分区就行了
```
alter table slate drop partition p2017;
```


# list分区

list是枚举值集合，range是连续区间值的集合，list的值必须是整数

```sql
CREATE TABLE employees (
    id INT NOT NULL,
    name VARCHAR(30),
    hired DATE NOT NULL DEFAULT '1970-01-01',
    separated DATE NOT NULL DEFAULT '9999-12-31',
    store_id INT
)
PARTITION BY LIST(store_id)(
    PARTITION pNorth VALUES IN (3,5,6,9,17),
    PARTITION pEast VALUES IN (1,2,10,11,19,20),
    PARTITION pWest VALUES IN (4,12,13,14,18),
    PARTITION pCentral VALUES IN (7,8,15,16)
)
```

# hash分区

- 数据庞大，并没有明显分区的特征字段
- 基于分区个数，将数据分配到不同的分区

```sql
create table member(
    id int not null autoincreament primary_key,
    name varchar(30),
    created_at timestamp,
    job_code int
)
partition by hash(id),
partitions 4;
```
- partitions 指的是分区数，如上有4个分区，默认1
- 底层实现是基于mod
    ```
    mod(year(2017),4) = mod(2017,4) = 1
    ```

每100万条记录一个分区
```sql
CREATE TABLE sales (
    id INT PRIMARY KEY AUTO_INCREMENT,
    amount DOUBLE NOT NULL,
    order_day DATETIME NOT NULL
) ENGINE=Innodb PARTITION BY HASH(id DIV 1000000);
```

# linear hash分区

与hash不同的是，他基于另一种算法，不是基于mod

```sql
create table member(
    id int not null autoincreament primary_key,
    name varchar(30),
    created_at timestamp,
    job_code int
)
partition by linear hash(id),
partitions 4;
```

优点
- 适用于TB级别
- 增加，删除，合并拆分分区会更快

缺点
- 相对于hash数据分布不均匀

# key 分区

相似于hash分区,不同点
- key分区允许多列，hash分区只允许一列
- 如果在有主键或者唯一键的情况下，key中的分区可不指定，默认为主键或者唯一键，没有必须显示指定
- key分区必须基于列不能是基于列的表达式
- 算法不一样，MOD取值对象是表达式的返回值，key是基于列的md5值

```sql
create table member(
    id int not null autoincreament primary_key,
    name varchar(30),
    created_at timestamp,
    job_code int
)
partition by key(),
partitions 4;
``` 
限制

- 一个表最多1024个分区
- 分区表达式必须是整数，或者返回整数的表达式，也可以直接使用列来分区
- 如果分区字段有主键或唯一索引列，主键列和唯一索引列都必须包含进来
- 分区表无法使用外键约束

# 常见问题
分库分表
1. 多维度分库分表

如果数据查询使用的维度和数据拆分分布的规格不一致， 单挑sql可能会在多个分库分表上执行，出现跨库查询，会增加网络I/O成本，查询效率必然下降

eg. 按照用户维度分表，同时又需要根据商品纬度查询

思路：

    - 原则还是让sql在单库上完成
    - ‘空间换效率’， 冗余存储多份，根据不同业务场景进行拆分，保持拆分维度和使用维度统一

eg.  join联合查询

    - 尽量保证参与join的数据表尽量保证拆分纬度统一，否则可能会产生多个物理分库的join,进行多个底层的实例大量数据传输
    - 如果不能保证拆分纬度的统一，原则是尽量减少join数据传输
    - 如果join的表数据量很小，更新少，这些表无需拆分，通常采用单表模式，统一存储在一个分库下边，然后将小表全量或者增量变更复制到分库分表中，将join控制在单单机join
    - 
    
2.  [分布式事务一致性](../最终一致性/README.md)    

4. 分库分表单表多大合适
    - 单表1000万
        - 数值型800万以下
        - 字符型500万以下

5. 业务需要强一致性，就算有主从一致性问题
6. 



# 总结

- mysql如果存在主键或者唯一键，分区列必须包含在其中
- 原生的list,range,hash分区，分区时返回的只能是整数
- 分区字段不能为null,要不然没法确定分区，尽量为not null
