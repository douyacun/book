---
Title: etcd新手教程
Keywords: etcd应用场景
Description: etcd
Label: etcd
Date: 2020-09-19 21:03:21
LastEditTime: 2020-09-19 21:03:21
---

# 概要

- etcd集群

- 配置UI管理界面

- 监控与维护

- 容灾

etcd 的应用场景：

- 服务发现
- 配置管理
- 分布式锁
- 监控机器活动



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

