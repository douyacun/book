---
Title: 
Keywords: 
Description: 
Author: douyacun
Date: 2019-06-26 13:38:15
LastEditTime: 2019-06-28 18:48:20
---

如果需要获取响应内容的话,需要用到lua脚本，推荐使用[openresty](https://openresty.org/cn/download.html)


你可以在你的 CentOS 系统中添加 openresty 仓库，这样就可以便于未来安装或更新我们的软件包（通过 yum update 命令）。运行下面的命令就可以添加我们的仓库：
```
sudo yum install yum-utils
sudo yum-config-manager --add-repo https://openresty.org/package/centos/openresty.repo
```

然后就可以像下面这样安装软件包，比如 openresty：
```
sudo yum install openresty

ln -s /usr/local/openresty ~/
cp /usr/local/openresty/nginx/sbin/nginx /usr/sbin/nginx
````

安装完

如果你想安装命令行工具 resty，那么可以像下面这样安装 openresty-resty 包：

```
sudo yum install openresty-resty
```

命令行工具 opm 在 openresty-opm 包里，而 restydoc 工具在 openresty-doc 包里头。

列出所有 openresty 仓库里头的软件包：
```
sudo yum --disablerepo="*" --enablerepo="openresty" list available
````



# log_format 配置 `nginx/conf/nginx.conf`
```conf
log_format  json escape=json '{"@timestamp":"$time_iso8601",'
        '"host":"$server_addr",'
        '"clientip":"$remote_addr",'
        '"remote_port":"$remote_port",'
        '"http_x_forwarded_for":"$http_x_forwarded_for",'
        '"status":"$status",'
        '"request_method":"$request_method", '
        '"request_url": "$request_uri", '
        '"request_time":$request_time,'
        '"size":$body_bytes_sent,'
        '"upstream_time":"$upstream_response_time",'
        '"upstream_host":"$upstream_addr",'
        '"server_name":"$host",'
        '"uri":"$uri",'
        '"http_referer":"$http_referer",'
        '"http_user_agent":"$http_user_agent",'
        '"request_body":"$request_body",'
        '"resp_body":"$resp_body"'
        '}';
access_log  /var/log/nginx/access.log json;
```
- `escape=json` : `request_body`和`resp_body` 一般都是json格式的，加了`escape=json`之后把json格式内容转义

# lua脚本
```
    lua_need_request_body on;

    set $resp_body "";
    body_filter_by_lua '
        if "POST" == ngx.var.request_method then
            local resp_body = string.sub(ngx.arg[1], 1, 1000)
            ngx.ctx.buffered = (ngx.ctx.buffered or "") .. resp_body
            if ngx.arg[2] then
                ngx.var.resp_body = ngx.ctx.buffered
            end
        end
    ';
```
