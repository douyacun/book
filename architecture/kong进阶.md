这篇文章目的

-   熟悉kong的配置
-   线上已经在运行openresty，如何无缝接入kong



# 配置

## 注入kong nginx指令

`nginx_http_` 会注入到 `http` 块下面

`nginx_proxy_` 注入到kong 代理端口 `server` 块下面

```nginx
server {
    server_name kong;
    listen 0.0.0.0:8000 reuseport backlog=16384;
    listen 0.0.0.0:8443 ssl http2 reuseport backlog=16384;
	# ...
    # injected nginx_proxy_* directives
    real_ip_header X-Real-IP;
    real_ip_recursive off;
    # ...
}
```

`nginx_admin_` 注入到kong 管理端口 `server` 块下面

```nginx
server {
    server_name kong_admin;
    listen 127.0.0.1:8001 reuseport backlog=16384;
    listen 127.0.0.1:8444 ssl http2 reuseport backlog=16384;
	# ...
    # injected nginx_admin_* directives
    location / {
        default_type application/json;
        content_by_lua_block {
            Kong.admin_content()
        }
        header_filter_by_lua_block {
            Kong.admin_header_filter()
        }
    }
    # ...
}
```

### 配置server块

新增配置文件

```nginx
server {
  listen 2112;
  location / {
    # ...more settings...
    return 200;
  }
}
```

`kong.conf`  `nginx_http_include` 引入此配置文件

```nginx
nginx_http_include = /path/to/your/my-server.kong.conf
```

## 自定义kong nginx模版

极少用到

-   kong.conf 配置命令无法满足
-   线上已经在运行openresty实例，可以使用kong生成配置文件并且引入

### 模版

```
/usr/local/kong
├── nginx-kong.conf
└── nginx.conf
```

重启命令

```
kong start -c kong.conf --nginx-conf custom_nginx.template
```

## 嵌入kong到openresty中

kong下的文件，kong是openresty的插件，索引如果线上已经有openresty在运行的话，可以无缝衔接

```shell
[root@ali conf]# ll /usr/local/kong/
total 292
drwxr-xr-x 2 root   root   4096 May 28 14:31 bin
drwx------ 2 nobody root   4096 May 28 15:30 client_body_temp
-rw-r--r-- 1 root   root 234703 Apr 25 00:45 COPYRIGHT
drwx------ 2 nobody root   4096 May 28 15:30 fastcgi_temp
drwxr-xr-x 3 root   root   4096 May 28 14:31 include
drwxr-xr-x 4 root   root   4096 May 28 14:31 lib
drwxr-xr-x 2 root   root   4096 May 28 15:30 logs
-rw-r--r-- 1 root   root    297 Jun  1 19:18 nginx.conf
-rw-r--r-- 1 root   root   7807 Jun  1 19:18 nginx-kong.conf
-rw-r--r-- 1 root   root   1821 Jun  1 19:18 nginx-kong-stream.conf
drwxr-xr-x 2 root   root   4096 Jun  4 15:25 pids
drwx------ 2 nobody root   4096 May 28 15:30 proxy_temp
drwx------ 2 nobody root   4096 May 28 15:30 scgi_temp
drwxr-xr-x 2 root   root   4096 May 28 15:30 ssl
drwx------ 2 nobody root   4096 May 28 15:30 uwsgi_temp
```

启动方式:

```shell
$ /usr/local/openresty/nginx/sbin/nginx -p /usr/local/kong -c /usr/local/kong/nginx.conf
```

