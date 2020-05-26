---
Title: go json 解析小技巧
Keywords: omitempty,数据流json解码,原生字符串
Description: json包的使用不是像php那样json_encode/json_decode,有更大操作空间
Label: json解析
Author: douyacun
Date: 2019-08-20 00:01:11
LastEditTime: 2019-10-09 20:33:48
---

1. `omitempty` 字段为空时( 空指针, 空接口, 空数组, slice, map, or string.)，json encode时不返回该字段

```go
type person struct {
	Name string `json:"name,omitempty"`
	Age  int    `json:"age"`
}

var p = person{
  Name: "",
  Age:  18,
}
if res, err := json.Marshal(&p); err != nil {
  fmt.Println(err)
	return
} else {
  fmt.Println(string(res))
}
// {"age":18}
```

2. `string` 字符串和数字类型转换，对于php和js这类弱类型语言字符串和数字很容易就混

```go
type person struct {
	Name string `json:"name,omitempty"`
	Age  int    `json:"age,string"`
}

var p1 person
str := `{"name":"douyacun", "age": "18"}`
if err := json.Unmarshal([]byte(str), &p1); err != nil {
  fmt.Println(err)
  return
}
fmt.Println(p1)
// {douyacun 18}
```

如果age 没有string tag的话，会报错 `json: cannot unmarshal string into Go struct field person.age of type int`

3. `-` 忽略字段

```go
type person struct {
	Name   string `json:"name,omitempty"`
	Age    int    `json:"age,string"`
	Height int    `json:"-"`
}
var p = person{
  Name: "",
  Age:  18,
  Height: 180,
}
if res, err := json.Marshal(&p); err != nil {
  fmt.Println(err)
  return
} else {
  fmt.Println(string(res))
}
// {"age":"18"}
var p1 person
str := `{"name":"douyacun", "age": "18", "height": 180}`
if err := json.Unmarshal([]byte(str), &p1); err != nil {
  fmt.Println(err)
  return
}
fmt.Println(p1)
// {douyacun 18 0}
```

4. 判断json字符串是否有效

```json
fmt.Println(json.Valid([]byte(`{"name":"douyacun", "age": "18", "height": 180}`)))
// true
```

5. 从数据流读取字符串并解码

```go
var jsonStream = `
	{"Message": "Hello", "Array": [1, 2, 3], "Null": null, "Number": 1.234}
`
des := json.NewDecoder(strings.NewReader(jsonStream))
for{
  t, err := des.Token()
  if err == io.EOF {
    break
  }
  if err != nil {
    fmt.Printf("%v\n", err)
    return
  }
  if _, ok := t.(json.Delim); !ok {
    fmt.Printf("%T: %v\n", t, t)
  }
}
// string: Hello
// string: Array
// float64: 1
// float64: 2
// float64: 3
// string: Null
// <nil>: <nil>
// string: Number
// float64: 1.234
```

6. 对于不确定是int或者float的字段，可以使用json.Number

```go
type person struct {
	Weight json.Number `json:"weight"`
}

var p person
str := `{"weight": 19}`
if err := json.Unmarshal([]byte(str), &p); err != nil {
  fmt.Println(err)
  return
}
fmt.Println(p.Weight.Int64())
// 19 <nil>
```

7. 可以像orm一样写原生sql。像es的查询结果如何定义结构体：

**编码**

```go
type person struct {
	Weight json.Number     `json:"weight"`
	Skill  json.RawMessage `json:"skill"`
}

skill := map[string]interface{}{
  "language": []string{"c", "php", "js", "golang"},
  "js":       []string{"react"},
  "php":      []string{"Laravel", "ThinkPHP"},
  "golang":   []string{"gin"},
}
res, err := json.Marshal(skill)
if err != nil {
  fmt.Println(err)
}
p := person{
  Weight: "89",
  Skill:  json.RawMessage(res),
}
if res, err := json.MarshalIndent(p, "", "\t"); err != nil {
  fmt.Println(err)
  return
} else {
  fmt.Println(string(res))
}
```

输出

```json
{
        "weight": 89,
        "skill": {
                "golang": [
                        "gin"
                ],
                "js": [
                        "react"
                ],
                "language": [
                        "c",
                        "php",
                        "js",
                        "golang"
                ],
                "php": [
                        "Laravel",
                        "ThinkPHP"
                ]
        }
}
```

**解码**

像ES search结果集，外层有公共的数据结构

```json
{
  "took" : 0,
  "timed_out" : false,
  "_shards" : {
    "total" : 1,
    "successful" : 1,
    "skipped" : 0,
    "failed" : 0
  },
  "hits" : {
    "total" : {
      "value" : 12,
      "relation" : "eq"
    },
    "max_score" : 1.0,
    "hits" : []
  }
}
fmt.Println(p1)
// {douyacun 18}
// 如何不加string，会报错: json: cannot unmarshal string into Go struct field person.age of type int
```

结构体

```go
type ESListResponse struct {
	Took    int  `json:"took"`
	Timeout bool `json:"timed_out"`
	Shards  struct {
		Total      int `json:"total"`
		Successful int `json:"successful"`
		Skipped    int `json:"skipped"`
		Failed     int `json:"failed"`
	} `json:"_shards"`
	Hits struct {
		Total struct {
			Value    int    `json:"value"`
			Relation string `json:"relation"`
		} `json:"total"`
		MaxScore float64         `json:"max_score"`
		Hits     []json.RawMessage `json:"hits"`
	} `json:"hits"`
}
type ESItemResponse struct {
	Index  string          `json:"_index"`
	Type   string          `json:"_type"`
	Id     string          `json:"_id"`
	Score  float64         `json:"_score"`
	Source json.RawMessage `json:"_source"`
}
```

这个有点复杂，写个简单的例子：

```go
type person struct {
	Weight json.Number     `json:"weight"`
	Skill  json.RawMessage `json:"skill"`
}


str := `{
	"weight": 180,
	"skill": {
		"language": ["php", "golang", "c", "python"],
		"database": ["elasticsearch", "mysql", "redis"]
	}
}`
var p person
if err := json.Unmarshal([]byte(str), &p); err != nil {
  fmt.Println(err)
  return
}
fmt.Printf("weight: %s\nskill: %s\n", p.Weight, p.Skill)

type _skill struct {
  Language []string `json:"language"`
  Database []string `json:"database"`
}

var skill _skill
if err := json.Unmarshal(p.Skill, &skill); err != nil {
  fmt.Println(err)
  return
}
fmt.Printf("%+v\n", skill)
```

输出：

```
weight: 180
skill: {
                "language": ["php", "golang", "c", "python"],
                "database": ["elasticsearch", "mysql", "redis"]
        }
{Language:[php golang c python] Database:[elasticsearch mysql redis]}
```

其实 p.Skill 还是 []byte,可以再次被 Unmarshal
