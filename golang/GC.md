---
Title: go GC内存回收 - 三色标记清除算法
Keywords: gc,三色标记,垃圾回收
Description: 初步了解一下golang的gc，有个宏观概念即可
Author: douyacun
Cover: 
Label: GC内存回收
Date: 2019-11-20 09:25:56
LastEditTime: 2019-12-01 23:37:34
---

推荐阅读：
- [Golang’s Real-time GC in Theory and Practice](https://making.pusher.com/golangs-real-time-gc-in-theory-and-practice/index.html)
- [三色标记算法 这个是翻译](https://laily.net/article/Golang GC(垃圾回收) - 三色标记算法)



这里只是熟悉gc的基础概念，方便对于整个内存架构熟悉后，能深入学习源码，形成自己的分析思路



# 概念

GC过程会有两种**后台任务**（G）

- 标记，在需要时启动
- 清扫，程序启东时启动一个，进入清扫阶段时唤醒

**三色定义**

- 白色：对象在这次GC中未标记，初始化是白色，不可达对象。
- 灰色：对象在这次GC中已标记，对象本身被引用到，但是子引用还没有处理完成
- 黑色：对象在这次GC中已标记，对象本身被引用到，且子引用也处理完成

**标记过程**

- 所有对象初始化都是白色
- 从root开始找到所有对象，标记为灰色，放入待处理队列
- 遍历灰色队列，将其引用对象标记为灰色放入待处理队列，自身标记黑色
- 处理完灰色对象队列，执行清扫工作

**写屏障**
go支持gc，gc的扫描和用户程序是并行的，用户程序一直在修改内存，带来的问题是gc扫描的过程中可能改变了对象的依赖树

例如：开始扫描时根对象A，B。B拥有c的指针，GC先扫描A，然后B把c交给了A，GC在扫描B这时c就不会被扫描到，避免这个问题gc在标记阶段启用写屏障

issue：

- gc对象都是包含什么， 全局变量、函数内变量、函数
