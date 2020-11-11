---
Title: kafka 概括
Keywords:
Descriptions:
Cover:
Date:
LastEditTime:
---



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

简单写了个shell脚本：和kafka同一级目录就可以

```shell
#!/bin/bash
curDir=$(cd $(dirname "$0"); pwd)
kafkaDir="${curDir}/kafka/"
startStatus() {
	startSuccess=$(lsof -i tcp:"$1")
	if [ -z "$startSuccess" ]; then
		echo "service $1 start failed"; exit 1
	fi
}

cleanup() {
	rm -rf "${curDir}/data*"
}

start(){
	pushd $kafkaDir
	echo "zookeeper starting ..."
	./bin/zookeeper-server-start.sh -daemon config/zookeeper.properties 
	sleep 1
	startStatus 2181

	echo "start kafka broker 1 ..."
	./bin/kafka-server-start.sh -daemon config/server1.properties 
	

	if [ -n "$1" ]; then
		if [ "$1" -eq 2 ]; then
			echo "start kafka broker 2 ..."
			./bin/kafka-server-start.sh -daemon config/server2.properties
		elif [ "$1" -eq 3 ]; then
			echo "start kafka broker 2 ..."
			./bin/kafka-server-start.sh -daemon config/server2.properties
			echo "start kafka broker 3 ..."
			./bin/kafka-server-start.sh -daemon config/server3.properties
		fi
	fi
}

COMMAND=$1
case $COMMAND in
  	-start)
		cleanup
		initLogs
		start $2
    	shift
  		;;
  	-stop)
        pushd "$kafkaDir"
		./bin/kafka-server-stop.sh
		./bin/zookeeper-server-stop.sh
    	;;
  	*)
    	;;
esac
```





# zookeeper 与 kafka 的关系

