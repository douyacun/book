---
Title: go json 解析技巧,go json.Unmarshal 常见问题
Keywords: Unmarshal,UnmarshalJSON,MarshalJSON
Description: cannot unmarshal bool into Go struct field githubUser.site_admin of type models.IsSiteAdmin, go语言的json解析的确没有python和php安逸
Label: json解析
Author: douyacun
Date: 2019-08-20 00:01:11
LastEditTime: 2019-10-09 20:33:48
---

在做github oauth第三方登录，code->access_token->user, user接口返回的是json数据，思路是：拿到access_token，请求用户信息，拿到response.json, `json.Unmarshal(data, &github)`, 直接入库就ok了，问题就出在json.Unmarshal上

response.json
```json
{
	"login": "douyacun",
	"id": 35790720,
	"node_id": "MDQ6VXNlcjM1NzkwNzIw",
	"avatar_url": "https://avatars1.githubusercontent.com/u/35790720?v=4",
	"gravatar_id": "",
	"url": "https://api.github.com/users/douyacun",
	"html_url": "https://github.com/douyacun",
	"followers_url": "https://api.github.com/users/douyacun/followers",
	"following_url": "https://api.github.com/users/douyacun/following{/other_user}",
	"gists_url": "https://api.github.com/users/douyacun/gists{/gist_id}",
	"starred_url": "https://api.github.com/users/douyacun/starred{/owner}{/repo}",
	"subscriptions_url": "https://api.github.com/users/douyacun/subscriptions",
	"organizations_url": "https://api.github.com/users/douyacun/orgs",
	"repos_url": "https://api.github.com/users/douyacun/repos",
	"events_url": "https://api.github.com/users/douyacun/events{/privacy}",
	"received_events_url": "https://api.github.com/users/douyacun/received_events",
	"type": "User",
	"site_admin": false,
	"name": "douyacun",
	"company": null,
	"blog": "",
	"location": null,
	"email": null,
	"hireable": null,
	"bio": null,
	"public_repos": 22,
	"public_gists": 0,
	"followers": 2,
	"following": 1,
	"created_at": "2018-01-25T03:54:18Z",
	"updated_at": "2019-07-26T05:26:33Z"
}
var p = person{
    Name: "",
    Age:  0,
}
if res, err := json.Marshal(&p); err != nil {
    fmt.Println(err)
    return
} else {
    fmt.Println(string(res))
}
//  {"age":"0"}
```

2.  string, 与php、js若语言类型交互的时候，经常面临数字类型和字符串转换问题

```go
type person struct {
	Name string `json:"name,omitempty"`
	Age  int    `json:"age,string"` // go期望是int类型，而json串里的age是字符串
}
var p1 person
str := `{"name": "douyacun", "age": "18"}`
if err := json.Unmarshal([]byte(str), &p1); err != nil {
    fmt.Println(err)
    return
}
fmt.Println(p1)
// {douyacun 18}
// 如何不加string，会报错: json: cannot unmarshal string into Go struct field person.age of type int
```



