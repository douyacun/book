姓名：刘宁

求职意向：go、php后端开发

联系方式: 15258157582 ｜ [douyacun@gmail.com](mailto:douyacun@gmail.com)



# 技术技能

- 编程语言: Go、PHP、 Python
- Web开发: Gin、 Echo、Laravel、Yii
- 数据库: MySQL、Redis、Elasticsearch
- 消息队列: Kafka, RabbitMQ
- 系统/网络: Linux、Docker、Shell脚本、Http、 gRPC、WebSocket
- 前端开发：了解React、微信小程序、Qt



## 工作经历

### 上海泛为科技有限公司

>  2020.07 - 至今   go语言开发工程师

负责米派聚合广告SDK后台和广告DSP/ADX系统开发，项目使用go语言开发。项目结构采用了DDD分层架构、微服务直接采用gpc调用，业务开发负责广告计划、广告组、创意接口开发

- **公共库** 业务开发过程中提取基础组件，减少重复开发提高效率
    - 开发go语言[di/ioc](https://github.com/douyacun/go-ioc)容器实现依赖注入控制反转，使各模块更独立更方便定义模块interface。
    - 开发报表数据导出模块，针对运营同学多样的excel需求，封装[go-struct-excel](https://github.com/douyacun/go-struct-excel)支持go结构体导入/导出为excel
    - 接入Gorm数据库ORM，封装将请求参数结构体tag转化成sql语句，优化过多的if else
    - 解决proto多目录编译报错，优化proto多路径混乱报错问题，[proto编译](https://www.douyacun.com/article/73094d8520b8582b217ae5c424195e3c)
- **报表服务** 开发tracking服务，为SDK提供接口上报竞价、点击、展示数据，数据写入kafka缓冲峰值流量，对数据汇总和原始写入h a doop
- **Excel批量导入服务** 开发go异步处理excel批量导入功能，使用channel实现异步处理池。解决同步处理超时、支持平滑重启、重试
- **业务开发**
    - 负责开发了企业、商户、供应商、用户、角色和权限模块的数据结构设计和代码开发。
    - 设计并开发了基于 JWT、Envoy 和企业微信扫码登录的企业内部登录流程。
- **技术栈** go、grpc、etcd、proto、etcd 数据库mysql、redis，消息队列：kafka

### 趣头条

> 2019.01 - 2020.07 go语言开发工程师

参与设计与实现DSP广告主系统与架构设计微服务重构，对接运营和产品需求，同时负责接口开发与系统编程。

**项目历程**：

- **服务化拆分** 随着业务发展流量增大，老系统面临线上事故频发、数据库读写压力超载。使用go语言重构系统，根据业务将现有平台服务拆分为客户、运营、建站系统。突破单库性能瓶颈、完成数据库分库分表，提高日常业务开发效率，减少验证和上线时间

- **微服务日志追踪与收集** 服务化拆分后问题排查难度增加，各系统出现日志记录不全无法了解上下文，日志格式参差不齐，或者没有记录导致线上问题排查难度大，修复慢。新老系统php和go同时接入opentracing收集接口参数、sql和redis命令到elk中。
- **外部MarketApi接口**  针对广告主实时关注广告数据的需求，提供API接口查询广告的展点消报表数据，开发鉴权中间件、分布式接口限流服务(redis + 令牌桶)
- **数据变更日志与回滚**  针对广告主误操作或者查找操作记录的需求，开发操作日志模块并提供广告版本一键恢复功能。使用mysql同步elasticsearch记录账户操作日志，支持多维度搜索。
- **充值业务** 负责开发老系统充值业务都是运营线下处理，重复劳动且容易误充值，需要同时支持老系统和新系统其他系统业务充值。选用DTM作为事务管理器TCC事务保证最终一致性。
- **技术栈** 使用kong作为网关层实现路由分发，Istio管理超时重试、熔断，PHP/Go同步开发、mysql主从同步、redis缓存、消息队列kafka和roketmq、框架 yii、gin

### 展盟网络科技有限公司

> 2017.01 - 2019.01 PHP语言开发工程师

项目从0到1开发新闻类APP。我主要负责系统与架构设计, 同时负责接口开发与模块功能实现。后端使用laravel框架部署lnmp环境，前后分离项目同时为SDK和后台提供API接口，并负责部分后台react页面的开发

**项目历程**

- **积分裂变** 开发积分系统提供发分、交易、冻结流程的交易，退分以及取消发分、积分过期等一系列的完备积分功能，支持积分微信、支付宝提现，提高用户粘性与快速拉取用户。
- **积分商城** 开发积分商城、支持积分抢购活动，通过redis缓存减少库存压力，利用redis单线程处理解决超卖问题，roketmq消息队列保证redis和数据库的最终一致性
- **接口安全** 制定安全防刷策略，接口限流、接口数据签名加密规则
- **运营活动 ** RabbitMQ，订阅用户登录、阅读等行为。异步处理运营业务加速开发迭代，保证基础业务的运行
- **SDK推送服务 **封装接口oppo、⼩⽶、华为、魅族以及jpush安卓推送服务合力提升多通道消息到达率，推送用户关注的内容或热点资讯，提升用户活跃。
- **技术栈：** lnmp、php框架laravel、mysql、redis、cdn、roketmq



## 个人项目

[博客](https://www.douyacun.com): 个人开发react项目

[webrtc](https://www.douyacun.com/p2p/send) : 在线网站p2p发送文件

[PDF工具箱](https://www.douyacun.com/pdf/remove-watermark) : 网站、桌面软件、微信小程序 实现 PDF去水印、PDF转word、PDF文字编辑







