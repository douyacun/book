---
Title: centos使用ss代理的方法
Keywords: ss代理,centos,翻墙
Description: 不知道是谁做的决定限速github，心里默默诅咒他
Label: ss
Date: 2020-04-01 23:41:00
LastEditTime: 2020-04-01 23:41:00
---

# shadowsocks

centos安装ss，需要先安装pip，ss是python实现的， libsodium库有些加密方法依赖

```shell
yum -y install epel-release
yum -y install python-pip
yum install libsodium -y
```

安装ss

```shell
pip -v
# 安装最新的ss版本
pip install https://github.com/shadowsocks/shadowsocks/archive/master.zip -U
```

shadowsocks.json

```json
{
  "server":"ss服务器ip",            
  "server_port": 端口,                
  "local_address": "127.0.0.1",   
  "local_port":1080,                 
  "password":"password",         
  "timeout":300,                  
  "method":"加密方法",         
  "workers": 1                    
}
```

本地启动ss

```shell
nohup sslocal -c /etc/shadowsocks.json > /dev/null 2>&1 &
```

测试本地scoekt代理：

```curl
curl --socks5 127.0.0.1:1080 http://httpbin.org/ip
```

 可以直接设置socket5代理:



# privoxy

ss 是socket5代理， 安装privoxy将socket5转发成http代理

```shell
yum -y install privoxy
systemctl start privoxy
```

配置privoxy `/etc/privoxy/config`

```
listen-address 127.0.0.1:8118 # 8118 是默认端口，不用改  
forward-socks5t / 127.0.0.1:1080 . #转发到本地端口，注意别忘了最后的.
```