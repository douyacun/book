---
Title: "go面试题：闭包函数指针引用 & 标准输出阻塞"
Keywords: "闭包函数,指针引用,标准输出是阻塞"
Description: "1. 闭包指针引用外部变量 2. 标准输出会阻塞"
Label: "go面试题"
---

面试题:

```go
package main

func main() {
	m := make(map[string]*student)
	stus := []*student{
		&student{Name: "zhou", Age: 24},
		&student{Name: "li", Age: 23},
		&student{Name: "wang", Age: 22},
	}
	l := sync.RWMutex{}
	wg := sync.WaitGroup{}
	for _, stu := range stus {
		wg.Add(1)
		go func() {
			defer wg.Done()
			l.Lock()
			local := stu
			// fmt.Println(local)
			m[local.Name] = local
			l.Unlock()
		}()
		//fmt.Println("111")
	}
	wg.Wait()
	fmt.Printf("%+v", m)
}
```

第一次测试输出：

```
map[wang:0xc00000c060]
```

第二次，main gouroutine 是不是和 其他的gouroutine并行, 在 循环中 下打印  `fmt.Println("111")`， 在 goroutine中 打印 stu 看看啥情况，多次运行都是：

```
111
111
111
{zhou 24}
{wang 22}
{wang 22}
map[wang:0xc0000a6020]
```

第一次的输出还是可以想到 闭包指针引用外部变量，那这第二次开始怀疑人生了。我又试了试注释掉 `fmt.Println(local)`

```
第一次:
111
111
111
map[wang:0xc00000c060 zhou:0xc00000c060]

第二次:
111
111
111
map[li:0xc00000c060 zhou:0xc00000c060]
```

见鬼了...



这个问题平常开发也是经常会犯错误的，不过goland可以给出提示出来

1. 闭包引用外部变量是 指针引用，参数传值是值复制

2. main gourouine 和 其他goroutine 都是并发运行的

3. 标准输出阻塞的，所以我们每次打印内容以后导致和想象的结果不一样了

    