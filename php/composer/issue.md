## composer安装时报错 `composer require tymon/jwt-auth`
- 错误
```
Content-Length mismatch
http://packagist.org could not be fully loaded, package information was loaded from the local cache                                       and may be out of date
```
- 解决办法
- 清除缓存
```
composer clearcache
```
- 设置vpn
```
 composer config -g repo.packagist composer https://packagist.phpcomposer.com
```

- composer dump-autoload
    ```
    生产环境:
    composer dump-autoload -o
    ```