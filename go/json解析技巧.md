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
```

我的model结构体:
```golang
type Github struct {
	Id                int         `json:"id"`
	Login             string      `json:"login"`
	NodeId            string      `json:"node_id"`
	AvatarUrl         string      `json:"avatar_url"`
	GravatarId        string      `json:"gravatar_id"`
	Url               string      `json:"url"`
	HtmlUrl           string      `json:"html_url"`
	FollowersUrl      string      `json:"followers_url"`
	FollowingUrl      string      `json:"following_url"`
	GistsUrl          string      `json:"gists_url"`
	StarredUrl        string      `json:"starred_url"`
	SubscriptionsUrl  string      `json:"subscriptions_url"`
	OrganizationsUrl  string      `json:"organizations_url"`
	ReposUrl          string      `json:"repos_url"`
	EventsUrl         string      `json:"events_url"`
	ReceivedEventsUrl string      `json:"received_events_url"`
	Type              string      `json:"type"`
	SiteAdmin         int8        `json:"site_admin"`       // <------- issue
	Name              string      `json:"name"`
	Company           string      `json:"company"`
	Blog              string      `json:"blog"`
	Location          string      `json:"location"`
	Email             string      `json:"email"`
	Hireable          string      `json:"hireable"`
	Bio               string      `json:"bio"`
	PublicRepos       int         `json:"public_repos"`
	PublicGists       int         `json:"public_gists"`
	Followers         int         `json:"followers"`
	Following         int         `json:"following"`
	CreatedAt         time.Time   `json:"created_at"`
	UpdatedAt         time.Time   `json:"updated_at"`
}

func (g *Github) TableName() string {
	return "github"
}
```
github response.json.site_admin 是 bool 类型，但是mysql入库的话是存储为tinyint(int8)类型,json.Unmarkshal的就会返回一个error
```error
json: cannot unmarshal bool into Go struct field githubUser.site_admin of type models.IsSiteAdmin
```

看了一下大家的玩法，都是通过interface类型断言来转换数据的
```golang
data := ioutil.readall(response.Body)
log.info(string(data))
var d interface
err := json.Unmarshal(data, &d)
m := i.(map[string]interface{})
for k, v := range m {
    switch r := v.(type) {
    case string:
        fmt.Println(k, " is string ", r)
    case int:
        fmt.Println(k, " is int ", r)
    case []interface{}:
        fmt.Println(k, " is array ", )
        for i, u := range r {
            fmt.Println(i, u)
        }
    default:
        fmt.Println(k, " cannot be recognized")
    }
}
```
这样虽然能正确拿到各个值，问题是，拿到这些个fields之后怎么赋值给github model struct？挨个匹配字段然后赋值给struct？想起来laravel的优雅，我就去用跛脚的英语研究官方给出的json文档了
```golang
package main

import (
	"encoding/json"
	"fmt"
	"log"
	"strings"
)

type Animal int

const (
	Unknown Animal = iota
	Gopher
	Zebra
)

func (a *Animal) UnmarshalJSON(b []byte) error {
	var s string
	if err := json.Unmarshal(b, &s); err != nil {
		return err
	}
	switch strings.ToLower(s) {
	default:
		*a = Unknown
	case "gopher":
		*a = Gopher
	case "zebra":
		*a = Zebra
	}

	return nil
}

func (a Animal) MarshalJSON() ([]byte, error) {
	var s string
	switch a {
	default:
		s = "unknown"
	case Gopher:
		s = "gopher"
	case Zebra:
		s = "zebra"
	}

	return json.Marshal(s)
}

func main() {
	blob := `["gopher","armadillo","zebra","unknown","gopher","bee","gopher","zebra"]`
	var zoo []Animal
	if err := json.Unmarshal([]byte(blob), &zoo); err != nil {
		log.Fatal(err)
	}

	census := make(map[Animal]int)
	for _, animal := range zoo {
		census[animal] += 1
	}

	fmt.Printf("Zoo Census:\n* Gophers: %d\n* Zebras:  %d\n* Unknown: %d\n",
		census[Gopher], census[Zebra], census[Unknown])

}
```
json给出的两个interface, 类型只要实现这两个接口，json.Unmarshal/json.Marshal 就会调用这两个方法
```golang
type Unmarshaler interface {
	UnmarshalJSON([]byte) error
}
type Marshaler interface {
	MarshalJSON() ([]byte, error)
}
```

```golang
type IsSiteAdmin int8

type Github struct {
	SiteAdmin         IsSiteAdmin `json:"site_admin"`
	// ....
}

func (g *Github) TableName() string {
	return "github"
}

// 这里会把要解析的`data []byte`传进来
func (d *IsSiteAdmin) UnmarshalJSON(data []byte) error {
	if string(data) == "true" {
		*d = 1
	} else {
		*d = 0
	}
	return nil
}

func (d IsSiteAdmin) MarshalJSON() (data []byte, err error) {
	if d == 1 {
		return []byte("true"), nil
	} else {
		return []byte("false"), nil
	}
}
```