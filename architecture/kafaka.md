---
Title: kafka 概括
Keywords:
Descriptions:
Cover:
Date:
LastEditTime:
---



# 安装

zookeeper 下载地址: [http://zookeeper.apache.org/releases.html#download](http://zookeeper.apache.org/releases.html#download)

解压：`tar -zxvf kafka_2.12-2.6.0.tgz`

配置:

运行：



kafka 下载地址: [https://kafka.apache.org/downloads](https://kafka.apache.org/downloads)

解压：`tar -zxvf kafka_2.12-2.6.0.tgz`

配置: 

- Broker
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
- topic
  - retension.hours 保留时长
  - retension.bytes 为该Topic预留多大的磁盘空间，-1无限使用磁盘空间

运行：







# zookeeper 与 kafka 的关系

