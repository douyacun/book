---
Title: goroutine和线程的区别
Keywords: goroutine,操作系统线程,线程,channel,
Description: 操作系统只认线程调用，goroutine和线程有哪些区别
Label: goroutine
Author: douyacun
Cover: assert/groutine-cover.jpeg
Date: 2019-09-10 23:14:14
LastEditTime: 2019-11-15 18:08:11
---

疑问：

1. 系统线程和goroutine有哪些区别？
2. 什么原子操作，系统线程和goroutine是如何进行原子操作？

# 目录

[TOC]

# thread

先说一下线程出现的背景，也就是有了进程为什么还需要多线程的原因，首先在许多应用同时发生着多种活动（并行），某种活动会随着时间推移会被阻塞，将这些应用程序分解成可以准并行运行的多个顺序线程，程序设计模型会变得更简单。其次由于线程比进程更轻量级，它们比进程更容易创建和销毁。第三个就是性能的比较，如果是CPU密集型，多线程并不能获得性能上的提升，但如果是I/O密集型或者计算密集型的，多个线程会允许彼此重叠运行，加快应用程序执行的速度

POSIX（pthread）线程库，C/C++标准线程API,线程比fork新的进程开销更少，是因为系统不会为进程初始化新的虚拟内存空间和环境，进程中所有线程共享相同的地址空间，通过定义一个函数及其将在线程中处理的参数来生成一个线程。在软件中使用POSIX线程库的目的是更快地执行软件。

基础知识:

- 线程操作包括线程创建，终止，同步（连接、阻塞），调度，数据管理和进程交互
- 线程不维护以创建线程的列表，也不知道它创建的线程
- 进程中所有的线程共享相同的地址空间
- 同一进程中的线程共享:
  - 流程
  - 大多数数据
  - 打开的文件描述符
  - 信号和信号处理程序
  - 当前的工作目录
  - 用户和组id
- 每个线程独享：
  - 线程id
  - 寄存器集，堆栈指针
  - 堆栈用于局部变量，返回地址
  - 信号掩码
  - 优先级
  - 返回值：errno（错误码）
- Pthread 函数返回0表示成功

## C - Basic Thread Routines

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void *print_message_func(void *ptr);
int main()
{
    pthread_t thread1, thread2;//线程id
    int iret1, iret2;
    char *message1 = "Thread 1";
    char *message2 = "Thread 2";

    iret1 = pthread_create(&thread1, NULL, print_message_func, (void *) message1);
    iret2 = pthread_create(&thread2, NULL, print_message_func, (void *) message2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    printf("Thread 1 return %d\n", iret1);
    printf("Thread 2 return %d\n", iret2);
		exit(0);
}
void *print_message_func(void *ptr)
{
    char *message;
    message = (char *)ptr;
    printf("%s\n", message);
}
// 输出
// Thread 2
// Thread 1
// Thread 1 return 0
// Thread 2 return 0

// gdb 调试打印一下thread1、thread2
// (gdb) p thread2
// $5 = 140737337296640
// (gdb) p thread1
// $6 = 140737345689344
// (gdb)
```

### creation

```c
int pthread_create(pthread_t * thread, 
                   const pthread_attr_t * attr,
                   void * (*start_routine)(void *), 
                   void *arg);
```

- pthread_t：在 `/usr/include/bits/pthreadtypes.h` 中定义`typedef unsigned long int pthread_t;`
- pthread_attr_t：

| **属性**       | **值**                                                       | **结果**                                                     |
| -------------- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| __scope        | `PTHREAD_SCOPE_SYSTEM`: 与系统中所有线程一起竞争CPU时间<br />`PTHREAD_SCOPE_PROCESS:` 仅与同进程中的线程竞争CPU | 竞争CPU的范围,缺省值`PTHREAD_SCOPE_SYSTEM`                   |
| __detachstate  | `PTHREAD_CREATE_DETACHED`: 新线程与其他线程脱离同步，不能使用pthread_join来同步<br />`PTHREAD_CREATE_JOINABLE`: 新线程与其他线程同步，新线程创建后可以使用pthread_detached来脱离同步 | 新线程是否与进程中其他线程脱离同步，缺省值为`PTHREAD_CREATE_JOINABLE` |
| __stackaddr    | `NULL`                                                       | 新线程具有系统分配的栈地址。                                 |
| __stacksize    |                                                              | 新线程具有系统定义的栈大小。默认2M                           |
| __schedpolicy  | `SCHED_OTHER`：正常、非实时 <br />`SCHED_RR`: 实时、轮转法<br />`SCHED_FIFO`： 实时、先入先出 | 线程的调度策略，缺省值为`SCHED_OTHER`                        |
| __inheritsched | `PTHREAD_EXPLICIT_SCHED`: 显示式指定调度策略和调度参数<br />`PTHREAD_INHERIT_SCHED`: 继承调用者线程的值 | 新线程是否继承父线程调度优先级。                             |

- `void * (*start_routine)` 指向线程运行的函数，函数有一个参数执行`void *`
- `void *arg`执行函数参数的指针，如果传递多个参数，发送指向结构体的指针

### Termination

```c
void pthread_exit（void *retval）;
```

- retval 函数的返回值

一般情况下，进程中各个线程的运行都是相互独立的，线程的终止并不会通知，也不会影响其他线程，终止线程所占用的资源也并不会随着线程的终止而释放，正如进程之间可以用wait()系统调用来同步终止并释放资源一样，线程就是pthread_join等待并释放

注意pthread_exit只会让主线程自己退出不会让产生的子线程退出；用return则所有的线程退出

使线程退出的方法有：

- 调用`pthread_exit`
- 使调用函数return
- 调用exit会使进程退出，包括已经创建的线程
- 主线程退出

如何让子线程完整执行？

1. 使用pthread_join等待其执行完成
2. 执行时，主线程执行pthread_exit，这样子线程能继续执行
3. 使用pthread_detach使新线程与其他线程脱离同步，要保证主线成不能退出

### detach and join

```c
int pthread_detach(pthread_t thread);
```

- `pthread_t thread` 线程id

线程创建默认是joinable，但如果一个线程结束但没有被join，则他的状态类似于僵尸进程(Zombie Process),还有一部分资源没有被回收，所以创建线程者应该调用pthread_join来等待线程退出，并可得到线程的退出代码，回收其资源，但是调用pthread_join后，线程没有结束，调用者会被阻塞，有些情况我们并不希望如此，比如在web服务器创建一个字线程来处理每一个请求，主线程并不希望因为调用pthread_join而阻塞，这时可以在子线程代码中加入pthread_detach(pthread_self())或是在父线程调用pthread_detach(pthread_t thread_id)非阻塞立即返回，这将使子线程的状态设置为detached,该线程结束后会自动释放所有资源

```c
int pthread_join(pthread_t thread, void **retval);
```

- `pthread_t thread`线程id
- `retval`返回值

调用pthread_join的线程会阻塞，直到指定的线程返回或调用了pthread_exit,如果线程简单的返回，retval会被设置成函数的返回值；如果调用了pthread_exit，那个可以传参给pthread_exit("hello world")当作线程的返回值；如果线程被取消，retval被设置成`PTHREAD_CANCELED`，注意一个线程只允许唯一的一个线程使用pthread_join()等待它的终止，并且被等待线程必须是joinable状态，非Detached状态

## Syncronizing Threads - 线程同步

问题：

```
int bar = 0;
void foo()
{
    if (bar == 0)
			bar = 1;	/* This must only be done once. */
}
这里有一个问题：
 	    Thread 1				    Thread 2
 		if (bar == 0)
 									if (bar == 0)
 						    		bar = 1;
 		bar = 1;
线程1、2会同时设置bar
```

线程库提供了三种线程同步机制：

- mutexes - 互斥锁, 阻止其他线程访问变量，
- join - 等待线程完成
- condition variables - pthread_cond_t

### mutexes - 互斥锁

- 互斥锁用于防止因竞争条件引起的数据不一致。
- 两个或多个线程需要同时在同一块存储区域上执行操作时，通常会发生竞争条件，计算结果取决于执行这些操作的顺序。互斥锁用于序列化共享资源。
- 每当多个线程访问全局资源时，应该有一个与之关联的互斥锁。互斥锁只能用在同一进程的线程，不像信号可以用于多个进程之间的通信。

```c
#include <pthread.h>
#include <stdio.h>

void *foo(void *ptr);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int bar = 0;

int main()
{
    pthread_t t1, t2;
    int rc1, rc2;
    if ((rc1 = pthread_create(&t1, NULL, foo, NULL))) {
        printf("Thread creation failed: %d\n", rc1);
    }
    if ((rc2 = pthread_create(&t2, NULL, foo, NULL))) {
        printf("Thread creation failed: %d\n", rc2);
    }
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Final counter bar is %d\n", bar);
}
void *foo(void *ptr)
{
    printf("Thread number %llu\n", pthread_self());
    pthread_mutex_lock(&mutex1);
    bar++;
    pthread_mutex_unlock(&mutex1);
}
// cc -g -lpthread main.c -o thread
// -g 允许gdb调试
```

### join

通过pthread_join可以等待多个线程执行完成并获取执行结果

```c
#include <pthread.h>
#include <stdio.h>

#define NUM_OF_THREAD 10
void *foo(void *ptr);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int bar = 0;

int main()
{
    pthread_t ts[NUM_OF_THREAD];
    int i,j;
    for (i = 0; i < NUM_OF_THREAD; i++) {
        pthread_create(&ts[i], NULL, foo, NULL);
    }
    for (j = 0; j < NUM_OF_THREAD; j++) {
        pthread_join(ts[j], NULL);
    }
    printf("Final counter bar is %d\n", bar);
}
void *foo(void *ptr)
{
    printf("Thread number %llu\n", pthread_self());
    pthread_mutex_lock(&mutex1);
    bar++;
    pthread_mutex_unlock(&mutex1);
}
```

### condition variables - 条件变量

互斥锁，平时接触的比较多，那条件变量是干啥的？为什么条件变量总是和互斥锁搭配来使用

条件变量，线程可以原子方式阻塞，直到满足某个条件为止，比如：线程A和B存在依赖关系，B要在某个条件发生之后才能继续运行，而这个条件只有A才能完成，这个时候就可以用条件变量来完成

create/destory:

```c
int	pthread_condattr_init(pthread_condattr_t *cattr);
pthread_cond_t cond = PTHREAD_COND_INITIALIZER
int	pthread_condattr_destroy(pthread_condattr_t *cattr);
```

- 动态：需要用pthread_condattr_destroy来释放条件变量的内存空间
- 静态：可以把常量`PTHREAD_COND_INITIALIZER`给静态分配的条件变量

waiting on condition：

```CQL
int pthread_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex);
int pthread_cond_timewait(pthread_cond_t *cond,pthread_mutex *mutex,const timespec *abstime);
```

这两个函数分别是阻塞等待和超时等待

waking thread based on condition:

```c
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```

这两个函数通知线程条件已经满足

以下代码就是典型的生产者消费者模型

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define NUM_OF_THREAD 5
#define QUEUE_CAPACITY 5

struct Queue{
	int ppos;
	int cpos;
	char *data;
	int capacity;
	int len;
}q;
pthread_cond_t cd = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void *start(void *args);
void producer();
void consumer();

int count = 0;

int main()
{
	pthread_t threads[NUM_OF_THREAD];
	int i, j;
	memset(&q, 0, sizeof(struct Queue));
	q.capacity = QUEUE_CAPACITY;
	q.data = (char *)malloc(sizeof(char) * QUEUE_CAPACITY);
	for(i=0; i<NUM_OF_THREAD;i++) {
		int *n = (int *)malloc(sizeof(int) * 1);
		*n = i;
		pthread_create(&threads[i], NULL, start, n);
	}
	for(j=0;j<NUM_OF_THREAD;j++){
		pthread_join(threads[j], NULL);
	}
	exit(0);
}
void *start(void *args)
{
	int n = *(int *)args;
	if (n==0) {
		producer();// 1个生产者
	} else {
		consumer();// 4个消费者
	}
}
void producer()
{
	char c;
	while(1)
	{
		if ((c = getchar()) == '\n'){
			continue;
		}
		pthread_mutex_lock(&mtx);
		if (q.len >= QUEUE_CAPACITY) {
			printf("Queue is full\n");
		} else {
			q.len++;
			q.ppos = (q.ppos + 1) % q.capacity;
			q.data[q.ppos] = c;
			printf("get a task\n");
		}
		pthread_cond_broadcast(&cd); // 通知消费线程 来活了
		pthread_mutex_unlock(&mtx);
	}
	return;
}

void consumer()
{
	int len, cpos;
	char c;
	while(1)
	{
		pthread_mutex_lock(&mtx);
		while(q.len <= 0) {
			printf("waitting ...\n");
			pthread_cond_wait(&cd, &mtx);
		}
		q.len--;
		len = q.len;
		q.cpos  =(q.cpos + 1) % q.capacity;
		cpos = q.cpos;
		c = q.data[q.cpos];
		pthread_mutex_unlock(&mtx); // 这里生产任务已经接到没必要在占用锁了，否则就没有并发了
		sleep(3);
		printf("completed this task from queue: 【%c】, len 【%d】, cpos 【%d】\n", c, len, cpos);
	}
	return;
}
```

## Thread Pitfalls - 线程陷阱

- 竞争条件： 线程由操作系统调度并执行。不能假定线程按创建顺序执行，他们可以以不同的速度执行。当线程正在执行时（竞赛完成），它们可能会产生意外结果（竞争条件）。必须利用mutexes和join来实现可预测的执行顺序和执行结果
- 线程安全（并发安全）：线程必须调用“线程安全”的函数，这意味着没有静态或全局变量，其他线程可能会假设单线程操作而破坏或读取。如果使用静态或全局变量，则必须使用全局变量或重写程序避免使用这些变量。在c语言中，局部变量在堆栈上分配的，因此任何不使用静态数据或其他共享资源的线程都是线程安全的。线程不安全函数程序中一次只能由一个线程使用，必须确保线程的唯一性；
- 互斥锁死锁：当应用锁互斥锁未解锁时会发生此情况，这导致程序执行无限停止。将两个或多个互斥锁用于代码片段时要小心，当线程第一个pthread_mutex_lock已经锁住，而第二个pthread_mutex_lock被其他线程锁住时，那么第一个互斥锁最终可能会锁定其他所有线程访问数据，包括第二个互斥锁的线程。线程无限期地等待资源变得空闲，从而变成死锁。

```c
void * function1（）
{
	...
	pthread_mutex_lock（＆lock1）;           - 执行步骤1
	pthread_mutex_lock（＆lock2）;           - 执行步骤3 DEADLOCK !!!
	...
	...
	pthread_mutex_lock（＆lock2）;
	pthread_mutex_lock（＆lock1）;
	...
}
void * function2（）
{
	...
	pthread_mutex_lock（＆lock2）;           - 执行步骤2
	pthread_mutex_lock（＆lock1）;
	...
	...
	pthread_mutex_lock（＆lock1）;
	pthread_mutex_lock（＆lock2）;
	...
}
int main（）
{
	...
	pthread_create（＆thread1，NULL，function1，NULL）;
	pthread_create（＆thread2，NULL，function1，NULL）;
	...
}
```



# goroutine

```go
import (
	"fmt"
	"sync"
)
func main() {
	wg := sync.WaitGroup{} // 这里waitgroup类似于pthread_join,等待线程运行完成
	wg.Add(2)
	go worker(&wg)
	go worker(&wg)
	wg.Wait()
}
func worker(wg *sync.WaitGroup) {
	defer wg.Done()
	fmt.Println("Hello world")
}
```

Grouting是go语言特有的并发体，是一种轻量级的线程，由go关键字启动，goroutine和系统线程也不是等价的，两者的区	别实际上只是一个量的区别，正是这个量变引发了质变

- 创建线程容易，退出难，创建一个线程虽然参数不少，但是可以接受，但是一旦涉及到退出就要考虑thread是detached还是joinable
- 线程并发单元间通信困难，易错，多个线程之间虽然有多种机制可选，用起来相当复杂，一个设计到共享内存，就绪各种锁，切容易造成死锁
- `__stacksize`的设定，大小难以确定，系统线程默认是2M
- 一个线程代价相对于进程来说已经很小了，但我们依然不能不能大量创建线程，因为除了每个线程占用的资源除外，操作系统切换线程的代价也不小
- 对于网络程序来说，由于不能创建大量的线程，就要在少量的线程中做网络多路复用，使用epoll这套机制，即便有[libevent](https://github.com/libevent/libevent)/[libev](http://software.schmorp.de/pkg/libev.html)，但是使用起来也相对比较复杂

go采用了用户层轻量级thread，goroutine的stack size非常小（默认2k），goroutine调度的切换不会陷入(trap)操作系统内核层完成，代价很低

一个go程序对于一个操作系统来说知识一个用户层程序，对于操作系统而言只有线程没有goroutine，goroutine的调度全靠go自己完成，一个go程序中，除了用户代码，剩下的就是go runtime了；

线程竞争的是真实物理CPU，go程序是用户层程序，本身整体运行在一个或多个操作系统线程，因此goroutine竞争的 *CPU资源* 实际指的是系统线程

## Atomicity - 原子操作

原子操作，并发编程中“最小的且不可并行化”的操作。从线程角度说，在当前线程修改共享资源期间，其他的线程时不能访问改资源的。go语言是使用sync.Mutex来实现，类似于线程的pthread_mutex_t

```go
var total struct {
    sync.Mutex
    value int
}
func worker(wg *sync.WaitGroup) {
    defer wg.Done()
    for i := 0; i <= 100; i++ {
        total.Lock()
        total.value += i
        total.Unlock()
    }
}
func main() {
    var wg sync.WaitGroup
    wg.Add(2)
    go worker(&wg)
    go worker(&wg)
    wg.Wait()
    fmt.Println(total.value)
}
```

使用互斥锁保护一个数值型的共享资源，效率比较低，有点大才小用的感觉，标准库sync/atomic提供了丰富的支持

```go
var total uint64
func worker(wg *sync.WaitGroup) {
    defer wg.Done()
    var i uint64
    for i = 0; i <= 100; i++ {
        atomic.AddUint64(&total, i)
    }
}
func main() {
    var wg sync.WaitGroup
    wg.Add(2)
    go worker(&wg)
    go worker(&wg)
    wg.Wait()
}
```

## channel

无缓存的发送操作总在对应的接收操作完成之前发生

```go
var message string

var done = make(chan struct{})
func main() {
	go worker()
	<-done// 优先执行接收操作，但会被阻塞直到channel被写入，message被正常赋值
	fmt.Println(message)
}
func worker() {
	message = "hello world"
  done <- struct{}{}
}
```

对于无缓存channel进程的接收，发生在对该channel进行的发送完成之前

```go
var message string

var done = make(chan struct{})
func main() {
	go worker() 
	done <- struct{}{}// 
	fmt.Println(message)
}
func worker() {
	message = "hello world"
	<-done
}
```

对于channle的第K个接收操作发生在第K+C个操作完成之前，其中c是channle的capacity