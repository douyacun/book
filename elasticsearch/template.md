---
Title: "es使用template自动初始化mapping"
Keywords: "elasticsearch,template,mapping"
Description: "使用es存储log日志来分析数据，需要按日期来分割数据（douyacun_log-2021-02）定时删除历史数据"
Lable: "es template"
---

数据量大的话：mysql是通过partition分区表支持按日期来分区，es通过index名称来达到分区的策略

es可以通过put mapping来创建，但是每天都要新的index，mysql需要写脚本自动来添加partition。es提供了template来完成，当POST doc时，如果不存在会匹配template初始化mapping

**创建template**

支持通配符匹配，如果匹配到多个，按照order优先排序使用

```
PUT _template/tracking_template
{
  "order": 0,
  "index_patterns": [
    "spider_*"
  ],
  "settings": {},
  "mappings": {
    "properties": {
      "date": {
        "type": "date"
      },
      "spider": {
        "type": "keyword"
      }
    }
  },
  "aliases": {}
}
```

**查看索引模版**

```curl
GET _template                							// 查看所有模板
GET _template/spider_*          					// 查看与通配符相匹配的模板
GET _template/spider_2020,spider_2021   	// 查看多个模板
GET _template/spider_template  						// 查看指定模板
```

**删除索引模版**

```curl
DELETE _template/spider_template
```