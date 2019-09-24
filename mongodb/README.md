# mongodb

## 推荐 [中文文档](http://www.mongoing.com/docs/index.html)

## 安装

选择操作系统直接下载就可以, [官网下载](https://www.mongodb.com/download-center/community) Servers

```
// 解压
tar xzvf mongodb-linux-x86_64-rhel70-4.0.0.tgz -C /opt/

// 软件一般都放到 /usr/local/
cd /opt/
mv mongodb-linux-x86_64-rhel70-4.0.0/ /usr/local/mongodb
```

## 配置 数据库 和 日志 的位置
```
mkdir -p /data/mongodb/db
mkdir -p /var/log/mongodb/
```

## 添加全局变量
```
vim /etc/profile
export MONGODB_HOME=/usr/local/mongodb
export PATH=$PATH:$MONGODB_HOME/bin

source /etc/profile
```

## 配置文件
```
vim /etc/mongodb.conf

dbpath = /data/mongodb/db/
logpath = /var/log/mongodb/mongodb.log
fork=true
bind_ip=0.0.0.0
#auth=true
```

## 启动mongodb
```
mongod -f /etc/mongodb.conf
```


>>> 如果报错, 记得查看 mongodb 日志文件


## 设置密码
> 需要首先创建 root 角色的用户， 否则无法命令行登录

```
use admin
db.createUser( { user: "admin", pwd: "password", roles: [ { role: "root", db: "admin" } ] } );
exit
```
重启进程
```
ps -aux|grep mongodb
kill -9  进程号
```
配置密码登录， 改配置文件的 auth =  true， 启动进程
```
mongod -f /etc/mongodb.conf
```
登录
```
mongo -u root -p 123456 --authenticationDatabase admin
```

配置普通用户

内建角色

- Read：允许用户读取指定数据库
- readWrite：允许用户读写指定数据库
- dbAdmin：允许用户在指定数据库中执行管理函数，如索引创建、删除，查看统计或访问system.profile
- userAdmin：允许用户向system.users集合写入，可以找指定数据库里创建、删除和管理用户
- clusterAdmin：只在admin数据库中可用，赋予用户所有分片和复制集相关函数的管理权限。
- readAnyDatabase：只在admin数据库中可用，赋予用户所有数据库的读权限
- readWriteAnyDatabase：只在admin数据库中可用，赋予用户所有数据库的读写权限
- userAdminAnyDatabase：只在admin数据库中可用，赋予用户所有数据库的userAdmin权限
- sdbAdminAnyDatabase：只在admin数据库中可用，赋予用户所有数据库的dbAdmin权限。
- root：只在admin数据库中可用。超级账号，超级权限