---
Title: explain详解
Keywords: explain
Description: 详细了解explain参数含义，后续还有trace的用法
Label: explain
Date: 2019-12-26 09:36:00
LastEditTime: 2019-12-26 09:37:00
---



```shell
mysql> explain select * from media where id = 100\G;
*************************** 1. row ***************************
           id: 1
  select_type: SIMPLE
        table: media
   partitions: NULL
         type: const
possible_keys: PRIMARY
          key: PRIMARY
      key_len: 4
          ref: const
         rows: 1
     filtered: 100.00
        Extra: NULL
1 row in set, 1 warning (0.00 sec)
```

列的含义：

- id: 每个select都会自动分配一个唯一的标识符
- select_type: select 查询的类型
- table：查询的是哪张表
- partitions：匹配哪个区
- type：join类型
- possible_keys: 此次查询中可能选用的索引
- key：此次查询中确认用到的索引
- ref：哪个字段或常数与key一起被使用
- rows：此查询一共扫描了多少行，估计值
- filtered：此查询条件所过滤数据的百分比
- Extra：额外的信息

