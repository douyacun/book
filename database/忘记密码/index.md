---
Title: 忘记密码
LegacyId: 5837158700388a68ee4f531349c73863
Slug: 忘记密码
Category: database
Summary: '/** * mysql 无法登录 * @author 刘宁 * @date 2017/04/11 **/ 场景描述: ``` 1 yum --enablerepo=remi
  install -y mysql mysql-server 2 s'
SeoTitle: 忘记密码
SeoDescription: '/** * mysql 无法登录 * @author 刘宁 * @date 2017/04/11 **/ 场景描述: ``` 1
  yum --enablerepo=remi install -y mysql mysql-server 2 s'
Date: 2019-09-24T20:04:29+08:00
LastEditTime: 2019-09-24T20:04:29+08:00
---

/**
* mysql 无法登录
* @author 刘宁
* @date 2017/04/11
**/
场景描述:
```
    1   yum --enablerepo=remi install -y mysql mysql-server
    2   service mysqld start
    3   mysqladmin -u root password 'a123456'
    4   mysql -u root -p123456
    5   use mysql
    7   select * from user
    8   update user set Host = '%' wherer User = 'root'

    9   下次登录
        mysql -u root -p123456
        Access denied for user 'root'@'localhost' (useing password:YES)
```
解决方法:
```
    1   service mysqld stop
    2   rm -rf /var/lib/mysql
    3   mysql_install_db --user=mysql
    4   service mysqld start
    5   mysqladmin -uroot password 123456
    6   mysql -u root -p123456
    7   select * from user
    8   delete from user where password = ""
    9   update user set Host = '%' where 'Host'='localhost'
```

# 忘记密码
```
mysqld_safe --skip-grant-tables
```
