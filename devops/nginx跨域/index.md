---
Title: nginx跨域
LegacyId: 5f041ad77e342c747db187115cae365d
topic: devops
Keywords:
Label:
description: "``` add_header ''Access-Control-Allow-Origin'' *; add_header ''Access-Control-Allow-Credentials'' true; add_header ''Access-C"
Date: 2019-09-24T20:04:29+08:00
LastEditTime: 2019-09-24T20:04:29+08:00
Publish: false
Lang: zh
---

```
add_header 'Access-Control-Allow-Origin' *;
add_header 'Access-Control-Allow-Credentials' true;
add_header 'Access-Control-Allow-Methods' 'GET, POST, OPTIONS, DELETE';
add_header 'Access-Control-Allow-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range';
add_header 'Access-Control-Expose-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range';
```
