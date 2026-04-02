---
Title: nginx跨域
LegacyId: 5f041ad77e342c747db187115cae365d
Slug: nginx跨域
Category: devops
Summary: '``` add_header ''Access-Control-Allow-Origin'' *; add_header ''Access-Control-Allow-Credentials''
  true; add_header ''Access-C'
SeoTitle: nginx跨域
SeoDescription: '``` add_header ''Access-Control-Allow-Origin'' *; add_header ''Access-Control-Allow-Credentials''
  true; add_header ''Access-C'
Date: 2019-09-24T20:04:29+08:00
LastEditTime: 2019-09-24T20:04:29+08:00
Publish: false
---

```
add_header 'Access-Control-Allow-Origin' *;
add_header 'Access-Control-Allow-Credentials' true;
add_header 'Access-Control-Allow-Methods' 'GET, POST, OPTIONS, DELETE';
add_header 'Access-Control-Allow-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range';
add_header 'Access-Control-Expose-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range';
```
