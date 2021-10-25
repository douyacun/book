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

运营同学经常会找我拉取一些数据，这种情况肯定要给她们出一个excel导出功能让她们能自己解决，需求无非是excel中展示行列数据，甚至不会有合并单元格这么高深的操作。

这时如果有一个方法只需要我给出文件名和数据就可以下载表格就很开心了。

推荐使用go的excel库：[excelize](https://github.com/xuri/excelize)

**场景**

1. 导出excel表格
2. sql查询条件（告别if else）

**概要**

1. 遍历结构体字段和tag
2. 遍历slice
3. 复制一个新的结构体
4. 创建一个指针类型的结构体
5. 创建一个指定类型的数组



## 一、遍历结构体字段和tag



