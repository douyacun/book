---
Title: 基础数据结构：interface数据结构，源码分析
Keywords: interface,数据结构,源码分析
Description: 
Label: interface
Date: 20202-05-09 14:58:00
LastEditTime: 20202-05-09 14:58:00
---

接口是多个组件共享的边界，不同的组件能够在边界上交换信息，本质是个中间件，接触上下游的耦合，上层模块不再需要依赖下层的具体模块，只需要一个约定好的接口

数据结构、类型转换、类型断言

# 数据结构

使用`iface`表示包含方法的接口，`eface` 结构体表示不包含任何方法的 `interface{}` 类型

`runtime/runtime2.go`

```go
type eface struct {
   _type *_type
   data  unsafe.Pointer
}

type _type struct {
	size       uintptr
	ptrdata    uintptr
	hash       uint32
	tflag      tflag
	align      uint8
	fieldalign uint8
	kind       uint8
	alg        *typeAlg
	gcdata    *byte
	str       nameOff
	ptrToThis typeOff
}
```

-   `_type` : 指向类型的指针
    -   `size` 类型占用的内存空间
    -   `hash` 快速确定类型是否相等
-   `data` : 指向底层数据

```go
type iface struct {
    tab *itab
    data unsafe.Pointer
}

type itab struct {
	inter *interfacetype
	_type *_type
	hash  uint32 // copy of _type.hash. Used for type switches.
	_     [4]byte
	fun   [1]uintptr // variable sized. fun[0]==0 means _type does not implement inter.
}
```

-   `tab` 接口和具体类型的组合
    -   `hash` _type.hash的拷贝，用于快速判断类型是否一致
    -   `func` 动态大小的数组，虽然该变量声明成固定大小的指针，使用时会通过原始指针获取其中数据，func数组中元素的数量时不确定的
-   `data` 指向底层数据

# 类型转换

**指针类型:**

```go
package main
type Duck interface {
	Quack()
}
type Cat struct {
	Name string
}
//go:noinline
func (c *Cat) Quack() {
	println(c.Name + " meow")
}
func main() {
	var c Duck = &Cat{Name: "grooming"}
	c.Quack()
}
```

上述代码的执行过程,:

1.  初始化结构体Cat中name变量: Name.Len = 8 Name.Data = &"grooming"
2.  赋值出发类型转换过程
3.  调用Duck接口的Quack方法

解引用： go编译器可以隐式的对变量解引用获取指针指向的结构体



