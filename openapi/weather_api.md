---
Title: "中国城市天气接口：小时预报,7天天气预报,尾号限行,生活指数,空气质量,日升日落时间"
Keywrod: "天气API接口,免费,小时预报,7天天气预报,尾号限行,生活指数,空气质量,日升日落时间,weather api"
Description: "数据来源于中国天气网, 每小时更新一次"
Label: "中国城市天气接口"
---

**特点：**

- 中国各大城市，不包括自治州地区，支持的城市地区:  [https://www.douyacun.com/static/中国城市adcode.json](https://www.douyacun.com/static/中国城市adcode.json)
- 预报纬度： 小时预报，7天天气预报，生活指数，空气质量，日升日落时间，尾号限行
- [数据来源于中国天气网](http://www.weather.com.cn/)，长期稳定维护
- 天气数据每小时更新
- 接口支持jsonp调用

**请求限制:**

- 开通接口权限 <a name="token"></a>
    - 关注微信公众号，私信说明 **开通天气API**，后续如果接口有调整会通过推送文章

- 5 次/s ，最大发量 10， 5万次/天

![douyacun1](https://cdn.douyacun.com/images/blog/1/openapi/assert/douyacun%E7%B2%89%E4%B8%9D%E7%BE%A4.webp)

# 文档

**请求方法**:

- GET

**接口地址:**

- https://www.douyacun.com/api/openapi/weather

**测试Token:**

- eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJBY2NvdW50SWQiOiJlZWQ4ZmQ1ODBmYTRmNjkyIn0.d7qF_mjdXMC0R5M6f04Lnh6x61kaU4lqHT0Axt9xUOY

**参数:**

| 参数名       | 是否必选 | 类型   | 参数说明                                                     |                             备注                             |
| ------------ | -------- | ------ | ------------------------------------------------------------ | :----------------------------------------------------------: |
| province     | 否       | string | 省份                                                         |                             北京                             |
| city         | 否       | string | 城市                                                         |                             北京                             |
| adCode       | 否       | string | 城市AdCode                                                   | [城市code](https://www.douyacun.com/static/%E4%B8%AD%E5%9B%BD%E5%9F%8E%E5%B8%82adcode.json) |
| ip           | 否       | string | ip地址定位                                                   |                        60.207.131.25                         |
| callback     | 否       | string | jsonp调用参数                                                |                                                              |
| token        | 是       | string | [token申请](#token)                                          |                                                              |
| weather_type | 是       | string | forecast_hour\|forecast_day\|alarm\|limit\|rise\|observe\|life_index\|air |                           `|` 分割                           |

**备注:**

- 如果不传参数将会使用 IP 定位城市

- 参数优先级：province,city > adcode > ip 
- weather_type
    - `forecast_hour` 小时预报
    - `forecast_day` 7天天气预报
    - `alarm` 报警
    - `limit` 限行
    - `rise` 日出日落时间
    - `observer` 今日天气
    - `life_index` 生活指数
    - `air` 空气质量

**响应JSON**

```json
{
    "code": 0, // 0 成功
    "data": {
       // air 空气质量
        "air": {
            "aqi": 55, // 空气质量
            "aqi_level": 2,// 空气质量等级
            "aqi_name": "良",// 空气质量等级
            "pm10": "59",// Pm10 总悬浮颗粒物是指漂浮在空气中的固态和液态颗粒物的总称，[1]其粒径范围约为0.1-100 微米
            "pm2.5": "21",// pm2.5 细颗粒物指环境空气中空气动力学当量直径小于等于 2.5微米的颗粒物
            "co": "1",// 一氧化碳
            "no2": "11",// 二氧化氮
            "so2": "13",// 二氧化硫
            "update_time": "20210425120000"
        },
        // alarm 报警
        "alarm": [
            {
                "detail": "哈尔滨市人民政府森林（草原）防灭火指挥部、市气象局和市林业和草原局4月24日13:00联合发布全市森林（草原）火险黄色预警信号：预计24下午至26日，我市以晴或多云天气为主，气温逐步回升，最高气温15到19℃，平均风力4到5级，阵风7级左右，森林草原火险等级为3级。请各区、县（市）人民政府森林草原防灭火指挥部及各有关单位和人员立即按照“预警响应方案”进入三级森林草原防灭火工作状态，做好森林草原防灭火各项措施的落实和扑火准备工作。（预警信息来源：国家预警信息发布中心）",// 报警详情
                "info": "202104241300509532森林（草原）火险黄色",// 报警信息
                "level_code": "00",// 报警级别
                "level_name": "黄色",// 报警级别
                "type_code": "00",
                "type_name": "森林（草原）火险"// 报警类别
            }
        ],
        // 7天预报
        "forecast_day": [
            {
                "day_weather": "阴", // 天气状况
                "day_weather_code": "02",
                "day_weather_short": "阴",
                "day_wind_direction": "北风",// 白天风向
                "day_wind_direction_code": "8",
                "day_wind_power": "4",// 风力
                "day_wind_power_code": "1",
                "max_degree": "7",// 最高温度
                "min_degree": "2",// 最低温度
                "night_weather": "晴",// 白天天气
                "night_weather_code": "00",
                "night_weather_short": "晴",
                "night_wind_direction": "西北风", // 夜晚风向 
                "night_wind_direction_code": "01",
                "night_wind_power": "3",// 夜晚风力
                "night_wind_power_code": "0",
                "time": "2021-04-24" // 日期
            },,,,,,,,,
        ],
        // 小时预报
        "forecast_hour": [
            {
                "degree": "12",// 温度
                "weather": "晴",// 天气
                "weather_code": "00",// 天气代码
                "weather_short": "晴",
                "wind_direction": "北风",// 风向
                "wind_power": "3",// 风力
                "update_time": "2021-04-25 12:00:00"
            },,,,,,,,,,,
        ],
        // 尾号限行
        "limit": {
            "tail_number": "" // 尾号限行
        },
        // 当前天气 
        "observe": {
            "degree": "13",// 温度
            "humidity": "28",// 湿度
            "precipitation": "0",// 降水量
            "pressure": "1010",// 气压
            "weather": "晴",// 天气
            "weather_code": "00",
            "weather_short": "晴",
            "wind_direction": "8",// 风向
            "wind_power": "3"// 风力
        },
        "life_index": {
            "airconditioner": {
                "detail": "您将感到很舒适，一般不需要开启空调。",
                "info": "较少开启",
                "name": "空调开启"
            },
            "allergy": {
                "detail": "天气条件较易诱发过敏，宜穿长衣长裤，远离过敏源，适当佩戴眼镜和口罩。",
                "info": "较易发",
                "name": "过敏"
            },
            "carwash": {
                "detail": "适宜洗车，未来持续两天无雨天气较好，适合擦洗汽车，蓝天白云、风和日丽将伴您的车子连日洁净。",
                "info": "适宜",
                "name": "洗车"
            },
            "chill": {
                "detail": "温度未达到风寒所需的低温，稍作防寒准备即可。",
                "info": "无",
                "name": "风寒"
            },
            "clothes": {
                "detail": "建议着厚外套加毛衣等服装。年老体弱者宜着大衣、呢外套加羊毛衫。",
                "info": "较冷",
                "name": "穿衣"
            },
            "cold": {
                "detail": "昼夜温差较大，较易发生感冒，请适当增减衣服。体质较弱的朋友请注意防护。",
                "info": "较易发",
                "name": "感冒"
            },
            "comfort": {
                "detail": "白天天气晴好，但仍会使您感觉偏冷，不很舒适，请注意适时添加衣物，以防感冒。",
                "info": "较不舒适",
                "name": "舒适度"
            },
            "diffusion": {
                "detail": "气象条件对空气污染物稀释、扩散和清除无明显影响。",
                "info": "中",
                "name": "空气污染扩散条件"
            },
            "dry": {
                "detail": "天气较好，路面比较干燥，路况较好。",
                "info": "干燥",
                "name": "路况"
            },
            "drying": {
                "detail": "天气不错，适宜晾晒。赶紧把久未见阳光的衣物搬出来吸收一下太阳的味道吧！",
                "info": "适宜",
                "name": "晾晒"
            },
            "fish": {
                "detail": "较适合垂钓，但风力稍大，会对垂钓产生一定的影响。",
                "info": "较适宜",
                "name": "钓鱼"
            },
            "heatstroke": {
                "detail": "天气舒适，令人神清气爽的一天，不用担心中暑的困扰。",
                "info": "无中暑风险",
                "name": "中暑"
            },
            "makeup": {
                "detail": "温湿适宜，最好使用SPF20以上防晒霜打底，建议使用中性保湿型化妆品。",
                "info": "防晒",
                "name": "化妆"
            },
            "mood": {
                "detail": "天气较好，空气温润，和风飘飘，美好的天气会带来一天接踵而来的好心情。",
                "info": "好",
                "name": "心情"
            },
            "morning": {
                "detail": "天气不错，空气清新。",
                "info": "适宜",
                "name": "晨练"
            },
            "sports": {
                "detail": "天气较好，无雨水困扰，较适宜进行各种运动，但因气温较低，在户外运动请注意增减衣物。",
                "info": "较适宜",
                "name": "运动"
            },
            "sunglasses": {
                "detail": "白天天气晴朗，太阳辐射较强，建议佩戴透射比1级且标注UV380-UV400的遮阳镜",
                "info": "必要",
                "name": "太阳镜"
            },
            "sunscreen": {
                "detail": "紫外辐射极强，应特别加强防护，建议涂擦SPF20以上，PA++的防晒护肤品，并随时补涂。",
                "info": "极强",
                "name": "防晒"
            },
            "tourism": {
                "detail": "天气较好，温度适宜，是个好天气哦。这样的天气适宜旅游，您可以尽情地享受大自然的风光。",
                "info": "适宜",
                "name": "旅游"
            },
            "traffic": {
                "detail": "天气较好，路面干燥，交通气象条件良好，车辆可以正常行驶。",
                "info": "良好",
                "name": "交通"
            },
            "ultraviolet": {
                "detail": "紫外线辐射极强，建议涂擦SPF20以上、PA++的防晒护肤品，尽量避免暴露于日光下。",
                "info": "很强",
                "name": "紫外线强度"
            },
            "umbrella": {
                "detail": "天气较好，您在出门的时候无须带雨伞。",
                "info": "不带伞",
                "name": "雨伞"
            }
        }
    },
    "message": "success"
}
```









