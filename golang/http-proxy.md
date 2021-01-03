---
Title: "go代理: http/socks5 proxy"
Keyworlds: "go,http,proxy,Transport,代理"
Description: "解决: 在服务上使用代理访问github、google"
---

![134245504_789637481968571_8633124284968352778](./assert/134245504_789637481968571_8633124284968352778.jpg)

博客支持github、gg登录，github虽然偶尔能访问成功，但大部分是timeout，gg直接不用说



最初我的解决方案是从阿里云租一台轻量级香港的服务器每月25元，持续了有大半年。但是实在没有别的用途，我又有其他代理可以访问。最近就决定释放阿里云的轻量级服务器了。



关于我是如何通过阿里云服务器代理访问国外服务的，代码放在gtihub上: https://github.com/douyacun/gproxy

支持http代理  、文件下载代理：https://github.com/douyacun/gproxy/blob/master/internal/controllers/request.go



go也是支持通过 socks5 和 http 代理访问的:

# http代理

正确的http代理使用姿势

```go
proxy, _ := url.Parse("http://127.0.0.1:1081")
clt := http.Client{
  Timeout: 5 * time.Second,
  Transport: &http.Transport{
    Proxy:           http.ProxyURL(proxy),  // 使用代理请求
    TLSClientConfig: &tls.Config{InsecureSkipVerify: true}, // 跳过https证书资格验证
  },
}
resp, err := clt.Get(fmt.Sprintf("http://www.google.com/ping?sitemap=%s", sitemapUrl))
```

go http提供了2个函数:

```go
// 使用环境变量: HTTP_PROXY,HTTPS_PROXY
func ProxyFromEnvironment(req *Request) (*url.URL, error)
// 指定端口
func ProxyURL(fixedURL *url.URL) func(*Request) (*url.URL, error)
```



# Socks5 代理

http代理直接使用http包即可完成，不过socks5就得用: [golang.org/x/net/proxy](https://pkg.go.dev/golang.org/x/net/proxy#Dialer)

```go
package main

import (
	"crypto/tls"
	"fmt"
	"golang.org/x/net/proxy"
	"io/ioutil"
	"log"
	"net/http"
	"time"
)

func main() {
	dialer, err := proxy.SOCKS5("tcp", "127.0.0.1:1080", nil, proxy.Direct)
	if err != nil {
		log.Fatalf("socks5 init err: %s", err.Error())
		return
	}
	clt := http.Client{
		Timeout: 5 * time.Second,
		Transport: &http.Transport{
			TLSClientConfig: &tls.Config{InsecureSkipVerify: true},
			Dial:            dialer.Dial,
		},
	}
	resp, err := clt.Get("https://www.google.com")
	if err != nil {
		log.Fatalf("ping google sitemap err: %s", err.Error())
		return
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		log.Fatalf("curl google failed~")
		return
	}
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("google response body read err: %s", err.Error())
	}
	fmt.Println(string(body))
	return
}
```

结果是可以正常返回的，服务器是在北京的。

![image-20210103211632721](./assert/image-20210103211632721.png)

如果你想问我梯子是如何搭的话，可以邮箱联系我，不公开讨论！

