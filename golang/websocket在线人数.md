---
Title: websocket如何实现统计当前在线人数
Keywords: 在线人数,统计,go,react,ping,pong
Desciption: 鉴于之前开发的聊天功能没人用，考虑在三去掉了聊天功能，改成统计当前在线人数
Lable: 统计在线人数
Cover: assert/websocket-在线人数.png
---

背景：之前开发的聊天是的功能，用的人太少了，考虑再三就决定去掉聊天室的功能了，改做一些其他有意思的事情，比如目前已经开发好了的左边栏当前在线人数，阅读时长统计（正在开发中）

遇到的问题：

- 页面虽然是react写的，但是nextsj服务端渲染的，不是单页应用就不能像单页应用那样有全局变量存储整站的websocket
- 服务端使用nginx作为代理服务器（之前是kong，后来干掉了），nginx proxy模块默认有 read_timeout/send_timeout 默认值 60s
- 服务端socket存储数据结构选型
- 限流

实现思路：

因为是多页模式，就需要每个页面初始化一个wss连接，按cookie去重以后就是当前在线人数。

页面wss重联机制，毕竟断网是很常见的

```jsx

```

nginx：proxy_read_timeout/proxy_send_timeout默认60s超时，心跳检测，只需要服务端在60s内定时发送ping(0x9)消息就可以了，浏览器会自动回复pong消息

```go

```

服务端存储socket结构体







