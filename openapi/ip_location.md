---
Title: "免费不限速ip地理位置(GEO IP)接口"
Keywords: "ip location,geoip,ip定位,地理位置"
Description: "免费开源ip地理位置接口"
Label: "GEO IP"
---

# 后端接口调用

**接口地址:**

 `https://www.douyacun.com/api/geo/ip`

**请求方法:**  

`HTTP GET`

**参数：**

- `ip` :  `118.28.8.8`

**header:**

- `token` : `UgDS8nRousuEQ9LHXHQ2JaBCSbIn0iqE`

> 此token仅用于测试，`5次/秒 `  `500次/天`

**示例1:**

 `curl "https://www.douyacun.com/api/geo/ip?ip=178.252.91.188" -H "Token: UgDS8nRousuEQ9LHXHQ2JaBCSbIn0iqE"`

**响应：**

```json
{
    "code": 0,
    "message": "success",
    "data": {
        "countryCode": "CN",
        "country": "中国",
        "province": "浙江",
        "city": "杭州",
        "ip": "47.98.208.18",
        "latitude": "30.29365",
        "longitude": "120.16142",
        "zipcode": "310099",
        "timezone": "+08:00",
        "refer": "douyacun.com"
    }
}
```

**示例2:**

 `curl "https://www.douyacun.com/api/geo/ip?ip=178.252.91.188" -H "Token: UgDS8nRousuEQ9LHXHQ2JaBCSbIn0iqE"`

**响应:**

```json
{
    "code": 0,
    "message": "success",
    "data": {
        "countryCode": "RU",
        "country": "Russian Federation",
        "province": "Sankt-Peterburg",
        "city": "Saint Petersburg",
        "ip": "178.252.91.188",
        "latitude": "59.89444",
        "longitude": "30.26417",
        "zipcode": "196240",
        "timezone": "+03:00",
        "refer": "douyacun.com"
    }
}
```

# jsonp调用

**接口地址：**

`https://www.douyacun.com/api/geo/location?callback=__jp0`

**示例:**

 `curl "https://www.douyacun.com/api/geo/location?callback=__jp0"`

**响应:**

```
__jp0(
{
    "code": 0,
    "message": "success",
    "data": {
        "countryCode": "CN",
        "country": "中国",
        "province": "天津",
        "city": "天津",
        "ip": "118.28.8.8",
        "latitude": "39.14222",
        "longitude": "117.17667",
        "zipcode": "300120",
        "timezone": "+08:00",
        "refer": "douyacun.com"
    }
}
)
```

