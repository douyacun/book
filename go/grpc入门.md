---
Title: gRPC入门
Keywords: gRPC,rpc,protoc,发布订阅模式,pub/sub
Description: 什么是rpc，rpc和http的区别，为什么要用rpc
Author: douyacun
Cover: assert/grpc-cover.png
Label: gRPC
Date: 2019-10-16 17:38:36
LastEditTime: 2019-11-15 18:07:38
---

[TOC]

# 什么是rpc，rpc和http的区别，为什么要用rpc

访问量加大以后，为了解决用户访问卡顿的问题，我们就上集群架设nginx部署多个服务，由nginx负载均衡把请求转发到其他服务上。过一段时间发现发现数据库扛不住了，应用服务完好，这时候我们就上数据库读写分离，再架设几台数据库服务器，做主从，分库分表。服务访问量越来越高，项目功能也越来越多，整个项目也愈发庞大。修改一个类就需要全盘上传，切换nginx重启，分布流程越来越长，也越来越复杂。我们开始把模块拆分，用户系统、订单系统、评论系统...，这样就达到用户模块代码修改的时候只需要修改用户信息服务。但是还是需要切换顶层的nginx，把需要重启的服务的流量切到可用服务上。这个时候rpc登场了，所有服务启动的时候注册到一个注册机里面，然后顶层处理在接收到nginx请求后，去注册机里找一个可用的服务并调用接口。

用户和订单肯定是分不开的，分离成为两个系统不同的主机不同的进程以后如何进行交互，可以通过http或者rpc，http的优点是：简单、通用、开发方便。相较http rpc的优点是：长连接（不必像http 3次握手）、rpc一般都有注册中心，有丰富的监控管理；

一般来说，RPC服务主要是针对大型企业的，而HTTP服务主要是针对小企业的，因为RPC效率更高，而HTTP服务开发迭代会更快。

# 前期准备

## 安装 Protocol Buffers v3 工具
从 [https://github.com/google/protobuf/releases](https://github.com/google/protobuf/releases) 下载工具
1. 解压
2. bin/protoc 添加环境变量

## 安装 protoc 插件 protoc-gen-go
```shell
go get -u github.com/golang/protobuf/protoc-gen-go
```

# c & s 基本服务与请求

[【源代码】](https://github.com/douyacun/foo_go/tree/master/cmd/rpc/cs)

protoc 是 [Protocol Buffers](https://developers.google.com/protocol-buffers/docs/overview) 的一个工具，负责生成协议的序列化、反序列化相应语言的代码，看个例子:

首先定义proto协议文件：
```protobuf
// 这里声明协议版本，默认是proto2, 不向下兼容
syntax = "proto3";

// 这里指定 go 包名
package greeter;

// 定义要序列化的信息的结构
message HelloRequest {
    // string 字段类型， 也可以自定义字段类型
    string name = 1; // 注意这里1是唯一的， 1-15 占用1个字节， 16 - 2047 占用2个字节
    // 预留字段
    reserved "foo", "bar";
}
// 同一个proto文件中可以定义多个message type
message HelloReply {
    string message = 1;
}
// 定义接口
service Greeter {
    rpc SayHello (HelloRequest) returns (HelloReply);
}
```
具体的字段类型个语言对照可以查看[https://developers.google.com/protocol-buffers/docs/proto3#scalar](https://developers.google.com/protocol-buffers/docs/proto3#scalar)


编译grpc生成序列化和反序列化代码
```shell
protoc --go_out=plugins=grpc:. hello.proto
```

gRPC插件会为服务端和客户端生成不同的接口：
```go
//client api
type GreeterClient interface {
	SayHello(ctx context.Context, in *HelloRequest, opts ...grpc.CallOption) (*HelloReply, error)
}

// server API
type GreeterServer interface {
	SayHello(context.Context, *HelloRequest) (*HelloReply, error)
}
```

## server

1. 定义结构体，实现服务端接口
2. 注册rpc 服务
3. 启用tcp服务，接受一个链接启用一个goroutine，完成http/2握手，接收gRPC请求，调用注册函数

```go
const Port = ":4000"
// 定义结构体
type server struct{}
// 实现 service greeter 定义的接口
func (p *server) SayHello(ctx context.Context, req *greeter.HelloRequest) (*greeter.HelloReply, error) {
	reply := &greeter.HelloReply{
		Message:              "hello " + req.GetName(),
	}
	return reply, nil
}
func main() {
  // 注册rpc server
	greeter.RegisterGreeterServer(s, new(server))
	lis, err := net.Listen("tcp", Port)
	if err != nil {
		log.Fatal(err)
	}
	if err := grpc.NewServer().Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
```

## client

1. 连接服务器，`grpc.WithInsecure` 不需要权限验证
2. 初始化客户端，gRPC自动生成的客户端
3. 调用api

```go
func main() {
	conn, err := grpc.Dial("localhost:4000", grpc.WithInsecure())
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()
	client := greeter.NewGreeterClient(conn)
	if err != nil {
		log.Fatal(err)
	}
	reply, err := client.SayHello(context.Background(), &greeter.HelloRequest{Name: "world"})
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(reply.GetMessage())
}
```

# stream 简单聊天室

[【源代码】](https://github.com/douyacun/foo_go/tree/master/cmd/rpc/stream)

定义protobuf

```protobuf
// 定义接口
service Greeter {
		...
    // 声明关键字stream，这里可以只有参数使用或者返回值使用stream
    rpc RouteChat (stream HelloRequest) returns (stream HelloReply);
}
```

## server

```go
type Greeter_RouteChatServer interface {
	Send(*HelloReply) error
	Recv() (*HelloRequest, error)
	grpc.ServerStream
}
```

greeter.Greeter_RouteChatServer 接口,有Send/Recv两个方法相当于write/read，当前stream要当作单个connection，接下来的操作就是对stream的读写操作了

```go
func (p *server) RouteChat(stream greeter.Greeter_RouteChatServer) error {
	for {
    // Recv 返回前面定义 HelloRequest 结构体
		request, err := stream.Recv()
		if err != nil {
      // EOF 表示客户端结束了连接
			if err == io.EOF {
				return nil
			}
			return err
		}
		err = stream.Send(&greeter.HelloReply{Message: "hello " + request.GetName()})
		if err != nil {
			return err
		}
	}
}

```

## client

同服务端建立长连接，间隔1秒发送一次请求

```go
func main() {
	conn, err := grpc.Dial("localhost:4000", grpc.WithInsecure())
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()
	client := greeter.NewGreeterClient(conn)
	stream, err := client.RouteChat(context.Background())
	if err != nil {
		log.Fatal(stream)
	}
	go func() {
		for{
			if err := stream.Send(&greeter.HelloRequest{Name: "world"}); err != nil {
				log.Fatal(err)
			}
			time.Sleep(time.Second)
		}
	}()
	for {
		reply, err := stream.Recv()
		if err != nil {
			if err == io.EOF {
				break
			}
			log.Fatal(err)
		}
		fmt.Println(reply.GetMessage())
	}
}
```

# 发布订阅模式

[【源码阅读】](https://github.com/douyacun/foo_go/tree/master/cmd/rpc/pubsub)

找个优秀的包，godoc搜索 pubsub https://godoc.org/?q=pubsub 看到docker的pubsub引用最多，而且和第二名差距甚大，决定使用docker提供的pubsub

本地使用发布订阅：

```go
p := pubsub.NewPublisher(100*time.Millisecond, 10)
c := p.Subscribe()

p.Publish("hi")

msg := <-c
fmt.Println(msg)
```

使用grpc 实现 发布订阅模式， 创建protobuf文件

```protobuf
syntax = "proto3";

package douyacun;

message Topic {
    string name = 1;
}
message PubsubMessage {
    bytes data = 1;
}
message PublishRequest {
    Topic topic = 1;
    PubsubMessage messages = 2;
}
message PublishResponse {
    uint64 message_id = 1;
}
service Publisher {
    rpc Publish (PublishRequest) returns (PublishResponse);
    rpc Subscribe (Topic) returns (stream PubsubMessage);
}
```

## server

```go
type server struct {
	Pub *pubsub.Publisher
}

func (s *server) Publish(c context.Context, pub *douyacun.PublishRequest) (*douyacun.PublishResponse, error) {
	s.Pub.Publish(pub)
	return &douyacun.PublishResponse{MessageId: 1}, nil
}

func (s *server) Subscribe(req *douyacun.Topic, stream douyacun.Publisher_SubscribeServer) error {
	ch := s.Pub.SubscribeTopic(func(v interface{}) bool {
    // 订阅全部消息
		if req.GetName() == "" {
			return true
		}
		if it, ok := v.(*douyacun.PublishRequest); ok {
			if it.Topic.GetName() == req.GetName() {
				return true
			}
		}
		return false
	})
	for v := range ch {
		if pub, ok := v.(*douyacun.PublishRequest); ok {
			if err := stream.Send(pub.GetMessages()); err != nil {
				return err
			}
		}
	}
	return nil
}
```

## client

### 订阅

这样就可以多启动几个订阅进程

```go
cc, err := grpc.Dial(":12345", grpc.WithInsecure())
if err != nil {
  log.Fatalf("grpc.Dial 错误：%v", err)
}
client := douyacun.NewPublisherClient(cc)
stream, err := client.Subscribe(context.Background(), &douyacun.Topic{Name: "golang"})
if err != nil {
  log.Fatalf("client.Subscribe 错误: %v", err)
}
for {
  msg, err := stream.Recv()
  if err != nil {
    if err == io.EOF {
      return
    }
    log.Fatal(err)
  }
  fmt.Printf("新消息：%s\n", msg.String())
}
```

### 发布

```go
cc, err := grpc.Dial(":12345", grpc.WithInsecure())
if err != nil {
  log.Fatal(err)
}
client := douyacun.NewPublisherClient(cc)
for {
  resp, err := client.Publish(context.Background(), &douyacun.PublishRequest{
    Topic: &douyacun.Topic{
      Name: "golang",
    },
    Messages: &douyacun.PubsubMessage{
      Data: []byte("welcome!"),
    },
  })
  if err != nil {
    log.Fatal(err)
  }
  fmt.Println(resp.MessageId)
  time.Sleep(time.Second)
}
```

