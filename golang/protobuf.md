---
Title: protobuf技巧总结
Keywords: protobuf,import
Description: protobuf使用过程中总结的经验，import如何使用？不同包的import如何处理？
Date: 2020-07-19 22:46:00
LastEditDate: 2020-07-19 22:46:00
---



文档:

- [Language Guide (proto3)](https://developers.google.com/protocol-buffers/docs/proto3)



# protobuf import

import的用法，文档只给出了一个import public的用法，目的是为了解决proto文件移动了位置或者换了名字的话需要修改所有的调用的import，详细内容可以看一下文档。

这里我遇到的问题：

1. 分文件定义message, import如何使用?
2. 不同的包message如何import

## import用法





