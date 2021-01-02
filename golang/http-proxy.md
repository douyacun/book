---
Title: go代理请求
Keyworlds: go,http,proxy,Transport,代理
Description: 解决请求 github api timeout的问题
---

![134245504_789637481968571_8633124284968352778](./assert/134245504_789637481968571_8633124284968352778.jpg)

作为一名程序员有经常访问github、google的情况。比如github、google登录



最初我的解决方案是从阿里云租一台轻量级香港的服务器每月25元，持续了有大半年。但是实在没有别的用途，我又有其他代理可以访问。最近就决定释放阿里云的轻量级服务器了。



关于我是如何通过阿里云服务器代理访问国外服务的，代码放在gtihub上: https://github.com/douyacun/gproxy

支持http代理  、文件下载代理：https://github.com/douyacun/gproxy/blob/master/internal/controllers/request.go



这里说一下go使用代理的方式：

```go
proxy, _ := url.Parse("http://127.0.0.1:7891")
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

