* yar 安装,使用
* @date 2017-07-16

# yar 安装


### yum 安装   会报错  unsurport extension `msgpack`
```
$ yum install -y --enablerepo=remi-php70 php-yar
```
安装之后可能报错:
```
PHP Warning:  Module 'msgpack' already loaded in Unknown on line 0
```
解决方法:
```
$ vim /etc/php.ini
```
搜索 `msgpack`
```
:/msgpack
```
注释
```
;extension=msgpack.so
```


### pecl安装

- 先安装php-pear php-devel
```
$ yum install -y --enablerepo=remi-php70 php-pear php-devel
```
- 安装yar
```
$ pecl install yar-2.0.0 //多试几次
```
- vim /etc/php.ini 添加
```
extension=yar.so
```
- 报错
```
$ PHP Warning:  PHP Startup: Unable to load dynamic library '/usr/lib64/php/modules/yar.so' - /usr/lib64/php/modules/yar.so: undefined symbol: php_json_decode_ex in Unknown on line 0
```
- vim /etc/php.ini 添加
```
extension=json.so
```
