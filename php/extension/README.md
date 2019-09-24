# php 扩展安装

## 比如安装 fileinfo 扩展

看看php安装包里面有没有 fileinfo 扩展

```
find / -name fileinfo
```

```
phpize

./configure
--with-php-config=PHP安装路径下的/bin/php-config
make && make install


Build complete.
Don't forget to run 'make test'.

Installing shared extensions:     /usr/local/php7/lib/php/extensions/no-debug-non-zts-20170718/

```
安装成功

添加扩展到配置文件 ：PHP安装路径下的/etc/php.ini
extension=fileinfo.so

```
php -m |grep fileinfo
```