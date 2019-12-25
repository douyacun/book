---
Title: elasticsearch文档结构mapping定义
Keywords: mapping定义,结构定义
Description: mapping等价于mysql表结构schema
Lable: es mapping
Cover:
Date: 2019-12-24 23:46:00
LastEditTime: 2019-12-24 23:47:00
---

原文文档位置：[https://www.elastic.co/guide/en/elasticsearch/reference/current/mapping.html](https://www.elastic.co/guide/en/elasticsearch/reference/current/mapping.html)

# 基础字段类型

**string**

[`text`](https://www.elastic.co/guide/en/elasticsearch/reference/current/text.html) and [`keyword`](https://www.elastic.co/guide/en/elasticsearch/reference/current/keyword.html)

**[Numeric](https://www.elastic.co/guide/en/elasticsearch/reference/current/number.html)**

```
long`, `integer`, `short`, `byte`, `double`, `float`, `half_float`, `scaled_float
```

**[Date](https://www.elastic.co/guide/en/elasticsearch/reference/current/date.html)**

```
date
```

**[Date nanoseconds](https://www.elastic.co/guide/en/elasticsearch/reference/current/date_nanos.html)**

```
date_nanos
```

**[Boolean](https://www.elastic.co/guide/en/elasticsearch/reference/current/boolean.html)**

```
boolean
```

**[Binary](https://www.elastic.co/guide/en/elasticsearch/reference/current/binary.html)**

```
binary
```

**[Range](https://www.elastic.co/guide/en/elasticsearch/reference/current/range.html)**

```
integer_range`, `float_range`, `long_range`, `double_range`, `date_range
```

**[Object](https://www.elastic.co/guide/en/elasticsearch/reference/current/object.html)**

`object` for single JSON objects

**[Nested](https://www.elastic.co/guide/en/elasticsearch/reference/current/nested.html)**

`nested` for arrays of JSON objects

# 定义字段类型

- [`index`](https://www.elastic.co/guide/en/elasticsearch/reference/current/mapping-index.html) 控制当前字段是否被索引，默认为true，设置false，该字段不可被搜索到

  ```
  PUT my_index
  {
    "mappings": {
      "properties": {
        "text": {
          "type": "text",
          "index": false
        }
      }
    }
  }
  ```

  

- [`null_value`](https://www.elastic.co/guide/en/elasticsearch/reference/current/null-value.html) json null值默认是不可以被搜索到的，设置`"null_value":"NULL"` 可以对NULL值实现搜索

  ```
  PUT my_index
  {
    "mappings": {
      "properties": {
        "status_code": {
          "type":       "keyword",
          "null_value": "NULL" 
        }
      }
    }
  }
  ```

- [`copy_to`](https://www.elastic.co/guide/en/elasticsearch/reference/current/copy-to.html) 老版本是`_all`来实现的，拼接几个字段到一个字段中，`copy_to`字段不会出现`_source`中

  ```
  PUT my_index
  {
    "mappings": {
      "properties": {
        "first_name": {
          "type": "text",
          "copy_to": "full_name" 
        },
        "last_name": {
          "type": "text",
          "copy_to": "full_name" 
        },
        "full_name": {
          "type": "text"
        }
      }
    }
  }
  ```

- [`analyzer`](https://www.elastic.co/guide/en/elasticsearch/reference/current/analyzer.html) 分析器，可以查看 es[预定义的分析器](https://www.elastic.co/guide/en/elasticsearch/reference/current/analysis-analyzers.html)

  - 这里推荐安装中文[IK插件](https://github.com/medcl/elasticsearch-analysis-ik), github下载极其慢，可以通过迅雷或者其他国外的服务下载下来，注意版本要一致，es7.5要下载ik也是7.5的release， `./bin/elasticsearch-plugin install file:///root/elasticsearch-analysis-ik-7.5.0.zip`

  ```
  PUT /my_index
  {
    "mappings": {
      "properties": {
        "text": { 
          "type": "text",
          "analyzer": "ik_max_word",
          "search_analyzer": "ik_smart"
        }
      }
    }
  }
  
  GET my_index/_analyze
  {
    "field": "text",
    "text": "我和老婆的故事要从7年前，年初的时候开始说起"
  }
  ```

  

  

  

  

  

  

  

  

  

  

  

  

  

  

  

  

  

  

  

  





