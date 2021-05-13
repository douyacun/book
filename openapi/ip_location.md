---
Title: "免费ip地理位置(GEO IP)接口"
Keywords: "ip location,geoip,ip定位,地理位置"
Description: "免费开源ip地理位置接口"
Label: "GEO IP"
---

特点：

1. 支持传参IP定位，如果不传参数IP，默认使用当前客户端的IP地址定位

2. 支持前端jsonp调用，避免跨域

    

# 接口调用


**接口地址:**

 `https://www.douyacun.com/api/openapi/geo/location`

**请求方法:**  

`HTTP GET`

**参数：**

- `ip` :  `118.28.8.8`

**header:**

- `token` : `eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJBY2NvdW50SWQiOiJlZWQ4ZmQ1ODBmYTRmNjkyIn0.d7qF_mjdXMC0R5M6f04Lnh6x61kaU4lqHT0Axt9xUOY`

> 此token仅用于测试使用，限流 1次/s，大家共享此token

**限流**
- 开通接口权限 <a name="token"></a>
    - 关注微信公众号，私信说明开通 **开通IP定位API** ，后续如果接口有调整会通过推送文章

- 默认 5 次/s, 50万次/天， 
- 最大支持 10次/秒, 100万次/天，够用了。。。。
- 更大频次需要支付流量费用，100元长期支持，不限频次


![douyacun](https://www.douyacun.com/static/image/douyacun%E5%BE%AE%E4%BF%A1%E5%85%AC%E4%BC%97%E5%8F%B7.png)

**示例1:**

 `curl "https://www.douyacun.com/api/openapi/geo/location?ip=178.252.91.188" -H "Token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJBY2NvdW50SWQiOiJlZWQ4ZmQ1ODBmYTRmNjkyIn0.d7qF_mjdXMC0R5M6f04Lnh6x61kaU4lqHT0Axt9xUOY"`

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

 `curl "https://www.douyacun.com/api/openapi/geo/location?ip=178.252.91.188" -H "Token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJBY2NvdW50SWQiOiJlZWQ4ZmQ1ODBmYTRmNjkyIn0.d7qF_mjdXMC0R5M6f04Lnh6x61kaU4lqHT0Axt9xUOY"`

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

 `curl "https://www.douyacun.com/api/geo/location?callback=__jp0 -H "Token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJBY2NvdW50SWQiOiJlZWQ4ZmQ1ODBmYTRmNjkyIn0.d7qF_mjdXMC0R5M6f04Lnh6x61kaU4lqHT0Axt9xUOY"`

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
