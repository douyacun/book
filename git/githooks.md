* 网上很多资料
* 此文档是本人亲自实践并且成功的案例

# githooks 自动化部署

## 第一步 配置ssh
```shell
$ cd  ~/.ssh
$ vim authorized_keys
```
- 粘贴你的ssh 公钥
- 可以使用 `ssh-keygen -t rsa -f test`
-  `-f`是指定生成的秘钥文件名

<hr />

## 第二步创建远程代码仓库
```bash
$ cd ~
$ mkdir codespace
$ git init --bare
$ cd codespace/hooks
$ cp post-receive.sample post-receive
$ vim post-receive
```

- 以下是 `post-receive` 的代码,别问我为什么自己去查

```shell
#!/bin/sh

unset GIT_DIR

DIR_CHAT=/www/chat

cd $DIR_CHAT
git add . -A && git stash && git stash clear
git pull origin master

```
- git stash 注意一下

## 第三步  自动部署代码到项目根目录 - 服务器

```
$ cd /www/
$ mkdir demo
$ git init 
$ git remote add origin ~/codespace
$ git pull
```

## 第四部  本地仓库
- git clone root@hostname:~/codespace
- root登录密码
- touch readmd.md
- git add --all
- git commit -m 'readme'
- git push -u origin master 

## 第五步  开心的时刻
- cd /www/demo
- ll
- 查看readmd是否存在
