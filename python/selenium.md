# selenium
 
 - 安装
 
 ```pip install selenium```
 
 - 错误:
```
FileNotFoundError: [Errno 2] No such file or directory: 'chromedriver': 'chromedriver'
```

需要安装 \
[selenium](https://sites.google.com/a/chromium.org/chromedriver/downloads)

直接放到 环境变量中就行

 - 使用
```
from lxml import etree
from selenium import webdriver

def fetch(self, url):
    browser = webdriver.Chrome()
    browser.get(url)
    html = browser.execute_script("return document.documentElement.outerHTML")
    browser.close()
    print(html)
    selector = etree.HTML(html)
    return selector
```

# centos7 安装

需要安装 Linux chrome

/etc/yum.repos.d/google-chrome.repo
```
[google-chrome]
name=google-chrome
baseurl=http://dl.google.com/linux/chrome/rpm/stable/x86_64
enabled=1
gpgcheck=1
gpgkey=https://dl.google.com/linux/linux_signing_key.pub
```

安装 `yum -y install google-chrome-stable --nogpgcheck`


python 中使用
```python
from selenium import webdriver

chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--headless')
chrome_options.add_argument('--no-sandbox')
chrome_options.add_argument('--disable-dev-shm-usage')
browser = webdriver.Chrome(chrome_options=chrome_options)
browser.get("https://www.baidu.com")
```
