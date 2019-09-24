# 使用scrapinghub

建议上传到github上, 可以与 scrapinghub 关联,每次commit都会自动部署,很方便  \

## 设置python运行环境

[scrapinghub.yml](https://github.com/scrapinghub/shub/blob/master/docs/configuration.rst)详情,

```
导出配置:

pip freeze > requirements.txt
```

## 配置环境
```
vim scrapinghub.yml

projects:
    default: 1 # your-project-id为项目id
stacks:
    default: scrapy:1.1-py3 # 这个注意，如果是python2.7 则写成scrapy:1.1
requirements_file: requirements.txt  #第三方库时引用
```

