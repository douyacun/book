---
Title: 了解一下高大上的epoll
Keywords: epoll
Description: go实现websocket时，使用epoll优化来节省goroutine,这里特地来了解一下
Cover:
Label: epoll
Date: 2020-03-18 21:37:00
LastEditTime: 2020-03-18 21:37:00
---

epoll时linux内核的可扩展I/O事件通知机制，于Linux 2.5.44首度登场， 让需要大量操作文件描述的程序得以发挥更优异的性能，poll和select的时间复杂度是O(n), 而epoll的复杂度时O(log n)。

# 接口

```c#
int epoll_create(int size);
```

在内核中创建`epoll`实例并返回一个`epoll`文件描述符。 在最初的实现中，调用者通过 `size` 参数告知内核需要监听的文件描述符数量。如果监听的文件描述符数量超过 size, 则内核会自动扩容。而现在 size 已经没有这种语义了，但是调用者调用时 size 依然必须大于 0，以保证后向兼容性。

```c
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

向 epfd 对应的内核`epoll` 实例添加、修改或删除对 fd 上事件 event 的监听。

- op 可以为
  -  `EPOLL_CTL_ADD`  添加新的事件
  -  `EPOLL_CTL_MOD` 修改文件描述符上监听的事件类型
  - `EPOLL_CTL_DEL` 从实例上删除一个事件
- 如果 event 的 events 属性设置了 `EPOLLET` flag，那么监听该事件的方式是边缘触发。

```c
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

- events是结构体epoll_event数组， epoll会把就绪文件描述符赋值给events
- maxevents告诉内核这个events数组有多大

- 当 timeout 为 0 时，epoll_wait 永远会立即返回。而 timeout 为 -1 时，epoll_wait 会一直阻塞直到任一已注册的事件变为就绪。当 timeout 为一正整数时，epoll 会阻塞直到计时 timeout 毫秒终了或已注册的事件变为就绪。因为内核调度延迟，阻塞的时间可能会略微超过 timeout 毫秒。

# 原理

epoll只告知就绪的文件描述符，epoll_wait返回的int数值是已经就绪的文件描述符数量，从events数组中文件描述符取相应的文件描述符即可

**内存映射(mmap)**，省掉了这些文件描述符在系统调用时复制的开销

**就绪通知** eselect/poll，进程只有调用一定方法后，内核才对所有监视的文件描述符遍历，而epoll是通过epoll_ctl注册一个文件描述符，一旦某个文件描述符就绪时，迅速激活这个文件描述符，进程调用epoll_wait会得到通知

# 触发模式

epoll有2种触发模式，水平触发 / 边缘触发，在边缘触发模式中，epoll_wait仅会在新的事件首次被加入epoll队列时返回。在水平触发模式中，在事件状态未变更前将不断触发(epoll_wait时没次都会返回)。

考虑读的情况。假设我们注册了一个读事件到`epoll`实例上，`epoll`实例会通过`epoll_wait`返回值的形式通知我们哪些读事件已经就绪。简单地来说，在水平触发模式下，如果读事件未被处理，该事件对应的内核读缓冲器非空，则每次调用 `epoll_wait` 时返回的事件列表都会包含该事件。直到该事件对应的内核读缓冲器为空为止。而在边缘触发模式下，读事件就绪后只会通知一次，不会反复通知。

考虑写的情况。水平触发模式下，只要文件描述符对应的内核写缓冲器未满，就会一直通知可写事件。而在边沿触发模式下，内核写缓冲器由满变为未满后，只会通知一次可写事件。

举例来说，倘若有一个已经于`epoll`注册接手数据，`epoll_wait`将返回，并发出数据读取的信号。现假设缓冲器的数据仅有部分被读取并处理，在水平触发模式下，任何对`epoll_wait`之调用都将即刻返回，直到缓冲器中的数据全部被读取；然而，在边缘触发的情境下，`epoll_wait`仅会于再次接收到新数据(亦即，新数据被写入管线)时返回。

边缘触发模式，程序可能在用户态缓存I/O状态，nginx就是边缘触发模式，有2种情况时推荐使用边缘触发模式：

- read或者write系统调用返回 EAGAIN
- 非阻塞的文件描述符

边缘触发模式，可能的缺陷：

- 假设某个文件描述符上有大量不间断的输入流，而边缘触发模式只会通知一次，一次是读不完的

# epoll是如何实现高效处理百万句柄的

执行epoll_create时，创建了红黑树和就绪链表，执行epoll_ctl时，如果增加socket句柄，则检查在红黑树中是否存在，存在立即返回，不存在则添加到树干上，然后向内核注册回调函数，用于当中断事件来临时向准备就绪链表中插入数据。执行epoll_wait时立刻返回准备就绪链表里的数据即可。

select/poll 每次调用需要将监听句柄作为参数传递，这样在进程和内核之间数据拷贝很浪费时间

epoll还维护了一个双链表，当调用epoll_wait时，仅需要观察这个链表中有没有就绪的句柄即可，每次需要拷贝的句柄很少，非常高效。

那么，这个准备就绪list链表是怎么维护的呢？当我们执行epoll_ctl时，除了把socket放到epoll文件系统里file对象对应的红黑树上之外，还会给内核中断处理程序注册一个回调函数，告诉内核，如果这个句柄的中断到了，就把它放到准备就绪list链表里。所以，当一个socket上有数据到了，内核在把网卡上的数据copy到内核中后就来把socket插入到准备就绪链表里了。

```c
struct eventpoll {
	/*
	 * This mutex is used to ensure that files are not removed
	 * while epoll is using them. This is held during the event
	 * collection loop, the file cleanup path, the epoll file exit
	 * code and the ctl operations.
	 */
	struct mutex mtx;

	/* Wait queue used by sys_epoll_wait() */
	wait_queue_head_t wq;

	/* Wait queue used by file->poll() */
	wait_queue_head_t poll_wait;

	/* List of ready file descriptors */
	struct list_head rdllist;

	/* Lock which protects rdllist and ovflist */
	rwlock_t lock;

	/* RB tree root used to store monitored fd structs */
	struct rb_root_cached rbr;

	/*
	 * This is a single linked list that chains all the "struct epitem" that
	 * happened while transferring ready events to userspace w/out
	 * holding ->lock.
	 */
	struct epitem *ovflist;

	/* wakeup_source used when ep_scan_ready_list is running */
	struct wakeup_source *ws;

	/* The user that created the eventpoll descriptor */
	struct user_struct *user;

	struct file *file;

	/* used to optimize loop detection check */
	int visited;
	struct list_head visited_list_link;

#ifdef CONFIG_NET_RX_BUSY_POLL
	/* used to track busy poll napi_id */
	unsigned int napi_id;
#endif
};
```

