---
Title: "go kafka producer 实践与落地"
Keywords: "go,kafka,producer,sync,async"
Description: "go kafka producer在写消息时，写了1，2条就阻塞了？"
Lable: "kafka producer"
---

- kafka 库
- producer config 配置
- sync vs async
- 压缩方式

背景：博客文章想统计访问数据，每日pv/uv/ip数据

分析：

- 网页浏览数据，主要以写为主
- 时效性、事务持久要求不高
- 数据量大

kafka特别适合这种场景，高吞吐量，较低的端到端延迟，强大的持久性。

# go kafka 库

- [github.com/Shopify/sarama](https://pkg.go.dev/github.com/Shopify/sarama)  使用人数较多，我公司也是用的这个库
- [github.com/confluentinc/confluent-kafka-go](https://github.com/confluentinc/confluent-kafka-go) 我没有做过实验，有兴趣的朋友自己尝试一下

```
go get github.com/Shopify/sarama
```

# sync vs async

同步异步的区别, 如果因为实时性要求没那么高的话，推荐使用async，总结一下原因：kafka面临的2大难题之一，**持久化要面临磁盘大量的I/O操作**，如此频繁的写，即便是顺序写、有pagecache可以缓冲、有mmap减少一次内核态的复制，磁盘也是难以承受。

kafka的处理也是很简单，加缓存，本来1毫秒100次写，broker缓存10次一写（数据块），这样可以减轻10倍的I/O压力。

所以这里也是同样的思想，kafka加了一层缓存，我们producer也可以在加一层缓存，又可以减轻n倍的I/O压力。所以在实时性要求不高的前提下，推荐使用async

async有4个配套参数

```go
conf := sarama.NewConfig()
conf.Producer.Flush.Messages = 10                      // 缓存条数
conf.Producer.Flush.Frequency = 500 * time.Millisecond // 缓存时间

Flush struct {
  // 达到多少字节时，触发一次broker请求
  Bytes int
  // 达到多少条消息时，触发一次broker请求
  Messages int 
  // producer缓存消息的时间, 默认缓存500毫秒 => queue.buffering.max.ms
  Frequency time.Duration 
  // producer缓存队列里最大缓存的消息数量 => queue.buffering.max.messages
  MaxMessages int
}
```

完整的prodcuer用法

```go
type Produce struct {
	AsyncProducer sarama.AsyncProducer
}

func NewProducer(ctx context.Context) *Produce {
	conf := sarama.NewConfig()
	conf.Producer.RequiredAcks = sarama.WaitForAll
	conf.Producer.Return.Errors = true                     // 接收producer的error，下面细说用法
	conf.Producer.Compression = sarama.CompressionLZ4      // 压缩方式，如果kafka版本大于1.2，推荐使用zstd压缩
	conf.Producer.Flush.Messages = 10                      // 缓存条数
	conf.Producer.Flush.Frequency = 500 * time.Millisecond // 缓存时间

	producer, err := sarama.NewAsyncProducer(config.Kafka.Broker(), conf)
	if err != nil {
		panic(errors.Wrapf(err, "failed to open kafka producer: "))
	}
	pd := &Produce{AsyncProducer: producer}
	// 接受结束信号
	go func() {
		for {
			select {
			case err := <-producer.Errors():
				logger.Errorf("kafka producer send error %s", err.Err.Error())
			case <-ctx.Done(): // 接收主进程结束信号，持久化缓存数据，防止丢失，关闭producer连接
				_ = producer.Close()
				logger.Infof("quit: kafka producer")
				return
			}
		}
	}()
	return pd
}

func (p *Produce) Send(topic string, value []byte) {
	p.AsyncProducer.Input() <- &sarama.ProducerMessage{
		Topic: topic,
		Value: sarama.ByteEncoder(value),
	}
}
```

- `conf.Producer.Return`: 这里是go语言的专属用法，有2个channel，**重点：**如果 `conf.Producer.Return.Errors = true` 设置为了true，就必须有相应读取channel，否则会阻塞producer写消息，success channel/error channel默认capacity只有256字节 <a name="producer_return"></a>
    - `success`: 写消息成功channel 
    -  `errors`:  错误channel
- `conf.Producer.RequiredAcks` 对应java producer的 `request.required.acks`
    - `sarama.NoResponse` - `0`:   producer不等待来自broker同步完成的确认继续发送下一条（批）消息。延迟最低/消息持久化保证性最低（当服务器发生故障时某些数据会丢失，如leader已死，但producer并不知情，发出去的信息broker就收不到）。
    - `sarama.WaitForLocal` - `1`: producer在leader已成功收到的数据并得到确认后发送下一条message，没有确认其他副本是否收到。持久化保证较好（被写入死亡leader但尚未复制将失去了唯一的消息）
    - `sarama.WaitForAll` - `2`（默认）:  producer在leader以及follower副本确认接收到数据后才算一次发送完成。 

- `conf.Producer.Compression` : kafka 消息压缩了，综合考量来说，如果kafka版本大于2.1可以使用zstd，小于2.1版本可以使用lz4, 无论是cpu占用还是压缩比例的角度上看

| 算法           | cpu  | 压缩比（带宽同理） | producer吞吐量(压缩快慢) | consumer吞吐量(解压缩快慢) |
| -------------- | ---- | ------------------ | ------------------------ | -------------------------- |
| Gzip           | 1    | 3                  | 4                        | 3                          |
| Snappy         | 4    | 4                  | 2                        | 2                          |
| LZ4            | 3    | 2                  | 1                        | 1                          |
| ZSTD (2.1版本) | 2    | 1                  | 3                        | 4                          |

# issue

1. go kafka producer在写消息时，写了1，2条就阻塞了？ 

答：[conf.Producer.Return配置姿势不对？](#producer_return) 

`conf.Producer.Return`: 这里是go语言的专属用法，有2个channel，**重点：**如果 `conf.Producer.Return.Errors = true` 设置为了true，就必须有相应读取channel，否则会阻塞producer写消息，success channel/error channel默认capacity只有256字节 <a name="producer_return"></a>

- `success`: 写消息成功channel 
-  `errors`:  错误channel

