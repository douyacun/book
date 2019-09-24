# innodb 内核

# 基础知识

## 事物ACID

- 原子性：指的是整个事物要么全部成功，要么全部失败，的对于innodb,只要收到client的commit报文，事物一定是成功的，收到rollback报文事物一定是失败的，所有操作一定要回滚掉，如果连接中断或server crash也要保证事物回滚，通过undo log保证事物回滚
- 一致性：指的是任何时刻，正常提供服务的时候，从异常恢复过来的时候，数据都是一致的，保证不会读到中间状态的数据，主要通过crash recover和double write buffer保证数据的一致性
- 隔离性：多个事物可以同时对事物进行修改，隔离级别： 读未提交，读已提交，不可重复读，序列化
- 持久性：事物的commit数据一定不会丢失，内部通过redo log实现

## 多版本控制 multi-version control

指的是提高并发的技术，早期数据库只有读读可以并发，读写，写读，写写都要堵塞，引入多版本之后，只有写写会堵塞，innodb是在undolog中实现的，通过undolog可以找回历史数据，找回的历史版本可以提供给用户读，innodb内部维护了一个全局活跃读写事物数组

## 垃圾清理 purge

对于用户删除的数据，innodb并不是立即删除，而是标记一下，后台purge线程批量的删除，过期的undolog也需要回收，过期是指不需要用来构建之前的版本，也不需要回滚

## 回滚段 rollback segment

回滚段可以理解为事物的修改链，链表最前面的是最老的版本，后面是最新的版本，
- undolog是最小的粒度，
- undolog所在的页是undo page
- 若干个undo page组成undo slot,一个事物最多两个undo slot,
- insert undo slot：主要记录主键，方便在回滚是快速找到这一行
- update undo slot：另一个是update undo slot存储delete/update log记录了修改之前的每一列的数据，
- 1024个undo slot构成一个undo segment,若干个undo segment构成undo tablespace

## 历史链表

insert undolog可以在事物 提交/回滚 后立即删除

update undolog其他请求可能需要使用update undo构建之前的版本，事物提交的时候会把undolog加入全局链表(history list),链表按照事物提交顺序排序，purge线程从最老的事物清理，如果链表太长说明很多记录没有被清除这会要看一下是否有长事物没有提交

## 回滚点 savepoint

事物回滚的时候可以指定回滚点，这样可以保证回滚到指定的点，而不是回滚整个事物

# 核心数据结构

## trx_t(建议看一下链表结构)

整个结构体每个连接持有一个，在创建链接后执行第一个事物开始，整个结构体就被初始化了，后续这个链接的所有事物一直复用里面的数据结构，直到这个链接断开

- 事物启动后，会把这个结构体加入到全局事物链表中`trx_sys -> mysql_trx_list`
- 如果是读写事物链表，会加入到全局读写事物链表中`trx_sys-> rw_trx_list` 
- 事物提交后会加入到全局事物提交链表中,`trx_sys->serial_list`

`state`记录事物的四种状态
- TRX_STATE_NOT_STARTED
- TRX_STATE_ACTIVATE
- TRX_STATE_PREPARED
- TRX_STATE_COMMIT_IN_MEMORY

`id`\
事物刚创建时分配，只读事物永远为0，只读事物通过指针地址区分。读写事物通过全局id产生器分配，是为了区分不同的事物，

`no` \
事物提交前，通过全局id生成器生成，目的是为了确定事物的提交顺序，保证加入到history list中的update undo有序，方便purge线程有序

`read_view`\
表示当前事物的可见范围，如果是只读事物`read_only`会被标记为true

## trx_sys_t 

维护系统的事物信息，全局只有一个，数据库启动的时候初始化

`max_trx_id`\
表示当前系统改为分配的最小事物id，如果有新的事物这个值会作为新事物的id,然后这个字段递增

`descriptors`\
这是一个数组，里面存放了所有活跃的读写事物id,当需要开启一个readview时，就从这个字段里面拷贝一份，用来判断记录对事物的可见性

`rw_trx_id`\
记录当前系统的所有读写事物，包括活跃和已经提交的，按照事物id排序，此外崩溃恢复后产生的事物和系统的事物也放在上面

`mysql_trx_list`\
存放所有用户创建，系统事物和崩溃后的事物不会出现，可能会出现还没有开始的事物

`trx_seara_list`\
按照no排序已经提交的事物

`rseg_array`\
指向系统所有可以用的回滚段`undo segments`，当某个事物需要回滚时，就从这里分配

`rseg_history_len`\
所有update undo的长度，指的是`history_list`的长度

`view_list`\
系统当前所有的`readview`,所有开启`readview`的事物，都会把自己的readview放在这上面，按照事物no排序

## trx_purge_t
purge线程使用的结构体，系统启动时初始化，全局只有一个，

`view`\
readview,purge线程不会删除所有大于`view->low_limit_no`的undolog

`limit`\
所有小于这个值得undolog,都可以被truncate掉，因为标记的日志已经被删除，且不需要他们构建之前的版本

`rseg`,`page_no`,`offset`,`hdr_page_no`,`hdr_offset`主要用来保存最后一个改为被purge的undolog

## read_view_t

辅助innodb判断某条记录是否对当前记录可见，每个事物的trx_t里面都有一个readview,在事物需要一致性读的时候初始化，读结束的时候释放。

`low_limit_no`\
主要是给purge线程使用，readview创建的时候会把当前最小的事物id赋值给low_limit_no,这样purge线程就可以把所有已提交事物的undo log删除

`low_limit_id`\
所有>=此值的记录都不应该被此readview看到,

`up_limit_id`\
所有小于此值的记录都应该被此readview看到

`descripts`\
里面存了创建的时候全局读写事物的id,除了事物自己做的变更外，应该看不到 descripts 中事物所做的变更

## trx_id_t
每个事物都会通过全局id产生器，产生一个id，只读事物为0只有当其切换为读写事物时再分配事务id

为了保证任何情况下，事物id都不重复，innodb的全局id产生器每分配256个事物id，就会把当前的max_trx_id同步到ibdata的系统页上，数据库每次重启，都从系统页上读取，然后加上256

## trx_segment_t

undo segment内存中的结构体，每个undo segment都对应一个

`update_undo_list` \
表示已经分配出去的update undo链表

`insert_undo_list` \
表示已经分配出去的inser undo链表

`update_undo_cached` 和 `insert_undo_cache`\
表示缓存起来的undo链表


# 事物的启动
读写事物和只读事物，读写事物比只读事物都几步操作，

- 分配回滚段，因为修改数据，需要找地方把老数据存储起来
- 需要通过全局事物id产生器产生一个事物id
- 把读写事物id加入到全局读写事物链表
- 加入全局活跃读写事物数组

`start transaction` `begin work` `begin`默认都是以只读事物启动

`start transaction read only` 启动事物，这种语句就把`thd->tx_read_only`设置为true,后续如果做了修改语句就会报错

# 事物的提交
入口函数 `innobase_commit`，每条语句结束的时候都会调用这个函数，`commit_trx`控制是否真的提交，如果为true,或者配置了autocommit=1,则进入提交核心逻辑，否则释放auto_inc造成的表锁，并且记录到undo_no
(innodb_rollback_on_timeout),提交的核心逻辑：

1. 依据innobase_commit_concurrence来判断是否有过多的线程同时提交
2. 设置事物状态为committing,可以在`show processlist` 看到 `trx_commit_for_mysql`
3. 使用全局事物id产生器生成事物no，然后把事物trx_t加入到`trx_serial_list`中，如果当前undo  segment没有设置为最后一个未purge的undo,则用此事物no更新`trx_searialisation_number_get`
4. 标记undo
5. 
如果这个事物