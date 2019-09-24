---
Title: 负载均衡_https_nginx_haproxy
Keywords: 负载均衡,https配置,nginx,haproxy
Description: haproxy和nginx http负载均衡配置
Author: douyacun
Date: 2018-03-11 12:09:19
LastEditTime: 2019-02-02 16:20:18
---
介绍两种实现方式:

1. nginx upstram
2. HAProxy


# HAProxy

### 简介
HAProxy是一个免费的负载均衡软件，可以运行于大部分主流的Linux操作系统上。
HAProxy提供了L4(TCP)和L7(HTTP)两种负载均衡能力，具备丰富的功能。 \
HAProxy的社区非常活跃，版本更新快速（最新稳定版1.7.2于2017/01/13推出）。最关键的是，HAProxy具备媲美商用负载均衡器的性能和稳定性。

- 负载均衡：L4和L7两种模式，支持RR/静态RR/LC/IP Hash/URI Hash/URL_PARAM Hash/HTTP_HEADER Hash等丰富的负载均衡算法
- 健康检查：支持TCP和HTTP两种健康检查模式
- 会话保持：对于未实现会话共享的应用集群，可通过Insert Cookie/Rewrite Cookie/Prefix Cookie，以及上述的多种Hash方式实现会话保持
- SSL：HAProxy可以解析HTTPS协议，并能够将请求解密为HTTP后向后端传输
- HTTP请求重写与重定向
- 监控与统计：HAProxy提供了基于Web的统计信息页面，展现健康状态和流量数据。基于此功能，使用者可以开发监控程序来监控HAProxy的状态



### 建议:
1. 使用3.x内核的Linux操作系统运行HAProxy
2. 运行HAProxy的主机上不要部署其他的应用，确保HAProxy独占资源，同时避免其他应用引发操作系统或主机的故障
3. 至少为HAProxy配备一台备机，以应对主机硬件故障、断电等突发情况（搭建双活HAProxy的方法在后文中有描述）
4. sysctl的建议配置（并不是万用配置，仍然需要针对具体情况进行更精细的调整，但可以作为首次使用HAProxy的初始配置使用）：
```
vim /etc/sysctl.conf


net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_max_tw_buckets = 40000
net.ipv4.tcp_max_syn_backlog = 10240
net.ipv4.ip_local_port_range = 1024 65023
net.ipv4.tcp_max_orphans = 60000
net.ipv4.tcp_synack_retries = 3

# 表示开启SYN Cookies。当出现SYN等待队列溢出时，启用cookies来处理，可防范少量SYN攻击，默认为0，
net.ipv4.tcp_tw_reuse = 1

#该参数设置系统的TIME_WAIT的数量，如果超过默认值则会被立即清除
net.ipv4.tcp_max_tw_buckets = 40000

# 表示用于向外连接的端口范
net.ipv4.tcp_max_syn_backlog = 10240

# 探测消息发送的频率，乘以tcp_keepalive_probes就得到对于从开始探测以来没有响应的连接杀除的时间
net.ipv4.ip_local_port_range = 1024 65023

# 该参数决定了, 每个网络接口接收数据包的速率比内核处理这些包的速率快时，允许送到队列的数据包的最大数目, 不要设的过大
net.ipv4.tcp_max_orphans = 60000

# 系统所能处理不属于任何进程的TCP sockets最大数量。
net.ipv4.tcp_synack_retries = 3

# 定义了系统中每一个端口最大的监听队列的长度，这是个全局的参数
net.core.somaxconn = 10000

# 解决
net.ipv4.ip_nonlocal_bind=1
```

### [haproxy官网](http://www.haproxy.org)

```
yum install -y openssl openssl-devel readline-devel pcre-devel libssl-dev libpcre3
wget http://www.haproxy.org/download/1.8/src/haproxy-1.8.4.tar.gz
tar zxvf haproxy-1.8.4
cd haproxy-1.8.4
make PREFIX=/usr/local/haproxy \
TARGET=linux2628 \
USE_PCRE=1 \
USE_OPENSSL=1 \
USE_ZLIB=1 \
USE_CRYPT_H=1 \
USE_LIBCRYPT=1
make install PREFIX=/usr/local/haproxy
```

PREFIX为指定的安装路径，TARGET则根据当前操作系统内核版本指定

- linux22     for Linux 2.2
- linux24     for Linux 2.4 and above (default)
- linux24e    for Linux 2.4 with support for a working epoll (> 0.21)
- linux26     for Linux 2.6 and above
- linux2628   for Linux 2.6.28, 3.x, and above (enables splice and tproxy)

配置系统服务:
```
vim /usr/lib/systemd/system/haproxy.service


[Unit]
Description=HAProxy Load Balancer
After=syslog.target network.target


[Service]
Type=forking
PIDFile=/run/haproxy.pid
ExecStartPre=/usr/bin/haproxy -f /etc/haproxy/haproxy.cfg -p /run/haproxy.pid
ExecStart=/usr/bin/haproxy -f /etc/haproxy/haproxy.cfg -p /run/haproxy.pid
ExecReload=/bin/kill -s HUP $MAINPID
ExecStop=/bin/kill -s TERM $MAINPID

[Install]
WantedBy=multi-user.target
```

配置分段:
```
global
    配置参数：log,maxconn,...，应用程序，性能，调试
proxies
    defaults:定义frontend与backend常用属性
    frontend:定义前端，用来接受用户请求，根据用户请求调度至后端
    backend:定义后端，用来定义后端服务器属性，
    listen:关联了前后端，不用分开定义
```

# 配置信息
```
#---------------------------------------------------------------------
# Example configuration for a possible web application.  See the
# full configuration options online.
#
#   http://haproxy.1wt.eu/download/1.4/doc/configuration.txt
#
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Global settings
#---------------------------------------------------------------------
global
    	log         127.0.0.1 local2

    	chroot      /var/lib/haproxy
    	pidfile     /var/run/haproxy.pid
    	maxconn     4000
    	user       	haproxy
   	group       haproxy
    	daemon

    	# turn on stats unix socket
    	stats socket /var/lib/haproxy/stats
	tune.ssl.default-dh-param 2048
#---------------------------------------------------------------------
# common defaults that all the 'listen' and 'backend' sections will
# use if not designated in their block
#---------------------------------------------------------------------
defaults
    mode                    http
    log                     global
    option                  httplog
    option                  dontlognull
    option http-server-close
    option forwardfor       except 127.0.0.0/8
    option                  redispatch
    retries                 3
    timeout http-request    10s
    timeout queue           1m
    timeout connect         10s
    timeout client          1m
    timeout server          1m
    timeout http-keep-alive 10s
    timeout check           10s
    maxconn                 3000

#---------------------------------------------------------------------
# http 80 https 443  负载均衡
#---------------------------------------------------------------------
frontend  http-in
	bind 	*:80
	bind    *:443 ssl crt /etc/ssl/00h.tv/00h.tv.pem


    	# Redirect if HTTPS is *not* used
    	#redirect scheme https if !{ ssl_fc }

	#访问控制列表，使用 80 端口，进行 renew
    	acl letsencrypt-acl path_beg /.well-known/acme-challenge/
    	use_backend letsencrypt-backend if letsencrypt-acl

    	default_backend           servers
#---------------------------------------------------------------------
# let's encrypt
#---------------------------------------------------------------------
backend letsencrypt-backend
	server letsencrypt 127.0.0.1:4431

#---------------------------------------------------------------------
# round robin balancing between the various backends
#---------------------------------------------------------------------
backend servers

        # 负载均衡的方式,source根据客户端IP进行哈希的方式
        balance source
        # weight：权重。默认是1.数字越大，权重越大。 inter : 两个连续的检查之间的延时，默认为2000ms。fall: 在连续的几次健康检查中失败，则视为dead（当机）rise : 在连续的几次健康检查中成功
    	server  server1 159.65.22.214:80 check weight 3 check inter 2000 rise 3 fall 3
    	server  server2 127.0.0.1:8080 check weight 1 check inter 2000 rise 3 fall 3
```

### 错误解决:
报错信息 [ALERT] 066/104255 (10558) : Starting frontend http-in: cannot bind socket

```
内核配置缺少
net.ipv4.ip_nonlocal_bind=1
```
