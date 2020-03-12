---
Title: golang开发的聊天室
Keywords: 聊天,chat,golang,websocket,react,nextjs
Description: 一直以来对websocket保持一颗崇拜的心，php这方面比较弱势，学习go语言以后开发一套聊天功能，一起参观一下
Cover: assert/chat-cover.png
Label: websocket
Date: 2020-03-02 13:57:00
LastEditTime: 2020-03-02 13:57:00
---



# 环境

后端

-   语言golang
-   数据库elastisearch
-   web框架是基于[gin](https://gin-gonic.com/zh-cn/docs/)封装的
-   websocket库用的是[gorilla/websocket](https://github.com/gorilla/websocket)
-   日志: [zap](https://github.com/uber-go/zap)
-   Error: [pkg/errors](https://github.com/pkg/errors)

前端

-   框架: [react](https://zh-hans.reactjs.org/docs/getting-started.html)
-   服务端渲染: [nextjs](https://nextjs.org/docs/api-reference/next.config.js/static-optimization-indicator)
-   登录认证:  cookie [nookie](https://github.com/maticzav/nookies)  
-   websocket是浏览器原生支持

# 如何建立连接

-   [websocket协议](https://www.cnblogs.com/chyingp/p/websocket-deep-in.html)

websocket复用http的握手通道，客户端通过HTTP请求与WebSocket服务端协商升级协议，协议升级完成后，后续的数据交换则按按照websocket的协议。

**1. 客户端：申请协议升级**

```http
Connection: Upgrade
Sec-WebSocket-Extensions: permessage-deflate; client_max_window_bits
Sec-WebSocket-Key: m3xNMIzhueJyd3N66EAK6w==
Sec-WebSocket-Version: 13
Upgrade: websocket
```

-   `Connection: Upgrade` 表示要升级协议
-   `Upgrade: websocket`  升级到websocket协议
-   `Sec-WebSocket-Version: 13` websocket协议版本，如果服务端不支持该版本，需要返回一个`Sec-WebSocket-Version`header，里面包含服务端支持的版本号。

-   `Sec-WebSocket-Key: m3xNMIzhueJyd3N66EAK6w==` 与后面服务端响应首部的`Sec-WebSocket-Accept`是配套的，提供基本的防护，比如恶意的连接，或者无意的连接

**2. 服务端：响应协议升级**

```http
HTTP/1.1 101 Switching Protocols
Connection: upgrade
Sec-WebSocket-Accept: qgWIKoKflyd9H8L/jCNa8XP4CYQ=
Upgrade: websocket
```

状态代码`101`表示协议切换。到此完成协议升级，后续的数据交互都按照新的协议来。

**3. Sec-WebSocket-Accept的计算**

`Sec-WebSocket-Accept`根据客户端请求首

`Sec-WebSocket-Accept`根据客户端请求首部的`Sec-WebSocket-Key`计算出来。

计算公式为：

1.  将`Sec-WebSocket-Key`跟`258EAFA5-E914-47DA-95CA-C5AB0DC85B11`拼接。
2.  通过SHA1计算出摘要，并转成base64字符串。

伪代码如下：

```
>toBase64( sha1( Sec-WebSocket-Key + 258EAFA5-E914-47DA-95CA-C5AB0DC85B11 )  )
```

验证下前面的返回结果：

```js
const crypto = require('crypto');
const magic = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';
const secWebSocketKey = 'm3xNMIzhueJyd3N66EAK6w==';

let secWebSocketAccept = crypto.createHash('sha1')
    .update(secWebSocketKey + magic)
    .digest('base64');

console.log(secWebSocketAccept);
// qgWIKoKflyd9H8L/jCNa8XP4CYQ=
```

# websocket server 布局

服务端需要维护

```go
type Responser interface {
	Members() []string
	Bytes() []byte
	GetChannelID() string
}

// Hub maintains the set of active clients and broadcasts messages to the
// clients.
type Hub struct {
	// Registered clients.
	clients map[string]*Client

	// Inbound messages from the clients.
	broadcast chan Responser

	// Register requests from the clients.
	register chan *Client

	// Unregister requests from clients.
	unregister chan *Client
}
```







# 遗留问题











- [websocket优化方向](https://learnku.com/articles/23560/using-golang-to-achieve-million-level-websocket-services)
- [go websocket库](https://github.com/gorilla/websocket)

https://github.com/sejr/react-messenger

https://github.com/Detaysoft/react-chat-elements/blob/master/src/MessageList/MessageList.js

