# scrapy-splash js渲染:

### 参见[教程](http://scrapy-cookbook.readthedocs.io/zh_CN/latest/scrapy-12.html)


### 安装

```
centos7.*
yum install docker

centos6.*
yum install docker-io
```

```
# docker pull scrapinghub/splash

非常慢, 推荐使用阿里云镜像加速:
https://dev.aliyun.com/search.html
管理中心,可以获得专属加速地址

sudo mkdir -p /etc/docker
sudo tee /etc/docker/daemon.json <<-'EOF'
{
  "registry-mirrors": ["https://c09yde5p.mirror.aliyuncs.com"]
}
EOF
systemctl daemon-reload
systemctl restart docker
```

### 启动容器
```
docker run -d -p 5023:5023 -p 8050:8050 -p 8051:8051 scrapinghub/splash
```

### 安装scrapy-splash

在你的scrapy工程的配置文件settings.py中添加

```
SPLASH_URL = 'http://192.168.203.92:8050'
```

添加Splash中间件，还是在settings.py中通过DOWNLOADER_MIDDLEWARES指定，并且修改HttpCompressionMiddleware的优先级

```
DOWNLOADER_MIDDLEWARES = {
    'scrapy_splash.SplashCookiesMiddleware': 723,
    'scrapy_splash.SplashMiddleware': 725,
    'scrapy.downloadermiddlewares.httpcompression.HttpCompressionMiddleware': 810,
}
```

设置splash的去重中间件

```
DUPEFILTER_CLASS = 'scrapy_splash.SplashAwareDupeFilter'
```

### 使用scrapy-splash

最简单的渲染请求的方式是使用scrapy_splash.SplashRequest，通常你应该选择使用这个

```
yield SplashRequest(url, self.parse_result,
    args={
        # optional; parameters passed to Splash HTTP API
        'wait': 0.5,

        # 'url' is prefilled from request url
        # 'http_method' is set to 'POST' for POST requests
        # 'body' is set to request body for POST requests
    },
    endpoint='render.json', # optional; default is render.html
    splash_url='<url>',     # optional; overrides SPLASH_URL
    slot_policy=scrapy_splash.SlotPolicy.PER_DOMAIN,  # optional
)
```

可以在普通的scrapy请求中传递splash请求meta关键字达到同样的效果

```
yield scrapy.Request(url, self.parse_result, meta={
    'splash': {
        'args': {
            # set rendering arguments here
            'html': 1,
            'png': 1,

            # 'url' is prefilled from request url
            # 'http_method' is set to 'POST' for POST requests
            # 'body' is set to request body for POST requests
        },

        # optional parameters
        'endpoint': 'render.json',  # optional; default is render.json
        'splash_url': '<url>',      # optional; overrides SPLASH_URL
        'slot_policy': scrapy_splash.SlotPolicy.PER_DOMAIN,
        'splash_headers': {},       # optional; a dict with headers sent to Splash
        'dont_process_response': True, # optional, default is False
        'dont_send_headers': True,  # optional, default is False
        'magic_response': False,    # optional, default is True
    }
})

```


参数说明:

- meta[‘splash’][‘args’] 包含了发往Splash的参数。
- meta[‘splash’][‘endpoint’] 指定了Splash所使用的endpoint，默认是render.html
- meta[‘splash’][‘splash_url’] 覆盖了settings.py文件中配置的Splash URL
- meta[‘splash’][‘splash_headers’] 运行你增加或修改发往Splash服务器的HTTP头部信息，注意这个不是修改发往远程web站点的HTTP头部
- meta[‘splash’][‘dont_send_headers’] 如果你不想传递headers给Splash，将它设置成True
- meta[‘splash’][‘slot_policy’] 让你自定义Splash请求的同步设置
- meta[‘splash’][‘dont_process_response’] 当你设置成True后，SplashMiddleware不会修改默认的scrapy.Response请求。默认是会返回SplashResponse子类响应比如SplashTextResponse
- meta[‘splash’][‘magic_response’] 默认为True，Splash会自动设置Response的一些属性，比如response.headers,response.body等

接下来我使用splash来爬取

```
import scrapy
from scrapy_splash import SplashRequest


class JsSpider(scrapy.Spider):
    name = "jd"
    allowed_domains = ["jd.com"]
    start_urls = [
        "http://www.jd.com/"
    ]

    def start_requests(self):
        splash_args = {
            'wait': 0.5,
        }
        for url in self.start_urls:
            yield SplashRequest(url, self.parse_result, endpoint='render.html',
                                args=splash_args)

    def parse_result(self, response):
        logging.info(u'----------使用splash爬取京东网首页异步加载内容-----------')
        guessyou = response.xpath('//div[@id="guessyou"]/div[1]/h2/text()').extract_first()
        logging.info(u"find：%s" % guessyou)
        logging.info(u'---------------success----------------')
```