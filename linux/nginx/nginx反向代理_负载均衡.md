---
Title: nginx proxy module_配置_负载均衡配置upstream
Keywords: http proxy module_配置_反向代理服务器_负载均衡upstream`
Description: 
Author: douyacun
Date: 2019-03-18
LastEditTime: 2019-03-19
---

# nginx反向代理的特点
相比squid， 在客户端发送请求，nginx不会立即转发给上游服务器，而是先把用户的请求（包括http包体）完整的接收到nginx服务器所在的内存或磁盘中，再向上游服务器发起连接, 把缓存的客户端请求转发给上游服务器。

缺点是延长了一个请求的处理时间，增加了用户缓存内容的内存和磁盘空间\
优点是降低了上游服务器的负载，把压力放在了nginx服务器上

如果上游服务器返回内容，则不会先完整的缓存到nginx代理服务器，而是边接受边转发


# 负载均衡

## upstream块
```

upstream backend {
    server backend1.example.com;
    server backend2.example.com;
    server backend3.example.com;
}

server {
    location / {
        proxy_pass http://backend;
    }
}

http
```
## server
```
server name [parameters];
upstream;
```
指定一台上有服务器的名字，可以是域名，ip地址端口、unix句柄，其后可以跟参数：
- weight=number: 设置向这台上游服务器转发的权重，默认为1
- max_fails=number: 与fail_timeout配合使用，在fail_timeout时间段内，如果向当前的上有服务器转发失败次数超过number，则当前服务器不可用，默认为1，0表示，不检查失败次数
- fail_timeout=time: 该段时间内转发失败多少次后认为上游服务其暂时不可用，用于优化反向代理，默认为10秒
- down：表示所在的上有服务器下线
- backup：使用ip_hash配置时无效，表示上游服务器是备份服务器，所有的上游服务器都不可用时，才会向所在的上游服务器转发请求
- ip_hash：根据用户ip地址计算key，key按照upstream集群里的上游服务器取模，然后以取模后的结果把请求转发到响应的上有服务器中


# 反向代理

## proxy_pass
```
proxy_pass URL
```
将当前请求反向代理到URL参数指定的服务器


主机名或IP地址加端口的形式
```
proxy_pass http://www.baidu.com/search
```

upstream块
```
location / {
    proxy_pass http://backend;
}
```

## proxy_method
```
proxy_method POST;
```

## proxy_hide_header
```
proxy_hide_header header;
```
默认不会转发：
- date
- server
- x-pad
- x-accel-*
可以任意指定哪些HTTP头部字段不能转发

## proxy_pass_header
```
proxy_pass_header header;
http、server、location
```
可以将原来禁止转发的header设置为允许转发

## proxy_redirect
```
proxy_redirect default|off|redirect|replacment;
proxy_redirect default;
http、server、location
```

