# scrapy - spider 爬虫

# 问题记录

### bug1: ignore process 400 ...
解决方案:
1. 造成400的原因主要是因为请求头的问题, 谷歌搜到解决方式: 

    - content-length: 长度不要自己去计算,
    
    - User-Agent: 可以随机, 使用 middleware fake-userAgent
        ```

        ```