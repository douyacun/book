---
title: innodb_多版本控制_mvc
description: inmodb事物日志
---


innodb 多版本

# multi-version


## rollback segment
- innodb 是一个多版本存储引擎
- 为了支持事物回滚和事物并发,数据存储在rollback segment数据结构中
- innodb使用rollback segment的数据完成事物的rollback数据回滚操作
- innodb使用rollback segment的数据构建未修改版本，支持一致性读

## 3个隐藏字段

innodb默认在每个节点上有3个字段
- 6byte DB_TRX_ID 最后插入或更新这条记录的事物id，删除在内部被认为是更新，有一个特殊的位表示这条记录已经删除
- 7 byte DB_ROLL_PTR 回滚指针，回滚指针指向 roll back segment 的 undo log 记录，如果这条记录更新了，他会重新生成更新之前的必要信息
- 6 byte DB_ROW_ID 是一个自动增加的行号，如果是innodb 自动生成的聚簇索引，索引中会包含这个行，否则这个行号不会出现在任何索引中

## undo log
- undo log 分为 insert undo log 和 update undo log,
- insert log只用于事物回滚，事物提交之后会被立即丢弃.
- update undo log 也用于一致性读，在事物没有创建快照之前无法丢弃的，此时需要update undo log中的数据构建早期版本的row
- 要经常的 commit 事物，包含一致性读的事物，否则事物无法丢失 update undo log 中的数据，导致rollback segment过大，填满表空间
- rollback segment的 undo log 记录占用磁盘空间一般比表中相应的记录小
- 在innodb中，执行一个 delete sql ,不会立即从磁盘上删除这条记录，只有在删除update undo log中对应的删除记录后删除磁盘上的行和索引中对应的记录，通常采用和删除sql相同的时间顺序
- 如果插入和删除以相同的速率执行，清除的速率就会变慢，表会越来越大由于这些 `dead line`, 使所有的进程受到磁盘速率限制变得很慢，可以调整 innodb_max_purge_tag,让系统分配更多的资源给purge 线程

# multi-version and secondary index

- innodb对待聚簇索引和辅助索引不一样，记录在系统隐藏列会指向undo log中的选项，可以从undo log 重构早期版本数据，不想聚簇索引，辅助索引没有隐藏列，不能一步更新到位
- 辅助索引的列更新，旧的辅助索引记录会被标记删除，新的记录插入，然后标记删除的索引会被清除，当辅助索引被标记删除或二级索引页面被新的事物更新时，innodb会在聚簇索引搜索数据记录，在聚簇索引中，如果读取到事物启动后修改了记录，则会从undo log检索之前的版本
- 辅助索引的记录被标记为删除，或辅助索引页被新事物更新，innodb会从聚簇索引中检索索引，而不是从聚簇索引返回

