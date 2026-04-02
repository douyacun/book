---
Title: fsync-fdatasync-同步内核的页缓存到磁盘
LegacyId: 6db15910cac7885f6a97958db5ac631c
Slug: fsync-fdatasync-同步内核的页缓存到磁盘
Category: devops
Summary: '```c #include <unistd.h> int fsync(int fd); int fdatasync(int fd); ``` -
  fsync： 文件描述符(fd)在内核中所有的[脏页](/linux/页缓存.md#dirty'
SeoTitle: fsync-fdatasync-同步内核的页缓存到磁盘
SeoDescription: '```c #include <unistd.h> int fsync(int fd); int fdatasync(int fd);
  ``` - fsync： 文件描述符(fd)在内核中所有的[脏页](/linux/页缓存.md#dirty'
Date: 2019-09-24T20:04:29+08:00
LastEditTime: 2019-09-24T20:04:29+08:00
Publish: true
---

# fsync,  fdatasync 同步内核的页缓存到磁盘
```c
#include <unistd.h>

int fsync(int fd);
int fdatasync(int fd);
```

- fsync： 文件描述符(fd)在内核中所有的[脏页](/devops/页面缓存#dirty_page)同步到的磁盘，调用会被阻塞直到磁盘写入完成，能确保被修改过的块立即写入到磁盘
- fdatasync：类似于fysnc，文件属性(st_atime,st_mtime等)不会同步到磁盘，目的是为了减少磁盘交互

# 与fdatasync相比fsync的性能问题#性能
fysnc 除了同步文件的修改内容(脏页)，还会同步文件的描述信息(metadata、包括size、访问时间st_atime、st_mtime等)，因为文件数据和metadata通常存在磁盘不同的地方，所以fsync至少需要两次I/O操作

多余的一次IO操作，有多么昂贵呢？根据Wikipedia的数据，当前硬盘驱动的平均寻道时间（Average seek time）大约是3~15ms，7200RPM硬盘的平均旋转延迟（Average rotational latency）大约为4ms，因此一次IO操作的耗时大约为10ms左右。

文件的尺寸(st_size)如果发生变化，是需要立即同步的否则OS一旦崩溃，即使文件的数据部分已同步，由于metadata没有同步，依然读不到修改的内容，最后的饿访问时间和修改时间是不需要每次同步的
