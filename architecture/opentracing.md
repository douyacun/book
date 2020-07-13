---
Title: go opentracing 实践与落地
Keywords: opentracing,gin,gorm,elasticsearch,jaeger
Description: 分布式全链路日志追踪
Date: 2020-05-04 00:45:42
LastEditTime: 2020-05-06 10:23:00
---

本文概要:

1. 了解opentracing
2. 开源库jaeger/zipkin都实现了opentracing
3. zipkin架构和组件
   1. xorm sql 埋点
   2. 自定义span
4. go语言接入jaeger

# zipkin vs jaeger

|                                    | **JAEGER**                                                   | **ZIPKIN**                                                   |
| ---------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| **OpenTracing compatibility**      | Yes                                                          | Yes                                                          |
| **OpenTracing-compatible clients** | [Python](https://github.com/uber/jaeger-client-python)[Go](https://github.com/uber/jaeger-client-go)[Node](https://github.com/uber/jaeger-client-node)[Java](https://github.com/uber/jaeger-client-java)[C++](https://github.com/jaegertracing/jaeger-client-cpp)[C#](https://github.com/jaegertracing/jaeger-client-csharp)[Ruby](https://github.com/salemove/jaeger-client-ruby) *[PHP](https://github.com/jukylin/jaeger-php) *[Rust](https://github.com/sile/rustracing_jaeger) * | [Go](https://github.com/openzipkin/zipkin-go-opentracing)[Java](https://github.com/openzipkin-contrib/brave-opentracing)[Ruby](https://github.com/salemove/zipkin-ruby-opentracing) *[C++](https://github.com/rnburn/zipkin-cpp-opentracing)Python (work in [progress](https://github.com/openzipkin-attic/zipkin-python-opentracing/pull/1)) |
| **Storage support**                | In-memoryCassandraElasticsearchScyllaDB (work in [progress](https://github.com/uber/jaeger/pull/201)) | In-memoryMySQLCassandraElasticsearch                         |
| **Sampling**                       | Dynamic sampling rate  (supports rate limiting and  probabilistic sampling strategies) | Fixed sampling rate (supports probabilistic sampling strategy) |
| **Span transport**                 | UDPHTTP                                                      | HTTPKafkaScribeAMQP                                          |
| **Docker ready**                   | Yes                                                          | Yes                                                          |

这两个都是全链路日志追踪，都经过了时间的沉淀，文档都比较齐全。go语言推荐使用jaeger，两者都是非常优秀。

## zipkin

### 流程图

> 来自 https://zipkin.io/pages/architecture.html

```
┌─────────────┐ ┌───────────────────────┐  ┌─────────────┐  ┌──────────────────┐
│ User Code   │ │ Trace Instrumentation │  │ Http Client │  │ Zipkin Collector │
└─────────────┘ └───────────────────────┘  └─────────────┘  └──────────────────┘
       │                 │                         │                 │
           ┌─────────┐
       │ ──┤GET /foo ├─▶ │ ────┐                   │                 │
           └─────────┘         │ record tags
       │                 │ ◀───┘                   │                 │
                           ────┐
       │                 │     │ add trace headers │                 │
                           ◀───┘
       │                 │ ────┐                   │                 │
                               │ record timestamp
       │                 │ ◀───┘                   │                 │
                             ┌─────────────────┐
       │                 │ ──┤GET /foo         ├─▶ │                 │
                             │X-B3-TraceId: aa │     ────┐
       │                 │   │X-B3-SpanId: 6b  │   │     │           │
                             └─────────────────┘         │ invoke
       │                 │                         │     │ request   │
                                                         │
       │                 │                         │     │           │
                                 ┌────────┐          ◀───┘
       │                 │ ◀─────┤200 OK  ├─────── │                 │
                           ────┐ └────────┘
       │                 │     │ record duration   │                 │
            ┌────────┐     ◀───┘
       │ ◀──┤200 OK  ├── │                         │                 │
            └────────┘       ┌────────────────────────────────┐
       │                 │ ──┤ asynchronously report span     ├────▶ │
                             │                                │
                             │{                               │
                             │  "traceId": "aa",              │
                             │  "id": "6b",                   │
                             │  "name": "get",                │
                             │  "timestamp": 1483945573944000,│
                             │  "duration": 386000,           │
                             │  "annotations": [              │
                             │--snip--                        │
                             └────────────────────────────────┘
```

### go语言支持

- [zipkin-go-opentracing ](https://github.com/openzipkin-contrib/zipkin-go-opentracing) zipkin同样支持opentracing

- [opentracing-go](https://github.com/opentracing/opentracing-go) 
- [go-grpc-middleware](https://github.com/grpc-ecosystem/go-grpc-middleware) grpc 请求日志需要 中间件支持
- [otgrpc](https://github.com/grpc-ecosystem/grpc-opentracing/tree/master/go/otgrpc) 记录grpc 记录请求/响应，是否记录本次请求，更像是一个装饰器

#### server

```go
func main() {
  // reporter: 输出到哪?
	reporter := zipkinhttp.NewReporter("http://localhost:9411/api/v2/spans")
	defer reporter.Close()
  // endpoint: 记录服务名称和端口
	endpoint, err := zipkin.NewEndpoint("zipkin-demo", "0.0.0.0")
	if err != nil {
		log.Fatalf("zipkin new endpoint error: %v", err)
	}
	// tracer
	nativeTracer, err := zipkin.NewTracer(reporter, zipkin.WithLocalEndpoint(endpoint))
	if err != nil {
		log.Fatalf("zipkin new tracer error: %v", err)
	}
  // 接入opentracing
	tracer := zipkinot.Wrap(nativeTracer)
	opentracing.SetGlobalTracer(tracer)
  // 初始化grpc server,并注册中间件
	grpcServer := grpc.NewServer(
		// otgrpc.LogPayloads 是否记录 入参和出参
		// otgrpc.SpanDecorator 装饰器，回调函数
		// otgrpc.IncludingSpans 是否记录
		grpc.UnaryInterceptor(grpc_middleware.ChainUnaryServer(otgrpc.OpenTracingServerInterceptor(tracer,
			otgrpc.LogPayloads(),
			// IncludingSpans是请求前回调
			otgrpc.IncludingSpans(func(parentSpanCtx opentracing.SpanContext, method string, req, resp interface{}) bool {
				log.Printf("method: %s", method)
				log.Printf("req: %+v", req)
				log.Printf("resp: %+v", resp)
				return true
			}),
			// SpanDecorator是请求后回调
			otgrpc.SpanDecorator(func(span opentracing.Span, method string, req, resp interface{}, grpcError error) {
				log.Printf("method: %s", method)
				log.Printf("req: %+v", req)
				log.Printf("resp: %+v", resp)
				log.Printf("grpcError: %+v", grpcError)
			}),
		))),
	)
  // 注册服务
	greet.RegisterServiceServer(grpcServer, &handler{})
	listener, err := net.Listen("tcp", ":1234")
	if err != nil {
		log.Fatalf("net listen error: %v", err.Error())
	}
	grpcServer.Serve(listener)
}

```

> otgrpc 实现源码很简单，看一下
>
> 本次实践源码:  [https://github.com/douyacun/opentracing-demo](https://github.com/douyacun/opentracing-demo)

#### xorm

```go

var DB *xorm.Engine

func Init() error {
	var err error
	// XORM创建引擎
	DB, err = xorm.NewEngine("mysql", "root:root@(127.0.0.1:3306)/toilet?charset=utf8mb4")
	if err != nil {
		return err
	}
	// 创建自定义的日志实例
	log := logrus.New()
	log.Out = os.Stdout
	// 将日志实例设置到XORM的引擎中
	DB.SetLogger(&TracerLogger{
		logger:  log,
		level:   xormLog.LOG_DEBUG,
		showSQL: true,
	})
	return nil
}

type TracerLogger struct {
	logger  *logrus.Logger
	level   xormLog.LogLevel
	showSQL bool
	span    opentracing.Span
}

// 主要是实现 SQLLogger 接口
// type SQLLogger interface {
// 	BeforeSQL(context LogContext) // only invoked when IsShowSQL is true
// 	AfterSQL(context LogContext)  // only invoked when IsShowSQL is true
// }
func (l *TracerLogger) BeforeSQL(ctx xormLog.LogContext) {
	l.span, _ = opentracing.StartSpanFromContext(ctx.Ctx, "xorm")
}

func (l *TracerLogger) AfterSQL(ctx xormLog.LogContext) {
	defer l.span.Finish()
	var sessionPart string
	l.span.LogFields(opentracingLog.String("db.statement", ctx.SQL))
	l.span.LogFields(opentracingLog.Object("db.args", ctx.Args))
	l.span.LogFields(opentracingLog.String("db.type", "sql"))
	l.span.LogFields(opentracingLog.Object("db.execute_time", ctx.ExecuteTime))
	if ctx.ExecuteTime > 0 {
		l.logger.Infof("[SQL]%s %s %v - %v", sessionPart, ctx.SQL, ctx.Args, ctx.ExecuteTime)
	} else {
		l.logger.Infof("[SQL]%s %s %v", sessionPart, ctx.SQL, ctx.Args)
	}
}
```

> 需要xorm: 1.0 以上的版本
>
> - `"xorm.io/xorm/log"`
> - `"xorm.io/xorm"`

## jaeger

### 推荐阅读

推荐:

- [异构系统链路追踪—滴滴 trace 实践](https://v.qq.com/x/page/p0546gars01.html)   落地实践，推荐

- [opentracing](https://opentracing.io/docs/overview/what-is-tracing/)  必须看
- [opentracing tutorial](https://github.com/yurishkuro/opentracing-tutorial) 可以不看
- [The OpenTracing Specification repository](https://github.com/opentracing/specification) 命名规范
- [go elasticsearch trace](https://www.elastic.co/guide/en/apm/agent/go/current/introduction.html) 收集es查询日志
- [使用阿里云sls存储](https://github.com/aliyun/aliyun-log-jaeger) 公司一般都是用sls    

### 开源库

- [jaeger](https://www.jaegertracing.io/docs/1.17/)

### 安装

1. 创建一个bridge桥接的网络

```shell
docker network create douyaucn
```

2. 创建es容器，jaeger默认存储是用es

```shell
docker run -d \
-p 9200:9200 \
-p 9300:9300  \
--network douyacun \
--network-alias d1 \
-e "discovery.type=single-node" \
docker.elastic.co/elasticsearch/elasticsearch:7.5.0
```

3. 安装一下kibana，方便看一下es里的数据到底是怎么存储的，jaeger默认使用thrift序列化的数据，看不出源数据长啥样，注意kibana和es版本要一致

```shell
docker run -d --network douyacun --network-alias d3 --link 3ee3312d488d:elasticsearch -p 5601:5601 kibana:7.5.0
```

看一眼es是怎么存的数据:

```json
{
    "traceID": "64e770b12d5dfce0",
    "spanID": "64e770b12d5dfce0",
    "flags": 1,
    "operationName": "dyc/internal/controllers.NewRouter.func2",
    "references": [],
    "startTime": 1588482117635784,
    "startTimeMillis": 1588482117635,
    "duration": 17,
    "tags": [
        {
            "key": "sampler.type",
            "type": "string",
            "value": "const"
        },
        {
            "key": "sampler.param",
            "type": "bool",
            "value": "true"
        },
        {
            "key": "internal.span.format",
            "type": "string",
            "value": "proto"
        }
    ],
    "logs": [],
    "process": {
        "serviceName": "api.douyacun.com",
        "tags": [
            {
                "key": "jaeger.version",
                "type": "string",
                "value": "Go-2.23.1"
            },
            {
                "key": "hostname",
                "type": "string",
                "value": "liuningdeMacBook-Pro.local"
            },
            {
                "key": "ip",
                "type": "string",
                "value": "192.168.31.164"
            },
            {
                "key": "client-uuid",
                "type": "string",
                "value": "57c0ef9eaec2fabb"
            }
        ]
    }
}
```

4. collectors 

```shell
docker run -d \
--network douyacun \
--network-alias d2 \
-e SPAN_STORAGE_TYPE=elasticsearch \
-e ES_SERVER_URLS=http://d1:9200 \
jaegertracing/jaeger-collector:1.17
```

5. agent 这里只用6831端口就可以了，其他端口都是为了兼容性

```shell
docker run \
--network douyacun \
--network-alias d3 \
-p 6831:6831/udp  \
jaegertracing/jaeger-agent:1.17 
--reporter.grpc.host-port=d2:14250
```

6. trace UI界面

```shell
docker run  \
--network douyacun \
--network-alias d4  \
-p 16686:16686  \
-p 16687:16687 \
-e SPAN_STORAGE_TYPE=elasticsearch \
-e ES_SERVER_URLS=http://d1:9200 \
jaegertracing/jaeger-query:1.17
```

### gin集成jaeger

opentracing.tracer 初始化，只初始化一次就够了

```go
func InitTrace() opentracing.Tracer {
  // GlobalTracer 是单例
	if opentracing.IsGlobalTracerRegistered() {
		return opentracing.GlobalTracer()
	} else {
    // jaeger配置
		cfg := config.Configuration{
			ServiceName: "api.douyacun.com",
			Sampler: &config.SamplerConfig{
				Type:  "const",
				Param: 1,
			},
      // 配置agent收集
			Reporter: &config.ReporterConfig{
				LocalAgentHostPort: "127.0.0.1:6831",
			},
      // 这里配置header，默认值 uber-trace-id（我不喜欢）
			Headers: &jaeger.HeadersConfig{
				JaegerDebugHeader:        "x-debug-id",
				JaegerBaggageHeader:      "x-baggage",
				TraceContextHeaderName:   "x-trace-id",
				TraceBaggageHeaderPrefix: "x-ctx",
			},
		}
		tracer, _, err := cfg.NewTracer()
		if err != nil {
			panic(errors.Wrapf(err, "Error: cannot init tracer"))
		}
		opentracing.SetGlobalTracer(tracer)
		return tracer
	}
}
```

gin middleware 记录每次请求的参数，url

```go
func Trace() gin.HandlerFunc {
	return func(ctx *gin.Context) {
		tracer := opentracing.GlobalTracer()
		if tracer == nil {
			ctx.Next()
			return
		}
		var span opentracing.Span
		// Extract 就是读 request header 中的 `x-trace-id`
    // 读的到就继承
    // 读不到就初始化一个新的
		wireCtx, err := tracer.Extract(opentracing.HTTPHeaders, opentracing.HTTPHeadersCarrier(ctx.Request.Header))
		if err != nil {
			span = opentracing.StartSpan(ctx.Request.URL.Path)
		} else {
			span = opentracing.StartSpan(ctx.Request.URL.Path, opentracing.ChildOf(wireCtx))
		}

		defer span.Finish()
		if b := ctx.Request.GetBody; b != nil {
			if body, err := b(); err == nil {
				var buf bytes.Buffer
				if _, err = buf.ReadFrom(body); err == nil {
					span.LogKV("form-body", buf.String())
				}
			}
		}
    // response header
		if sp, ok := span.Context().(jaeger.SpanContext); ok {
      // 这里设置一个root span，本次请求的其他span挂在root span下面
			ctx.Set("root_span", span)
			ctx.Writer.Header().Set("x-request-id", sp.TraceID().String())
			ctx.Writer.Header().Set("x-trace-id", sp.TraceID().String())
			ctx.Writer.Header().Set("X-Span-id", sp.SpanID().String())
		}
		ctx.Next()
	}
}

```

gorm jaeger 这里用了https://github.com/smacker/opentracing-gorm 提供的方法，稍微改造了一下

```go

var db_write *gorm.DB
var DB_ONCE sync.Once

type _db struct {
	gorm.DB
}

const (
	parentSpanGormKey = "opentracingParentSpan"
	spanGormKey       = "opentracingSpan"
)

func NewDB(dsn string) {
	// videos_t
  var err error
  db_write, err = gorm.Open("mysql", dsn)
  if err != nil {
    logger.Fatal("gorm open %s", err)
  }
  if err = db_write.DB().Ping(); err != nil {
    logger.Fatal(err)
  }
  db_write.SingularTable(true)
  AddGormCallbacks(db_write)
}

func Write(ctx *gin.Context) *gorm.DB {
	return SetSpanToGorm(ctx, db_write)
}

// SetSpanToGorm sets span to gorm settings, returns cloned DB
func SetSpanToGorm(ctx *gin.Context, d *gorm.DB) *gorm.DB {
	if ctx == nil {
		return d
	}
	if parentSpan, ok := ctx.Get("root_span"); ok {
		return d.Set(parentSpanGormKey, parentSpan)
	}
	return d
}

// AddGormCallbacks adds callbacks for tracing, you should call SetSpanToGorm to make them work
func AddGormCallbacks(db *gorm.DB) {
	callbacks := newCallbacks()
	registerCallbacks(db, "create", callbacks)
	registerCallbacks(db, "query", callbacks)
	registerCallbacks(db, "update", callbacks)
	registerCallbacks(db, "delete", callbacks)
	registerCallbacks(db, "row_query", callbacks)
}

type callbacks struct{}

func newCallbacks() *callbacks {
	return &callbacks{}
}

func (c *callbacks) beforeCreate(scope *gorm.Scope)   { c.before(scope) }
func (c *callbacks) afterCreate(scope *gorm.Scope)    { c.after(scope, "INSERT") }
func (c *callbacks) beforeQuery(scope *gorm.Scope)    { c.before(scope) }
func (c *callbacks) afterQuery(scope *gorm.Scope)     { c.after(scope, "SELECT") }
func (c *callbacks) beforeUpdate(scope *gorm.Scope)   { c.before(scope) }
func (c *callbacks) afterUpdate(scope *gorm.Scope)    { c.after(scope, "UPDATE") }
func (c *callbacks) beforeDelete(scope *gorm.Scope)   { c.before(scope) }
func (c *callbacks) afterDelete(scope *gorm.Scope)    { c.after(scope, "DELETE") }
func (c *callbacks) beforeRowQuery(scope *gorm.Scope) { c.before(scope) }
func (c *callbacks) afterRowQuery(scope *gorm.Scope)  { c.after(scope, "") }

func (c *callbacks) before(scope *gorm.Scope) {
	val, ok := scope.Get(parentSpanGormKey)
	if !ok {
		return
	}
	parentSpan := val.(opentracing.Span)
	tr := parentSpan.Tracer()
	sp := tr.StartSpan("sql", opentracing.ChildOf(parentSpan.Context()))
	ext.DBType.Set(sp, "sql")
	scope.Set(spanGormKey, sp)
}

func (c *callbacks) after(scope *gorm.Scope, operation string) {
	val, ok := scope.Get(spanGormKey)
	if !ok {
		return
	}
	sp := val.(opentracing.Span)
	if operation == "" {
		operation = strings.ToUpper(strings.Split(scope.SQL, " ")[0])
	}
	ext.Error.Set(sp, scope.HasError())
	ext.DBStatement.Set(sp, scope.SQL)
	sp.SetTag("db.table", scope.TableName())
	sp.SetTag("db.method", operation)
	sp.SetTag("db.err", scope.HasError())
	sp.SetTag("db.vars", scope.SQLVars)
	sp.SetTag("db.count", scope.DB().RowsAffected)
	sp.Finish()
}

func registerCallbacks(db *gorm.DB, name string, c *callbacks) {
	beforeName := fmt.Sprintf("tracing:%v_before", name)
	afterName := fmt.Sprintf("tracing:%v_after", name)
	gormCallbackName := fmt.Sprintf("gorm:%v", name)
	// gorm does some magic, if you pass CallbackProcessor here - nothing works
	switch name {
	case "create":
		db.Callback().Create().Before(gormCallbackName).Register(beforeName, c.beforeCreate)
		db.Callback().Create().After(gormCallbackName).Register(afterName, c.afterCreate)
	case "query":
		db.Callback().Query().Before(gormCallbackName).Register(beforeName, c.beforeQuery)
		db.Callback().Query().After(gormCallbackName).Register(afterName, c.afterQuery)
	case "update":
		db.Callback().Update().Before(gormCallbackName).Register(beforeName, c.beforeUpdate)
		db.Callback().Update().After(gormCallbackName).Register(afterName, c.afterUpdate)
	case "delete":
		db.Callback().Delete().Before(gormCallbackName).Register(beforeName, c.beforeDelete)
		db.Callback().Delete().After(gormCallbackName).Register(afterName, c.afterDelete)
	case "row_query":
		db.Callback().RowQuery().Before(gormCallbackName).Register(beforeName, c.beforeRowQuery)
		db.Callback().RowQuery().After(gormCallbackName).Register(afterName, c.afterRowQuery)
	}
}

func Close() {
	_ = db_write.Close()
}
```

### 预览

![](./assert/opentracing.png)