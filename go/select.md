---
Title: go select 源码分析,用法
Keywords: select源码,用法,技巧
Description: go select 是一种仅能用于channel发送和接收消息的专用语句，此语句运行期间是阻塞的。select是go在语言层面提供的I/O多路复用的机制，专门检测多个channel是否准备完毕，可读或可写
Author: douyacun
Date: 2019-09-23 11:14:31
LastEditTime: 2019-10-09 11:12:35
typora-root-url: ./assert
---
[TOC]

go select 是一种仅能用于channel发送和接收消息的专用语句，此语句运行期间是阻塞的。select是go在语言层面提供的I/O多路复用的机制，专门检测多个channel是否准备完毕，可读或可写

# mind map

![](/select.png)

# 编译器优化

1. case数量为0的话，compiler会优化为gopark阻塞线程

```go
select{} -> runtime.block() -> gopark()

// gc/select/walkselectcases
// optimization: zero-case select
if n == 0 {
  return []*Node{mkcall("block", nil, nil)}// 这里编译器优化为调用runtime.block
}

// runtime.block. 最终调用还是 gopark() 阻塞goroutine
func block() {
	gopark(nil, nil, waitReasonSelectNoCases, traceEvGoStop, 1) // forever
}
```

2. case数量为1的话，compiler会优化为if 

```go
select{								if v<-foo {
case <-foo:			-> 		....
}											}
// 源码在gc/select.walkselectcases
```

3. case数量为1,有default的话compiler会优化为 if else

```go
select{								if v<-foo {
case <-foo:			-> 		....
default:							} else {
}											}
```

# 常见用法

## 阻塞

上面已经提过，编译器会优化case == 0 的G 变为阻塞你状态

```go
select{} -> runtime.block() -> gopark()
```

## 非阻塞

如果ch没有数据的话，就会一直while死循环cpu飙升，每次都会走default

```go
for {
    select {
        case d := <-ch:
        default:
    }
}
```

## 超时控制

```go
c1 := make(chan int)

go func() {
  time.Sleep(4 * time.Second)
  c1 <- 1
}()

select {
  case d, ok :=  <-c1:
  if ok {
    fmt.Println(d)
  }
  case <-time.After(3 * time.Second):
  fmt.Println("timeout...")
}

close(c1)
```

## 随机数

生成随机3位数

```
var num int64
ch := make(chan int64, 3)
for i := 0; i < 3; i++ {
  select {
  case ch <- 0:
  case ch <- 1:
  case ch <- 2:
  case ch <- 3:
  case ch <- 4:
  case ch <- 5:
  case ch <- 6:
  case ch <- 7:
  case ch <- 8:
  case ch <- 9:
  }
  num = num*10 + <-ch
}
fmt.Println(num)
```



## 关闭channel

```go
for {
    select {
        case d := <-ch:
    }
}
```

这样写也会有问题，如果ch被close掉以后，就会一直返回0值。如果select中实际只有一个case，for rang比较安全，上面的大家都实践一下。都是平时见到比较多的坑，如果是多个返回值的话，老老实实的写双返回值。

```go
for {
    select {
        case d, ok := <-ch1:
            if !ok {
                break outer
            }
        case d, ok := <-ch2:
            if !ok {
                break outer
            }
    }
}
```

# 如何随机

go select是用heap sort来实现随机,首先看下什么是堆结构和堆排序

## 堆结构

堆是一种树形结构：

- 堆的每一个子节点都要大于或小于根节点
- 大根堆：每个元素大于其所有子元素
- 小根堆：每个元素小于其所有子元素

## 堆排序

算法思想：堆排序利用堆结构根节点的元素最大的特点，不断取出根节点并维护堆结构来排序

实现步骤：

1. 建堆，将初始化序列建成一个大根堆
2. 去掉堆顶元素，剩余的元素调整为二叉堆
3. 完成最后堆排序

时间复杂度：最坏为O(nlogn)

优势：能在插入操作和删除最大元素操作混合的动态场景中保证对数级别的运行时间，代码实现简单。

## 根据什么来排序的呢？

```go
// 打乱顺序
for i := 1; i < ncases; i++ {
  // fastrandn 随机数
  j := fastrandn(uint32(i + 1))
  pollorder[i] = pollorder[j]
  pollorder[j] = uint16(i)
}
// sort the cases by Hchan address to get the locking order.
// 根据channel的内存地址来获取上锁的顺序
// simple heap sort, to guarantee n log n time and constant stack footprint.
// 堆排序：O(nlogn)时间保证和堆栈占用的保证
for i := 0; i < ncases; i++ {
    j := i
    // Start with the pollorder to permute cases on the same channel.
    c := scases[pollorder[i]].c
    for j > 0 && scases[lockorder[(j-1)/2]].c.sortkey() < c.sortkey() {
        k := (j - 1) / 2
        lockorder[j] = lockorder[k]
        j = k
    }
    lockorder[j] = pollorder[i]
}
for i := ncases - 1; i >= 0; i-- {
    o := lockorder[i]
    c := scases[o].c
    lockorder[i] = lockorder[0]
    j := 0
    for {
        k := j*2 + 1
        if k >= i {
            break
        }
        if k+1 < i && scases[lockorder[k]].c.sortkey() < scases[lockorder[k+1]].c.sortkey() {
            k++
        }
        if c.sortkey() < scases[lockorder[k]].c.sortkey() {
            lockorder[j] = lockorder[k]
            j = k
            continue
        }
        break
    }
    lockorder[j] = o
}
```

# 调用栈

1. `func Select(cases []SelectCase) (chosen int, recv Value, recvOK bool)`  -> reflect/value.Select
2. `func reflect_rselect(cases []runtimeSelect) (int, bool)`  -> reflect/select.reflect_rselect
3. `func selectgo(cas0 *scase, order0 *uint16, ncases int) (int, bool)` -> reflect/select.selectgo

3个函数有3个结构体(SelectCase/runtimeSelect/scase)，都是针对case抽象出来的结构体，判断每个case的类型，接收/发送/默认。主要功能是在selectgo中，代码过长，只看下主要代码：

```go
// cas0 points to an array of type [ncases]scase, and order0 points to
// an array of type [2*ncases]uint16. Both reside on the goroutine's
// stack (regardless of any escaping in selectgo).
func selectgo(cas0 *scase, order0 *uint16, ncases int) (int, bool) {
	...
  // 指向case0数组首地址
	cas1 := (*[1 << 16]scase)(unsafe.Pointer(cas0))
  // 长度是case1的两倍
	order1 := (*[1 << 17]uint16)(unsafe.Pointer(order0))
  // slice里面有两个冒号什么意思呢？ a[x:y:z] 切片长度: y-x(ncases-0 = ncases) 切片容量:z-x (ncases-0 = ncases)
  scases := cas1[:ncases:ncases] //scases len: ncases cap: ncases
  // 轮询顺序
	pollorder := order1[:ncases:ncases]
  // cannel 加速顺序
	lockorder := order1[ncases:][:ncases:ncases]
  // 下面操作就是剔除channel == nil的case
	for i := range scases {
		cas := &scases[i]
		if cas.c == nil && cas.kind != caseDefault {
			*cas = scase{}
		}
	}
	var t0 int64
	if blockprofilerate > 0 {
		t0 = cputicks()
		for i := 0; i < ncases; i++ {
			scases[i].releasetime = -1
		}
	}
	// 这里的代码牵扯到select是如何随机的，下面统一讲	
	...

	// 对所有的channel加锁
	sellock(scases, lockorder)

	var (
		gp     *g
		sg     *sudog
		c      *hchan
		k      *scase
		sglist *sudog
		sgnext *sudog
		qp     unsafe.Pointer
		nextp  **sudog
	)

loop:
	// pass 1 - look for something already waiting
	var dfli int
	var dfl *scase
	var casi int
	var cas *scase
	var recvOK bool
  // 这里开始遍历case数组
	for i := 0; i < ncases; i++ {
		casi = int(pollorder[i])
		cas = &scases[casi] // case结构体
		c = cas.c // case对应的channel
		switch cas.kind {
		case caseNil: // channel为nil
			continue
		case caseRecv:// channel为接收者
			sg = c.sendq.dequeue()
			if sg != nil {// channel.sendq 发送队列存在runnable G
				goto recv
			}
			if c.qcount > 0 {// channel.qcount channel元素数量大于0
				goto bufrecv
			}
			if c.closed != 0 {// channel已经关闭了
				goto rclose
			}
		case caseSend:// channel为发送者
      ...
			if c.closed != 0 {
				goto sclose
			}
			sg = c.recvq.dequeue()
			if sg != nil {// channel的接收者队列 存在 runnable G
				goto send
			}
			if c.qcount < c.dataqsiz {// channel队列，没有满
				goto bufsend
			}
		case caseDefault:// default case
			dfli = casi
			dfl = cas
		}
	}

	if dfl != nil {
		selunlock(scases, lockorder)
		casi = dfli
		cas = dfl
		goto retc
	}

	// pass 2 - enqueue on all chans
  // 当前G 入队 && runnable -> waitting
	gp = getg()
	if gp.waiting != nil {
		throw("gp.waiting != nil")
	}
	nextp = &gp.waiting
	for _, casei := range lockorder {
		... // 这些就是序列化当前G
		switch cas.kind {
		case caseRecv:
      // 入队：channel接收者队列
			c.recvq.enqueue(sg)
		case caseSend:
      // 入队：channel发送者队列
			c.sendq.enqueue(sg)
		}
	}
	// wait for someone to wake us up
	gp.param = nil
	gopark(selparkcommit, nil, waitReasonSelect, traceEvGoBlockSelect, 1)
	sellock(scases, lockorder)
	gp.selectDone = 0
	sg = (*sudog)(gp.param)
	gp.param = nil

	// pass 3 - dequeue from unsuccessful chans
	// otherwise they stack up on quiet channels
	// record the successful case, if any.
	// We singly-linked up the SudoGs in lock order.
  // 被唤醒 - 这种情况不存在default
	casi = -1
	cas = nil
	sglist = gp.waiting
	// Clear all elem before unlinking from gp.waiting.
	for sg1 := gp.waiting; sg1 != nil; sg1 = sg1.waitlink {
		sg1.isSelect = false
		sg1.elem = nil
		sg1.c = nil
	}
	gp.waiting = nil

	for _, casei := range lockorder {
		k = &scases[casei]
		if k.kind == caseNil {
			continue
		}
		if sglist.releasetime > 0 {
			k.releasetime = sglist.releasetime
		}
		if sg == sglist {
			// sg has already been dequeued by the G that woke us up.
			casi = int(casei)
			cas = k
		} else {
			c = k.c
			if k.kind == caseSend {
				c.sendq.dequeueSudoG(sglist)
			} else {
				c.recvq.dequeueSudoG(sglist)
			}
		}
		sgnext = sglist.waitlink
		sglist.waitlink = nil
		releaseSudog(sglist)
		sglist = sgnext
	}
	// 当前G被唤醒，如果没有对应的case，重新进入loop循环
	if cas == nil {
		// We can wake up with gp.param == nil (so cas == nil)
		// when a channel involved in the select has been closed.
		// It is easiest to loop and re-run the operation;
		// we'll see that it's now closed.
		// Maybe some day we can signal the close explicitly,
		// but we'd have to distinguish close-on-reader from close-on-writer.
		// It's easiest not to duplicate the code and just recheck above.
		// We know that something closed, and things never un-close,
		// so we won't block again.
		goto loop
	}

	c = cas.c

	if debugSelect {
		print("wait-return: cas0=", cas0, " c=", c, " cas=", cas, " kind=", cas.kind, "\n")
	}

	if cas.kind == caseRecv {
		recvOK = true
	}

	if raceenabled {
		if cas.kind == caseRecv && cas.elem != nil {
			raceWriteObjectPC(c.elemtype, cas.elem, cas.pc, chanrecvpc)
		} else if cas.kind == caseSend {
			raceReadObjectPC(c.elemtype, cas.elem, cas.pc, chansendpc)
		}
	}
	if msanenabled {
		if cas.kind == caseRecv && cas.elem != nil {
			msanwrite(cas.elem, c.elemtype.size)
		} else if cas.kind == caseSend {
			msanread(cas.elem, c.elemtype.size)
		}
	}

	selunlock(scases, lockorder)
	goto retc
...
```