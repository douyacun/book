---
Title: gRPC进阶
Keywords: gRPC,pbgo,grpc,权限认证,中间件,拦截器
Description: gRPC是基于http2.0，这里实现证书认证、rpc和RESTful HTTP API共存
Author: douyacun
Cover: grpc-cover.png
Label: pbgo
Date: 2019-10-21 19:17:31
LastEditTime: 2019-10-23 20:33:06
---

[TOC]

# gRPC权限认证

系统设计要满足最低安全标准，尽管平时我们设计的系统不是专门为了高安全性应用而设计的，但必须要用最低标准的加密和身份验证。例如：web服务要加密密码和身份证信息。同样分布式系统也要能加密二进制数据和节点通信数据，进行身份和权限认证。虽然这些事有开销的，但是这些开销相比安全风险来说是值得的。

gRPC本身支持几种授权验证机制：

- SSL/TLS: 
  - SSL/TLS 两端授权认证
  - 加密通信
- Token Authentication

## 创建证书

通过证书加密数据，使用openssl生成

server.key: 私钥

server.crt: 公钥

```shell
$ openssl genrsa -out server.key 2048
$ openssl req -new -x509 -days 3650 \
    -subj "/CN=douyacun.com" \
    -key server.key -out server.crt
```

`-subj` 内的值等价于：

```shell
$ openssl req -new -x509 -sha256 -key server.key -out server.crt -days 3650
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) []:CH
State or Province Name (full name) []:shandong
Locality Name (eg, city) []:laiwu
Organization Name (eg, company) []:douyacun
Organizational Unit Name (eg, section) []:douyacun
Common Name (eg, fully qualified host name) []:douyacun.com
Email Address []:douyacun@gmail.com
```

## 服务端加密

[【源码阅读】](https://github.com/douyacun/foo_go/tree/master/cmd/rpc/tls-auth)

使用gRPC加密通信最简单方式就是服务端加密，服务端需要初始化密钥对(server.key/server.crt)，客户端只需要公钥

```go
// 初始化tcp通道
lis, err := net.Listen("tcp", Port)
if err != nil {
  log.Fatal(err)
}
// 初始化TLS证书
creds, err := credentials.NewServerTLSFromFile("../cert/server.crt", "../cert/server.key")
if err != nil {
  log.Fatal(err)
}
// 创建 gRPC 服务，并配置证书
s := grpc.NewServer(grpc.Creds(creds))
// 注册函数
greeter.RegisterGreeterServer(s, new(server))
// 启动服务
if err := s.Serve(lis); err != nil {
  log.Fatalf("failed to serve: %v", err)
}
```

客户端为了能创建连接，需要获取server.crt(公钥)。

```go
// 初始化TLS证书
creds, err := credentials.NewClientTLSFromFile("../cert/server.crt", "server.douyacun.com")
if err != nil {
  log.Fatal(err)
}
// 拨号通信
conn, err := grpc.Dial("localhost:4000", grpc.WithTransportCredentials(creds))
if err != nil {
  log.Fatal(err)
}
defer conn.Close()
// 初始化客户端接口
client := greeter.NewGreeterClient(conn)
reply, err := client.SayHello(context.Background(), &greeter.HelloRequest{Name: "world"})
if err != nil {
  log.Fatal(err)
}

fmt.Println(reply.GetMessage())
```

## 双向证书验证

[【源码阅读】](https://github.com/douyacun/foo_go/tree/master/cmd/rpc/mutual-tls-auth)

上面的方式需要提前将证书给到客户端，每次变更都需要维护证书，为了避免这种麻烦。可以通过一个安全可靠的根证书分别对服务器和客户端的证书进行签名，这样客户端或服务端在收到对方证书后可以通过根证书验证证书的有效性

生成客户端证书

```shell
$ openssl genrsa -out client.key 2048
$ openssl req -new -x509 -days 3650 \
    -subj "/CN=douyacun.com" \
    -key client.key -out client.crt
```

生成根证书ca

```shell
$ openssl genrsa -out ca.key 2048
$ openssl req -new -x509 -days 3650 \
    -subj "/CN=douyacun.com" \
    -key ca.key -out ca.crt
```

对服务端证书签名

```shell
$ openssl req -new \
    -subj "/CN=douyacun.com" \
    -key server.key \
    -out server.csr
$ openssl x509 -req -sha256 \
    -CA ca.crt -CAkey ca.key -CAcreateserial -days 3650 \
    -in server.csr \
    -out server.crt
```

对客户端证书签名

```shell
$ openssl req -new \
    -subj "/CN=douyacun.com" \
    -key client.key \
    -out client.csr
$ openssl x509 -req -sha256 \
    -CA ca.crt -CAkey ca.key -CAcreateserial -days 3650 \
    -in client.csr \
    -out client.crt
```

服务端

```go
// 从磁盘加载证书
cert, err := tls.LoadX509KeyPair("../cert/server.crt", "../cert/server.key")
if err != nil {
  log.Fatalf("tls load key pair failed, %v",err)
}
certPool := x509.NewCertPool()
ca, err := ioutil.ReadFile("../cert/ca.crt")
if err != nil {
  log.Fatalf("ca.crt read filed %v", err)
}
if ok := certPool.AppendCertsFromPEM(ca); !ok {
  log.Fatal("failed to append client certs")
}
// 初始化tcp通道
lis, err := net.Listen("tcp", Port)
if err != nil {
  log.Fatal(err)
}
// 初始化TLS证书
creds := credentials.NewTLS(&tls.Config{
  ClientAuth:   tls.RequireAndVerifyClientCert,
  Certificates: []tls.Certificate{cert},
  ClientCAs:    certPool,
})
// 创建 gRPC 服务，并配置证书
s := grpc.NewServer(grpc.Creds(creds))
// 注册函数
greeter.RegisterGreeterServer(s, new(server))
// 启动服务
if err := s.Serve(lis); err != nil {
  log.Fatalf("failed to serve: %v", err)
}
```

客户端

```go
// 从磁盘加载证书
certificate, err := tls.LoadX509KeyPair("../cert/client.crt", "../cert/client.key")
if err != nil {
  log.Fatalf("tls load key pair failed, %v", err)
}
certPool := x509.NewCertPool()
ca, err := ioutil.ReadFile("../cert/ca.crt")
if err != nil {
  log.Fatalf("ca read failed, %v", err)
}
if ok := certPool.AppendCertsFromPEM(ca); !ok {
  log.Fatal("cert append pool failed")
}
// 初始化TLS证书
creds := credentials.NewTLS(&tls.Config{
  ServerName:   "douyacun.com", // 对服务端证书签名的配置(/CN = ?)
  Certificates: []tls.Certificate{certificate},
  RootCAs:      certPool,
})
// 拨号通信
conn, err := grpc.Dial("localhost:4000", grpc.WithTransportCredentials(creds))
if err != nil {
  log.Fatal(err)
}
defer conn.Close()
client := greeter.NewGreeterClient(conn)

reply, err := client.SayHello(context.Background(), &greeter.HelloRequest{Name: "world"})
if err != nil {
  log.Fatal(err)
}

fmt.Println(reply.GetMessage())
```

# token认证

[【源码阅读】](https://github.com/douyacun/foo_go/tree/master/cmd/rpc/token)

token认证主要是需要实现credentials.PerRPCCredentials接口

```go
// PerRPCCredentials defines the common interface for the credentials which need to
// attach security information to every RPC (e.g., oauth2).
type PerRPCCredentials interface {
	// GetRequestMetadata gets the current request metadata, refreshing
	// tokens if required. This should be called by the transport layer on
	// each request, and the data should be populated in headers or other
	// context. If a status code is returned, it will be used as the status
	// for the RPC. uri is the URI of the entry point for the request.
	// When supported by the underlying implementation, ctx can be used for
	// timeout and cancellation.
	// TODO(zhaoq): Define the set of the qualified keys instead of leaving
	// it as an arbitrary string.
	GetRequestMetadata(ctx context.Context, uri ...string) (map[string]string, error)
	// RequireTransportSecurity indicates whether the credentials requires
	// transport security.
	RequireTransportSecurity() bool
}
```

实现PerRPCCredentials interface

```go
type Auth struct {
	Account     string
	Password string
}
func (a *Auth) GetRequestMetadata(ctx context.Context, uri ...string) (map[string]string, error) {
	return map[string]string{"account": a.Account, "password": a.Password}, nil
}
func (a *Auth) RequireTransportSecurity() bool  {
	return false
}
```

服务端使用中间件验证,这里集成[go-grpc-middleware](https://github.com/grpc-ecosystem/go-grpc-middleware)

```go
func main() {
	s := grpc.NewServer(grpc.UnaryInterceptor(grpc_middleware.ChainUnaryServer(
		grpc_auth.UnaryServerInterceptor(check),
		grpc_recovery.UnaryServerInterceptor(),
	)))
	greeter.RegisterGreeterServer(s, &server{})
	lis, err := net.Listen("tcp", Port)
	if err != nil {
		log.Fatal(err)
	}
	if err := s.Serve(lis); err != nil {
		log.Fatalf("failed to server: %v", err)
	}
}
func check(ctx context.Context) (context.Context, error){
  // FromIncomingContext 可以获取到 GetRequestMetadata 返回到数据
	md, ok := metadata.FromIncomingContext(ctx)
	if !ok {
		return nil, fmt.Errorf("missing credentials")
	}
	var (
		account, password string
	)
	if val, ok := md["account"]; ok {
		account = val[0]
	}
	if val, ok := md["password"]; !ok {
		password = val[0]
	}
	fmt.Println(account, password)
	return nil, nil
}
```

# gRPC扩展

## 默认值

需要引入扩展选项`import "google/protobuf/descriptor.proto";` 实现默认值

```protobuf
// 这里声明协议版本，默认是proto2, 3和2并不向下兼容
syntax = "proto3";

import "google/protobuf/descriptor.proto";

// 这里指定 go 包名
package greeter;

// 定义默认类型
extend google.protobuf.FieldOptions {
    string default_name = 5000;
}

// 定义要序列化的信息的结构
message HelloRequest {
    // string 字段类型， 也可以自定义字段类型
    string name = 1 [(default_name) = "world"]; // 注意这里1是唯一的， 1-15 占用1个字节， 16 - 2047 占用2个字节
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

##grpc-gateway

安装， 这里最好将GO111MODULE设为auto, `GO111MODULE=auto`,否则下面的安装会安装到`$GOPATH/pkg/mod`下面，不是安装到`$GOPATH/src`下

```go
go get -u github.com/grpc-ecosystem/grpc-gateway/protoc-gen-grpc-gateway
go get -u github.com/grpc-ecosystem/grpc-gateway/protoc-gen-swagger
go get -u github.com/golang/protobuf/protoc-gen-go
```

创建protobuf文件，生成gRPC使用的pb文件，grpc-gateway使用的gw文件

```shell
$ protoc -I/usr/local/include -I. \
  -I$GOPATH/src \
  -I$GOPATH/src/github.com/grpc-ecosystem/grpc-gateway/third_party/googleapis \
  --grpc-gateway_out=. \
  --go_out=plugins=grpc:. \
  hello.proto
```

```protobuf
// 这里声明协议版本，默认是proto2, 3和2并不向下兼容
syntax = "proto3";

// 我的goland这里会提示 file not found
import "google/api/annotations.proto";

// 这里指定 go 包名
package greeter;

// 定义要序列化的信息的结构
message HelloRequest {
    // string 字段类型， 也可以自定义字段类型
    string name = 1; // 注意这里1是唯一的， 1-15 占用1个字节， 16 - 2047 占用2个字节
}

// 同一个proto文件中可以定义多个message type
message HelloReply {
    string message = 1;
}

// 定义接口
service Greeter {
    //    rpc SayHello (HelloRequest) returns (HelloReply);
    rpc Echo (HelloRequest) returns (HelloReply) {
        option (google.api.http) = {
            get : "/echo/{name}"
        };
    }
}
```

创建httpserver文件

```go
ctx := context.Background()
ctx, cancel := context.WithCancel(ctx)
defer cancel()

// Register gRPC server endpoint
// Note: Make sure the gRPC server is running properly and accessible
mux := runtime.NewServeMux()
err := greeter.RegisterGreeterHandlerFromEndpoint(ctx, mux,  "localhost:4000", []grpc.DialOption{grpc.WithInsecure()})
if err != nil {
  log.Fatal(err)
}

// Start HTTP server (and proxy calls to gRPC server endpoint)
err = http.ListenAndServe(":8081", mux)
if err != nil {
  log.Fatal(err)
}
```

创建rpcserver文件，这里还是和上面的rpc server启动方式一样

```go
s := grpc.NewServer()
greeter.RegisterGreeterServer(s, new(server))
lis, err := net.Listen("tcp", Port)
if err != nil {
  log.Fatal(err)
}
if err := s.Serve(lis); err != nil {
  log.Fatalf("failed to serve: %v", err)
}
```

先启动rpcserver，然后启动httpserver，通过curl请求

```shell
$ curl -X GET "http://localhost:8081/echo/douyacun"
{"message":"hello "}%
```

# 推荐阅读

- https://bbengfort.github.io/programmer/2017/03/03/secure-grpc.html