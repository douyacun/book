---
Title: kafka生产环境配置
Keywords: kafka,配置,分区策略,压缩算法
Descriptions: kafka如果保证消息不丢失？
Date: 2020-12-07 10:50:00
LastEditTime: 2020-12-07 10:50:00
---

[toc]

# 安装

**zookeeper** 下载地址: [http://zookeeper.apache.org/releases.html#download](http://zookeeper.apache.org/releases.html#download)

解压：`tar -zxvf kafka_2.12-2.6.0.tgz`

运行：`./bin/zkServer.sh --config conf start`

> --config 指的是配置目录



**kafka** 下载地址: [https://kafka.apache.org/downloads](https://kafka.apache.org/downloads)

解压：`tar -zxvf kafka_2.12-2.6.0.tgz`

运行：`./bin/kafka-server-start.sh config/server.properties`

# 配置

**kafka配置：**

`kakfa-configs.sh` 可以对kafka配置查看或修改

别名： 每次敲这么多命令很烦

`alias kconfigs="~/Documents/kafka/kafka/bin/kafka-configs.sh --bootstrap-server localhost:9092"`

查看:  --describe

`kconfigs --describe --entity-type [topics/clients/users/brokers/broker-loggers] --all`

- brokers
  - log.dirs：指定文件存储目录, 多个逗号分隔，建议目录指定不同的磁盘
    - 提升读写性能，多个磁盘同时读写的性能更高
    - 故障转移：Failover 1.1 版本添加
  - listeners：通过什么协议访问kafka服务，内网服务指定
  - advertised.listeners：外网访问kafka服务指定
  - log.retension.hours=168：记录保留时长 默认7天（hours/minutes/ms）
  - log.retension.bytes=1073741824：可用磁盘大小 默认 128G， -1不限制
  - zookeeper.connect=localhost:2181：zookeeper连接地址
  - auto.create.topics.enable: false topic不存在时是否自动创建（建议显示配置false）
  - unclean.leader.election.enblae: false 允许落后的副本参与leader选举（建议显示配置false）
  - auto.leader.rebalance.enable: false 定期leader选举，正常服务的leader到期也会被替换掉（建议显示配置false）
- topics
  - retension.hours 保留时长
  - retension.bytes 为该Topic预留多大的磁盘空间，-1无限使用磁盘空间
- users
- clients
- broker-loggers

修改：--alter

`kconfigs --alter --entity-type topics --entity-name test --add-config max.message.bytes=104858800`

- --alter：代表修改
- --entity-type: 代表上面这5中实例
- --entity-name: 指定那个broker（broker.id）, topic (topic.name)
- --add-config: --help 可以查看支持实时修改的配置

**JVM配置**：

- 堆大小(heap size)配置：建议6G  环境变量
  - `export KAFKA_HEAP_OPTS=--Xms6g  --Xmx6g`
- 垃圾回收：G1收集器 
  - `export  KAFKA_JVM_PERFORMANCE_OPTS= -server -XX:+UseG1GC -XX:MaxGCPauseMillis=20 -XX:InitiatingHeapOccupancyPercent=35 -XX:+ExplicitGCInvokesConcurrent -Djava.awt.headless=true`

**系统配置**:

- 文件描述符限制：`ulimit -n`
  - `/etc/security/limits.conf` 
    - `soft nofile 102400`
    - `hard nofile 104800`
- 文件系统类型： xfs 性能较好
- swap内存：建议配置成1，方便报警
- 脏页刷新时间：默认5秒

- 磁盘大小
  - 新增消息数
  - 消息留存时间
  - 平均消息大小
  - 备份数
  - 是否启用压缩

# 本地集群

本地运行多个实例，需要指定不同的端口，和broker.id, 创建多个server.propertie，指定端口

```
config/server-1.properties:
  broker.id=1
  listeners=PLAINTEXT://:9093
  log.dir="～/Documents/kafka/data1"
```

```
config/server-2.properties:
  broker.id=2
  listeners=PLAINTEXT://:9094
  log.dir="~/Documents/kafka/data2"
```

启动：

```shell
./bin/zookeeper-server-start.sh -daemon config/zookeeper.properties 
./bin/kafka-server-start.sh -daemon config/server.properties
./bin/kafka-server-start.sh -daemon config/server1.properties 
./bin/kafka-server-start.sh -daemon config/server2.properties 
```

创建：3副本/1分区 topic

> alias ktopics="~/Documents/kafka/bin/kafka-topics --bootstrap-server localhost:9092,localhost:9093,localhost:9094"

```shell
ktopics --create --replication-factor 3 --partitions 1 --topic test
```

查看：topic状态

```shell
ktopics --descibe --topic test

Topic: test	PartitionCount: 1	ReplicationFactor: 3	Configs: segment.bytes=1073741824
	Topic: test	Partition: 0	Leader: 3	Replicas: 2,3,1	Isr: 3,1,2
```

# kafka 分区策略 & 压缩算法

分区策略是决定生产者将消息发送到哪个分区的算法

常见的分区策略:

1. 轮询策略，能保证消息尽可能分配到不同的分区上 
2. 随机策略，随机返回小于partition的数，不能保证均匀性，适用及其性能不一的情形
3. hash策略，具有相同key的消息会保证在同一个分区中，这样可以保证消息的前后顺序

go语言包：[github.com/Shopify/sarama](https://pkg.go.dev/github.com/Shopify/sarama)  提供了这3种分区策略的实现

**轮询:** `func NewRoundRobinPartitioner(topic string) Partitioner`

**随机:** `func NewRandomPartitioner(topic string) Partitioner`

**hash:** `func NewHashPartitioner(topic string) Partitioner`

**指定分区** `func NewManualPartitioner(topic string) Partitioner`



压缩：

保证Producer 和 Consumer 压缩算法一致

```go
const (
	//CompressionNone no compression
	CompressionNone CompressionCodec = iota
	//CompressionGZIP compression using GZIP
	CompressionGZIP
	//CompressionSnappy compression using snappy
	CompressionSnappy
	//CompressionLZ4 compression using LZ4
	CompressionLZ4
	//CompressionZSTD compression using ZSTD
	CompressionZSTD
)
```

吞吐量： LZ4 > Snappy > zstd > GZIP

压缩比：zstd > LZ4 > GZIP > Snappy

CPU:  各算法差不多压缩时 Snappy 算法使用的 CPU 较多, 解压缩时 GZIP 算法可能使用更多的 CPU



# FAQs

**如何保证消息不丢失?**

producer: 

1. 消息发送异常处理, [sarama](https://github.com/Shopify/sarama)提供了2种发送消息的方式，sync/async，同步如果发送失败会返回error，异步提供了 `Errors() <-chan *ProducerError` 接口来处理错误
2. 消息发送异常重试, `config.Producer.Retry`, 默认是 3次
3. `config.Producer.RequiredAcks=WaitForAll` 所有副本都接受到消息，该消息才算做提交
   1. `NoResponse RequiredAcks = 0`
   2. `WaitForLocal RequiredAcks = 1`
   3. `WaitForAll RequiredAcks = -1`

broker:

1. `unclean.leader.election.enable=false` 控制落后台太多的broker不参加leader选举
2. `replication.factor >= 3` 副本数量不要少于3份
3. `min.insync.replicas >= 1` 消息最少写入多少个副本才算做已提交，正式环境中建议>=3
4. `replication.factor > min.insync.replicas`  如果2着相等，只要有1个副本挂掉，整个分区就无法工作了

consumer:

1. 先消费，在更新位移, `cfg.Consumer.Offsets.AutoCommit.Enable = false` 默认是自动提交的

