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

 `https://www.douyacun.com/api/tools/location`

#### 请求方法:  

`HTTP GET`

#### 参数：

- `ip` :  `118.28.8.8`

#### header:

- `token` : `UgDS8nRousuEQ9LHXHQ2JaBCSbIn0iqE`

  > 此token仅用于测试，`5次/秒 `  `500次/天`

#### 示例:

 `curl "https://www.douyacun.com/api/tools/location?ip=118.28.8.8" -H "Token: UgDS8nRousuEQ9LHXHQ2JaBCSbIn0iqE"`

#### 响应:

```json
{
    "code": 0,
    "message": "success",
    "data": {
        "city": {
            "name": "上海市",
            "adcode": "310100",
            "code": ""
        },
        "country": {
            "name": "中国",
            "adcode": "",
            "code": "CN"
        },
        "province": {
            "name": "上海市",
            "adcode": "310000",
            "code": ""
        }
    }
}
```



## token获取方式

顶部导航栏登录以后可以获取token

![image-20200730230550806](../../../../Library/Application Support/typora-user-images/image-20200730230550806.png)



