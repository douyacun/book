---
Title: "go kafka consumer group 使用姿势"
Keywords: "go,consumer,consumer group,幂等,重复消费"
Description: "1. 优雅重启 2.kafka崩溃恢复"
Label: "kafka consumer"
Cover: "assert/go-kafka-producer.jpeg"
---

概述：

- kafka消费者、消费者组和消费者位移？
- kafka幂等性以及避免重复消费？
- go kafka consumer 封装

背景：和producer同样的用途，统计博客网页浏览数据

# kafka消费者、消费者组和消费者位移？

kafka consumer 从 broker 中 pull 数据，producer push 数据到 broker 中。

优点：consumer可以承受能力的范围内控制消费速度，而且可以批量获取消息块

缺点：如果broker中没有数据，consumer会有轮询broker接口，这里kafka提供了参数来阻塞fetch请求直到有数据可以消费

**消费者组的特性：**

1. 一个消费者组可以有一个或者多个消费者
2. topic的一个partition只能有一个消费者组中的一个消费者消费，要想实现分布式消费，必须要有多个patition

**消费者位移：**

kafka面临的问题，如何让broker 和 consumer 就被消费的数据保持一致性

- broker发送消息给consumer之后立即标记为consumed。如果 consumer 崩溃或者请求超时或者其他原因，就会导致消息丢失
- 增加确认机制，消息发送标记消息为sent，consumer消费成功后标记消息为consumed。
    - 如果consumer消费成功了，但是在发送确认之前崩溃了，broker需要再次发送消息，就会导致消息重复消费。
    - 性能问题：broker需要为每个消息保存多个状态，状态的变动需要加锁
    - 如果消息一直得不到确认该如何处理

kafka的是如何实现消息确认机制的：

每一个 partition 在任意给定的时间内只能被每个订阅了这个 topic 的 consumer 组中的一个 consumer 消费，只需要保存这个consumer下一条要消费的消息的位移即可

- 同一个消费者组的消费者 不会并发消费 同一个partition，不需要加锁
- 不需要保存多个状态，消费者确认要消费到了哪一条，只想下一条即可。
- 如果consumer消费逻辑出现bug，可以在 bug 修复后通过回退到之前的 offset 来再次消费这些数据 （重要）, go 可以指定出初始位移`conf.Consumer.Offsets.Initial`来再次消费



# kafka幂等性以及避免重复消费

正常情况下producer push一条消息到broker，然后consumer从broker pull消息然后消费，这只是理想状态的情况下。我们需要考虑的是发生异常的情况kafka如何做到保证消息是不丢失，不重复消费的。

- producer 发布消息的持久性保证
- consumer 消费消息的保证，只被消费一次

**如果一个 producer 在试图发送消息的时候发生了网络故障， 则不确定网络错误发生在消息提交之前还是之后，broker到底有没有收到这条消息并持久化**？

答：如果我们想要确保消息可以重传，但绝不丢失。kafka 0.11.0.0 之前的版本就是重发机制，之后的版本增加了幂等性，会为每条消息分配序列号来避免在broker log中产生重复消息。同时kafka提供了事务机制，多条消息写入log要么都成功要么都失败。

kafka所有的副本都有相同的 log 和相同的 offset。consumer 负责控制它在 log 中的位置，理想状态下consumer永远不会崩溃，没问题。**如果consumer崩溃了，这个topic partition必须要有另一个进程来接管，此时该从哪个offset开始接管？**

1. consumer先读取消息，然后将它的位置保存在log中，在处理相应的消息，如果异常发生在位置保存后，即使有其他进程接管，这条消息可能也会丢失了
2. consumer先读取消息，处理消息，最后保存它的位置，如果异常是在消息处理完成后，还没有保存消息时。消息可能会被重复处理。

答：2种解决方案，步骤就是先读取，在消费，最后更新位移，这样可以保证消息不丢失，可能会造成消息重复消费

- 简单的方案：幂等性保证，使用offset作为唯一键，数据和offset一起写库，如果offset不存在就先建，否则更新。
- 复杂的方案：2步提交，消费完成写库，状态为待提交，更新kafka log offset，更新库的状态为已提交，此方案会复杂化问题，推荐幂等性保证，简单有效。

重点：consumer消费数据时，要做一次幂等性消费，避免重复消费数据



# kafka 消费者 & 分区

Kafka 消费者从属于消费者群组。一个群组里的消费者订阅的是同一个主题，每个消费者 接收主题一部分分区的消息。

- 假设主题 T1 有 4 个分区，我们创建了消费者 C1，它是群组 G1 里唯一的消费者，我们用 它订阅主题 T1。消费者 C1 将收到主题 T1 全部 4 个分区的消息
- 如果在群组 G1 里新增一个消费者 C2，那么每个消费者将分别从两个分区接收消息。假设消费者 C1 接收分区 0 和分区 2 的消息，那么消费者 C2 节会接收分区 1 和分区 3 的消息
- 如果群组 G1 有 4 个消费者，那么每个消费者可以分配到一个分区
- 如果群组添加更多的消费者，超过主题的分区数量，那么有一部分消费者就会被 闲置，不会接收到任何消息

**再均衡**：分区的所有权从一个消费者转移到另一个消费者

消费者通过向consumer-group 协调器的broker发送心跳来维持他们和群组的从属关系以及它们对分区的所有权关系，只要消费者以正常的时间间隔发送心跳，就被认为是活跃的，说明还在正常读取分区的消息，

如果消费者发生崩溃，consumer-group协调器broker，在确认了该consumer死亡后会出发再均衡。

`max.poll.interval.ms` 可以控制心跳的间隔时间

# go kafka consumer 

封装一下go consumer支持

- 优雅关闭consumer
- 记录consumer异常，同producer一样Consumer.Return channel设置为true以后必须有相应的读取，否则会阻塞
- kafka崩溃恢复，可重新加入consumer group，恢复消费

```go
type ConsumerGroup interface {
	Consume(topic []string, handler sarama.ConsumerGroupHandler)
}

type consumerGroup struct {
	ctx     context.Context
	client  sarama.ConsumerGroup
	groupId string
}

func NewConsumerGroup(ctx context.Context, groupId string) ConsumerGroup {
	conf := sarama.NewConfig()
	conf.Version = sarama.V0_11_0_2
	conf.Consumer.Return.Errors = true
	client, err := sarama.NewConsumerGroup(config.Kafka.Broker(), groupId, conf)
	if err != nil {
		panic(errors.Wrap(err, "kafka new consumer client err"))
	}
	// Track errors
	go func() {
		for err := range client.Errors() {
			logger.Wrapf(err, "kafka %s consume err", groupId)
		}
	}()
	go func() {
		for {
			select {
			// 主进程退出，通知consumer关闭
			case <-ctx.Done():
				_ = client.Close()
				//logger.Infof("quit: kafka consumer %s", groupId)
				return
			}
		}
	}()
	return &consumerGroup{
		ctx:     ctx,
		client:  client,
		groupId: groupId,
	}
}

func (c *consumerGroup) Consume(topic []string, handler sarama.ConsumerGroupHandler) {
	go func() {
		defer c.client.Close()
		for {
			err := c.client.Consume(c.ctx, topic, handler)
			if err != nil {
				switch err {
				case sarama.ErrClosedClient, sarama.ErrClosedConsumerGroup:
					// 退出
					logger.Infof("quit: kafka consumer %s", c.groupId)
					return
				case sarama.ErrOutOfBrokers:
					logger.Errorf("kafka 崩溃了~")
				default:
					logger.Errorf("kafka exception: %s", err.Error())
				}
				time.Sleep(1 * time.Second)
			}
		}
	}()
}
```
---