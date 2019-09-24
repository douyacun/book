---
Title: vscode_fileheader_自定义[markdown]注释 
Keywords: vscode,fileheader,自定义注释,markdown头部注释
Description: fileheader生成注释,方便markdown解析seo优化
Author: douyacun
Date: 2019-02-01
LastEditTime: 2019-02-01
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
