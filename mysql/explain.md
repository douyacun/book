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

# select_type

-   SIMPLE: 此查询不包含子查询或者union查询
-   PRIMARY：此查询是最外层查询
-   UNION: 此查询是UNION的第二或随后的查询
-   DEPENDEND UNION, UNION中的第二个或后面的查询语句，取决于外面的查询
-   UNION RESULT: UNION的结果
-   SUBQUERY: 子查询的中第一个SELECT
-   DEPENDEND SUBQUERY: 子查询中的第一个SELECT，取决于外面的的查询
-   MATERIALIZED：物理化，将查询结果作为临时表以便后续需要时直接使用结果，详情见[Optimizing Subqueries with Materialization](https://dev.mysql.com/doc/refman/8.0/en/subquery-materialization.html)

```sql
explain select m.target_id, (
    select d.content_after from m_log d
    where d.target_id = m.target_id
    and d.target_type = 5
    and d.create_time <= "2019-11-13 07:59:30"
    and d.create_time >= "2019-10-01 00:00:00"
    order by d.id desc limit 0,1
) as 变更后金额
from `m_log` m
where m.target_type = 5
  and m.target_id in (select child from `relation` where parent = 1698747)
and m.create_time <= "2019-11-13 07:59:30"
and m.create_time >= "2019-10-01 00:00:00"
group by m.target_id;
```

| id   | select_type        |    table    | type | possible_keys             | key           | key_len | ref               | rows | Extra                           |
| ---- | ------------------ | :---------: | ---- | ------------------------- | ------------- | ------- | ----------------- | ---- | ------------------------------- |
| 1    | PRIMARY            | <subquery3> | ALL  | NULL                      | NULL          | NULL    | NULL              | NULL | Using temporary; Using filesort |
| 1    | PRIMARY            |      m      | ref  | create_time,idx_target_id | idx_target_id | 4       | <subquery3>.child | 2    | Using where                     |
| 3    | **MATERIALIZED**   |  relation   | ref  | child,idx_par_st          | idx_par_st    | 4       | const             | 14   | Using index                     |
| 2    | DEPENDENT SUBQUERY |      d      | ref  | create_time,idx_target_id | idx_target_id | 4       | func              | 2    | Using where; Using filesort     |

# type

-   system：表只有一行，这是一个`const` type 的特殊情况。
-   consts:  最多只有一行匹配。当你使用主键或者唯一索引的时候，就是`const`类型

```SQL
explain SELECT * from foo where id = 1;
```

| id   | select_type | table | type | possible_keys | key     | key_len | ref  | rows  |               Extra               |
| ---- | ----------- | ----- | ---- | ------------- | ------- | ------- | ---- | ----- | :-------------------------------: |
| 1    | SIMPLE      | foo   |      | const         | PRIMARY | PRIMARY | 4    | const | Directly search via Primary Index |

-   eq_ref: 通常出现在join中，表示前一列的结果都只能匹配到后一行的结果，并且查询的匹配的操作通常是 `=`,查询效率较高

```sql
explain select * from u, `u_ocpc` where u.id = u_ocpc.u_id and u.id in (2657099,2657010,2656981)
```

| id   | select_type | table  | type   | possible_keys | key     | key_len | ref                   | rows |    Extra    |
| ---- | ----------- | ------ | ------ | ------------- | ------- | ------- | --------------------- | ---- | :---------: |
| 1    | SIMPLE      | u_ocpc | range  | PRIMARY       | PRIMARY | 4       | NULL                  | 3    | Using where |
| 1    | SIMPLE      | u      | eq_ref | PRIMARY       | PRIMARY | 4       | adv.unit_ocpc.unit_id | 1    |    NULL     |

-   ref: 多表join，触发联合索引最左原则，或者这个索引不是唯一索引/主键索引

```sql
explain select * from `config` C left JOIN `dark` D on D.medias = C.value where C.group_id = 1;
```

| id   | select_type | table | type    | possible_keys       |         key         | key_len | ref   | rows | Extra                                              |
| ---- | ----------- | ----- | ------- | ------------------- | :-----------------: | ------- | ----- | ---- | -------------------------------------------------- |
| 1    | SIMPLE      | C     | **ref** | config_group_id_key | config_group_id_key | 4       | const | 12   | NULL                                               |
| 1    | SIMPLE      | D     | ALL     | NULL                |        NULL         | NULL    | NULL  | 4    | Using where; Using join buffer (Block Nested Loop) |

-   unique_subquery：使用了in的子查询，而且子查询是主键或者唯一索引

