---
Title: ci_github_自动部署 
Keywords: ci,github,自动部署
Description: github ci 自动部署
Author: douyacun
Date: 2018-03-24 14:36:50
LastEditTime: 2019-02-02 17:09:20
---
# travis

一个和github结合紧密的持续集成部署工具。

## 安装travis
```
mac:
# brew install ruby
# gem install travis

```


## .travis.yml文件

1. 加密ssh私钥：

```
travis encrypt-file ~/.ssh/id_rsa --add

- openssl aes-256-cbc -K $encrypted_fee1ab4be628_key -iv $encrypted_fee1ab4be628_iv
  -in id_rsa.enc -out ~\/.ssh/id_rsa -d

注意: 删掉 ~\/.ssh/id_rsa -d  反斜线, 否则发生莫名的错误

- openssl aes-256-cbc -K $encrypted_fee1ab4be628_key -iv $encrypted_fee1ab4be628_iv
  -in id_rsa.enc -out ~/.ssh/id_rsa -d
```

## 配置介绍:

```
language: android   # 声明构建语言环境

notifications:      # 每次构建的时候是否通知，如果不想收到通知邮箱（个人感觉邮件贼烦），那就设置false吧
  email: false

sudo: false         # 开启基于容器的Travis CI任务，让编译效率更高。

android:            # 配置信息
  components:
    - tools
    - build-tools-23.0.2
    - android-23
    - extra-android-m2repository     # Android Support Repository
    - extra-android-support          # Support Library

before_install:
 - chmod +x gradlew  # 改变gradlew的访问权限

script:              # 执行:下面的命令
  - ./gradlew assembleRelease

before_deploy:       # 部署之前
  # 使用 mv 命令进行修改apk文件的名字
  - mv app/build/outputs/apk/app-release.apk app/build/outputs/apk/buff.apk

deploy:              # 部署
  provider: releases # 部署到GitHub Release，除此之外，Travis CI还支持发布到fir.im、AWS、Google App Engine等
  api_key:           # 填写GitHub的token （Settings -> Personal access tokens -> Generate new token）
    secure: 7f4dc45a19f742dce39cbe4d1e5852xxxxxxxxx
  file: app/build/outputs/apk/buff.apk   # 部署文件路径
  skip_cleanup: true     # 设置为true以跳过清理,不然apk文件就会被清理
  on:     # 发布时机
    tags: true       # tags设置为true表示只有在有tag的情况下才部署
```

```
language: php
php:
- 7.2
- nightly
env:
- APP_DEBUG=false
before_install:
- openssl aes-256-cbc -K $encrypted_2168355daa83_key -iv $encrypted_2168355daa83_iv
  -in id_rsa.enc -out ~/.ssh/id_rsa -d
- chmod 600 ~/.ssh/id_rsa
install:
- composer install --prefer-dist --optimize-autoloader --quiet
script:
- cp .env.example .env
- php artisan key:generate
- ./vendor//bin/phpunit
after_success:
- ssh root@35.201.219.121 "cd /www/test && git pull"
addons:
  ssh_known_hosts: 35.201.219.121
```

```
language: php
php:
- 7.2
- nightly
before_install:
- openssl aes-256-cbc -K $encrypted_845395c4607a_key -iv $encrypted_845395c4607a_iv
  -in brahmaos.pem.enc -out ~/.ssh/brahmaos.pem -d
- chmod 600 ~/.ssh/brahmaos.pem
- echo -e "Host tokyo\n\thostname ${hostname_tokyo}\n\tport ${port_tokyo}\n\tuser ${user_tokyo}\n\tIdentityFile  ~/.ssh/brahmaos.pem\nHost london\n\thostname ${hostname_london}\n\tport ${port_london}\n\tuser ${user_london}\n\tIdentityFile  ~/.ssh/brahmaos.pem" > ~/.ssh/config
- cat ~/.ssh/config
after_success:
- ssh tokyo "cd /www/brahmaos.io/ && sudo git pull"
- ssh london "cd /www/brahmaos.io/ &&  git pull"
env:
- APP_DEBUG=false
addons:
  ssh_known_hosts:
  - 13.230.159.122
  - 159.65.22.214
```

