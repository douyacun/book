---
Title: nginx下载防盗链_图片防盗链_文件下载防盗链
Keywords: nginx防盗链_图片防盗链_下载防盗链_文件下载防盗链
Description: nginx防盗链常用措施
Author: douyacun
Date: 2019-02-02 16:34:31
LastEditTime: 2019-02-02 16:34:32
---

# nginx图片防盗链

## nginx referer 介绍

ngx_http_referer_module用于非法来源的请求
- 伪装Referer头部是非常简单的事情,这个模块只能拦截大部分非法请求,
- 有些合法的请求头没有referer,不要拒绝请求头（referer）为空的请求.

nginx防盗链指令
- referer_hash_bucket_size size
  - 默认size： 64
  - server, location
- referer_hash_max_size size
  - 默认size：2048
  - server, location
- valid_referers none | blocked | server_names | string ...
  - 默认值: -
  - server, location
  
valid_referers：指定合法"referers",决定`$invalid_referer`的值， 如果referer是合法的，变量为1,否则为0
- none: `referer`为空，eg. 直接访问，在新的标签打开一张图片
- blocked: `referer`不为空, 里面的值被代理或者防火墙删除了,不以`http`|`https`开头，表示被防火墙标记过的来路
- server_names: `referer`包含当前域名
- string: 任意字符串，定义服务器名或这可uri前缀，来源域名的主机端口会被忽略
- regular expression: 正则表达式, `~`：表示排除`https`|`http`

## 防盗的几种方法

### 不同文件类型的防盗链

```nginx
location ~* \.(gif|jpg|png|bmp)$ {
      valid_referers none blocked *.douyacun.com server_names ~\.google\. ~\.baidu\.;
      if ($invalid_referer) {
        rewrite ^/ https://www.douyacun.com;
      }
}
```
允许douyacun.com所有二级域名访问，douyacun.com是不允许的允许包含google和baidu的站点访问\
直接在浏览器中输入图片链接，显示正常

```nginx
location ~ .*\.(wma|wmv|asf|mp3|mmf|zip|rar|jpg|gif|png|swf|flv)$ {
     valid_referers none blocked *.douyacun.com douyacun.com;
     if($invalid_referer){
        rewrite ^/ https://www.douyacun.com;
     }
}
```
对wma|wmv|asf|mp3|mmf|zip|rar|jpg|gif|png|swf|flv后缀的文件实行防盗链\
允许douyacun.com或搜索二级域名访问\

```
location ~ .*\.(gif|jpg|jpeg|png|bmp|swf)$ {
    valid_referers *.douyacun.com douyacun.com;
    if($invalid_referer) {
        rewrite ^/ https://www.douyacun.com;
    }
    expires 30d;
}
```
不允许空白'referer'请求\
在浏览器直接输入图片地址就不会再显示图片出来了，也不可能会再右键另存什么的\
这是真正意义上的防盗链

### 针对目录的防盗链
```nginx
location /img/ {
    root /data/img/;
    valid_referers none blocked *.douyacun.com douyacun.com;
    if($invalid_referer){
        ewrite ^/ http://www.wangshibo.com/images/error.gif;
    }
}

location /images/ { 
    alias /data/images/; 
    valid_referers none blocked server_names *.douyacun.com douyacun.com;
    if ($invalid_referer) {
        return 403;
    } 
}
```

### 使用`ngx_http_accesskey_module`防盗链
