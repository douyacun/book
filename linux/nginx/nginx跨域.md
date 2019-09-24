---
Title: nginx跨域
Keywords: nginx,跨域
Description: nginx跨域正确解决姿势
Author: douyacun
Date: 2017-11-20 19:51:42
LastEditTime: 2019-02-02 16:25:53
---

```
add_header 'Access-Control-Allow-Origin' *;
add_header 'Access-Control-Allow-Credentials' true;
add_header 'Access-Control-Allow-Methods' 'GET, POST, OPTIONS, DELETE';
add_header 'Access-Control-Allow-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range';
add_header 'Access-Control-Expose-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range';
```