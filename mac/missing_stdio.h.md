# mac 升级 导致 `stdlib.h` 和 `stdio.h` not found

/usr/include no such file or directory

## 原因
Xcode command line tools 没有安装

```
xcode-select --install
```

之后还是没有 `/usr/include`

- 确认 SIP 关闭
```
$ csrutil status
System Integrity Protection status: disabled.
```
- 关闭 SIP
mac 重新启动 安装 `command` +  `R` 进入恢复模式， 顶部选择命令行工具

```
csrutil disable
```

- 安装 `command line tools`
```
cd /Library/Developer/CommandLineTools/Packages/
open macOS_SDK_headers_for_macOS_10.14.pkg
```

- 查看 `/usr/include`
