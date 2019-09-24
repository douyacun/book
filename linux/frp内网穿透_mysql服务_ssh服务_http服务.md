---
title: frp内网穿透_mysql服务_ssh服务_http服务
keywords: frp内网穿透,mysql服务,ssh服务,http服务
description: 通过frp加公网服务器提供mysql，ssh，http服务
---

使用的[frp](https://github.com/fatedier/frp),详情可以去github看，那里有中文文档，可以使用[supervisor管理frp连接进程](/linux/supervisor_安装_进程管理_配置_frp|spiderkeeper|jobs配置.md),以下介绍的是我的使用场景和一些个人理解

需要两台服务器
- 内网服务器(家里或者公司没有公网ip的服务器)
- 有公网ip的服务器，阿里云服务器/腾讯云服务器

# 安装

从[frp github realeas](https://github.com/fatedier/frp/releases)下载最新版，解压安装最新版本, 具体下载哪个文件没搞清，centos7.4,用的是下面的
- centos: frp_*_linux_amd64.tar.gz

# 配置

内网部署 `frpc.ini`
```conf
[common]
# 公网
server_addr = *.*.*.* # 这里是公网ip
server_port = 7000

[mysql]
local_port = 3306
local_ip = 127.0.0.1
local_port = 3306
# 公网服务器： mysql -u root -H 127.0.0.1 -P 3307 -p123456
# 公网服务器的 3307 端口，代理到内网的3306端口
remote_port = 3307

[ssh]
type = tcp
local_ip = 127.0.0.1
local_port = 22
# 如果需要在任何地方可以连接需要暴露公网的6000端口
# 公网服务器的 6000 端口，代理到内网的22端口
# ssh -p 6000 root@【公网ip地址】
remote_port = 6000

[http]
type = tcp
local_ip = 127.0.0.1
local_port = 80
# 需要暴露公网的80端口
# 公网服务器的 80 端口，代理到内网的80端口
remote_port = 80
```

公网部署 `frps.ini`
```conf
[common]
bind_port = 7000
```


# 启动连接

内网
```
./frpc -c frpc.ini
```
```log
2019/01/30 21:53:07 [I] [service.go:124] frps tcp listen on 0.0.0.0:7000
2019/01/30 21:53:07 [I] [root.go:204] Start frps success
2019/01/30 21:53:36 [I] [service.go:317] client login info: ip [139.227.1.5:39536] version [0.23.2] hostname [] os [linux] arch [amd64]
2019/01/30 21:53:36 [I] [tcp.go:66] [604c65b49901ac56] [ssh] tcp proxy listen port [6000]
2019/01/30 21:53:36 [I] [control.go:397] [604c65b49901ac56] new proxy [ssh] success
2019/01/30 21:56:50 [I] [proxy.go:80] [604c65b49901ac56] [ssh] get a new work connection: [139.227.1.5:39536]
```

公网
```
./frps -c frps.ini
```

```log
2019/01/30 21:53:36 [I] [service.go:206] login to server success, get run id [604c65b49901ac56], server udp port [0]
2019/01/30 21:53:36 [I] [proxy_manager.go:137] [604c65b49901ac56] proxy added: [ssh]
2019/01/30 21:53:36 [I] [control.go:143] [ssh] start proxy success
2019/02/01 00:45:53 [I] [service.go:206] login to server success, get run id [c387c847febafa3c], server udp port [0]
2019/02/01 00:45:53 [I] [proxy_manager.go:137] [c387c847febafa3c] proxy added: [mysql ssh]
2019/02/01 00:45:53 [I] [control.go:143] [mysql] start proxy success
2019/02/01 00:45:53 [I] [control.go:143] [ssh] start proxy success
```

[supervisor frp管理配置](/linux/supervisor_安装_进程管理_配置_frp|spiderkeeper|jobs配置.md#frp配置)