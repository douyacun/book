---
Title: go opentracing 实践与落地
Keywords: opentracing,gin,gorm,elasticsearch,jaeger
Description: 分布式全链路日志追踪
Date: 2020-05-04 00:45:42
LastEditTime: 2020-05-06 10:23:00
---

[toc]

### 了解trace日志

推荐:

- [异构系统链路追踪—滴滴 trace 实践](https://v.qq.com/x/page/p0546gars01.html)   落地实践，推荐

- [opentracing](https://opentracing.io/docs/overview/what-is-tracing/)  必须看
- [opentracing tutorial](https://github.com/yurishkuro/opentracing-tutorial) 可以不看
- [The OpenTracing Specification repository](https://github.com/opentracing/specification) 命名规范
- [go elasticsearch trace](https://www.elastic.co/guide/en/apm/agent/go/current/introduction.html) 收集es查询日志
- [使用阿里云sls存储](https://github.com/aliyun/aliyun-log-jaeger) 公司一般都是用sls    

### 开源库

- [jaeger](https://www.jaegertracing.io/docs/1.17/)

jaeger实现了全套服从，收集到存储，我用es实现了一套，es提供个全套的docker服务[(All In One)](https://www.jaegertracing.io/docs/1.17/getting-started/)，整体还是不清楚，毕竟agent也不会和collector放在同一台机器上，不过我本地也没有多个机器，只能用一下docker

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