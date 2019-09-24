# 安装配置phpRedisAdmin:

1. 安装
```
# git clone https://github.com/ErikDubbelboer/phpRedisAdmin.git

# cd phpRedisAdmin

# git clone https://github.com/nrk/predis.git vendor

 #mv /phpRedisAdmin /var/www/html/

```

2.配置phpredisadmin

```
 #cd  /var/www/html/phpRedisAdmin/includes

 #vim config.sample.inc.php

'servers' => array(

array(

 'name' => 'local server',       #服务器名称，随便写

 'host' => '127.0.0.1',          #服务器ip（要与/home/redis/redis.conf的ip相同）

 'port' => 6379,                 #redis在服务器开启的端口，默认为6379

 'filter' => '*'

 //'auth' => 'redispasswordhere' //   #如果你的redis里面设置了连接密码，这里写那个密码，建议不要设置，因为一些cacti监控redis状                                               态，如果设置密码就无法读取数据

),


#接下来是关于phpredisadmin登陆设置密码用户的选项

// Uncomment to enable HTTP authentication  #取消注释即可启用用户登陆

 'login' => array(                        #总的设置



 'admin' => array(                        #组合设置，这个是admin用户

 'password' => '123456',                  #admin用户的密码

),



'guest1' => array(                       #这里是我创建的guest1用户，

'password' => '123456',                  #用户密码

'servers'  => array(1)                   #设置guest1用户能查询到的服务器编号，这即                                                  guest1用户只能查看1编号的服务器

),                                        #注意，因为这里已经创建一个用户，所以这个用                                                户的）后面一定要加一个，
```