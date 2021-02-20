---
Title: "kafka监控"
Keywords: "kafka,监控,ISR"
Description: "jmx prometheus grafana 监控kafka"
Label: "kafka监控"
---

看了一下其他kafka监控文章都没有自信说明kafka各项指标，指标含义，以及如何收集，再次自信整理了一下kafka监控指标、含义以及如何收集：

- broker度量指标
    - ISR （in-sync replication） 未出在未复制的分区数量
    - 活跃控制器数量
    - 请求处理器空闲率 
- partition度量指标
    - 分区数量
    - 首领分区数量
    - 离线分区数量
    - 分区占用磁盘大小
    - 分区文件数量
    - 分区消息最大偏移量
    - 分区消息最小偏移量
- topic度量指标
    - 主题流入字节速率
    - 主题流出字节速率
    - 主题流入消息速率
    - 主题流出消息速率
- 生产者度量指标
    - 每秒钟消息的字节数
    - 每秒钟消息的数量
    - 每秒钟生产者发送给 broker 的请求数
    - 单个请求里所包含的消息平均个数
    - 发送一个生产者请求到 broker 所需要 的平均时间
    - 消息发送给kafka之前在生产者客户端等待的平均毫秒数
- 消费者度量指标
    - 从消费者向 broker 发送请求所 需要的时间
    - 消费者每秒发出请求的数量
    - 请求的平均字节数
    - 每个请求的平均消息个数
    - 每秒读取的消息字节数
    - 每秒读取的消息个数
- 操作系统

    - CPU
    - 网络输入吞吐量
    - 网络输出吞吐量
    - 磁盘平均等待时间
    - 磁盘使用百分比




grafana dashboards

https://grafana.com/grafana/dashboards/10973

# jmx-exporter 指标收集

prometheus 提供了 jmx_exporter 来收集 kafka 指标

Github：[https://github.com/prometheus/jmx_exporter](https://github.com/prometheus/jmx_exporter)

配置文件：[https://github.com/prometheus/jmx_exporter/blob/master/example_configs/kafka-2_0_0.yml](https://github.com/prometheus/jmx_exporter/blob/master/example_configs/kafka-2_0_0.yml)

配合kafka启动脚本，修改 `bin/kafka_server_start.sh`，添加常量，只需要在一个实例中添加即可

```shell
export KAFKA_OPTS="-javaagent:$base_dir/jmx_prometheus_javaagent-0.15.0.jar=7071:$conf_dir/kafka-2_0_0.yml"
```

prometheus targets 收集 metrics

```yaml
static_configs:
  - targets: ['127.0.0.1:7071']
```

> 如果是docker启动的prometheus，而kafka是宿主机启动的话

```yaml
static_configs:
  - targets: ['docker.for.mac.host.internal:7071']
```

# broker 度量指标

**under replicated partitions : 未复制分区数量**

> kafka.server:type=RelicaManager,name=UnderReplicatedPartitions

如果说 broker 只有一个可监控的度量指标，那么它一定是指同步分区的数量（ISR）。

该 topic 下的 partition，其中副本处于失效或者失败的比率。失败或者失效是指副本不处于 ISR 队列中。目前控制副本是否处于 ISR 中由 **replica.log.max.ms** 这个参数控制。

Kafka具有副本的功能，不同的broker上保存了每个parition的不同副本，具体存在几台broker上，是由配置的副本因子所决定的。尽管有大量副本的存在，但kafka只会在最初把数据写入partition的leader（一个leader多个follower），leader是随机的在ISR（in-sync replicas）池（所有处于同步状态的partition副本）中选举出来的。另外，消费者只会读取partition leader，这样follower副本将作为备份存在，以保证kafka的高可用性，从而防某个broker挂掉。



