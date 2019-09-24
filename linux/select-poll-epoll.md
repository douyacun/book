
# 问题：
1w个客户端需要连上一个服务器并保持TCP链接

# I/O多路复用技术
内核一旦发现进程指定的一个或多个I/O条件准备读取。就通知该进程，这样多个描述符的I/O操作就能在一个线程内并发交替的完成，复用指的是复用同一个线程，减少创建进程/线程的开销

# select
```
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
当用户process调用select时，会将需要监控的readfds拷贝到内核空间，然后遍历整个监控的socket，挨个调用每个socket的poll检查是否有可读事件发生，

1. 单个进程所打开的fds限制为1024，由`FD_SETSIZE`限制
2. fds集合需要从用户空间拷贝到内核空间
3. 每次调用都需要遍历整个被监控的socket来确定是否可读写

# poll
```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd{
    int fd;
    short events;
    short revents;
}
```
poll 只是改变fds描述方式，解决了fds大小限制问题
- 没有改变用户空间和内核空间复制大量文件描述符的问题
- 个别socket就绪就触发整个集合遍历的问题


# epoll
解决问题的思想：
- 添加一个中间层
- 变集中处理为分布式处理

```c
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```
- epoll_ctl 通过(EPOLL_CTL_ADD、EPOLL_CTL_MOD、EPOLL_CTL_DEL)来分散对需要监控的fds集合的修改，有变化才变更
- mmap(内存映射)将用户空间的一块内存和内核空间的一块内存映射到同一块物理内存
- 使用红黑树存储监控的fds集合
- wait_entry
