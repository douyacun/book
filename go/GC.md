---
Title: go GC内存回收 - 三色标记清除算法
Keywords: gc,三色标记,垃圾回收
Description: 初步了解一下golang的gc，有个宏观概念即可
Author: douyacun
Cover: 
Label: GC内存回收
Date: 2019-11-20 09:25:56
LastEditTime: 2019-11-24 23:18:46
---

推荐阅读：
- [Golang’s Real-time GC in Theory and Practice](https://making.pusher.com/golangs-real-time-gc-in-theory-and-practice/index.html)
- [三色标记算法 这个是翻译](https://laily.net/article/Golang GC(垃圾回收) - 三色标记算法)

GC过程会有两种**后台任务**（G）
- 标记，在需要时启动
- 清扫，程序启东时启动一个，进入清扫阶段时唤醒

GC流程会进行两次**STW**（stop the world）
1. 第一次STW Mark开始阶段，准备根对象的扫描，启动写屏障(write barrier)和辅助GC(mutator assist)
2. Mark Termination阶段，STW重新扫描根对象，禁用写屏障(write barrier)和辅助GC(mutator assist)

**gc的四个阶段**
- sweep termination
- mark
- mark termination
- sweep

**三色定义**：
- 白色：对象在这次GC中未标记，初始化是白色，不可达对象。
- 灰色：对象在这次GC中已标记，对象本身被引用到，但是子引用还没有处理完成
- 黑色：对象在这次GC中已标记，对象本身被引用到，且子引用也处理完成

标记过程：
- 所有对象初始化都是白色
- 所有初始对象，即全局对象或根对象，标记为灰色
- 任意取一个灰色对象，将它所有引用到的白色对象标记为灰色，，然后将它标记为黑色
- 重复上一步，直到找不到灰色对象
- 所有的黑色对象都是可达，而白色对象是不可达的，回收掉白色对象

issue：
- 对象都是包含什么， 全局变量、函数内变量、函数

**写屏障**
go支持gc，gc的扫描和程序是并行的，带来的问题是gc扫描的过程中可能改变了对象的依赖树\
例如：开始扫描时根对象A，B。B拥有c的指针，GC先扫描A，然后B把c交给了A，GC在扫描B这时c就不会被扫描到，避免这个问题gc在标记阶段启用写屏障