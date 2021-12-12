---
Title: "PDF工具箱教程"
Keywords: "PDF工具箱,PDF去水印,PDF转Word,PDF强力水印,PDF添加/解除密码"
Description: "PDF工具箱有PDF去水印、PDF转Word功能，支持windows、mac的桌面工具类软件"
---

PDF工具箱功能：

[toc]

下载地址：

- [windows](https://www.douyacun.com/s/pdf_tools.exe)
- [mac](https://www.douyacun.com/s/pdftools)

# 一、PDF去水印-专业版

**先介绍常见的PDF水印：**

- 文字水印，<img src="assert/image-20211212103433649.png" alt="image-20211212103433649" style="zoom:20%;" />可以选中，并且文字可以复制出来（空空说考研）的，就是文字水印，此类水印一般是在正文图层下方



- 图片水印，此类水印需要分为2种
  - 正文中嵌入水印图片<img src="assert/image-20211212104737497.png" alt="PDF嵌入图片水印" style="zoom:20%;" />，典型的像是扫描全能王的水印
  - 正文和水印是一张图片，这种水印是需要图片去水印的，目前PDF还不支持图片去水印
- 图形水印，<img src="assert/image-20211212105105880.png" alt="PDF图形水印" style="zoom:10%;" />将unicode文字用贝尔曲线画出来的图形（画图工具嵌入的文字就是贝尔曲线），可以选中但是文字复制不出来，这种水印是难以识别的，文字转图形以后就无法在转回来了。不过软件可以将这块内容标记出来，供大家判别是否为水印。

**去水印步骤（windows与mac一致）：**

1. 选择PDF文件

![image-20211212102644457](assert/image-20211212102644457.png)

选择文件以后软件会分析PDF文件中的文字水印、图片水印、图形水印

2. 图片水印

<img src="assert/image-20211212110159219.png" alt="image-20211212110159219" style="zoom:30%;" />

软件会提取嵌入在正文中的图片，如上图，像这种水印是和正文分开的，是可以去掉的。

但如果像是下面这种，正文文字和水印在同一张图片上是去不掉的。常见的就是扫描版的图书

<img src="assert/image-20211212111019997.png" alt="image-20211212111019997" style="zoom:30%;" />

3. 文字水印

选择文件后软件分析PDF最多前10页的所有文字段落并统计出现次数：<img src="assert/image-20211212103433649.png" alt="image-20211212103433649" style="zoom:20%;" />

<img src="assert/image-20211212111432056.png" alt="image-20211212111432056" style="zoom:30%;" />

大家可以用这个功能来删除文字

4. 图形水印

选择文件后会分析PDF中的文字图形水印，然后将其截图标记出来，如下图：

<img src="assert/image-20211212112341388.png" alt="image-20211212112341388" style="zoom:30%;" />

5. 预览保存

预览保存，会将判别为水印的文字移除掉，可以预览去水印效果是否能去掉水印，预览效果只会处理一次，再次预览不会二次处理，需要重选文件后预览。

<img src="assert/image-20211212112630337.png" alt="image-20211212112630337" style="zoom:20%;" />

# 二、PDF转Word

使用步骤：

1. 选择文件

<img src="assert/image-20211212113436540.png" alt="PDF转word选择图片" style="zoom:50%;" />

2. 保存为docx

<img src="assert/image-20211212113541485.png" alt="image-20211212113541485" style="zoom:30%;" />

控制台会显示处理进度：

<img src="assert/image-20211212113600615.png" alt="image-20211212113600615" style="zoom:30%;" />

# TODO

## PDF强力水印

## PDF添加/解除密码

## PDF截图（导出为图片）

## PDF重排版
