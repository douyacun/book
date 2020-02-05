---
Title: centos7新手安装lnmp环境
Keywords: lnmp,yum安装
Description: 
Cover: 
Label: 
Date: 2019-02-25 23:02:12
LastEditTime: 2019-11-29 11:46:43
---
- epel
    ```
    32位系统选择：
    rpm -ivh http://dl.fedoraproject.org/pub/epel/6/i386/epel-release-6-8.noarch.rpm
    rpm -ivh http://dl.fedoraproject.org/pub/epel/5/i386/epel-release-5-4.noarch.rpm
    64位系统选择：

    centos7:
    rpm -ivh http://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

    centos6:
    rpm -ivh http://dl.fedoraproject.org/pub/epel/6/x86_64/epel-release-6-8.noarch.rpm

    导入key：
    rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-EPEL-7
    rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-EPEL-6
    ```

- 使用阿里云镜像:
    ```
    mv /etc/yum.repos.d/CentOS-Base.repo /etc/yum.repos.d/CentOS-Base.repo.backup
    wget -O /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-7.repo

    yum clean all && yum makecache
    ```

- 设置remi
    ```
    On CentOS/RHEL/Scientific Linux 7 x86_64:

    wget http://rpms.famillecollet.com/enterprise/remi-release-7.rpm
   rpm -Uvh remi-release-7.rpm

    On CentOS/RHEL/Scientific Linux 6 i386 or x86_64:

    wget http://rpms.famillecollet.com/enterprise/remi-release-6.rpm
    rpm -Uvh remi-release-6.rpm

    wget http://download.fedoraproject.org/pub/epel/6/x86_64/epel-release-6-8.noarch.rpm
    rpm -ivh epel-release-6-8.noarch.rpm

    报错的话
    后面加上 --force --nodeps
   ```

- 安装nginx mysql php-fpm
```
# cd /etc/yum.repo.d/

# vim nginx.repo

[nginx]
name=nginx repo
baseurl=http://nginx.org/packages/centos/$releasever/$basearch/
gpgcheck=0
enabled=1

# yum list|grep nginx

不要用y,确定nginx版本后再安装,默认是安装最新稳定版本
# yum install nginx
```

- 安装php-fpm
```
yum --enablerepo=remi-php56 -y install php-fpm php-pdo php-mysql php-redis php-gd php-yar php-xml php-pdo php-pear php-devel php-mbstring;
yum --enablerepo=remi-php71 -y install php-fpm php-pdo php-mysql php-redis php-gd php-xml php-zip php-pdo  php-pear php-devel php-mbstring php-tokenizer php-bcmath php-mongodb php-xdebug 
```

- 安装myql  mysql-server  从repolist   [看官网](https://dev.mysql.com/doc/refman/5.7/en/linux-installation-rpm.html)
```

安装mysql 5.7
# yum list installed | grep mysql

删除系统自带的mysql及其依赖 - 非必须
# https://dev.mysql.com/get/mysql57-community-release-el6-11.noarch.rpm

下载源
# wget https://dev.mysql.com/get/mysql57-community-release-el7-11.noarch.rpm
# wget https://dev.mysql.com/get/mysql57-community-release-el6-11.noarch.rpm


安装源
# yum localinstall -y mysql57-community-release-el7-11.noarch.rpm
# yum localinstall -y mysql57-community-release-el6-11.noarch.rpm
或
# rpm -Uvh mysql57-community-release-el6-11.noarch.rpm

安装mysql5.7
# yum install mysql-community-{server,client,common,libs}-*


启动mysql
# systemctl start mysqld
# service mysqld start

初始化数据库位置:
# mysqld --initialize-insecure --user=mysql --basedir=/usr/local/mysql --datadir=/data/mysql

mysql5.7 关闭密码强度校验：
[mysqld]
validate_password=off

mysql8.0 关闭密码强度校验：
validate_password.check_user_name = OFF
validate_password.length = 4
validate_password.mixed_case_count = 0
validate_password.number_count = 0
validate_password.policy = 0
validate_password.special_char_count = 0

查看临时密码登录:
# cat /var/log/mysqld.log |grep "temporary password"

# mysql -uroot -p$passwd

# mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'Oi34)/2j';
# mysql> use mysql;
# mysql> delete from user where user <> 'root';
# mysql> update user set host='%';
# mysql> flush privileges;
```
- 注意:
> MySQL's validate_password plugin is installed by default. This will require that passwords contain at least one upper case letter, one lower case letter, one digit, and one special character, and that the total password length is at least 8 characters. \
  MySQL的默认validate_password插件安装。这将需要密码包含至少一个大写字母,一个小写字母,一个数字,和一个特殊字符,密码长度至少8个字符。

- 配置mysqld 的初始密码

	mysqladmin -u root password "new-password";

- 安装redis
```
# yum --enablerepo=remi -y install redis redis-cli
# vim /etc/redis.conf

密码
# requirepass foobared 去掉前面的注释#,并把foobared 替换为你自己的密码：123456
远程访问,修改bind为
# bind 0.0.0.0
```

- 简单配置nginx
```
server {
    listen 6801;
    location / {
            proxy_pass            http://127.0.0.1:6800/;
            auth_basic            "Restricted";
            auth_basic_user_file  /etc/nginx/conf.d/.htpasswd;
    }
}

server {

	listen 443 ssl; # managed by Certbot

	ssl_certificate /etc/letsencrypt/live/3cbook.com/fullchain.pem; # managed by Certbot
    ssl_certificate_key /etc/letsencrypt/live/3cbook.com/privkey.pem; # managed by Certbot
    include /etc/letsencrypt/options-ssl-nginx.conf; # managed by Certbot
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; # managed by Certbot

	server_name  3cbook.com default_server;
    root /www/;

	index index.php index.htm index.html;

	include /etc/nginx/default.d/*.conf;

    error_page 404 /404.html;
            location = /40x.html {
    }

	location ~ \.php$ {
		fastcgi_pass   127.0.0.1:9000;
		fastcgi_index  index.php;
		fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
		include        fastcgi_params;
	}

    error_page 500 502 503 504 /50x.html;
            location = /50x.html {
    }
}

server {
    if ($host = 3cbook.com) {
        return 301 https://$host$request_uri;
    } # managed by Certbot


    listen       80 default_server;
    server_name  3cbook.com default_server;
    return 404; # managed by Certbot
}
```

- 403:问题一定要记得关闭selinux,反正只要是各种权限问题就是selinux的问题
```
# vim /etc/selinux/config
...
SELINUX=disabled

# setenforce 0
设置 SELinux 状态
#getenforce
获取 SELinux 状态
```

# file not found
```
	location ~ \.php$ {
		fastcgi_pass   127.0.0.1:9000;
		fastcgi_index  index.php;
		fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;
		include        fastcgi_params;
	}
```