# scrapyd
> scrapyd是一个用于部署和运行scrapy爬虫的程序，它允许你通过JSON API来部署爬虫项目和控制爬虫运行


### 工作原理

scrapyd是一个守护进程，监听爬虫的运行和请求，然后启动进程来执行它们


### 安装

```
# pip install scrapyd
报错 少twisted

安装twisted
下载
https://twistedmatrix.com/trac/wiki/Downloads
https://twistedmatrix.com/Releases/Twisted/17.9/Twisted-17.9.0.tar.bz2

tar jxvf Twisted-17.9.0.tar.bz2
解压bz2
# yum install bzip

安装
# cd Twisted-17.9.0
# python setup.py install
# pip install scrapyd
```


### 配置文件 `/etc/scrapyd/scrapyd.conf`

```
[scrapyd]
eggs_dir    = /var/log/scrapyd/eggs
logs_dir    = /var/log/scrapyd/logs
items_dir   =
jobs_to_keep = 5
dbs_dir     = dbs
max_proc    = 0
max_proc_per_cpu = 4
finished_to_keep = 100
poll_interval = 5.0
bind_address = 35.201.219.121
http_port   = 6800
debug       = off
runner      = scrapyd.runner
application = scrapyd.app.application
launcher    = scrapyd.launcher.Launcher
webroot     = scrapyd.website.Root

[services]
schedule.json     = scrapyd.webservice.Schedule
cancel.json       = scrapyd.webservice.Cancel
addversion.json   = scrapyd.webservice.AddVersion
listprojects.json = scrapyd.webservice.ListProjects
listversions.json = scrapyd.webservice.ListVersions
listspiders.json  = scrapyd.webservice.ListSpiders
delproject.json   = scrapyd.webservice.DeleteProject
delversion.json   = scrapyd.webservice.DeleteVersion
listjobs.json     = scrapyd.webservice.ListJobs
daemonstatus.json = scrapyd.webservice.DaemonStatus
```

### 运行scrapyd

直接执行 `scrapyd` 即可运行scrapyd, 注意将scrapyd加入环境变量
```
# vim ~/.bash_profile
...
PATH=$PATH:$HOME/bin:/usr/local/python3/bin
export PATH

# source .bash_profile
```

# 部署scrapy项目 [scrapyd-client](https://scrapyd.readthedocs.io/en/stable/)

### 安装

```
# pip install scrapyd-client
```


## 使用 supervisor 和 spiderkeeper 管理 scrapyd

## 安装:
- supervisor `pip2 install supervisor`
    > supervisor 需要python2.* 环境
- scrapyd `pip3 install scrapyd`
- SpiderKeeper `pip3 install SpiderKeeper`



## 编写 `supervisord` 启动结束进程

1. 进入目录 /usr/lib/systemd/system/，增加文件 supervisord.service，来使得机器启动的时候启动supervisor，文件内容
```
# supervisord service for systemd (CentOS 7.0+)
# by ET-CS (https://github.com/ET-CS)
[Unit]
Description=Supervisor daemon

[Service]
Type=forking
ExecStart=/usr/bin/supervisord -c /etc/supervisor/supervisord.conf
ExecStop=/usr/bin/supervisorctl $OPTIONS shutdown
ExecReload=/usr/bin/supervisorctl $OPTIONS reload
KillMode=process
Restart=on-failure
RestartSec=42s

[Install]
WantedBy=multi-user.target
```

2. 激活开机启动命令
```
systemctl enable supervisord.service
```

3. 启动supervisor进程
```
systemctl start supervisord.service
```

4. 关闭supervisor进程
```
systemctl stop supervisord.service
```

5. 如果修改了supervisor.service文件，可以通过reload命令来重新加载配置文件
```
systemctl reload supervisord.service
```

## scrapyd 添加认证: 使用http认证

我们也可以在scrapyd前面加一层反向代理来实现用户认证。以nginx为例, 配置nginx

```
[root@singapore conf.d] yum install httpd

server {
        listen 6801;
        location / {
                proxy_pass            http://127.0.0.1:6800/;
                auth_basic            "Restricted";
                auth_basic_user_file  /etc/nginx/conf.d/.htpasswd;
        }
}

[root@singapore conf.d]# htpasswd -c .htpasswd root
New password:
Re-type new password:
Adding password for user root
```

*/5 * * * * source /root/bash/validateproxyip.sh
