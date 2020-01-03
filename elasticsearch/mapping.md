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

- [`coerce`](https://www.elastic.co/guide/en/elasticsearch/reference/current/coerce.html) 强制字段类型

  - integer 必须是int型数字

  ```curl
  PUT my_index
  {
    "mappings": {
      "properties": {
        "number_one": {
          "type": "integer"
        },
        "number_two": {
          "type": "integer",
          "coerce": false
        }
      }
    }
  }
  # 成功
  PUT my_index/_doc/1
  {
    "number_one": "10" 
  }
  # 失败，number_two强制是integer
  PUT my_index/_doc/2
  {
    "number_two": "10" 
  }
  ```

- [`dynamic`](https://www.elastic.co/guide/en/elasticsearch/reference/current/dynamic.html) 默认情况下，es会将字段动态映射到文档中

  - true （默认） 检测到新的字段添加到文档中
  - false 新的字段会被忽略，这些字段无法被搜索，仍会出现在`_source`中
  - strict 检测到新的字段，报错

- [`enabled`](https://www.elastic.co/guide/en/elasticsearch/reference/current/enabled.html) 只存储字段而不是建立索引，在`_source`中可见，像object可以存任何类型的值，但是如果索引了就会报错，也可以用于整个文档，整个文档都不会被索引

  ```
  PUT my_index
  {
    "mappings": {
      "properties": {
        "user_id": {
          "type":  "keyword"
        },
        "last_updated": {
          "type": "date"
        },
        "session_data": {
          "type": "object",
          "enabled": false
        }
      }
    }
  }
  PUT my_index/_doc/1
  {
    "user_id": "刘宁",
    "last_updated": "2019-12-26T11:34:00",
    "session_data": {
      "some_array": [
        "foo",
        "bar"
      ]
    }
  }
  # 如果session_data enabled为true的。这里会报错，不是对象类型
  PUT my_index/_doc/2
  {
    "user_id": "jpountz",
    "session_data": "none", 
    "last_updated": "2015-12-06T18:22:13"
  }
  # 整个文档都不会被索引
  PUT my_index
  {
    "mappings": {
      "enabled": false 
    }
  }
  ```

- [`format`](https://www.elastic.co/guide/en/elasticsearch/reference/current/mapping-date-format.html) 日期格式,es内置了很多格式，看文档就好

  ```
  PUT my_index
  {
    "mappings": {
      "properties": {
        "date": {
          "type":   "date",
          "format": "yyyy-MM-dd"
        }
      }
    }
  }
  ```

- [`ignore_above`](https://www.elastic.co/guide/en/elasticsearch/reference/current/ignore-above.html) 对于大于长度的字符串不会被索引或存储，字符串数组，会分别对每个元素判断

  ```
  PUT my_index
  {
    "mappings": {
      "properties": {
        "message": {
          "type": "keyword",
          "ignore_above": 20 
        }
      }
    }
  }
  PUT my_index/_doc/1 
  {
    "message": "Syntax error"
  }
  # 这里也被存储了，没搞懂
  PUT my_index/_doc/2 
  {
    "message": "Syntax error with some long stacktrace"
  }
  ```

- [`ignore_malformed`](https://www.elastic.co/guide/en/elasticsearch/reference/current/ignore-malformed.html#ignore-malformed-setting) 接受其他类型的数据，true： int可以接受字符串但是不会被索引

  ```
  PUT my_index
  {
    "mappings": {
      "properties": {
        "number_one": {
          "type": "integer",
          "ignore_malformed": true
        },
        "number_two": {
          "type": "integer"
        }
      }
    }
  }
  # number_one是int，ignore_malformed：true后可以写入字符串
  PUT my_index/_doc/1
  {
    "text":       "Some text value",
    "number_one": "foo" 
  }
  PUT my_index/_doc/2
  {
    "text":       "Some text value",
    "number_two": "foo" 
  }
  ```

  

# Mapping 写入建议

- 创建一个临时的index，写入一些样本数据
- 通过index的mapping api获得临时文件的动态mapping定义
- 修改后使用该配置创建你的索引
- 删除临时索引





