---
Title: websocket如何实现统计当前在线人数
Keywords: 在线人数,统计,go,react,ping,pong
Description: 鉴于之前开发的聊天功能没人用，考虑在三去掉了聊天功能，改成统计当前在线人数
Lable: 统计在线人数
Cover: assert/websocket-在线人数.png
---

背景：之前开发的聊天是的功能，用的人太少了，考虑再三就决定去掉聊天室的功能了，改做一些其他有意思的事情，比如目前已经开发好了的左边栏当前在线人数，阅读时长统计（正在开发中）

遇到的问题：

- 页面虽然是react写的，但是nextjs服务端渲染的，不是单页应用就不能像单页应用那样有全局变量存储整站的websocket
- 服务端使用nginx作为代理服务器（之前是kong，后来干掉了），nginx proxy模块默认有 read_timeout/send_timeout 默认值 60s
- 服务端socket存储数据结构选型
- 安全防护

实现思路：

因为是多页模式，就需要每个页面初始化一个wss连接，按cookie去重以后就是当前在线人数。

页面wss重联机制，毕竟断网是很常见的

```jsx
/**
* websocket: 初始化 
*/
const connect = () => {
  conn = new WebSocket(ws_address);
  conn.onmessage = handlerMessage;
  // ws 关闭需要重新连接，每隔1s尝试1次
  conn.onclose = function (e) {
    console.log('Socket is closed. Reconnect will be attempted in 1 second.', e.reason);
    setTimeout(function () {
      connect();
    }, 1000);
  };
  // 连接失败
  conn.onerror = function (e) {
    console.error('Socket encountered error: ', e.message, 'Closing socket');
    conn.close()
  }
}
```



nginx：proxy_read_timeout/proxy_send_timeout默认60s超时，心跳检测，只需要服务端在60s内定时发送ping(0x9)消息就可以了，浏览器会自动回复pong消息

```go
for {
  select {
    // 定时向客户端推送，当前在线人数
    case <-countTicker.C:
    c.send <- c.hub.Count().Bytes()
    // 定时心跳检测
    case <-pingTicker.C:
    _ = c.conn.SetWriteDeadline(time.Now().Add(writeWait))
    if err := c.conn.WriteMessage(websocket.PingMessage, nil); err != nil {
      return
    }
  }
}
```



服务端存储socket结构体，没有向指定socket写数据的需求，不需要额外维护socket的数据结构，维护一个cookie在线页面数即可

```go
type Hub struct {
	// Registered uid
	uuid map[string]int
	// Inbound messages from the clients.
	broadcast chan Responser
	// Register requests from the clients.
	register chan *Client
	// Unregister requests from clients.
	unregister chan *Client
}

func (h *Hub) Run() {
  for {
    select {
      case client := <-h.register:
      // 注册ws
      h.uuid[client.uuid]++
      client.send <- hub.Count().Bytes()
      case client := <-h.unregister:
      // 删除ws
      h.uuid[client.uuid]--
      if h.uuid[client.uuid] == 0 {
        delete(h.uuid, client.uuid)
      }
    }
  }
```

安全方面需要考虑的内容是：

1. 如何检测跨站点 WebSocket 劫持漏洞?
2. 同一设备  ws 连接数如何限制，避免bug导致连接数过多导致服务挂掉?

解决方法：

1. check origin, 确保是自己网站的来源
2. 服务器端为每个 WebSocket 客户端生成唯一的一次性 Token；
3. 客户端将 Token 作为 WebSocket 连接 URL 的参数（譬如 wss://www.douyacun.com/?token＝randomOneTimeToken），发送到服务器端进行 WebSocket 握手连接；
4. 服务器端验证 Token 是否正确，一旦正确则将这个 Token 标示为废弃不再重用，同时确认 WebSocket 握手连接成功；如果 Token 验证失败或者身份认证失败，则返回 403 错误。
5. 限流，限制设备最多最大链接数，必要时，deny ip地址