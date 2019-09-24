# rdb 持久化

redis中的数据都是存储在内存中，一旦进程推出就会导致redis中的数据丢失，redis提供了rdb持久化功能，可以将redis在内存中的数据保存到磁盘中

# rdb文件的创建 `rdb.c/rdbsave`

- save：命令会阻塞redis服务器进程,直到rdb文件写入完毕。服务器不能处理任何命令

- bgsave：会派生出一个子进程负责创建rdb文件，父进程继续处理命令请求；

> [redis文件写入](./rio.md)


# rdb文件的载入 `rdb.c/rdbload`
rdb文件的载入是在redis-server在启动时检测rdb文件存在，会自动载入rdb文件

因为AOF文件的更新频率比RDB文件的频率高，如果服务器开启了AOF持久化功能，会有限启用AOF文件，只有在AOF关闭时，才会使用rdb文件

# 自动间隔性保存
```conf
save 900 1
save 300 10
save 60  10000
```
- 服务器在900秒内至少修改了一次，bgsave就会被执行
- 服务器在300秒内至少修改了10次，bgsave就会被执行
- 服务器在60秒至少修改了10000次，bgsave就会被执行

```c
struct saveparam {
    time_t seconds;// 多少秒以内
    int changes;发生多少次修改
}

struct redisServer {
    ...
    struct saveparam *saveparams;// 记录保存条件的数组
}
```

# dirty/lastsave 计数器

dirty计数器记录上次save或bgsave之后服务器对数据库状态进行了多少次修改

lastsave是unix时间戳，记录上一次记录save/bgsave命令的时间

# 检查条件是否满足
serverCron每隔100毫秒就会执行一次,检查save选项设置的保存条件是否满足，满足的话就执行bgsave

# rdb文件结构

```c
REDIS db_version database EOF check_sum
```

save保存过程：`rdb.c/rdbSave`
- 创建临时文件
```c
snprintf(tempfile, 256, "temp-%d.rdb", int getpid());
```
- 初始化i/o `rdbInitWithFile`
- 设置校验函数
- 写入`REDIS`和版本号，载入文件时，会判断是否RDB文件
```c
snprintf(magic, sizeof(magic), 'REDIS%04d', REDIS_RDB_VERSION);
```
- 遍历数据库，如果数据非空，写入db选择器
```c
rdbSaveType(&rdb, REDIS_RDB_OPCODE_SELECT);
```
- 遍历数据库，写入,每个键值对的数据, 过期时间、键、值
```c
// 值
rdbSaveType(rdb, val);
// 键
rdbSaveStringType(rdb, key);
```

常量类型：
```c
#define REDIS_RDB_VERSION 6

/* The current RDB version. When the format changes in a way that is no longer
 * backward compatible this number gets incremented. */
#define RDB_VERSION 7

/* Defines related to the dump file format. To store 32 bits lengths for short
 * keys requires a lot of space, so we check the most significant 2 bits of
 * the first byte to interpreter the length:
 *
 * 00|000000 => if the two MSB are 00 the len is the 6 bits of this byte
 * 01|000000 00000000 =>  01, the len is 14 byes, 6 bits + 8 bits of next byte
 * 10|000000 [32 bit integer] => if it's 01, a full 32 bit len will follow
 * 11|000000 this means: specially encoded object will follow. The six bits
 *           number specify the kind of object that follows.
 *           See the RDB_ENC_* defines.
 *
 * Lengths up to 63 are stored using a single byte, most DB keys, and may
 * values, will fit inside. */
#define RDB_6BITLEN 0
#define RDB_14BITLEN 1
#define RDB_32BITLEN 2
#define RDB_ENCVAL 3
#define RDB_LENERR UINT_MAX

/* When a length of a string object stored on disk has the first two bits
 * set, the remaining two bits specify a special encoding for the object
 * accordingly to the following defines: */
#define RDB_ENC_INT8 0        /* 8 bit signed integer */
#define RDB_ENC_INT16 1       /* 16 bit signed integer */
#define RDB_ENC_INT32 2       /* 32 bit signed integer */
#define RDB_ENC_LZF 3         /* string compressed with FASTLZ */

/* Dup object types to RDB object types. Only reason is readability (are we
 * dealing with RDB types or with in-memory object types?). */
#define RDB_TYPE_STRING 0
#define RDB_TYPE_LIST   1
#define RDB_TYPE_SET    2
#define RDB_TYPE_ZSET   3
#define RDB_TYPE_HASH   4
/* NOTE: WHEN ADDING NEW RDB TYPE, UPDATE rdbIsObjectType() BELOW */

/* Object types for encoded objects. */
#define RDB_TYPE_HASH_ZIPMAP    9
#define RDB_TYPE_LIST_ZIPLIST  10
#define RDB_TYPE_SET_INTSET    11
#define RDB_TYPE_ZSET_ZIPLIST  12
#define RDB_TYPE_HASH_ZIPLIST  13
#define RDB_TYPE_LIST_QUICKLIST 14
/* NOTE: WHEN ADDING NEW RDB TYPE, UPDATE rdbIsObjectType() BELOW */

/* Test if a type is an object type. */
#define rdbIsObjectType(t) ((t >= 0 && t <= 4) || (t >= 9 && t <= 14))

/* Special RDB opcodes (saved/loaded with rdbSaveType/rdbLoadType). */
#define RDB_OPCODE_AUX        250
#define RDB_OPCODE_RESIZEDB   251
#define RDB_OPCODE_EXPIRETIME_MS 252
#define RDB_OPCODE_EXPIRETIME 253
#define RDB_OPCODE_SELECTDB   254
#define RDB_OPCODE_EOF        255
```

# 分析rdb文件
