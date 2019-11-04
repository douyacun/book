---
Title: spawn ENOMEM错误
Keywords: spawn ENOMEM,pm2 启动失败
Description: 重启了一下服务器，pm2 list之后一直保存spawn ENOMEM,最终定位是系统没有启动swap分区到知道
Author: douyacun
Cover: 
Label: swap spawn ENOMEM
Date: 2019-10-11 22:09:42
LastEditTime: 2019-10-11 22:20:14
---

pm2 list 报错
```
child_process.js:935
  throw errnoException(process._errno, 'spawn');
        ^

Error: spawn ENOMEM
  at errnoException (child_process.js:988:11)
  at ChildProcess.spawn (child_process.js:935:11)
  at Object.exports.spawn (child_process.js:723:9)
  at module.exports ([...]/node_modules/zbarimg/index.js:19:23)
```
# 定位swap分区未配置

free -m 
```
              total        used        free      shared  buff/cache   available
Mem:           1837         282        1408           0         146        1404
Swap:          0           0        0
```

swap大小为0导致node报错spawn ENOMEM,接下来就是挂在swap分区就好了,`swapon`可以检查系统是否已经配置过swap了，-s可以列出当前存储设备上swap使用情况
```
[www@00h www]$ swapon -s
```
如果什么也没有代表该系统尚未配置过swap。
```
[www@00h www]$ free -m
              total        used        free      shared  buff/cache   available
Mem:           1837         283        1407           0         147        1403
Swap:          0            0          0
```
这里swap total为0，同样说明系统没有配置过swap

# 检查存储空间，创建swap文件
```
[www@00h www]$ df -h
文件系统        容量  已用  可用 已用% 挂载点
/dev/vda1        79G   14G   61G   19% /
devtmpfs        909M     0  909M    0% /dev
tmpfs           919M     0  919M    0% /dev/shm
tmpfs           919M  440K  919M    1% /run
tmpfs           919M     0  919M    0% /sys/fs/cgroup
tmpfs           184M     0  184M    0% /run/user/1001
```

创建swap文件，分配600权限，启用swap文件
```
sudo fallocate -l 4G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```
确认是否生效
```
[www@00h www]$ swapon -s
文件名				类型		大小	已用	权限
/swapfile                              	file	4194300	0	-2
```