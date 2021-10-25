---
Title: "go reflect用法总结"
Keywords: "go reflect用户总结"
Description: "go reflect即使不用也可以写代码完成需求，要是想封装公共高可用的方法还是需要用的reflect包"
Label: "go reflect"
Author: douyacun
Cover: "assert/go_reflect_cover.jpeg"
Date: "2021-10-25 21:47:31"
LastEditTime: "2021-10-25 21:47:31"
---

**背景**

写go已经接近3年了，最开始的1年其实就没接触过reflect包，每天都会写重复的代码，就想封装公共部分，go不像php那样一个数组走天下，不同的结构体赋值就需要用到reflect包了。

**概要**

1. 遍历结构体字段
2. 遍历slice
3. 复制一个新的结构体
4. 创建一个指针类型的结构体
5. 创建一个指定类型的数组
6. 结构体tag用法



