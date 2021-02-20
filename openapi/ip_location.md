---
Title: 免费不限速ip地理位置(GEO IP)接口
Keywords: ip location,geoip,ip定位,地理位置
Description: 免费开源ip地理位置接口
Label: GEO IP
Date: 2020-07-29 22:06:39
LastEditTime: 2020-07-29 23:06:38
---

# 接口文档

#### 接口地址:

 `https://www.douyacun.com/api/geo/ip`

#### 请求方法:  

`HTTP GET`

#### 参数：

- `ip` :  `118.28.8.8`

#### header:

- `token` : `UgDS8nRousuEQ9LHXHQ2JaBCSbIn0iqE`

> 此token仅用于测试，`5次/秒 `  `500次/天`

#### 示例:

 `curl "https://www.douyacun.com/api/geo/ip?ip=118.28.8.8" -H "Token: UgDS8nRousuEQ9LHXHQ2JaBCSbIn0iqE"`

#### 响应:

```json
{
    "code": 0,
    "message": "success",
    "data": {
        "country": "中国",
        "province": "天津",
        "city": "天津",
        "refer": "ipip"
    }
}
```
