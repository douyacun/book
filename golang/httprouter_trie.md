---
Title: go httprouter_trie树_前缀树_字典树
Keywords: trie,go实现,前缀树
Description: go httprouter路由使用trie来实现快速查找对应的路由，学习一下trie的数据结构方便理解
Author: douyacun
Date: 2019-06-19 08:53:45
LastEditTime: 2019-08-22 21:33:06
typora-root-url: ./asserts
---

只是个人学习笔记，在学习go httprouter时，路由匹配用到trie树，在此总结一下

[TOC]

# httprouter特性

### Router struct

```go
type Router struct {
	trees map[string]*node
  // 自动重定向：
  // 如果没有匹配到路由,去掉尾随的/
  // http://localhost:8000/image/ 301重定向 http://localhost:8000/image
	RedirectTrailingSlash bool
	// 路由矫正
  // 如果路由没有匹配到, 会删除路由中的../ 和 //
  // //Foo 和 /../Foo 会301重定向到 /foo
	RedirectFixedPath bool
	// 请求方法是否允许
	HandleMethodNotAllowed bool
  // 自动回复options请求，如果有路由匹配优先级高于自动回复，建议开启
	HandleOPTIONS bool
  // 路由没有匹配到时调用这个路由，如果没有定义的话response: 404 Page Not Found
	NotFound http.Handler
  // 当一个请求是不被允许的，并且上面的 HandleMethodNotAllowed 设置为 ture 的时候，
  // 如果这个参数没有设置，将使用状态为 with http.StatusMethodNotAllowed 的 http.Error
  // 在 handler 被调用以前，为允许请求的方法设置 "Allow" header 
	MethodNotAllowed http.Handler
  // 异常处理函数，相当于 PHP的error_handler()
	PanicHandler func(http.ResponseWriter, *http.Request, interface{})
}
```



### 为什么要用http router

+ 路由显示匹配，相比之下httprouter没有非预期匹配，更符合人类的思维习惯

  + http://localhost:8080/image/  预期匹配router1，但是响应是: 404 page not found，pattern(/image)少了后面的`/`
  + http://localhost:8080/image/xxxx  预期`404`，但是响应`image`，子串匹配规则`/image/*`都会匹配到router2
  + http://localhost:8080/image/thumbnails/ 预期`Thumbnails`, 响应`Thumbnails`,长匹配规则，匹配到了`/image/thumbnails/`，优先使用

  ```go
  func main() {
  	// http.HandleFunc("/image", Image) // router1
    http.HandleFunc("/image/", Image) // router2
  	http.HandleFunc("/image/thumbnails/", Thumbnails)
  	log.Fatal(http.ListenAndServe(":8080", nil))
  }
  func Image(w http.ResponseWriter, r *http.Request) {
  	fmt.Fprint(w, "image\n")
  }
  func Thumbnails(w http.ResponseWriter, r *http.Request) {
  	fmt.Fprintf(w, "Thumbnails")
  }
  ```

  + 不需要关心是否以`/`结尾，httprouter回自动301重定向客户端url去掉`/`,可以通过`Router.RedirectTrailingSlash`配置是否开启自动重定向
  + 路由自动矫正，删除多余的路径元素(`../`,`//`)
    + http://localhost:8080// 匹配到`router1`
    + http://localhost:8080//FOO 会301到 http://localhost:8080/foo `router2`

  ```go
  func Index(w http.ResponseWriter, r *http.Request, _ httprouter.Params) {
  	fmt.Fprint(w, "Welcome!\n")
  }
  func Hello(w http.ResponseWriter, r *http.Request, ps httprouter.Params) {
  	fmt.Fprintf(w, "hello, %s!\n", ps.ByName("name"))
  }
  func main() {
  	router := httprouter.New()
  	router.GET("//", Index) // router1
    router.GET("/foo", Index) // router2
    router.GET("/FOO", Index) // router3
  	router.GET("/hello/:name", Hello) // router4
  	log.Fatal(http.ListenAndServe(":8080", router))
  }
  ```

  ```chro
  Request URL: http://localhost:8080//FOO
  Request Method: GET
  Status Code: 301 Moved Permanently
  Remote Address: [::1]:8080
  Referrer Policy: no-referrer-when-downgrade
  ```

+ 支持动态路由参数, http://localhost:8080/hello/douyacun ， 会输出 `hello, douyacun!`

+ 匹配和调度过程生成零字节的垃圾,唯一的堆分配是为路径参数构建键值对的切片，以及构建新的上下文和请求对象

+ 全局recover, 某个请求`panic`,不至于整个服务挂掉，`Router.PanicHandler`

+ RESTful API,内置options请求(现在的前端axios、fetch请求，都会预先发一个options请求，获取allow)



# 数据结构 - Radix tree

### Radix tree

radix tree是一种多叉搜索树，树的叶子结点是实际的数据条目。每个结点有一个固定的、2^n指针指向子结点（每个指针称为槽slot，n为划分的基的大小）

### 特点

- 根节点不包含字符，除跟节点外每个节点都只包含一个字符
- 从跟节点到某一节点，路径上经过的字符拼接起来，为该节点对应的字符串
- 每隔节点包含的字节点包含的字符都不相同

### 图



### httprouter中应用

看数据结构一定要看图

```table
Priority   Path             Handle
9          \                *<1>
3          ├s               nil
2          |├earch\         *<2>
1          |└upport\        *<3>
2          ├blog\           *<4>
1          |    └:post      nil
1          |         └\     *<5>
2          ├about-us\       *<6>
1          |        └team\  *<7>
1          └contact\        *<8>
```

每个method都有一个初始化node节点，

```go
func (r *Router) Handle(method, path string, handle Handle) {
	if path[0] != '/' {
		panic("path must begin with '/' in path '" + path + "'")
	}
	if r.trees == nil {
		r.trees = make(map[string]*node)
	}
	root := r.trees[method]
	if root == nil {
		root = new(node)
		r.trees[method] = root
	}
	root.addRoute(path, handle)
}
```

