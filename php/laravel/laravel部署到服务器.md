* laravel部署到服务器踩过的坑
* author 刘宁
* date 2017-07-01

环境:lnmp环境 laravel5.4 git工具 crp.aliyun.com代码自动化部署

# 第一步
从code.aliyun.com上克隆代码
```
git clone git@code.aliyun.com:douyacun/api.git
```
注意:
- 一般是不上传vendor目录下边的 Composer 依赖,.gitignore会忽略`/vendor`,所以我们需要在服务器上安装composer
- 安装composer : 2017年6月初的一个星期Windows上装composer一键安装包出现了bug导致当时无法安装laravel,千辛万苦的才弄清楚,使用composer.phar这个文件就可以使用composer ,`php composer.phar install`就可以安装laravel

```shell
curl -sS https://getcomposer.org/installer | php
/*全局调用*/
mv composer.phar /usr/local/bin/composer
/* 查看composer版本 */
composer -v
```

- 有`composer.json`和`composer.lock`我们就可以使用 `composer install`安装所有composer依赖包
- 到这一步问题就来了,`composer install` 没法用,解决问题PHP扩展的问题:
    + PHP >= 5.6.4
    + OpenSSL PHP Extension
    + PDO PHP Extension
    + Mbstring PHP Extension
    + Tokenizer PHP Extension
    + XML PHP Extension
- 我的是因为Mbstring的问题导致没法使用,解决:
```
/* 前提你是使用remi的yum源 */
yum install --enablerepo=remi-php70 -y php-Mbstring
```
到这里代码算是上到服务器了

# 第二步

首先需要配置一下Nginx将域名root解析到 `/public` 下,刚刚接触Nginx的同学需要去看一下 <strong style="color:red">[nginx](https://my.oschina.net/u/1156660/blog/364955)</strong> 语法
```
server {
listen 80;
server_name api.douyacun.com;
    //可以根据子域名解析到指定目录下
	#if ( $host ~* (.+)?douyacun.com ) {
	#	set $www_root $1;
	#}

    //解决跨域请求 前后端分离(单独域名) 前段请求后端会存在跨域的问题
	if ( $http_origin ~* demo\.com ) {
		set $allow_url $http_origin;
	}
	add_header Access-Control-Allow-Origin $allow_url;//设置允许请求源
    add_header Access-Control-Allow-Credentials true;
    add_header 'Access-Control-Allow-Headers' 'X-Requested-With,Cookie,Set-Cookie';//设置cookie

    index index.php index.htm index.html;
    root /www/public;//设置入口文件的目录

    //转发所有的请求到 index.php 前端控制器
	location / {
		try_files $uri $uri/ /index.php?$query_string;
	}

    //指定fastcgi解析PHP文件
	location ~* ^(.+\.php|php5)(.*)$ {
		fastcgi_pass 127.0.0.1:9000;
		fastcgi_index index.php;
		include fastcgi.conf;
	}
}
```
这个时候能访问了

# 第三步
这个问题困扰了我一天,现在晚上1点多,关于laravel的 <b>`/storage`</b>权限的问题,
刚开始我给的`chmod -R 775 storage/`,然后就刷新吧反正laravel的主页面就是不出来,
### <strong style="color:red">chmod -R 777 storage/</strong>

# 忠告
- git必须会
- 需要了解crp.aliyun.com的代码自动部署,或者githook做部署
- nginx语法
