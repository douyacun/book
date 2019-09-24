---
Title: linux内核参数优化_参数解释
Keywords: linux内核参数优化
Description: 
Author: douyacun
Date: 2019-03-03
LastEditTime: 2019-03-03
---

```
fs.file-max=999999
net.ipv4.tcp_tw_reuse=1
net.ipv4.tcp_keepalive_time=600
net.ipv4.tcp_fin_timeout=30
net.ipv4.tcp_max_tw_buckets=5000
net.ipv4.ip_local_port_range=1024 61000
net.ipv4.tcp_rmem=4096 32768 262142
net.ipv4.tcp_wmem=4096 32768 262142
net.core.netdev_max_backlog=8096
net.core.rmem_default=262144
net.core.wmem_default=262144
net.core.rmem_max=2097152
net.core.wmem_max=2097152
net.ipv4.tcp_syncookies=1
net.ipv4.tcp_max_syn_backlog=1024
```

- file-max: 这个参数表示进程可以同时打开的最大句柄数，这个参数直接限制最大并发连接数，根据世纪情况设置
- tcp_tw_reuse: 设置为1，允许将time-wait状态的socket重新用于新的tcp连接，对于服务器来说很有意义， 因为服务器上总会有大量的time-wait状态的连接
- tcp_keepalive_time: 表示当keepalive开启时，tcp发送keepalive的频度，默认时2小时，若设置的小一点，可以更快的清理无效的连接
- tcp_fin_timeout: 表示当服务器主动关闭连接时，socket保持在FIN—WAIT—2状态的最大时间
