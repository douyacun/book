---
Title: go调度器
Keyworkds: goroutine调度器,m系统线程,p调度器,groutine
Description: Goroutine, OS Thread and CPU Management
---

[Toc]

# M、P、G 解析

- G - goroutine
- M - 操作系统线程
- P - 调度上下文，可以被看作是运行于M上的本地调度器

goroutine运行在系统线程(M)上，M会分配给本地调度器(P)

**线程工作类型**

- CPU绑定：永远不会造成线程可能处于等待状态的情况，不断进行计算
- IO绑定：工作使线程进入等待状态，包括请求通过网络访问资源或对操作系统进行系统调用。需要访问数据库的线程将是IO-Bound

**问什么需要 p 调度上下文，为什么不把调度上下文交给系统线程管理？**

系统上线文切换代价高，在用户进程维护上下文可以保证在当前执行的goroutine在执行系统调用阻塞时，让出当前线程，交给B线程处理，go scheduler 确保充足可执行的线程来运行所有的上下文，线程B等待系统调用返回后仍然是可执行状态。

**schedule是如何平衡各个P之间工作量的？**

p1当前已经运行完了runq中的goroutines，如果系统中还有其他等待运行的goroutines,p1可以从全局runq中取goroutine来运行了，如果全局runq中也没有了，会从其他p中取gouroutines

**GOMAXPROCS**

Go 语言并发模型中的 M 其实表示的是操作系统线程，在默认情况下调度器能够允许创建 `10000` 个线程，但是其中绝大多数的线程都不会执行用户代码（可能陷入系统调用），最多只会有 `GOMAXPROCS` 个线程 M 能够正常运行。

Golang 程序中的最大『可运行』线程数其实就等于 `GOMAXPROCS` 这个变量的值

每一个 Go 语言程序中处理器的数量一定会等于 `GOMAXPROCS`，这是因为调度器在启动时就会创建 `GOMAXPROCS` 个处理器 P，这些处理器会绑定到不同的线程 M 上并为它们调度 Goroutine。

# 推荐阅读

- [Scheduling In Go : Part I - OS Scheduler](https://www.ardanlabs.com/blog/2018/08/scheduling-in-go-part1.html)
- [Scheduling In Go : Part II - Go Scheduler](https://www.ardanlabs.com/blog/2018/08/scheduling-in-go-part2.html)
- [ Scheduling In Go : Part III - Concurrency](https://www.ardanlabs.com/blog/2018/12/scheduling-in-go-part3.html)