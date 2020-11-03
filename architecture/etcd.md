---
Title: etcd新手教程
Keywords: etcd应用场景
Description: etcd
Label: etcd
Date: 2020-09-19 21:03:21
LastEditTime: 2020-09-19 21:03:21
---

Etcd几个关键特征：

**raft算法**：Leader选举/操作同步，无论访问任意节点，都将获得最终一致性数据，高度可靠。

**mvcc**:  etcd有2级存储结构 [B树内存索引](https://github.com/google/btree)  / [bbolt B+树](https://github.com/etcd-io/bbolt)

- 每个事务有唯一事务id，就是下面的main id int64
- 一个事务可以包含多个修改操作（put/delete），每一个操作就是一个revision, 共享同一个main id
- 一个事务内多个连续修改操作会从0开始递增，编号 sub id int64

```
type revision struct {
	// main is the main revision of a set of changes that happen atomically.
	main int64

	// sub is the sub revision of a change in a set of changes that happen
	// atomically. Each change has different increasing sub revision in that
	// set.
	sub int64
}
```



# 概要

- etcd集群

- 配置UI管理界面

- 监控与维护

- 容灾

#### 应用场景

- 服务发现
- 配置中心
- 分布式锁
- 监控机器活动

#### 基础功能

- 监控 watch
- 租约 lease
- 全局锁 lock
- 选举 elect
- 集群状态 
  - `etcdctl --write-out=table endpoint status`
  - `etcdctl endpoint health`
- 快照 snapshot
  - `etcdctl --endpoints=127.0.0.1:2379 snapshot save my.db`
  - `etcdctl --endpoints=127.0.0.1:2379 --write-out=table snapshot status my.db`
- 迁移 migrate
- 成员管理 member
  - `etcdctl --endpoints=127.0.0.1:2379 member list`

- 授权 auth



# 集群配置

`docker-compose.yml`

```yaml
services:
  etcd1:
    image: 'bitnami/etcd:latest'
    environment:
      - ALLOW_NONE_AUTHENTICATION=yes
      - ETCD_NAME=etcd1
      - ETCD_INITIAL_ADVERTISE_PEER_URLS=http://etcd1:2380
      - ETCD_LISTEN_PEER_URLS=http://0.0.0.0:2380
      - ETCD_LISTEN_CLIENT_URLS=http://0.0.0.0:2379
      - ETCD_ADVERTISE_CLIENT_URLS=http://etcd1:2379
      - ETCD_INITIAL_CLUSTER_TOKEN=etcd-cluster
      - ETCD_INITIAL_CLUSTER=etcd1=http://etcd1:2380,etcd2=http://etcd2:2380,etcd3=http://etcd3:2380
      - ETCD_INITIAL_CLUSTER_STATE=new
    networks:
      - app-tier
  etcd2:
    image: 'bitnami/etcd:latest'
    environment:
      - ALLOW_NONE_AUTHENTICATION=yes
      - ETCD_NAME=etcd2
      - ETCD_INITIAL_ADVERTISE_PEER_URLS=http://etcd2:2380
      - ETCD_LISTEN_PEER_URLS=http://0.0.0.0:2380
      - ETCD_LISTEN_CLIENT_URLS=http://0.0.0.0:2379
      - ETCD_ADVERTISE_CLIENT_URLS=http://etcd1:2379
      - ETCD_INITIAL_CLUSTER_TOKEN=etcd-cluster
      - ETCD_INITIAL_CLUSTER=etcd1=http://etcd1:2380,etcd2=http://etcd2:2380,etcd3=http://etcd3:2380
      - ETCD_INITIAL_CLUSTER_STATE=new
    networks:
      - app-tier
  etcd3:
    image: 'bitnami/etcd:latest'
    environment:
      - ALLOW_NONE_AUTHENTICATION=yes
      - ETCD_NAME=etcd3
      - ETCD_INITIAL_ADVERTISE_PEER_URLS=http://etcd3:2380
      - ETCD_LISTEN_PEER_URLS=http://0.0.0.0:2380
      - ETCD_LISTEN_CLIENT_URLS=http://0.0.0.0:2379
      - ETCD_ADVERTISE_CLIENT_URLS=http://etcd1:2379
      - ETCD_INITIAL_CLUSTER_TOKEN=etcd-cluster
      - ETCD_INITIAL_CLUSTER=etcd1=http://etcd1:2380,etcd2=http://etcd2:2380,etcd3=http://etcd3:2380
      - ETCD_INITIAL_CLUSTER_STATE=new
    networks:
      - app-tier
  etcdkeeper:
    image: deltaprojects/etcdkeeper:latest
    ports:
      - 8080:8080
    networks:
      - app-tier
```

etcdkeeper 连接etcd: `etcd1:2379` ，它们处在同一个network中直接使用服务名就可以

# 服务注册与服务发现

**服务注册：**

1. 申请lease租约，设置服务生存周期ttl，让key自动过期
2. 服务正常状态下，通过keep-alive定期去租约，避免过期
3. key有统一前缀，比如：`etcdctl --endpoints=127.0.0.1:2379 put /sdmaster-service/192.168.31.110   192.168.31.110`

**服务发现：**

1. 服务`watch /sdmaster-service --prefix`
2. `sdmaster-service` 下发生变化都会发生通知

相关代码已经发到github [discovery](https://github.com/douyacun/discovery)

**版本兼容**

1. 正常安装`github.com/coreos/etcd/clientv3`会存在 `module declares its path as: go.etcd.io/bbolt`

```
go: discovery/discovery-service-protocol/client imports
        github.com/coreos/etcd/clientv3 tested by
        github.com/coreos/etcd/clientv3.test imports
        github.com/coreos/etcd/auth imports
        github.com/coreos/etcd/mvcc/backend imports
        github.com/coreos/bbolt: github.com/coreos/bbolt@v1.3.5: parsing go.mod:
        module declares its path as: go.etcd.io/bbolt
                but was required as: github.com/coreos/bbolt
```

解决方法：

`replace github.com/coreos/bbolt latest => go.etcd.io/bbolt latest`

2. 如果使用grpc的话，会存在grpc版本和etcd不兼容的问题

使用最先版本grpc, 会报`google.golang.org/grpc/naming`缺失

`module google.golang.org/grpc@latest found (v1.33.1), but does not contain package google.golang.org/grpc/naming`

如果降低到1.29.0可以解决这个问题，如果运行代码会报

`undefined: resolver.BuildOption` 

`undefined: resolver.ResolveNowOption`

这是因为grpc版本和etcd不兼容的问题，需要降低版本到 `1.26.0` 以下

解决方法:

```
replace google.golang.org/grpc latest => google.golang.org/grpc v1.26.0
```

3. 目前 protobuf grpc 生成的go代码, 会报错 `undefined grpc.ClientConnInterface` `undefined: grpc.SupportPackageIsVersion6`

```
go get -d -u github.com/golang/protobuf/protoc-gen-go
git -C "$(go env GOPATH)"/src/github.com/golang/protobuf checkout $GIT_TAG
go install github.com/golang/protobuf/protoc-gen-go
```

# go-grpc 负载均衡

正常http负载均衡策略，client -> proxy gateway load balance -> service

grpc提供负载均衡策略，client  load balance -> service 

grpc关键点是负载均衡在客户端

1. 服务端启动时，首先将服务地址注册到服务注册表，定期keepalive健康检测
2. 客户端访问某个服务，watch订阅服务注册表，然后以某种负载均衡策略选择一个目标地址

![](./assert/grpc-load-balancing.png)

相关代码已发到github: [go-grpc 负载均衡](https://github.com/douyacun/discovery)

# etcd 配置中心

1. 配置集中管理、统一标准
2. 配置与应用分离
3. 实时更新
4. 高可用
5. 安全



# etcd 分布式锁

原理:

1. 租约创建一个key
2. key不存在，创建key，成功获取到锁
3. key存在，无法创建key，获取锁失败

etcd分布式锁的实现在`go.etcd.io/etcd/clientv3/concurrency`包中



`func NewSession(client *v3.Client, opts ...SessionOption) (*Session, error)` 

1. 初始化租约，keepalive 保证 互斥锁一直持有
2. 注册关闭信道

> 小技巧
>
> 1. 匿名函数 withoption 配置
> 2. context.WithCancel 可由上层调用控制是否中断
> 3. donec := make(chan struct, 0) 支持阻塞式调用



`func NewMutex(s *Session, pfx string) *Mutex `

新建一个`Mutex`





# 推荐阅读

- [高可用分布式存储 etcd 的实现原理](https://draveness.me/etcd-introduction/)
- [etcd使用经验总结](https://alexstocks.github.io/html/etcd.html)



