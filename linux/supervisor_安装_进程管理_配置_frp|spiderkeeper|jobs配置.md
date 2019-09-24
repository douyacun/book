---
title: supervisor_安装_进程管理_配置_frp|spiderkeeper|jobs配置
keywords: supervisor_frp配置_spiderkeeper配置_jobs配置
descripts: supervisor管理各种进程的配置和走过的坑
---


# supervisor安装

## centos安装
```
yum install supervisor
```
## ubuntu安装
```
yum install supervisor
```

## python安装
```
pip install supervisor
```

1. 创建文件夹和配置文件
```
mkdir /etc/supervisor
echo_supervisord_conf > /etc/supervisor/supervisord.conf
```

2. 编辑配置文件`vim /etc/supervisor/supervisord.conf`
```conf
;[include]
;files = relative/directory/*.ini

[include]
files = /etc/supervisor.d/*.ini
```

3. 创建目录 `/etc/supervisor.d/` 并编辑文件 `scrapyd.conf`

4. 进入目录 /usr/lib/systemd/system/，增加文件 supervisord.service，来使得机器启动的时候启动supervisor，文件内容
```conf
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
## 常用选项
- 启动: `supervisorctl start frp`
- 重启: `supervisorctl resart frp`
- 状体: `supervisorctl status`
- 重新加载配置文件: `supervisorctl reload`

# frp配置
内网

`/etc/supervisord.d/frpc.ini`
```ini
[program:frp]
# 这里推荐使用绝对路径
command=/root/software/frp/frpc -c /root/software/frp/frpc.ini
autostart=true
autorestart=true
user=root
numprocs=1
redirect_stderr=true
stdout_logfile=/var/log/frp/access.log
```

外网

`/etc/supervisord.d/frps.ini`
```ini
[program:frp]
command=/data/frp/frps -c /data/www/frp/frps.ini
autostart=true
autorestart=true
user=root
numprocs=1
redirect_stderr=true
stdout_logfile=/var/log/frp/access.log
```

# scrapyd配置

`/etc/supervisord.d/spiderkeeper.ini`
```conf
[program:spiderkeeper]
command=spiderkeeper --server=http://localhost:6800 --username=root --password=Mh2*1>0
directory=/opt/SpiderKeeper
user=root
stderr_logfile=/var/log/spiderkeeper.err.log
stdout_logfile=/var/log/spiderkeeper.out.log
```