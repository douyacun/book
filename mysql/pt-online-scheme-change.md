---
Title: pt-online-scheme-change使用
Keywords: 大表DDL,大表结构变更,pt工具,pt-online-scheme-change
Description: 经常遇见 热表、大表 表结构变更的场景，每次都会很头疼，和DBA聊过以后真正复杂的场景，获取不到MDL锁的时候，或者表有几千万上亿的数据量需要时间长。就用到pt了
Label: pt工具
Cover: assert/pt-online-scheme-change.png
Date: 2020-01-15 23:21:00
LastEditTime: 2020-01-15 23:23:00
---

# 安装

下载地址: [https://www.percona.com/downloads/percona-toolkit/LATEST/](https://www.percona.com/downloads/percona-toolkit/LATEST/)

官方文档: [https://www.percona.com/doc/percona-toolkit/LATEST/pt-online-schema-change.html](https://www.percona.com/doc/percona-toolkit/LATEST/pt-online-schema-change.html)

下载依赖包：

```
yum -y install perl-Digest-MD5 perl-DBI perl-DBD-MySQL perl-Time-HiRes perl-IO-Socket-SSL
```

# 执行

增加一列：

```
/bin/pt-online-schema-change --print --execute --alter "ADD COLUMN test varchar(255) NOT NULL DEFAULT '' COMMENT '测试pt' AFTER status" D=videos_t,t=media_copy1,u=root,p=123456
```

看下执行过程:

```shell
[root@douyacun percona-toolkit-3.1.0]# ./bin/pt-online-schema-change --print --execute --alter "ADD COLUMN test varchar(255) NOT NULL DEFAULT '' COMMENT '测试pt' AFTER status" D=videos_t,t=media_copy1,u=root,p=123456
No slaves found.  See --recursion-method if host home has slaves.
Not checking slave lag because no slaves were found and --check-slave-lag was not specified.
Operation, tries, wait:
  analyze_table, 10, 1
  copy_rows, 10, 0.25
  create_triggers, 10, 1
  drop_triggers, 10, 1
  swap_tables, 10, 1
  update_foreign_keys, 10, 1
Altering `videos_t`.`media_copy1`...
Creating new table...
CREATE TABLE `videos_t`.`_media_copy1_new` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `subtype` varchar(15) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '分类分类',
  `title` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '标题',
  `subject` bigint(19) unsigned DEFAULT NULL COMMENT '豆瓣subject，唯一索引',
  `source` tinyint(1) unsigned NOT NULL DEFAULT '1' COMMENT '1: 豆瓣 2: 百度百科 …',
  `torrent_num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '种子数量',
  `tags` varchar(255) NOT NULL COMMENT '标签',
  `original_title` varchar(500) NOT NULL DEFAULT '' COMMENT '原标题',
  `directors` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '导演',
  `casts` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT ' 演员阵容',
  `genres` varchar(60) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '类型',
  `released` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '上映时间',
  `released_timestamp` timestamp NULL DEFAULT NULL,
  `alias` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '别名',
  `summary` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '摘要',
  `rate` float(3,2) unsigned NOT NULL DEFAULT '0.00' COMMENT '豆瓣得分',
  `duration` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '片长',
  `region` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '地区',
  `language` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '语言',
  `official_website` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '官网',
  `poster` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '海报',
  `cover` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '视频封面',
  `new_cover` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '解析下载以后的封面',
  `current_season` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '更新集数',
  `episodes_update` int(11) unsigned NOT NULL DEFAULT '0',
  `episodes_count` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '总集数',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  `status` tinyint(3) NOT NULL DEFAULT '0' COMMENT '状态',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `med` (`subject`,`source`) USING BTREE,
  KEY `idx_title` (`title`) USING BTREE,
  KEY `idx_torrent_num` (`torrent_num`),
  KEY `idx_rate` (`rate`) USING BTREE,
  KEY `idx_created_at` (`created_at`) USING BTREE,
  KEY `idx_tags` (`tags`),
  KEY `idx_subtype_genres` (`subtype`,`genres`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=194664 DEFAULT CHARSET=utf8 COMMENT='视频库'
Created new table videos_t._media_copy1_new OK.
Altering new table...
ALTER TABLE `videos_t`.`_media_copy1_new` ADD COLUMN test varchar(255) NOT NULL DEFAULT '' COMMENT '测试pt' AFTER status
Altered `videos_t`.`_media_copy1_new` OK.
2020-01-15T21:59:11 Creating triggers...
2020-01-15T21:59:11 Created triggers OK.
2020-01-15T21:59:11 Copying approximately 167063 rows...
INSERT LOW_PRIORITY IGNORE INTO `videos_t`.`_media_copy1_new` (`id`, `subtype`, `title`, `subject`, `source`, `torrent_num`, `tags`, `original_title`, `directors`, `casts`, `genres`, `released`, `released_timestamp`, `alias`, `summary`, `rate`, `duration`, `region`, `language`, `official_website`, `poster`, `cover`, `new_cover`, `current_season`, `episodes_update`, `episodes_count`, `created_at`, `updated_at`, `status`) SELECT `id`, `subtype`, `title`, `subject`, `source`, `torrent_num`, `tags`, `original_title`, `directors`, `casts`, `genres`, `released`, `released_timestamp`, `alias`, `summary`, `rate`, `duration`, `region`, `language`, `official_website`, `poster`, `cover`, `new_cover`, `current_season`, `episodes_update`, `episodes_count`, `created_at`, `updated_at`, `status` FROM `videos_t`.`media_copy1` FORCE INDEX(`PRIMARY`) WHERE ((`id` >= ?)) AND ((`id` <= ?)) LOCK IN SHARE MODE /*pt-online-schema-change 24862 copy nibble*/
SELECT /*!40001 SQL_NO_CACHE */ `id` FROM `videos_t`.`media_copy1` FORCE INDEX(`PRIMARY`) WHERE ((`id` >= ?)) ORDER BY `id` LIMIT ?, 2 /*next chunk boundary*/
Copying `videos_t`.`media_copy1`:  32% 01:03 remain
Copying `videos_t`.`media_copy1`:  61% 00:36 remain
Copying `videos_t`.`media_copy1`:  86% 00:13 remain
2020-01-15T22:00:55 Copied rows OK.
2020-01-15T22:00:55 Analyzing new table...
2020-01-15T22:00:55 Swapping tables...
RENAME TABLE `videos_t`.`media_copy1` TO `videos_t`.`_media_copy1_old`, `videos_t`.`_media_copy1_new` TO `videos_t`.`media_copy1`
2020-01-15T22:00:56 Swapped original and new tables OK.
2020-01-15T22:00:56 Dropping old table...
DROP TABLE IF EXISTS `videos_t`.`_media_copy1_old`
2020-01-15T22:00:57 Dropped old table `videos_t`.`_media_copy1_old` OK.
2020-01-15T22:00:57 Dropping triggers...
DROP TRIGGER IF EXISTS `videos_t`.`pt_osc_videos_t_media_copy1_del`
DROP TRIGGER IF EXISTS `videos_t`.`pt_osc_videos_t_media_copy1_upd`
DROP TRIGGER IF EXISTS `videos_t`.`pt_osc_videos_t_media_copy1_ins`
2020-01-15T22:00:57 Dropped triggers OK.
Successfully altered `videos_t`.`media_copy1`.
```

**详细执行流程如下：**

1. 相关环境参数检查
2. 检查该表格是否存在
3. show create table media_copy1
4. create table _media_copy1_new
5. alter table _media_copy1_new 
6. 创建删除触发器: `pt_osc_videos_t_media_copy1_del` , (如果数据修改的时候，还没有拷贝过来，修改后再拷贝则是覆盖，正确；如果是已经拷贝过来，再修改，也是正确，这里同时会检查是否具有主键或者唯一索引，如果都没有，这一步会报错，提示The new table `videos_t`.`_media_copy1_new` does not have a PRIMARY KEY or a unique index which is required for the DELETE trigger.)
7. 创建更新触发器：`pt_osc_videos_t_media_copy1_upd`
8. 创建插入触发器：`pt_osc_videos_t_media_copy1_ins`
9. 按块拷贝数据到新表，拷贝过程对数据行持有S锁
10. analyze 新表
11. rename 表名，`RENAME TABLE videos_t.media_copy1 TO videos_t._media_copy1_old, videos_t._media_copy1_new TO videos_t.media_copy1`
12. 删除旧表
13. 删除新表上的删除、更新、插入 触发器

**DSN 选项**

- A 字符集设置
- D 需要Alter的表格是在哪个数据库，指定数据库
- F mysql read default file，如果数据源的相关选项存储在文件中，则通过 F 来指定
- h host，数据库主机名或IP
- p password，数据库用户的密码
- P port，数据库实例端口号
- S socket，实例socket文件
- t 表格名
- u 用户名

**从库延迟情况**

- --max-lag：默认1s，检查从库延迟情况，如果超过，则停止copy data，休息--check-interval秒后，再重新开始copy数据
- --check-interval：从库延迟超过指定的--max-lag，中断copy data休息的时间
- --recursion-method：MASTER寻找SLAVE的方式
  - processlist: 使用show processlist查找从库
  - hosts: 不是使用默认端口号3306，那么使用hosts方式来查找从库会更可靠
  - dsn: 手动在需要DDL的数据库内，创建 dsns 表格, pt会从表各种读取从库信息
    - `CREATE TABLE dsns (id int(11) NOT NULL AUTO_INCREMENT,parent_id int(11) DEFAULT NULL,dsn varchar(255) NOT NULL, PRIMARY KEY (id))`
    - 存储从库信息
    - insert into dsns(dsn) values(h=slave_host,u=repl_user,p=repl_password,P=port );
    - 该参数使用的时候，按以下格式（假设 dsns表格建立在数据库 dbosc）
  - none：不查找从库
- --check-slave-lag

**执行选项**

- --execute : 确定执行 alter 操作，如果不指定的话，只会做安全检查操作，类似于`nginx -t`
- --alter : 不包含前面的 `alter table t`

**打印选项**

- --print： 详细打印alter过程，不指定的时候，简略打印
- --statistics： 增加影响行数打印，可以查看copy进度

**服务器负载**

- --max-load： 
  - 默认为Threads_running=25
  - 每个chunk拷贝完后，会检查SHOW GLOBAL STATUS的内容，检查指标是否超过了指定的阈值,超过了就暂停

- --critical-load:
  - 默认为Threads_running=50
  - 类似`--max-load`,如果超过指定值，则工具直接退出，而不是暂停。

# 安全

- 大部分情况下，pt会拒绝给没有主键和唯一键的表做变更
- 如果发现影响主从超时厉害，max-lag参数要配置一下
- 如果发现服务器负载非常高，也会暂停或停止相关操作。`-—max-load`, `--critical-load`要配置一下
- 默认会设置 innodb_lock_wait_timeout=1 和 lock_wait_timeout=60来减少竞争
- 如果有外键约束，那么禁止改表，指定 —alter-foreign-keys-method.

# 使用限制

1. 禁止创建唯一索引，会丢失数据，更加不允许添加 --alter-check=no，--check-unique-key-change=no  
2. 如果原表没有主键，或者也没有唯一索引，这些表是不允许用pt做DDL的  
3. 禁止对外键的表进行pt ddl
4. 禁止对表进行重命名
5. 禁止对列进行重命名，如果一定要做，也必须先print出来检测清楚列名是否正确  
6. 新增字段，NOT NULL必须要指定默认值
7. 不允许删除主键
