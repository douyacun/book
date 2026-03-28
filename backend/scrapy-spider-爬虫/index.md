---
Title: scrapy-spider-爬虫
LegacyId: b9ded7117151164c0d0a3372691a6d47
Slug: scrapy-spider-爬虫
Category: backend
Summary: '### bug1: ignore process 400 ... 解决方案: 1. 造成400的原因主要是因为请求头的问题, 谷歌搜到解决方式:
  - content-length: 长度不要自己去计算, - User-Agent: 可以随机'
SeoTitle: scrapy-spider-爬虫
SeoDescription: '### bug1: ignore process 400 ... 解决方案: 1. 造成400的原因主要是因为请求头的问题, 谷歌搜到解决方式:
  - content-length: 长度不要自己去计算, - User-Agent: 可以随机'
Date: 2019-09-24T20:04:29+08:00
LastEditTime: 2019-09-24T20:04:29+08:00
---

# scrapy - spider 爬虫

# 问题记录

### bug1: ignore process 400 ...
解决方案:
1. 造成400的原因主要是因为请求头的问题, 谷歌搜到解决方式: 

    - content-length: 长度不要自己去计算,
    
    - User-Agent: 可以随机, 使用 middleware fake-userAgent
        ```

        ```
