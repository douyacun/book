---
Title: ssh端口映射_php通过ssh访问正式数据库
Keywords: ssh,端口映射,访问显示数据库
Description: 
Author: douyacun
Date: 2018-11-06 15:37:47
LastEditTime: 2019-02-02 17:23:55
---

有一台 LDAP 服务器（LDAP Server），但是限制了只有本机上部署的应用才能直接连接此 LDAP 服务器。现在我们想临时从本地机器（LDAP Client）直接连接到这个 LDAP 服务器，只需要在LDAP Client上运用本地端口转发：


```
# 这里解释一下，阿里云RDS不允许外网访问，但是允许内网访问，那就可以走内网网关,`127.0.0.1`是不生效的,通过·dev@121.40.160.207·代理,这里*是避
ssh -g -N -L 3307:10.168.43.*:3306 dev@121.40.160.*
```

- -L：命令中的L参数一共接受三个值，分别是"本地端口:目标主机:目标主机端口"
- -p：参数表示指定登录端口
- -f：指该命令在后台运行
- -N：指该命令不执行远程命令
- -q：表示该命令进入安静模式
- -C：压缩数据传输。
- -g ：在-L/-R/-D参数中，允许远程主机连接到建立的转发的端口，如果不加这个参数，只允许本地主机建立连接。

如果使用[ssh-config](/linux/ssh/ssh-config.md)配置的话：
```
Host douyacun
	hostname douyacun.com
	port 22
	User douyacun
	IdentityFile  ~/.ssh/id_rsa
```

```
ssh -g -N -L 3307:10.168.43.*:3306 douyacun
```