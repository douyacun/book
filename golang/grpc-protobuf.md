---
Title: "grpc proto插件使用,多目录(多包)编译"
Keywords: ""
Description: ""
Cover: "assert/go-protobuf.png"
---

## 概要

- Q&A
- Goland配置支持proto
- 每个微服务单独一个目录
- 插件
    - gogofaster
    - swagger

### Q&A

1. 如何安装protobuf 以及相应的依赖

protoc 是protobuf提供的命令行工具

通过 [github protoc release](https://github.com/protocolbuffers/protobuf/releases/tag/v3.17.3) 下载

mac 可以通过 `brew install protobuf`

go 语言使用protobuf 需要安装插件： `go install google.golang.org/protobuf/cmd/protoc-gen-go`

go 语言推荐安装插件：`go get github.com/gogo/protobuf/protoc-gen-gogofaster`

> gogofast 可以减少指针的使用，减少垃圾回收的时间
>
> Fields without pointers cause less time in the garbage collector. More code generation results in more convenient methods.

2. grpc和protobuf是什么关系，如何配合使用

protobuf 是协议定义的是结构体，方便不同语言之间的参数转换。

grpc是网络通信框架，可以用json、thrift、protobuf作为结构体

grpc不是go语言专有，go语言使用grpc需要安装:  `go get -u google.golang.org/grpc`

go语言grpc配合protobuf使用，需要安装protobuf插件: `go install google.golang.org/grpc/cmd/protoc-gen-go-grpc`

如何编译protobuf后面说。

3. 如何同时提供grpc、http接口

request body 转pb

