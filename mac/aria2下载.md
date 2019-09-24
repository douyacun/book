---
Title: aria2配置_种子下载配置_百度网盘下载配置
Keywords: aria2配置，种子下载加速配置
Description: 
Author: douyacun
Date: 2019-03-05
LastEditTime: 2019-03-05
---

# 常用配置

- [aria2使用说明](http://aria2c.com/usage.html)

`~/.aria2/aria2.conf`
```
## 全局设置 ## ============================================================
# 日志
#log-level=warn
log=/Users/liuning/Downloads/aria2/aria2.log

# 后台运行
daemon=true

# 下载位置, 默认: 当前启动位置
dir=/Users/liuning/Downloads/aria2

# 从会话文件中读取下载任务
input-file=/Users/liuning/.aria2/aria2.session

# 在Aria2退出时保存`错误/未完成`的下载任务到会话文件
save-session=/PATH/.aria2/aria2.session

# 定时保存会话, 0为退出时才保存, 需1.16.1以上版本, 默认:0
save-session-interval=30

# 断点续传
continue=true

# 启用磁盘缓存, 0为禁用缓存, 需1.16以上版本, 默认:16M
#disk-cache=32M

# 文件预分配方式, 能有效降低磁盘碎片, 默认:prealloc
# 预分配所需时间: none < falloc ? trunc < prealloc
# falloc和trunc则需要文件系统和内核支持
# NTFS建议使用falloc, EXT3/4建议trunc, MAC 下需要注释此项
file-allocation=none

# 客户端伪装
user-agent=netdisk;5.2.6;PC;PC-Windows;6.2.9200;WindowsBaiduYunGuanJia
referer=http://pan.baidu.com/disk/home

# 禁用IPv6, 默认:false
disable-ipv6=true

# 其他
always-resume=true
check-integrity=true

## 下载位置 ## ============================================================
# 最大同时下载任务数, 运行时可修改, 默认:5
max-concurrent-downloads=5

# 同一服务器连接数, 添加时可指定, 默认:1
max-connection-per-server=5

# 最小文件分片大小, 添加时可指定, 取值范围1M -1024M, 默认:20M
# 假定size=10M, 文件为20MiB 则使用两个来源下载; 文件为15MiB 则使用一个来源下载
min-split-size=10M

# 单个任务最大线程数, 添加时可指定, 默认:5
split=5

# 整体下载速度限制, 运行时可修改, 默认:0
#max-overall-download-limit=0

# 单个任务下载速度限制, 默认:0
#max-download-limit=0

# 整体上传速度限制, 运行时可修改, 默认:0
#max-overall-upload-limit=0

# 单个任务上传速度限制, 默认:0
#max-upload-limit=0

## RPC设置 ## ============================================================
# 启用RPC, 默认:false
enable-rpc=true

# 允许所有来源, 默认:false
rpc-allow-origin-all=true

# 允许非外部访问, 默认:false
rpc-listen-all=true

# 事件轮询方式, 取值:[epoll, kqueue, port, poll, select], 不同系统默认值不同
#event-poll=select

# RPC监听端口, 端口被占用时可以修改, 默认:6800
rpc-listen-port=6800

# 设置的RPC授权令牌, v1.18.4新增功能, 取代 --rpc-user 和 --rpc-passwd 选项
#rpc-secret=<TOKEN>

# 是否启用 RPC 服务的 SSL/TLS 加密,
# 启用加密后 RPC 服务需要使用 https 或者 wss 协议连接
#rpc-secure=true

# 在 RPC 服务中启用 SSL/TLS 加密时的证书文件,
# 使用 PEM 格式时，您必须通过 --rpc-private-key 指定私钥
#rpc-certificate=/path/to/certificate.pem

# 在 RPC 服务中启用 SSL/TLS 加密时的私钥文件
#rpc-private-key=/path/to/certificate.key

## BT/PT下载相关 ## ============================================================
# 当下载的是一个种子(以.torrent结尾)时, 自动开始BT任务, 默认:true
#follow-torrent=true

# BT监听端口, 当端口被屏蔽时使用, 默认:6881-6999
listen-port=51413

# 单个种子最大连接数, 默认:55
#bt-max-peers=55

# 打开DHT功能, 种子下载必备
enable-dht=true

# 打开IPv6 DHT功能, PT需要禁用
#enable-dht6=false

# DHT网络监听端口, 默认:6881-6999
#dht-listen-port=6881-6999

dht-file-path=/Users/liuning/.aria2/dht.dat
dht-file-path6=/Users/liuning/.aria2/dht6.dat

# 本地节点查找, PT需要禁用, 默认:false
#bt-enable-lpd=false

# 种子交换, PT需要禁用, 默认:true
enable-peer-exchange=false

# 每个种子限速, 对少种的PT很有用, 默认:50K
#bt-request-peer-speed-limit=50K

# 设置 peer id 前缀
peer-id-prefix=-TR2770-

# 当种子的分享率达到这个数时, 自动停止做种, 0为一直做种, 默认:1.0
seed-ratio=0

# 强制保存会话, 即使任务已经完成, 默认:false
# 较新的版本开启后会在任务完成后依然保留.aria2文件
#force-save=false

# BT校验相关, 默认:true
#bt-hash-check-seed=true

# 继续之前的BT任务时, 无需再次校验, 默认:false
bt-seed-unverified=true

# 保存磁力链接元数据为种子文件(.torrent文件), 默认:false
bt-save-metadata=true

bt-max-open-files=16

# 这里自己格式化， 去 https://github.com/ngosang/trackerslist 上自己找， ','分隔
bt-tracker=udp://tracker.coppersurfer.tk:6969/announce,udp://tracker.open-internet.nl:6969/announce,udp://tracker.leechers-paradise.org:6969/announce,udp://tracker.internetwarriors.net:1337/announce,http://tracker.internetwarriors.net:1337/announce,udp://tracker.opentrackr.org:1337/announce,udp://9.rarbg.to:2710/announce,udp://9.rarbg.me:2710/announce,http://tracker.opentrackr.org:1337/announce,udp://exodus.desync.com:6969/announce,udp://explodie.org:6969/announce,http://explodie.org:6969/announce,udp://tracker1.itzmx.com:8080/announce,http://tracker3.itzmx.com:6961/announce,http://tracker1.itzmx.com:8080/announce,udp://tracker2.itzmx.com:6961/announce,udp://ipv4.tracker.harry.lu:80/announce,http://tracker2.itzmx.com:6961/announce,udp://tracker.torrent.eu.org:451/announce,udp://tracker.tiny-vps.com:6969/announce,udp://open.stealth.si:80/announce,udp://open.demonii.si:1337/announce,udp://denis.stalker.upeer.me:6969/announce,udp://tracker.cyberia.is:6969/announce,udp://thetracker.org:80/announce,udp://retracker.lanta-net.ru:2710/announce,http://re-tracker.uz:80/announce,http://open.acgnxtracker.com:80/announce,udp://tracker.uw0.xyz:6969/announce,udp://tracker.trackton.ga:7070/announce,udp://tracker.nyaa.uk:6969/announce,udp://tracker.moeking.me:6969/announce,udp://tracker.filemail.com:6969/announce,udp://hk1.opentracker.ga:6969/announce,udp://bt.oiyo.tk:6969/announce,http://vps02.net.orel.ru:80/announce,udp://tracker4.itzmx.com:2710/announce,udp://tracker.port443.xyz:6969/announce,udp://retracker.netbynet.ru:2710/announce,https://tracker.fastdownload.xyz:443/announce,https://t.quic.ws:443/announce,https://opentracker.xyz:443/announce,http://tracker.port443.xyz:6969/announce,http://opentracker.xyz:80/announce,http://open.trackerlist.xyz:80/announce,http://0d.kebhana.mx:443/announce,udp://tracker.tvunderground.org.ru:3218/announce,udp://tracker.skynetcloud.tk:6969/announce,udp://tracker.novg.net:6969/announce,udp://tracker.iamhansen.xyz:2000/announce,udp://tracker.filepit.to:6969/announce,udp://tracker.dyn.im:6969/announce,udp://torrentclub.tech:6969/announce,udp://retracker.baikal-telecom.net:2710/announce,udp://pubt.in:2710/announce,udp://chihaya.toss.li:9696/announce,https://tracker.gbitt.info:443/announce,https://1.tracker.eu.org:443/announce,http://tracker.tvunderground.org.ru:3218/announce,http://tracker.novg.net:6969/announce,http://tracker.gbitt.info:80/announce,http://tracker.bz:80/announce,http://torrentclub.tech:6969/announce,http://torrent.nwps.ws:80/announce,http://t.nyaatracker.com:80/announce,http://share.camoe.cn:8080/announce,http://open.acgtracker.com:1096/announce,wss://tracker.openwebtorrent.com:443/announce,wss://tracker.fastcast.nz:443/announce,udp://tracker.swateam.org.uk:2710/announce,udp://tracker.kamigami.org:2710/announce,udp://tracker.justseed.it:1337/announce,udp://retracker.sevstar.net:2710/announce,udp://packages.crunchbangplusplus.org:6969/announce,udp://a.leopard-raws.org:6969/announce,https://tracker.vectahosting.eu:2053/announce,http://tracker4.itzmx.com:2710/announce,http://tracker.torrentyorg.pl:80/announce,http://tracker.tfile.me:80/announce,http://tracker.openzim.org:80/announce,http://tracker.open-tracker.org:1337/announce,http://t.acg.rip:6699/announce,http://retracker.mgts.by:80/announce,http://peersteers.org:80/announce,http://fxtt.ru:80/announce
```

# tracker 服务器

- [github tracker](https://github.com/ngosang/trackerslist)


# web管理

启动aria2 rpc
```
aria2c --enable-rpc --rpc-listen-all
```

- [webui-aria2](https://ziahamza.github.io/webui-aria2/)
