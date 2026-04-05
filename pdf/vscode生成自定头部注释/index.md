---
Title: vscode生成自定头部注释
LegacyId: 0adfd91209e17697b69fe14ee29ef414
Slug: vscode生成自定头部注释
Category: pdf
Summary: '[fileheader](https://github.com/OBKoro1/koro1FileHeader) vscode配置, code/首选项/设置
  - 搜索 `fileheader` - 在`setting.json`中配置 ``'
SeoTitle: vscode生成自定头部注释
SeoDescription: '[fileheader](https://github.com/OBKoro1/koro1FileHeader) vscode配置,
  code/首选项/设置 - 搜索 `fileheader` - 在`setting.json`中配置 ``'
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
