# php 安装 maxmind 扩展

## centos7

```
yum install libmaxminddb libmaxminddb-devel -y
```

## 需要两个composer包
```
composer require geoip2/geoip2:~2.0
composer require maxmind-db/reader:~1.0
```

## 安装
```
cd vendor/maxmind-db/reader/ext
phpize
./configure
make
make test
make install

echo "extension=maxminddb.so" > /etc/php.d/maxminddb.ini
```

## 验证
```
php -i | grep maxmind
```

## issue
```
PHP Warning: PHP Startup: Unable to load dynamic library '/usr/lib64/php/modules/maxminddb.so' - libmaxminddb.so.0: cannot open shared object file: No such file or directory in Unknown on line 0
To solve this, execute:
```
解决
```
sudo sh -c "echo /usr/local/lib  >> /etc/ld.so.conf.d/local.conf"
sudo ldconfig
```