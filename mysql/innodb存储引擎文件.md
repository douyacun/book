# 存储引擎相关

**innodb和myisam的区别**

- innodb支持事务，myisam不支持事务

- innodb支持外键，myisam不支持，innodb 外键表转myisam会报错

- innodb是聚集索引，myisam是非聚集索引，聚集索引的数据存放在主键的叶子节点上，非聚集索引的存放的是主键的id，需要查询两次。myisam是非聚集索引，数据文件是分离的，索引保存数据文件的指针

- innodb不保存具体行数，`select count(*) from user`需要扫面全表
- innodb最小粒度锁是行锁，myisam是表锁

**myisam static和myisam dynamic 有什么区别**

static所有字段都有固定宽度，dynqmic是变长

# 数据类型

