---
Title: go time常用方法
Keywords: time
Description: 学习go time提供的方法，实现方式
Author: douyacun
Label: time
Date: 2019-12-11 13:53:33
LastEditTime: 2019-12-11 19:22:05
---

[TOC]

# 	时间格式常量

## 标准

```go
const (
    ANSIC       = "Mon Jan _2 15:04:05 2006"
    UnixDate    = "Mon Jan _2 15:04:05 MST 2006"
    RubyDate    = "Mon Jan 02 15:04:05 -0700 2006"
    RFC822      = "02 Jan 06 15:04 MST"
    RFC822Z     = "02 Jan 06 15:04 -0700" // RFC822 with numeric zone
    RFC850      = "Monday, 02-Jan-06 15:04:05 MST"
    RFC1123     = "Mon, 02 Jan 2006 15:04:05 MST"
    RFC1123Z    = "Mon, 02 Jan 2006 15:04:05 -0700" // RFC1123 with numeric zone
    RFC3339     = "2006-01-02T15:04:05Z07:00"
    RFC3339Nano = "2006-01-02T15:04:05.999999999Z07:00"
    Kitchen     = "3:04PM"
    // Handy time stamps.
    Stamp      = "Jan _2 15:04:05"
    StampMilli = "Jan _2 15:04:05.000"
    StampMicro = "Jan _2 15:04:05.000000"
    StampNano  = "Jan _2 15:04:05.000000000"
)
```

## 时/分/秒/毫秒/微妙/纳秒

```go
const (
    Nanosecond  Duration = 1
    Microsecond          = 1000 * Nanosecond
    Millisecond          = 1000 * Microsecond
    Second               = 1000 * Millisecond
    Minute               = 60 * Second
    Hour                 = 60 * Minute
)
```

## 月份

```go
const (
	January Month = 1 + iota
	February
	March
	April
	May
	June
	July
	August
	September
	October
	November
	December
)
```

## 星期几

```go
const (
	Sunday Weekday = iota
	Monday
	Tuesday
	Wednesday
	Thursday
	Friday
	Saturday
)
```

# 超时控制 After

```go
var c chan int

func handle(int) {}

func main() {
	select {
	case m := <-c:
		handle(m)
	case <-time.After(10 * time.Second):
		fmt.Println("timed out")gg
	}
}
```

# 时间间隔 Duration

time.Add(t Duration)传参需要使用Durtaion 

## time.Sub

计算两个时间差，返回duration

```go
func expensiveCall() {}

func main() {
	t0 := time.Now()
	expensiveCall()
	t1 := time.Now()
	fmt.Printf("The call took %v to run.\n", t1.Sub(t0))
}
```

## ParseDuration

有效符号："ns", "us" (or "µs"), "ms", "s", "m", "h".

```go
func main() {
	hours, _ := time.ParseDuration("10h")
	complex, _ := time.ParseDuration("1h10m10s")
	micro, _ := time.ParseDuration("1µs")
	// The package also accepts the incorrect but common prefix u for micro.
	micro2, _ := time.ParseDuration("1us")

	fmt.Println(hours)
	fmt.Println(complex)
	fmt.Printf("There are %.0f seconds in %v.\n", complex.Seconds(), complex)
	fmt.Printf("There are %d nanoseconds in %v.\n", micro.Nanoseconds(), micro)
	fmt.Printf("There are %6.2e seconds in %v.\n", micro2.Seconds(), micro)
}
```

# 时间 Time

基础结构体:

```go
type Time struct {
	wall uint64
	ext  int64
	loc *Location
}
```

## Date

返回指定日期的time

```go
func main() {
	t := time.Date(2009, time.November, 10, 23, 0, 0, 0, time.UTC)
	fmt.Printf("Go launched at %s\n", t.Local())
}
// Go launched at 2009-11-11 07:00:00 +0800 CST
```

## 当前时间 Now

返回当前时间,这个用的最多

## 时间解析 Parse

```go
func Parse(layout, value string) (Time, error)
```

Layout 时间格式，value时间值，解析时间返回time

```go
func main() {
	t, _ := time.Parse(time.RFC3339, "2006-01-02T15:04:05Z")
	fmt.Println(t)
	m, _ := time.Parse(time.RFC3339, "2006-01-02T15:04:05+07:00")
	fmt.Println(m)
}
// 2006-01-02 15:04:05 +0000 UTC
// 2006-01-02 15:04:05 +0700 +0700
```

## 时间戳 Unix

解析时间戳为time

```go
func main() {
	t := time.Unix(1576053402, 100000000)
	fmt.Print(t)
}
// 2019-12-11 16:36:42.01 +0800 CST
```

## 格式化 Format

时间格式化

默认输出格式：`2006-01-02 15:04:05.999999999 -0700 MST`

```go
func main() {
	fmt.Printf("当前时间: %s\n", time.Now())
	fmt.Printf("RFC1123Z: %s\n", time.Now().Format(time.RFC1123Z))
	fmt.Printf("RFC3339: %s\n", time.Now().Format(time.RFC3339))
}

// 当前时间: 2019-12-11 16:25:53.987141 +0800 CST m=+0.000513629
// RFC1123Z: Wed, 11 Dec 2019 16:25:53 +0800
// RFC3339: 2019-12-11T16:25:53+08:00
```

## 时间加减 Add

当前时间加上 ？

```go
func main() {
	start := time.Date(2009, 1, 1, 12, 0, 0, 0, time.UTC)
	afterTenSeconds := start.Add(time.Second * 10)
	afterTenMinutes := start.Add(time.Minute * 10)
	afterTenHours := start.Add(time.Hour * 10)
	afterTenDays := start.Add(time.Hour * 24 * 10)

	fmt.Printf("start = %v\n", start)
	fmt.Printf("start.Add(time.Second * 10) = %v\n", afterTenSeconds)
	fmt.Printf("start.Add(time.Minute * 10) = %v\n", afterTenMinutes)
	fmt.Printf("start.Add(time.Hour * 10) = %v\n", afterTenHours)
	fmt.Printf("start.Add(time.Hour * 24 * 10) = %v\n", afterTenDays)
}
```

## 判断时间是否相等 Equal

判断两个时间是否一致

```go
func main() {
	secondsEastOfUTC := int((8 * time.Hour).Seconds())
	beijing := time.FixedZone("Beijing Time", secondsEastOfUTC)

	// Unlike the equal operator, Equal is aware that d1 and d2 are the
	// same instant but in different time zones.
	d1 := time.Date(2000, 2, 1, 12, 30, 0, 0, time.UTC)
	d2 := time.Date(2000, 2, 1, 20, 30, 0, 0, beijing)

	datesEqualUsingEqualOperator := d1 == d2
	datesEqualUsingFunction := d1.Equal(d2)

	fmt.Printf("datesEqualUsingEqualOperator = %v\n", datesEqualUsingEqualOperator)
	fmt.Printf("datesEqualUsingFunction = %v\n", datesEqualUsingFunction)
}
```

## Before/After

判断时间A是否在时间B之前还是之后，返回bool

```go
func main() {
	a := time.Now()
	b := time.Now()

	fmt.Printf("a 在 b 之前: %v\n", a.Before(b))
	fmt.Printf("a 在 b 之后: %v\n", a.After(b))
}

// a 在 b 之前: true
// a 在 b 之后: false
```

## 时间四舍五入 Round

时间4舍5入

```go
func main() {
	t := time.Date(0, 0, 0, 12, 15, 30, 918273645, time.UTC)
	round := []time.Duration{
		time.Nanosecond,
		time.Microsecond,
		time.Millisecond,
		time.Second,
		2 * time.Second,
		time.Minute,
		10 * time.Minute,
		time.Hour,
	}

	for _, d := range round {
		fmt.Printf("t.Round(%6s) = %s\n", d, t.Round(d).Format("15:04:05.999999999"))
	}
}
```

## 舍去 Truncate

```go
func main() {
	t, _ := time.Parse("2006 Jan 02 15:04:05", "2012 Dec 07 12:15:30.918273645")
	trunc := []time.Duration{
		time.Nanosecond,
		time.Microsecond,
		time.Millisecond,
		time.Second,
		2 * time.Second,
		time.Minute,
		10 * time.Minute,
	}

	for _, d := range trunc {
		fmt.Printf("t.Truncate(%5s) = %s\n", d, t.Truncate(d).Format("15:04:05.999999999"))
	}
	// To round to the last midnight in the local timezone, create a new Date.
	midnight := time.Date(t.Year(), t.Month(), t.Day(), 0, 0, 0, 0, time.Local)
	_ = midnight
}
```

# 定时器 Tick 

```go
func statusUpdate() string { return "" }

func main() {
	c := time.Tick(5 * time.Second)
	for now := range c {
		fmt.Printf("%v %s\n", now, statusUpdate())
	}
}
```

