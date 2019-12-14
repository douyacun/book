```javascript
# 加载任务, -w选项会将plist文件中无效的key覆盖掉，建议加上
$ launchctl load -w com.demo.plist

# 删除任务
$ launchctl unload -w com.demo.plist

# 查看任务列表, 使用 grep '任务部分名字' 过滤
$ launchctl list | grep 'com.demo'


# 开始任务
$ launchctl start  com.demo.plist

# 结束任务
$ launchctl stop   com.demo.plist
```

