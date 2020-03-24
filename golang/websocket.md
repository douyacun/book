---
Title: golang开发的聊天室
Keywords: 聊天,chat,golang,websocket,react,nextjs
Description: 一直以来对websocket保持一颗崇拜的心，php这方面比较弱势，学习go语言以后开发一套聊天功能，一起参观一下
Cover: assert/chat-cover.png
Label: websocket
Date: 2020-03-02 13:57:00
LastEditTime: 2020-03-24 10:29:35
---


[TOC]

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
-   UI: [Material UI](https://material-ui.com/zh/components)
-   登录认证:  cookie [nookie](https://github.com/maticzav/nookies)  
-   websocket是浏览器原生支持
-   后续聊天功能模仿：[https://getstream.io/](https://github.com/GetStream/stream-chat-react)

登录

- [github OAuth documentation](https://developer.github.com/apps/building-oauth-apps/authorizing-oauth-apps/)
- [google Sign-In for Websites](https://developers.google.com/identity/sign-in/web)

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

# 前端难点

**服务端渲染框架如何使用cookie**

nextjs是服务端渲染框架，nextjs会请求是在服务器上初始化一次dom，要获取cookie可以有2种方式

1. 从请求request中获取cookie，返回给组件
2. 客户端请求完成后用hook从浏览器本地加载cookie

cookie 库用的是 [nookies](https://github.com/maticzav/nookies)

```js
React.useEffect(() => {
  const all = parseCookies();
  if (all.douyacun) {
    const douyacun = JSON.parse(all.douyacun);
    setCook(douyacun);
  }
}, []);
```

另一个就是本地开发cookie跨域的问题, 本地域名localhost:3000,服务端域名localhost:9000：

本来是想用[http-proxy-middleware](https://github.com/chimurai/http-proxy-middleware) 来解决跨域的问题，但是websocket代理关于cookie的问题难以搞定，版本是1.0.3

最后还是nginx了，怎么统一一个域名呢？location，后端服务统一以api开头

```nginx
server {
	listen 80;
	server_name douyacun.io;

	location /api/ {
        		if ($request_method = 'OPTIONS') {
      					return 204;
           	}
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header Host  $http_host;
            proxy_set_header X-Nginx-Proxy true;
            proxy_http_version 1.1;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection 'upgrade';

    				# 不需要考虑到负载的，就无需配置upstream节点。
    				proxy_pass    http://127.0.0.1:9003;
        }
	location / {
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header Host  $http_host;
            proxy_set_header X-Nginx-Proxy true;
            proxy_set_header Connection "";

            # 不需要考虑到负载的，就无需配置upstream节点。
            proxy_pass    http//127.0.0.1:3000;
  }
}
```

修改本地dns douyacun.io => 127.0.0.1

**如何保证websocket初始化一次**

还是因为框架用的nextjs服务端渲染框架，不能在全局初始化websocket，只能在useEffect中使用

```js
useEffect(() => {
  initWebsocket();
  return () => {
  }
});
const initWebsocket = () => {
  conn = new WebSocket(WS_ADDRESS);
  conn.onmessage = handlerMessage;
  conn.onclose = function () {
    dispatch({ type: "dialog_open", dialogOpen: true })
  };
  conn.onerror = function () {
    console.log("连接失败");
  }
}
```

这样会有一个问题，只要有state变更，useEffect就会被调用，导致webSocket循环初始化

> todo: 后端服务如何识别这种重复性的初始化，如何拒绝？

react hook 文档给出了答案

> the **previous effect is cleaned up before executing the next effect**. In our example, this means a new subscription is created on every update. To avoid firing an effect on every update, refer to the next section. 
>
> 意味着组件的每一次更新都会创建新的订阅
>
> If you pass an empty array (`[]`), the props and state inside the effect will always have their initial values, 
>
> useEffect第二个参数传 []， 就不回每次state变更都调用了。

```js
useEffect(() => {
  initWebsocket();
  return () => {
  }
}, []);
```

**conn.onmessage(() => {setMessages()}) 不会触发state组件的重新渲染？**

这个问题没有弄懂，不过react hook提供另一种状态管理，useReducer(), 就像redux一样。

> state 逻辑较复杂且包含多个子值，或者下一个 state 依赖于之前的 state 等。并且，使用 `useReducer` 还能给那些会触发深更新的组件做性能优化，

**滑动条的位置**

1. 收到新消息时，滑动条回到最底部

hook 提供了useRef可以操作组件, 在收到消息时，控制滑动条到最底部，

```jsx
const scrollToBottom = () => {
  // 保证消息在最下面
  contentRef.current.scrollTop = contentRef.current.scrollHeight
}
const handlerMessage = function (evt) {
  let msg = JSON.parse(evt.data)
  dispatch({ type: "new_message", msg: msg, channel_id: msg['channel_id'] });
  scrollToBottom();
}

<div className="scrollable content" ref={contentRef} onScroll={upScrollLoadMore}>
  ...
</div>
```

2. 上拉加载更多历史消息

滑动条高度 = 加载历史消息完成后文档高度 - 加载前文档高度

```jsx
// 这里是为了在加载完成后，在控制滑动条位置，如果不是hook的话，setState(state, () => {})，第二个参数回掉函数完成，hook使用useEffect的代替了这种方式
useEffect(() => {
  // loadMore 是请求后端的接口
  let t = setTimeout(loadMore, 1000);
  if (!loading) {
    // 滑动条高度 = 加载历史消息完成后文档高度 - 加载前文档高度
    contentRef.current.scrollTop = contentRef.current.scrollHeight - messagesHeight;
  }
  return () => {
    clearTimeout(t);
  }
}, [loading]);

// contentRef.onScroll
const upScrollLoadMore = () => {
  let scrollTop = contentRef.current.scrollTop;
  // 记录加载前文档的高度
  let channelMessages = state.messages[state.currentId];
  if (scrollTop == 0 && !loading && channelMessages && channelMessages.length > 0) {
    let channel = getChannelById(state.currentId);
    if (channel.total > 0) {
      setloading(true);
    }
  }
}
```

3. 正在查看历史消息，如果有新消息来会导致滑动条到最底部

todo::

# nginx如何反向代理websocket?

前端开发环境配置的时候已经展示过一次

- `proxy_http_version 1.1;` : 代理http版本号
- `proxy_set_header Upgrade $http_upgrade;`: http协议升级的请求头Upgrade传给后端
- `proxy_set_header Connection 'upgrade';` http协议升级的请求头Connection传给后端



# 群聊和私聊是如何实现的

不管是私聊还是群聊，都会创建一个channel来包含成员，每次消息发送消息指定channel.id即可，服务器在收到消息后，根据channel.id向channel.members推送消息。

channel分了3中类型：

- global 全局，每个人都会订阅
- public 群聊 聊天人数超过2个
- private 私聊 聊天人群2个



# 一个连接的成本是多少？

这边使用[gorilla/websocket](https://github.com/gorilla/websocket/blob/master/examples/chat/main.go) 1万个连接测试

占用147.93MB RAM, 平均连接每个占用15kb 测试代码见：[github gwebsocket](https://github.com/douyacun/gwebsocket/blob/master/v3_ws_ulimit/wsserver.go)

```shell
(pprof) top
Showing nodes accounting for 137.93MB, 93.24% of 147.93MB total
Dropped 6 nodes (cum <= 0.74MB)
Showing top 10 nodes out of 51
      flat  flat%   sum%        cum   cum%
   73.79MB 49.88% 49.88%    73.79MB 49.88%  bufio.NewWriterSize
   34.63MB 23.41% 73.29%    34.63MB 23.41%  bufio.NewReaderSize
      11MB  7.44% 80.73%       11MB  7.44%  runtime.malg
       4MB  2.70% 83.44%     5.50MB  3.72%  net/textproto.(*Reader).ReadMIMEHeader
       3MB  2.03% 85.46%     3.50MB  2.37%  github.com/gorilla/websocket.newConn
       3MB  2.03% 87.49%    10.50MB  7.10%  net/http.readRequest
    2.50MB  1.69% 89.18%    16.50MB 11.16%  net/http.(*conn).readRequest
    2.50MB  1.69% 90.87%     3.50MB  2.37%  context.propagateCancel
       2MB  1.35% 92.23%        2MB  1.35%  syscall.anyToSockaddr
    1.50MB  1.01% 93.24%     1.50MB  1.01%  net.newFD
(pprof) web
failed to execute dot. Is Graphviz installed? Error: exec: "dot": executable file not found in $PATH
(pprof) list flat
Total: 147.93MB
```

goroutine是10003，每个goroutine占用4kb的内存

```shell
(pprof) top
Showing nodes accounting for 10001, 100% of 10003 total
Dropped 24 nodes (cum <= 50)
Showing top 10 nodes out of 19
      flat  flat%   sum%        cum   cum%
     10001   100%   100%      10001   100%  runtime.gopark
         0     0%   100%       9998   100%  bufio.(*Reader).Peek
         0     0%   100%       9998   100%  bufio.(*Reader).fill
         0     0%   100%       9999   100%  github.com/gorilla/websocket.(*Conn).NextReader
         0     0%   100%       9999   100%  github.com/gorilla/websocket.(*Conn).ReadMessage
         0     0%   100%       9999   100%  github.com/gorilla/websocket.(*Conn).advanceFrame
         0     0%   100%       9998   100%  github.com/gorilla/websocket.(*Conn).read
         0     0%   100%       9999   100%  internal/poll.(*FD).Read
         0     0%   100%      10001   100%  internal/poll.(*pollDesc).wait
         0     0%   100%      10001   100%  internal/poll.(*pollDesc).waitRead (inline)
(pprof) list flat
Total: 10003
(pprof)
```

根据  Eran Yanay 在 Gophercon Israel 分享的讲座 [https://www.youtube.com/watch?reload=9&v=LI1YTFMi8W4](https://www.youtube.com/watch?reload=9&v=LI1YTFMi8W4) 优化,  代码在[github](https://github.com/eranyanay/1m-go-websockets)


- 使用epoll优化, 复用goroutine， goroutine适合cpu密集型，而epoll适合I/O密集型，这里使用epoll来复用goroutine， 如果是1万个链接的话, 4kb *  10000 / 1024 ~= 39M , epoll的原理和用法可以看一下，[了解一下高大上的epoll](/linux/websocket.md)

这边使用epoll 内存节省了 147.93 - 79.94 = 67.99MB, 

```shell
(pprof) top
Showing nodes accounting for 79.94MB, 100% of 79.94MB total
Showing top 10 nodes out of 37
      flat  flat%   sum%        cum   cum%
   38.65MB 48.35% 48.35%    38.65MB 48.35%  bufio.NewReaderSize (inline)
   30.12MB 37.67% 86.02%    30.12MB 37.67%  bufio.NewWriterSize
    4.50MB  5.63% 91.65%        5MB  6.26%  github.com/gorilla/websocket.newConn
    2.50MB  3.13% 94.78%     2.50MB  3.13%  net.sockaddrToTCP
       2MB  2.50% 97.28%        2MB  2.50%  syscall.anyToSockaddr
    0.67MB  0.84% 98.12%     0.67MB  0.84%  main.(*epoll).Add
    0.50MB  0.63% 98.75%     0.50MB  0.63%  fmt.(*pp).handleMethods
    0.50MB  0.63% 99.37%     0.50MB  0.63%  net.newFD
    0.50MB  0.63%   100%     0.50MB  0.63%  github.com/gorilla/websocket.(*Conn).SetPingHandler
         0     0%   100%    38.65MB 48.35%  bufio.NewReader
(pprof) list flat
Total: 79.94MB
```

运行的goroutine只有5个

```shell
(pprof) top
Showing nodes accounting for 5, 100% of 5 total
Showing top 10 nodes out of 35
      flat  flat%   sum%        cum   cum%
         2 40.00% 40.00%          2 40.00%  runtime.gopark
         1 20.00% 60.00%          1 20.00%  runtime/pprof.writeRuntimeProfile
         1 20.00% 80.00%          1 20.00%  syscall.Syscall
         1 20.00%   100%          1 20.00%  syscall.Syscall6
         0     0%   100%          2 40.00%  internal/poll.(*FD).Accept
         0     0%   100%          1 20.00%  internal/poll.(*FD).Read
         0     0%   100%          2 40.00%  internal/poll.(*pollDesc).wait
         0     0%   100%          2 40.00%  internal/poll.(*pollDesc).waitRead (inline)
         0     0%   100%          2 40.00%  internal/poll.runtime_pollWait
         0     0%   100%          1 20.00%  main.(*epoll).Wait
(pprof) list flat
Total: 5
```

- 从上面内存占用情况来看还是buf占用内存比较多，接下来就考虑优化buf的使用。go websocket库有2个比较推荐，第一个就是上面一直用的另一个是：[gobwas/ws](https://github.com/gobwas/ws)， gobwas有几个特性：
  - Zero-copy upgrade
  - No intermediate allocations during I/O
  - Low-level API which allows to build your own logic of packet handling and buffers reuse
  - High-level wrappers and helpers around API in `wsutil` package, which allow to start fast without digging the protocol internals

1w个链接3M内存,  5个goroutine

```shell
(pprof) top
Showing nodes accounting for 3328.46kB, 100% of 3328.46kB total
Showing top 10 nodes out of 20
      flat  flat%   sum%        cum   cum%
 1024.12kB 30.77% 30.77%  1024.12kB 30.77%  net.newFD (inline)
 1024.05kB 30.77% 61.54%  1024.05kB 30.77%  net.sockaddrToTCP
  768.26kB 23.08% 84.62%   768.26kB 23.08%  main.(*epoll).Add
  512.03kB 15.38%   100%   512.03kB 15.38%  syscall.anyToSockaddr
         0     0%   100%   512.03kB 15.38%  internal/poll.(*FD).Accept
         0     0%   100%   512.03kB 15.38%  internal/poll.accept
         0     0%   100%  2560.20kB 76.92%  main.main
         0     0%   100%   768.26kB 23.08%  main.wsHandle
         0     0%   100%  2560.20kB 76.92%  net.(*TCPListener).Accept
         0     0%   100%  2560.20kB 76.92%  net.(*TCPListener).accept
(pprof) list flat
Total: 3.25MB

(pprof) top
Showing nodes accounting for 5, 100% of 5 total
Showing top 10 nodes out of 35
      flat  flat%   sum%        cum   cum%
         2 40.00% 40.00%          2 40.00%  runtime.gopark
         1 20.00% 60.00%          1 20.00%  runtime/pprof.writeRuntimeProfile
         1 20.00% 80.00%          1 20.00%  syscall.Syscall
         1 20.00%   100%          1 20.00%  syscall.Syscall6
         0     0%   100%          2 40.00%  internal/poll.(*FD).Accept
         0     0%   100%          1 20.00%  internal/poll.(*FD).Read
         0     0%   100%          2 40.00%  internal/poll.(*pollDesc).wait
         0     0%   100%          2 40.00%  internal/poll.(*pollDesc).waitRead (inline)
         0     0%   100%          2 40.00%  internal/poll.runtime_pollWait
         0     0%   100%          1 20.00%  main.(*epoll).Wait
(pprof) list flat
Total: 5
```

# 如何限流，防止ddos

https://studygolang.com/articles/13254

# 如何实现分布式?

https://learnku.com/articles/39701

https://cloud.tencent.com/developer/article/1509469