# intl 扩展安装


## 安装 icu
```
wget http://download.icu-project.org/files/icu4c/56.1/icu4c-56_1-src.tgz
tar -xzvf icu4c-56_1-src.tgz
cd icu/source
./configure --prefix=/usr/local/icu
make && make install
```

## 安装 intl for php
```
wget http://pecl.php.net/get/intl-3.0.0.tgz
tar -xzvf intl-3.0.0.tgz
cd intl-3.0.0
phpize ./configure –enable-intl –with-icu-dir=/usr/local/icu
make && make install
```

```
extension=intl.so
```

## php7.2 安装方式

这里是投机取巧, 本人用的remi源

```
yum search intl

yum install php72-php-intl.x86_64

find / -name intl.so

可以找到 so 文件
/opt/remi/php72/root/usr/lib64/php/modules/intl.so

mv /opt/remi/php72/root/usr/lib64/php/modules/intl.so /usr/lib64/php/modules
```
