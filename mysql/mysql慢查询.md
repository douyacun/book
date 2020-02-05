---
Title: mysql慢查询
Keywords：slow sql,慢查询
Description: 日志慢sql优化总计
Label:
Cover:
Date: 2020-01-30 14:24:00
LastEditTime: 2020-01-30 14:24:00
---

# 条件字段函数操作

```sql
CREATE TABLE `tradelog` (
  `id` int(11) NOT NULL,
  `tradeid` varchar(32) DEFAULT NULL,
  `operator` int(11) DEFAULT NULL,
  `t_modified` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `t_modified` (`t_modified`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

统计7月份的交易记录总数：

```sql
-- 用不上索引
select count(*) from tradelog where month(t_modified) = 7

-- 用的上索引
select count(*) from tradelog where (t_modified >= '2016-7-1' and t_modified<'2016-8-1') or (t_modified >= '2017-7-1' and t_modified<'2017-8-1') or (t_modified >= '2018-7-1' and t_modified<'2018-8-1') or (t_modified >= '2019-7-1' and t_modified<'2019-8-1') or
```

# 类型转换

**隐式类型转化**

```sql
select * from tradelog where tradeid=110717;
```

`tradeid` 表中结构式varchar类型，而输入的sql是整数类型，需要做类型转换。相当于

```sql
select * from tradelog where case(tradeid as signed int)=110717;
```

**隐式字符编码转换**

```sql
CREATE TABLE `trade_detail` (
  `id` int(11) NOT NULL,
  `tradeid` varchar(32) DEFAULT NULL,
  `trade_step` int(11) DEFAULT NULL, /*操作步骤*/
  `step_info` varchar(32) DEFAULT NULL, /*步骤信息*/
  PRIMARY KEY (`id`),
  KEY `tradeid` (`tradeid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

`trade_detail`  字符集是utf8和 `tradelog` 字符集是utf8mb4

```sql
select d.* from tradelog l, trade_detail d where d.tradeid=l.tradeid and l.id=2;
```



# 等待锁

**MDL锁（meta data lock）**



