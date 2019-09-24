# void *zmalloc(size_t size) 分配内存

1. 多分配一个sizeof(sizt_t)的大小用于存储
```c
void *ptr = malloc(size + sizeof(size_t))
```
2. 第一个字长（前8个字节）存储分配的字节大小
```c
*((size_t*)ptr) = size
```
3. 更新used_memory，和内存字长对齐
```c
// 相当于_n+=_n%8,位操作要比取余快
if (_n & (sizeof(long)-1) ) _n+=(_n&(sizeof(long)-1))
```
4. 返回指针
```c
// 先后偏移8个字节
return (char *)ptr + sizeof(site_t)
```

# void *zcalloc(size_t size) 分配空间，并初始化为0
1. 调用calloc在内存中动态分配 1 个 size+sizeof(size_t)大小的内存
```c
void *ptr = calloc(1, size+sizeof(size_t))
```
2. 前8个字节记录分配内存大小
3. 更新used_memory


`void *calloc(sizt_t num, size_t size)` \
calloc() 在内存中动态地分配 num 个长度为 size 的连续空间，并将每一个字节都初始化为 0。所以它的结果是分配了 num*size 个字节长度的内存空间，并且每个字节的值都是0。

# void *zralloc(void *ptr, size_t size) 重新分配内存
1. 获取起始指针
```c
void *realptr = (char *)ptr - sizeof(size_t);
```
2. 原分配内存的大小
```c
sizt_t oldsize = *((size_t *)realptr);
```
3. 重新分配内存大小 
```c
void * newptr = realloc(realptr, size+sizeof(size_t))
```
4. 释放掉ptr oldSize,然后加上新分配的内存
```c
used_memory - oldsize + size + sizeof(size_t)
```
`void *realloc(void *ptr, size_t size)`\
重新分配 ptr（malloc、calloc、realloc分配的堆指针）size大小的空间


# zmalloc_size 
每次内存分配的size都会字长(sizeof(long))对齐，然后在多分配一个字长(sizeof(size_t))记录ptr指向的内存大小
```

```