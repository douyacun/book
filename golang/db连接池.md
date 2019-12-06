---
Title: go数据库连接池
Keywords: 数据库连接池源码分析
Description: db连接池理解，疑难解惑
Label: 数据库连接池
Date: 2019-12-03 20:00:53
LastEditTime: 2019-12-06 14:51:01
---

**go连接mysql为什么需要 import _ "github.com/go-sql-driver/mysql"**

go中import _的作用只执行引入包的init函数，那么go-sql-driver/mysql 的init函数又做了什么，在`database/sql` 中的`drivers map[string]driver.Driver`注册引擎 mysql => MySQLDriver{}

```go
// go-sql-driver/mysql/driver.go
func init() {
	sql.Register("mysql", &MySQLDriver{})
}
```


**SetMaxIdleConns/SetMaxOpenConns/SetConnMaxLifetime 设置的值有什么用呢？**

- db.maxLifetime 连接从创建开始存活的时间，mysql默认tcp连接的超时时间 8h
- db.maxOpen 打开的连接最大数量，超过该数量后，query会被阻塞等待可用连接
- db.maxIdle 空闲池维持的最大连接数量


**sql.Open为什么只需要一次调用即可?**

1. 加载驱动程序 go-sql-driver/mysql
2. 初始化DB数据结构
3. 构造创建连接channel/重置连接channel

这里并有实际去和数据建立连接，也没有对数据库连接的参数校验，只是初始化了DB结构体，DB结构体中已经包含了连接池 freeConn []*driverConn, 没有必要多次调用open，全局维护一个DB即可，如果需要验证 账户密码/网络是否能通信，需要调用ping来检测。

```go
type DB struct {
	waitDuration int64 // 统计每次链接的等待时间

	connector driver.Connector

	// 已经关闭的连接数量
	numClosed uint64

	mu sync.Mutex

	freeConn     []*driverConn               // 空闲池
	connRequests map[uint64]chan connRequest // 等待连接队列，当前连接达到maxOpen时，就无法不在创建连接，创建一个请求的channel入队列并等待并阻塞当前goroutine
	nextRequest  uint64                      // connRequests[] 指向下一个坑位
	numOpen      int                         // 正在使用的连接数量

	// channel 通过此channel来接受建立新的session连接
	// DB.connectionOpener for select 接收channel
	openerCh          chan struct{}
	resetterCh        chan *driverConn       // 重置session以便其他query复用当前session
	closed            bool                   // 标记是否已经关闭链接
	dep               map[finalCloser]depSet //
	lastPut           map[*driverConn]string // 调试用
	maxIdle           int                    // 维持的最大空闲连接数，小于等于0使用defaultMaxIdleConns（2)
	maxOpen           int                    // 维持的最大连接数，0无限制
	maxLifetime       time.Duration          // 连接可以重用的最长时间
	cleanerCh         chan struct{}
	waitCount         int64 // 等待连接的总数
	maxIdleClosed     int64 // 由于空闲而连接的总数
	maxLifetimeClosed int64 // 连接存活时间超过maxLifetime而关闭的时间

	stop func() // 取消接收 建立连接channel(openerCh)/重置session channel(resetterCh)
}
```



**如果获取数据连接池的状态？当前的连接总数，SetMaxOpenConns/SetMaxIdleConns的数量是否合适。**

使用db.Stats可以查看当前连接池的一些状态,这边返回了一个DBStats结构体，一起看下:

```go
type DBStats struct {
	MaxOpenConnections int // 打开的最大连接数，包含已经关闭了的连接

	// 连接池状态
	OpenConnections int // 当前建立连接的数量，包括正在使用和空闲的数量
	InUse           int // 正在使用的连接数
	Idle            int // 已建立连接，空闲中的连接数

	// 统计
	WaitCount         int64         // 等待连接的总数，这里需要着重关注一下
	WaitDuration      time.Duration // query持续等待连接的总时长
	MaxIdleClosed     int64         // 达到SetMaxIdleConns，而关闭的连接数量
	MaxLifetimeClosed int64         // 达到SetConnMaxLifetime. 而关闭的连接数量
}
```

- WaitCount  着重关注一下，看下是不是有慢查询阻塞了可用的连接数量
- MaxIdleConns设置的过小，而请求数过多导致，会导致MaxIdleClosed比较大，WaitCount也会比较大

注意：db.Stats不要调用过于频繁，它会对整个DB连接池加锁，过于频繁有一定开销。



**DB.Ping()做了哪些事情？熟悉一下高大上的连接池记录了哪些属性。**

1. 获取连接，可以复用连接 （cachedOrNewConn）
2. 获取一个可用的连接 driverConn
   1. 复用空闲池中freeConn已有的连接
      1. 从空闲池中移除第一个连接conn
      2. 这期间都是有锁的，freeConn是一个切片，是并发不安全的
      3. 该连接是否在生命周期内 lifetime -- db.SetLifeTime设置的tcp连接存活时间
         - 本连接过了生命周期，返回 driver.ErrBadConn
      4. 返回此conn
   2. db.SetMaxOpenConns设置了最大打开的连接数，且当前打开的连接已经达到最大数
      1. 创建一个等待请求，放入等待队列，阻塞当前goroutine
      2. 等待超时使用context取消，或者等待直到获取可用的连接
         - ctx取消后还是获取到了连接，放回空闲池
      3. 获取到可用连接，统计本次阻塞时长，可以注意到如果DB.Stats().WaitDuration大了以后问题就很严重了
         - 如果本连接过了生命周期，返回 driver.ErrBadConn
      4. 返回此Conn
   3. 封装driver返回的connect到driverConn
      1. 标记driverConn inUse使用中
      2. 记录连接创建时间createdAt
      3. db指向连接池
      4. 真正底层的连接

源码小细节：

- 移除切片第一个元素：`copy(db.freeConn, db.freeConn[1:])`
- 删除map中的元素:  delete(db.connRequests, reqKey)
- map[key]interface: 也可以使用channel做为key
- 生命周期判断：createdAt.Add(lifetime).Before(time.now()) 

- 参观一下context的用法

```go
select {
// 这里留取消的口
case <-ctx.Done():
  select {
  // 之前我们分析过select尽量不要加default，单那是for select结构，会造成自旋锁，长期占用M不释放
  // 如果这里不用default它就阻塞一直等待req channel中有connect，这里并不是为了等待，只是为了清理一下channel的connect，防止孤儿connect
  default:
	case ret, ok := <-req:
		//...
  }
  return nil, ctx.Err()
case ret, ok := <-req:
  // ...
  return ret.conn, ret.err
}

// 我们在外面如何控制超时呢？
fun bar(){
  t := time.After(10)
  ctx := context.Background()
  res := make(chan struct{})
  go func() {
    ci, _ := conn(ctx)
    res<-ci
  }()
  select {
  case <-t:
    ctx.Done()
  case ci := <-res:
    if ci != nil {

    }
  }
}
```

