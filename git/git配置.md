* git配置

### crlf 和 lf的问题

关闭默认转crlf使用lf的换行符
```
git config core.autocrlf false
```

## 可能是.gitignore写得有问题，需要找出来到底哪个规则写错了，可以用git check-ignore命令检查：
git check-ignore -v App.class


## 如果你确实想添加该文件，可以用-f强制添加到Git：
$ git add -f App.class