---
Title: "go语言struct能否作为map的key"
Keywords: "go,struct,map,key"
Description: "数据库有联合主键，取出数据后存在map里面按主键直接取数据"
---

### 背景：

数据库是联合主键，取出数据后存在map里面按主键直接取数据，以 date + campaign_id + adgroup_id + ad_id 作为唯一索引。以 struct做为map key是可以的，但是struct中有 time.time 不确定是否可行。

官方给出的答案是：key1 == key2 ，能否可以比较

实践了几种需要作为hash map key的情形：

1. pointer
2. struct
3. channel
4. slice



### 实践：

**struct 作为map key**

struct包含time、int64 ，2者都是可以相互比较的，struct就是可比较的。

```go
type TimeStruct struct {
	Date time.Time
	Id   int64
}

func main() {
	dat, _ := time.Parse("2006-01-02", "2021-06-22")

	t1 := TimeStruct{
		Date: dat,
		Id:   1,
	}

	t2 := TimeStruct{
		Date: dat,
		Id:   1,
	}

	mapList := map[TimeStruct]int64{
		TimeStruct{
			Date: dat,
			Id:   1,
		}: 1,
		TimeStruct{
			Date: dat,
			Id:   2,
		}: 2,
	}

	fmt.Println("> 输出：", mapList[t1], mapList[t2])
}

> 输出：1 1
```

在试一下结构体中包含 sync.mutex

```go
type TimeStruct struct {
  Date time.Time
  Id   int64
  mu   sync.Mutex
}
fmt.Println("> 输出：", mapList[t1], mapList[t2])
> 输出： 1 1
```

mutex 同样是一个结构体, 里面包含state，sema都是可以比较的。所以包含mutex也是可以比较。

```go
type Mutex struct {
	state int32
	sema  uint32
}
```



**pointer作为map key**

指针在go中是可以进行比较的，但是不能运算，理论上也是可以作为map key。但是不太建议作为map key取值。

指针指向的是内存地址。内存地址一样才是相同的。即使结构体的值一样，如果属于2个不同的变量，那么内存地址就是不一样的无法作为取到 map value的。

t2就是直接变量指针作为key，t2可以取到对应的值，t1与map 第一个key内存地址不一致无法取到对应的值。

```go
type TimeStruct struct {
		Date time.Time
		Id   int64
		mu   sync.Mutex
	}
	dat, _ := time.Parse("2006-01-02", "2021-06-22")
	t1 := TimeStruct{
		Date: dat,
		Id:   1,
		mu:   sync.Mutex{},
	}
	t2 := &TimeStruct{
		Date: dat,
		Id:   2,
		mu:   sync.Mutex{},
	}
	mapList := map[*TimeStruct]int64{
		&TimeStruct{
			Date: dat,
			Id:   1,
			mu:   sync.Mutex{},
		}: 1,
		t2: 2,
	}
	fmt.Println("> 输出：", mapList[&t1], mapList[t2])

> 输出： 0 2
```



**channel struct作为map key**

go make(chan string) 以后返回的是指针。channel作为map key 可以和 指针结构体作为map key 看作是同一个问题:

```go
fmt.Printf("%+v", make(chan string, 0))

0xc00008c060
```



**slice struct 作为map key**

看了make chan返回指针以后，make slice也是返回的指针？不是

slice不能作为map key，slice 无法进行相等比较的

Invalid operation: t1 == t2 (the operator == is not defined on []string)