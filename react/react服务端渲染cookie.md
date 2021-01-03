---
Title: "react服务端渲染: cookie如何透传给后端，后端如何设置cookie"
Keywords: "react,server render,cookie,服务端渲染,useEffect"
Description: "nextjs getInitialProps 是在服务端执行的代码，可以通过 request.header 获取到 cookie 透传给后端接口"
---

react 服务端渲染 cookie 有2个问题：

1. [服务端执行的代码如何拿到cookie并解析透穿给后端接口，后端又如何设置cookie？](#服务端cookie透传)
2. [浏览器执行的代码如何读取cookie？](#客户端读取cookie)



# 服务端cookie透传

明白next的渲染流程很重要：

浏览器打开页面 -> node服务请求后端接口 -> node服务渲染页面 并响应



node服务透传cookie给后端接口：此时cookie只能从request中取的cookie:  [nextjs文档](https://nextjs.org/docs/api-reference/data-fetching/getInitialProps)

```react
Article.getInitialProps = async ({ req, query }) => {
    // 文章详情
    const { article, statusCode, errMessage } = await GET({
        url: `/api/article/${id}`,
        headers: {
            Cookie: req.headers.cookie
        }
    }).then(resp => {
        return {article: resp.data, statusCode: resp.code, errMessage: resp.message}
    })
    return { article: article, statusCode: statusCode, errMessage: errMessage }
}
```



后端设置cookie：

- 如果是node服务请求的后端接口，此时的node服务就相当于nginx一样的代理请求，需要将响应的cookie透传给浏览器

```react
import React from "react";
import {GET} from '../src/request';

function Demo ({data}) {
    return (
        <div>
            {data}
        </div>
    )
}

Demo.getInitialProps = async ({ req, query, res }) => {
    let cookie;
    const {data, code, message} = await GET({url: "http://douyacun.io/api/helper/demo", noCheck: true}).
    then((resp) => {
        res.setHeader("set-cookie", resp.headers["set-cookie"]);
        return resp.data
    }).
    then((data) => {
        return data
    })
    return {data: data}
}

export default Demo
```

GET 是封装的 axios，axios 的response不是node原生的 [response](https://github.com/axios/axios#response-schema)，而nextjs的res是node原生的[response](https://nodejs.org/api/http.html#http_class_http_serverresponse)，这里需要注意一下使用的请求库

**重点是：明白node服务渲染，其实是一层代理服务**

- 如果是浏览器客户端请求的后端接口，response是可以直接被浏览器读取到的，就不需要我们做什么事情了。



# 客户端读取cookie

客户端请求接口会有2个cookie问题：

1. 跨域请求携带cookie
2. 如何读取并解析cookie

### 跨域请求携带cookie

原理是一致的，先header请求服务端，验证服务端是否支持跨域，以及支持的域名：

```http
Access-Control-Allow-Origin: https://www.douyacun.com
```

> 这里一定要明确指定域名，否则请求还是无法携带cookie的

axios 需要指定：

```http
withCredentials: true
```



### 浏览器读取并解析cookie

用户信息存cookie的原因是不安全，也不建议的。当然前提用户信息存在隐私数据，解决方案是存在session中，cookie只是存session id。

我的博客cookie只存 昵称和ID。如何确保cookie不被篡改呢？cookie，对数据进行签名，签名使用的key是存储在服务端里。如果有更高的安全需求的，请移步～

重点：**react服务端渲染，cookie是存在浏览器上的，确保代码是在浏览器客户端上执行的才能读取到对吧**

```react
import React from 'react';
import { parseCookies } from 'nookies'

function Demo() {
    const [user, setUser] = React.useState({})
    // 首次加载
    React.useEffect(() => {
        const all = parseCookies();
        if (all.douyacun) {
            setUser(JSON.parse(all.douyacun));
        }
    }, [])
  return (<div>Hello {user.name}.</div>)
}

export default demo
```

[React.useEffect的用法](https://zh-hans.reactjs.org/docs/hooks-reference.html#useeffect), 需要注意的是:

> 默认情况下，effect 将在每轮渲染结束后执行，但你可以选择让它 [在只有某些值改变的时候](https://zh-hans.reactjs.org/docs/hooks-reference.html#conditionally-firing-an-effect) 才执行。
>
> 如果想执行只运行一次的 effect（仅在组件挂载和卸载时执行），可以传递一个空数组（`[]`）作为第二个参数。这就告诉 React 你的 effect 不依赖于 props 或 state 中的任何值，所以它永远都不需要重复执行，解析cookie只需要一次就够了～

