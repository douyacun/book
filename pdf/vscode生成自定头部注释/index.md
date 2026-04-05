---
Title: vscode生成自定头部注释
LegacyId: 0adfd91209e17697b69fe14ee29ef414
topic: pdf
Keywords: VSCode头部注释, fileheader, 自动注释模板, setting.json, 插件配置
Label: VSCode, 开发工具
description: 这篇文章记录在 VSCode 中使用 fileheader 插件自动生成文件头注释的配置方法和常用参数。
Date: 2019-09-24T20:04:29+08:00
LastEditTime: 2019-09-24T20:04:29+08:00
Publish: false
Lang: zh
---

[fileheader](https://github.com/OBKoro1/koro1FileHeader)

vscode配置, code/首选项/设置 - 搜索 `fileheader` - 在`setting.json`中配置
```json
{
  "fileheader.configObj": {
      "autoAdd": true,
      "timeNoDetail": true,
      "language": {
          "markdown": {
              "head": "---",
              "middle": "",
              "end": "---"
          }
      },
  },
  "fileheader.customMade": {
      "Title": "",
      "Keywords": "",
      "Description": "",
      "Author": "douyacun",
      "Date": "",
      "LastEditTime": "", 
  }
}
```

# 快捷键
文件头部添加注释:
- windows: ctrl+alt+i
- mac: ctrl+cmd+i 

在光标处添加函数注释:
- windows: ctrl+alt+t
- mac: ctrl+cmd+t
