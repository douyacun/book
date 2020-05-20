---
Title: tcp协议
Keywords: 三次握手,四次挥手,tcp,udp,滑动窗口
Description: tcp三次握手、四次挥手具体流程,tcp和udp的区别,tcp滑动窗口,tcp流量控制
Label: tcp
Date: 2020-05-14 11:36:03
LasteEditTime: 2020-05-14 11:38:00
---

### 三次握手

1. 客户端: 
   - 同步包：SYN = 1 
   - 序列号： Sequence Number = k 
   - 状态：SYN_SEND
2. 服务端:
   - 确认包：ACK = 1
   - 同步包：SYN = 1
   - 序列号：Sequence Number = y
   - 确认号：Acknowledgement Number = x + 1
   - 状态：SYN_WAIT

3. 客户端:

   - 确认包：ACK = 1

   - 确认号：Acknowledgement Number = y + 1

   - 状态：ESTABLISH

     - 服务端收到进入 ESTABLISH

     

### 四次挥手

1. 客户端：
   - 结束包：FIN = 1
   - 序列号：Sequence Number = k
   - 状态：FIN_WAIT_1
2. 服务端：
   - 确认包：ACK = 1
   - 确认号：Acknowledgement Number = k + 1
   - 状态：CLOSE_WAIT
     - 客户端收到以后进入: FIN_WAIT_2
3. 服务端：
   - 结束包：FIN = 1
   - 序列号：Sequence Number = y
   - 状态: LAST_WAIT
4. 客户端：
   - 确认包：ACK = 1
   - 确认号： Acknowledgement Number = y + 1
   - 状态：TIME_WAIT  一段时间没有收到ACK包进入 CLOSED
     - 服务端收到以后进入 CLOSED 状态



### DDOS攻击

在三次握手过程中，服务器发送 SYN-ACK 之后，收到客户端的 ACK 之前的 TCP 连接称为半连接(half-open connect)。此时服务器处于 SYN_RCVD 状态。当收到 ACK 后，服务器才能转入 ESTABLISHED 状态.

SYN 攻击指的是，攻击客户端在短时间内伪造大量不存在的IP地址，向服务器不断地发送SYN包，服务器回复确认包，并等待客户的确认。由于源地址是不存在的，服务器需要不断的重发直至超时，这些伪造的SYN包将长时间占用未连接队列，正常的SYN请求被丢弃，导致目标系统运行缓慢，严重者会引起网络堵塞甚至系统瘫痪。

**防御**

- 缩短超时时间（SYN Timeout）
- 增大半连接数
- 过滤网关
- SYN Cookie



### 滑动窗口

针对发送端和接收端进行流量控制的策略，窗口的意识就是 接收端 允许 发送端一次发送多少数据

**TCP滑动窗口确认机制**

1. 每一条消息都有一个识别编号，每一条消息都能够被独立的确认，Sequence Number就是消息id
2. 接收方定期向发送方发送限制参数（一次发送消息的最大数据）

**发送端的消息分类**

1. 已发送，且收到确认
2. 已发送，未收到确认
3. 未发送，准备发送
4. 未发送，不允许发送

**接收端的消息分类** 

1. 已接收确认
2. 未接收准备接收
3. 未接收不准备接收

**滑动窗口的可靠性**

发送窗口只有收到对端对于本段发送窗口内字节的ACK确认，才会移动发送窗口的左边界

**选择重传**

接收端缓存所有帧，当某个帧出现错误时，只会要求重传这一个帧，当某个序号的所有帧都收到以后才会提交给高层协议

接收端需要缓存所有帧

**超时重发**

加入5一直没有ACK包，超过一段时间后会重新发送给接收端

**图**

![](./assert/滑动窗口.jpg)

### UDP协议

1. UDP是无连接的，状态少
2. UDP报文很短只有8个字节
3. UDP使用尽最大努力交付
4. UDP面向报文

适用于：数据完整性要求不高的场景，视频聊天等

