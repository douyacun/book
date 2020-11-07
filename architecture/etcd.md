---
Title: etcd应用场景
Keywords: etcd应用场景,服务发现,配置中心,分布式锁
Description: 已实现 grpc负载均衡、服务发现、分布式锁、配置中心、
Label: etcd
Date: 2020-09-19 21:03:21
LastEditTime: 2020-11-07 21:53:21
---

Etcd几个关键特征：

**raft算法**：Leader选举/操作同步，无论访问任意节点，都将获得最终一致性数据，高度可靠。

**mvcc**:  etcd有2级存储结构 [B树内存索引](https://github.com/google/btree)  / [bbolt B+树](https://github.com/etcd-io/bbolt) ，内存中使用btree维护key/value索引，节点中存的是bbolt中键值K，通过这个k在bbolt查找，得到的才是用户传进去的value值

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

type keyIndex struct {
	key         []byte
	modified    revision // the main rev of the last modification
	generations []generation
}
```

内存索引中，每个原始key会关联一个key_index，里面维护了多版本信息

**事务**: 

- 只读事务
- 读写事务



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

**版本兼容:**

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

- 动态加载配置（watch）
- 历史版本（revision）
- 权限控制
- 命名空间

思路:

1. 加载配置文件 put etcd
2. watch prefix 配置变更可以及时变更配置

相关代码已经发到 github : [etcd配置中心](https://github.com/douyacun/etcd/)

# etcd 分布式锁

原理:

1. 租约创建一个key
2. key不存在，创建key，成功获取到锁
3. key存在，无法创建key，获取锁失败

etcd分布式锁的实现在`go.etcd.io/etcd/clientv3/concurrency`包中

1. 初始化sessions

```go
func NewSession(client *v3.Client, opts ...SessionOption) (*Session, error) {
	ops := &sessionOptions{ttl: defaultSessionTTL, ctx: client.Ctx()}
	for _, opt := range opts {
		opt(ops)
	}
	// 初始化租约
	id := ops.leaseID
	if id == v3.NoLease {
		resp, err := client.Grant(ops.ctx, int64(ops.ttl))
		if err != nil {
			return nil, err
		}
		id = v3.LeaseID(resp.ID)
	}
	// 由上层调用控制是否中断
	ctx, cancel := context.WithCancel(ops.ctx)
	keepAlive, err := client.KeepAlive(ctx, id)
	if err != nil || keepAlive == nil {
		cancel()
		return nil, err
	}
	donec := make(chan struct{})
	s := &Session{client: client, opts: ops, id: id, cancel: cancel, donec: donec}
	go func() {
		defer close(donec)
		for range keepAlive {
			// eat messages until keep alive channel closes
		}
	}()
	return s, nil
}
```

2. `func NewMutex(s *Session, pfx string) *Mutex `

初始化`Mutex`

3. `func (m *Mutex) Lock(ctx context.Context) error`

这里类似redis的分布式锁: `set key value NX EX 60`  如果key存在就返回，否则就设置

```go
func (m *Mutex) Lock(ctx context.Context) error {
	s := m.s
	client := m.s.Client()
  // 伪代码
  // if !m.myKey {
  //	client.Put(m.myKey, value)
  // } else {
  // 	client.Get(m.myKey)
	// }
	m.myKey = fmt.Sprintf("%s%x", m.pfx, s.Lease())
	cmp := v3.Compare(v3.CreateRevision(m.myKey), "=", 0)
	put := v3.OpPut(m.myKey, "", v3.WithLease(s.Lease()))
	get := v3.OpGet(m.myKey)
	getOwner := v3.OpGet(m.pfx, v3.WithFirstCreate()...)
	resp, err := client.Txn(ctx).If(cmp).Then(put, getOwner).Else(get, getOwner).Commit()
	if err != nil {
		return err
	}
 	// 此锁是否为自己获得
	m.myRev = resp.Header.Revision
	if !resp.Succeeded {
		m.myRev = resp.Responses[0].GetResponseRange().Kvs[0].CreateRevision
	}
	ownerKey := resp.Responses[1].GetResponseRange().Kvs
	if len(ownerKey) == 0 || ownerKey[0].CreateRevision == m.myRev {
		m.hdr = resp.Header
		return nil
	}
	// 阻塞直到获取到该锁
	hdr, werr := waitDeletes(ctx, client, m.pfx, m.myRev-1)
	if werr != nil {
		m.Unlock(client.Ctx())
	} else {
		m.hdr = hdr
	}
	return werr
}
```

4. `func (m *Mutex) Unlock(ctx context.Context) error` 删除 key

# 推荐阅读

- [高可用分布式存储 etcd 的实现原理](https://draveness.me/etcd-introduction/)
- [etcd使用经验总结](https://alexstocks.github.io/html/etcd.html)



