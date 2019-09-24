---
Title: ssh-config设置_别名设置_配置解释 
Keywords: ssh config配置,ssh别名设置,配置解释
Description: 通过ssh config配置快速连接
Author: douyacun
Date: 2019-02-02 16:46:32
LastEditTime: 2019-02-02 16:46:34
---

常用的配置
```
# 别名
Host douyacun
  # ip或域名
	hostname douyacun.com
  # 端口号
	port 22
  # 登录用户名
	User douyacun
  # 密钥地址
	IdentityFile  ~/.ssh/id_rsa
```

`ssh douyaucn` 就可以登录我的服务器了，不需要`ssh root@douyacun.com`了，甚至`scp -r ./a/ 00h:/root/a/`

# 配置文件
- `~/.ssh/config`  用户配置文件
- `/etc/ssh_config` 系统配置文件

# 参数说明

## Host
一般用作别名，`ssh *`的时候匹配到启用哪个配置

- *, 匹配所有主机名
- *.douyacun.com 匹配所有以douyacun.com结尾
- 支持正则

## HostName
真实的主机名，默认值为命令行输入的值（允许 IP）。你也可以使用 %h，它将自动替换，只要替换后的地址是完整的就 ok。

## IdentityFile
指定读取的认证文件路径，允许 DSA，ECDSA，Ed25519 或 RSA。值可以直接指定也可以用一下参数代替:
- %d，本地用户目录 ~
- %u，本地用户
- %l，本地主机名
- %h，远程主机名
- %r，远程用户名

## Port
指定连接远程主机的哪个端口，22(default)。

## User
登录用户名

## AddKeysToAgent
是否自动将 key 加入到 ssh-agent，值可以为 
  - no(default)
  - confirm
  - ask
  - yes
这个在ci部署的时候常用到,需要连接到服务器同步部署文件,如果有确认操作，ci便会被堵塞
> [ci-gitlab](/ci/gitlab.md)
> [ci-travis](/ci/travis.md)


## LocalForward
端口转发,`LocalForward [bind_address:]post host:hostport`

## ConnectionAttempts
退出前尝试连接的次数，值必须为整数，1(default)

## ConnectTimeout
连接 SSH 服务器超时时间，单位 s，默认系统 TCP 超时时间。

## Compression
是否压缩，值可以为 no(default)/yes。

## CompressionLevel
压缩等级，值可以为 1(fast)-9(slow)。6(default)，相当于 gzip。

# 常用技巧

## vim 访问远程文件
```shell
vim scp://root@douyacun.com//root/a.txt
vim scp://douyacun//root/a.txt
```

## 远程服务当本地用
通过 LocalForward 将本地端口上的数据流量通过 ssh 转发到远程主机的指定端口, 感觉就像是使用的本地服务
```
Host db
    HostName douyacun.com
    LocalForward 3306 localhost:3307
```
经常用的到场景就是本地php调试的时候需要访问一下正式的数据库，那正式的数据库肯定不允许你直接链接啊，就可以通过这种方式来操作
> [ssh端口映射](/linux/ssh/ssh端口映射.md)


## 代理登录
有的时候你可能没法直接登录到某台服务器，而需要使用一台中间服务器进行中转，如公司内网服务器。首先确保你已经为服务器配置了公钥访问，并开启了agent forwarding，那么你需要添加如下配置到 ~/.ssh/config
```
Host db
    HostName db.douyacun.com                          # 目标服务器地址
    User root                                         # 用户名
    # IdentityFile ~/.ssh/id_ecdsa                    # 认证文件
    ProxyCommand ssh gateway netcat -q 600 %h %p      # 代理命令
```