# 字典的实现

[](../assert/dict.png)

# hash 算法
计算hash值和索引值,使用 MurmurHash 算法
```c
// 设置字典的hash函数，计算key的hash值
hash = dict->type->hashFunction(key);

// 根据hash表的sizemask属性和哈希值，计算处索引
index = hash & dict->ht[x].sizemarsk;
```

# 解决键冲突
redis使用哈希使用链地址法，来解决键冲突，每个哈希表节点都有一个next指针，多个哈希表可以用next指针构成一个单向链表

# rehash
1. 为字典的ht[1]哈希表分配空间，
    - 如果是扩展操作，那么ht[1]的大小为第一个大于等于ht[0].size的2^n
    - 如果是收缩操作，那么ht[1]的大小为第一个大于等于ht[0].used的2^n
2. 将所有键值对rehash倒ht[1]上面，rehash指的是重新计算hash值和索引值
3. 当ht[0]，并将ht[1]设置为ht[0],然后ht[1]分配一个空白哈希表

# hash表的扩展与收缩
1. 服务器目前没有正在执行BGSAVE和BGREWRITEAOF，并且哈希表负载因子大于1
2. 服务器目前正在执行BGSAVE命令BGREWRITEAOF，并且哈希表负载因子大于等于5

# 渐进式rehash
扩展或收缩哈希表需要将ht[0]里面的所有键值对rehash到ht[1]里面，并不是一次性、集中式地完成的，而是分多次、渐进式地完成的。如果是四百万、四千万甚至四亿个键值对。
1. 为ht[1]分配空间，字典同时有ht[0]、ht[1]
2. 在字典中维持一个索引计数器rehashidx,并将他地值设置为0,表示rehash工作正式开始
3. 在rehash进行期间，每次curd，程序除了执行指定的操作以外，会顺带将ht[0]哈希表rehas到ht[1],完成后dict.rehashidx=-1
4. 随着字典的操作，最终每个时间点上，ht[0]的所有键值对都会被rehash值ht[1]

```c
#define dictIsRehashing(ht) ((ht)->rehashidx != -1);
// dictAddRaw、dictFind、dictDelete、dictReplace 都会进行的动作, 每次rehash一个桶
if (dictIsRehashing(d)) _dictReshStep(d,1);
```

