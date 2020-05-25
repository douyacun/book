---
Title: go json 解析技巧,go json.Unmarshal 常见问题
Keywords: Unmarshal,UnmarshalJSON,MarshalJSON
Description: cannot unmarshal bool into Go struct field githubUser.site_admin of type models.IsSiteAdmin, go语言的json解析的确没有python和php安逸
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

