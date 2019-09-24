---
Title: 最近为00h.tv开发评论组件，实际走下来，原来我误解了node server
Keywords: git comment,cookie cors,golang gorm
Description: go 为00h.tv开发的评论组件
Author: douyacun
Date: 2019-08-22 00:03:18
LastEditTime: 2019-08-27 12:46:40
---

# 架构（说的高大上一点）
- 前端框架
  - [react](https://zh-hans.reactjs.org/)
  - [nextjs](https://nextjs.org/docs)
  - [nocookie](https://github.com/maticzav/nookies)
  - [http-proxy-middleware](https://github.com/chimurai/http-proxy-middleware)
  - [markdown-it](https://github.com/markdown-it/markdown-it)

# 为什么做ssr
- 首屏渲染速度提升
- 主要目的还是seo优化,搜索引擎收录,免费流量来源，否则谁会搞这么费力不讨好的事情，好要服务器资源，明明前端都能完成的事情

SPA SEO很难

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <link rel="manifest" href="/emoji-search/manifest.json">
    <link rel="shortcut icon" href="/emoji-search/favicon.ico">
    <title>Emoji Search</title>
    <link href="/emoji-search/static/css/main.2e862781.css" rel="stylesheet">
</head>

<body><noscript>You need to enable JavaScript to run this app.</noscript>
    <div id="root"></div><a class="github-fork-ribbon" href="https://github.com/ahfarmer/emoji-search"
        title="Fork me on GitHub">Fork me on GitHub</a>
    <script type="text/javascript" src="/emoji-search/static/js/main.d4350923.js"></script>
</body>

</html>
```

在spa中，js填充内容到`<div>`容器中，googlebot从2014年是开始支持js内容爬取，建议不要依赖Googlebot进行JS抓取，在特定时期内的抓取/索引页面数量 - 与HTML不同，与JS的网站不同。后者更耗费资源并且需要执行渲染，往往具有较低的爬行预算。

# 选择nextjs 做ssr

1. 不需要配置路由，路由根据pages中的文件结构自动生成
```
pages
  ---index.js   // 路由为/index
  ---movie.js   // 路由为movie
  ---my
      --index.js      //路由为/my/index
      -- news-detail  //路由为/my/news-detail
```
2. 服务端请求数据：

```javascript
import React from 'react'

class HelloUA extends React.Component {
  static async getInitialProps({ req }) {
    const userAgent = req ? req.headers['user-agent'] : navigator.userAgent
    return { userAgent }
  }

  render() {
    return <div>Hello World {this.props.userAgent}</div>
  }
}
export default HelloUA
```
对于初始页面加载，`getInitialProps`仅在服务器上执行,`getInitialProps`只能在`pages`下的页面使用，接收以下6个参数

- `pathname` - URL的路径部分
- `query` - 解析为对象的URL的查询字符串部分
- `asPath`- `String`浏览器中显示的实际路径（包括查询）
- `req` - HTTP请求对象（仅限服务器）
- `res` - HTTP响应对象（仅限服务器）
- `err` - 如果在渲染过程中遇到任何错误，则为Error对象

```javascript
<script id="__NEXT_DATA__" type="application/json">{"dataManager":"[]","props":{"pageProps":{"medias":[{"title":"今日最新电影","medias":[{"subtype":"movie","id":"WG3jnN5V3D","title":"心之战","rate":0,"genres":["动作","剧情","犯罪"],"cover":"https://image.00h.tv/2019/08/26/3b602ef9e13034dd.jpg","region":"大陆","created_at":"2019-08-26 19:10:37","summary":"本片以百佳刑警原形人物故事为基准，还原度极高的展现了在这和平年代，伤亡率最多最高危的职业中，优秀刑警们无谓牺牲的精神，身负特殊使命和正义化身的真实写照。","duration":"","released":"2019","casts":["侯岩松","高郡伟","高欣生","齐纪深"]},{"subtype":"movie","id":"mYpDZQlOr4","title":"寻根问祖","rate":0,"genres":["喜剧","剧情"],"cover":"https://image.00h.tv/2019/08/26/ba3f96370742fcbc.jpg","region":"墨西哥","created_at":"2019-08-26 17:10:37","summary":"在他的初恋伤透了他的心后，一位年轻的美国歌手不情愿地离开了他在加利福尼亚州圣地亚哥的家，并开始了一次史诗般的旅行，通过与他的叔叔巴哈，以重新连接他的墨西哥根，找到自己。","duration":"","released":"2019","casts":["Paulina","Gaitan","Lia","Marie","Johnson"]},{"subtype":"movie","id":"Ab3VLwY1gD","title":"羁绊之踏板","rate":0,"genres":["剧情"],"cover":"https://image.00h.tv/2019/08/26/597cbccaa487de5e.jpg","region":"日本","created_at":"2019-08-26 13:10:55","summary":"根据传奇职业自行车手宫泽崇史和母亲的真实故事改编， 相叶雅纪、田中圭、波瑠、出演。  　　论单亲家庭的熊孩子 如何成长为感动日本的赛车手。","duration":"","released":"2019","casts":["相叶雅纪","药师丸博子","波瑠","田中圭","高橋恭平"]},{"subtype":"movie","id":"OAro6xwVg0","title":"滚蛋吧大魔王","rate":0,"genres":["喜剧","爱情","奇......
```
3. 动态路由支持，`/post/:id`, 在server.JS中配置服务
```javascript
server.get('/post/:slug', (req, res) => {
  // /post 是pages/post
  // { slug: req.params.slug }在getInitialProps({query}) query获取到
  return app.render(req, res, '/post', { slug: req.params.slug })
})
```
4. 头部标签支持
5. `req.cookies` - 包含请求发送的cookie的对象。默认为`{}`
6. `next export`是一种将Next.js应用程序作为独立静态应用程序运行的方法，无需Node.js服务器。导出的应用程序几乎支持Next.js的所有功能，包括动态URL，预取，预加载和动态导入。
7. 这些[文档上](https://nextjs.org/docs#static-html-export)都有，但是自己封装很麻烦

# 如何使用cookie

nextjsyou 个误区：你已经不只是写前端页面了，服务端的数据，也是来自于你server.js 或者getInitialProps中，所以cookie的设置也需要你在server.js或在getInitialProps中set

```javascript
  router.get('/oauth/github', async ctx => {
    const queryParams = { ...ctx.query }
    const { code, redirect } = queryParams
    if (code) {
      const response = await axios.post(host + 'api/oauth/github', {
        "code": ctx.query.code,
        validateStatus: function (status) {
          return status > 300 && status < 200; // 状态码在大于或等于500时才会 reject
        }
      })
      const data = await response.data
      if (data && data.code == 0) {
        userInfo = JSON.stringify(data.data)
        ctx.cookies.set("user", userInfo, {
          maxAge: 30 * 24 * 60 * 60,
          path: '/',
        })
      }
    }
    if (redirect) {
      ctx.redirect(redirect)
    } else {
      ctx.redirect('/')
    }
  })
```

我们的go在这的作用只是数据的存取功能了



# cookie跨域问题

上线有域名的话，一般建议前后端使用同一个域名，nginx路由前缀来区分，代理给node和go

```nginx
location /api/ {
  if ($request_method = 'OPTIONS') {
    return 204;
  }
  proxy_set_header X-Real-IP $remote_addr;
  proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  proxy_set_header Host  $http_host;
  proxy_set_header X-Nginx-Proxy true;
  proxy_set_header Connection "";

  proxy_pass    http://127.0.0.1:8000;
}

location / {
  proxy_set_header X-Real-IP $remote_addr;
  proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  proxy_set_header Host  $http_host;
  proxy_set_header X-Nginx-Proxy true;
  proxy_set_header Connection "";

  proxy_pass    http://127.0.0.1:3002;
}
```

那么开发环境怎么办？`localhost:3000`  -> ` www.douyacun.com` 域名不同肯定是存在跨域的，即便go启动的服务在本地`localhost:8000`，端口不同也是存在跨域的，怎么破？

1. 后端允许跨域

```http
// 响应头表示是否可以将对请求的响应暴露给页面
Access-Control-Allow-Credentials: true
// 必须是具体域名,
Access-Control-Allow-Origin: "http://localhost:3000"
// 允许跨域的HTTP方法
Access-Control-Allow-Methods: ["GET","POST","DELETE"]
// 列出将会在正式请求的 Access-Control-Expose-Headers 字段中出现的首部信息
Access-Control-Allow-Headers: ["Content-Type", "Authorization", "Accept"]
```

```javascript
// 表示跨域请求时是否需要使用凭证
axios.defaults.withCredentials = true
```

后端option请求方法，跨域的话前端会预先发起一次options请求，来获取允许请求的资源，就是这些header

2. 后端请求后端是存在跨域, 我们本地使用server.js启动的也是一个服务，从这做做文章，所有的前端请求都经过node启动的server来请求go服务代理拿到数据在返回给客户端，[http-proxy-middleware](https://github.com/chimurai/http-proxy-middleware)

```javascript
const proxy = require('http-proxy-middleware')
const c2k = require('koa2-connect')
router.all('/api/*', c2k(
  proxy({
    target: host,
    changeOrigin: true,
    cookieDomainRewrite: {
      '*': 'localhost:8000'// 这里是将cookie也转发过去
      // '*': 'www.douyacun.com'
    }
  })
))
```

