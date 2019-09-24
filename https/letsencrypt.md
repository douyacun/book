---
title: letsencrypt_acme_泛域名证书
author: douyacun
date: 2019-01-29
---

- [letsencrypt](https://letsencrypt.org/docs/client-options/)
- [acme.sh](https://github.com/Neilpang/acme.sh)
- [certbot](https://certbot.eff.org/)

# certbot

## 安装

使用最新安装
```
wget https://dl.eff.org/certbot-auto
chmod a+x ./certbot-auto
```


只需要 *.example.com 替换为你的域名
```
./certbot-auto certonly --manual -d *.example.com --agree-tos --no-bootstrap --manual-public-ip-logging-ok --preferred-challenges dns-01 --server https://acme-v02.api.letsencrypt.org/directory
```

dns解析上添加txt记录：
```
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Please deploy a DNS TXT record under the name
_acme-challenge.00h.tv with the following value:

c8tNSid7w3gA5a4FSP5m1_-1hMNjUIiqnf2V2t-N9zE

Before continuing, verify the record is deployed.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Press Enter to Continue
```

成功的标志， 证书存放位置：
```
IMPORTANT NOTES:
 - Congratulations! Your certificate and chain have been saved at:
   /etc/letsencrypt/live/00h.tv/fullchain.pem
   Your key file has been saved at:
   /etc/letsencrypt/live/00h.tv/privkey.pem
   Your cert will expire on 2019-05-01. To obtain a new or tweaked
   version of this certificate in the future, simply run certbot-auto
   again. To non-interactively renew *all* of your certificates, run
   "certbot-auto renew"
 - If you like Certbot, please consider supporting our work by:

   Donating to ISRG / Let's Encrypt:   https://letsencrypt.org/donate
   Donating to EFF:                    https://eff.org/donate-le
```

创建一个 2048 位的 Diffie-Hellman 文件
(nginx 默认使用 1024 位的 [Diffie–Hellman](https://zh.wikipedia.org/wiki/%E8%BF%AA%E8%8F%B2-%E8%B5%AB%E7%88%BE%E6%9B%BC%E5%AF%86%E9%91%B0%E4%BA%A4%E6%8F%9B) 进行密钥交换, 安全性太低)
```
openssl dhparam -out /etc/letsencrypt/live/dhparams.pem 2048
```

nginx 配置
```
server {
    listen 443 ssl;
    server_name example.com www.example.com;
    
    # 配置站点证书文件地址
    ssl_certificate      /etc/letsencrypt/live/example.com/fullchain.pem;
    # 配置证书私钥
    ssl_certificate_key  /etc/letsencrypt/live/example.com/privkey.pem;
    
    # 配置 Diffie-Hellman 交换算法文件地址
    ssl_dhparam          /etc/letsencrypt/live/dhparams.pem;
    
    # 配置服务器可使用的加密算法
    ssl_ciphers 'ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA256:DHE-RSA-AES256-SHA:ECDHE-ECDSA-DES-CBC3-SHA:ECDHE-RSA-DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:DES-CBC3-SHA:!DSS';

    # 指定服务器密码算法在优先于客户端密码算法时，使用 SSLv3 和 TLS 协议
    ssl_prefer_server_ciphers  on;
    
    # ssl 版本 可用 SSLv2,SSLv3,TLSv1,TLSv1.1,TLSv1.2 
    # ie6 只支持 SSLv2,SSLv3 但是存在安全问题, 故不支持
    ssl_protocols        TLSv1 TLSv1.1 TLSv1.2;
    
    # 配置 TLS 握手后生成的 session 缓存空间大小 1m 大约能存储 4000 个 session
    ssl_session_cache          shared:SSL:50m;
    # session 超时时间
    ssl_session_timeout        1d;
    
    # 负载均衡时使用 此处暂时关闭 详情见 https://imququ.com/post/optimize-tls-handshake.html 
    # 1.5.9 及以上支持
    ssl_session_tickets off;
    
    # 浏览器可能会在建立 TLS 连接时在线验证证书有效性，从而阻塞 TLS 握手，拖慢整体速度。OCSP stapling 是一种优化措施，服务端通过它可以在证书链中封装证书颁发机构的 OCSP（Online Certificate Status Protocol）响应，从而让浏览器跳过在线查询。服务端获取 OCSP 一方面更快（因为服务端一般有更好的网络环境），另一方面可以更好地缓存 以上内容来自 https://imququ.com/post/my-nginx-conf-for-wpo.html
    # 1.3.7 及以上支持
    ssl_stapling               on;
    ssl_stapling_verify        on;
    # 根证书 + 中间证书
    ssl_trusted_certificate    /etc/letsencrypt/live/example.com/fullchain.pem;
    
    # HSTS 可以告诉浏览器，在指定的 max-age 内，始终通过 HTTPS 访问该域名。即使用户自己输入 HTTP 的地址，或者点击了 HTTP 链接，浏览器也会在本地替换为 HTTPS 再发送请求 相关配置见 https://imququ.com/post/sth-about-switch-to-https.html
    add_header Strict-Transport-Security max-age=60;
    
    # 在此填写原本 http 协议中的配置
}
```



# acme.sh，一键安装

## 安装 

```
curl https://get.acme.sh | sh

or

wget -O -  https://get.acme.sh | sh


alias acme.sh=~/.acme.sh/acme.sh

```

letsencrypt有效期只有3个月， 安装完成后会自动在`crontab`添加一条自动更新证书策略
```
42 0 * * * "/root/.acme.sh"/acme.sh --cron --home "/root/.acme.sh" > /dev/null
```

泛域名需要用户依赖dns解析，关于不同云平台的的方法，看下面
- [DNS API](https://github.com/Neilpang/acme.sh/blob/master/dnsapi/README.md)

这里贴一下阿里云的用法：需要用到阿里云的 [key&secret](https://link.juejin.im/?target=https%3A%2F%2Fak-console.aliyun.com%2F%23%2Faccesskey)
```
export Ali_Key="LTAIHhIap9ux2"
export Ali_Secret="Ycc2BdWYIX5VBqwpIRLX8sdfsdf"

acme.sh --issue --dns dns_ali -d 00h.tv -d *.00h.tv
```

生成之后证书默认在: `~/.acme.sh/*.example.com/`


##  配置nginx

- `ssl_dhparam` 通过下面命令生成
```
sudo mkdir /etc/nginx/ssl
sudo openssl dhparam -out /etc/nginx/ssl/dhparam.pem 2048
```
- `ssl_certificate` & `ssl_trusted_certificate` 需要用 `fullchain.cer`

- 这里方便使用把证书软连接到nginx目录下
```
ln -s ~/.acme.sh/*.example.com/ /etc/nginx/ssl/*.example.com/
```

以下是nginx配置
```
server {
    listen 80;
    server_name example.com www.example.com;

    return 301 https://www.example.com$request_uri;
}

server {
    listen 443 ssl;
    server_name www.example.com;

    ssl_certificate /etc/nginx/ssl/*.example.com/fullchain.cer;
    ssl_certificate_key /etc/nginx/ssl/*.example.com/*.example.com.key;

    # disable SSLv2
    ssl_protocols TLSv1 TLSv1.1 TLSv1.2;

    # ciphers' order matters
    ssl_ciphers "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-SHA256:ECDHE-RSA-AES256-SHA384:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA:AES128-SHA:AES256-SHA:DES-CBC3-SHA:!aNULL";

    # the Elliptic curve key used for the ECDHE cipher.
    ssl_ecdh_curve secp384r1;

    # use command line
    # openssl dhparam -out dhparam.pem 2048
    # to generate Diffie Hellman Ephemeral Parameters
    ssl_dhparam /etc/nginx/ssl/dhparam.pem;

    # let the server choose the cipher
    ssl_prefer_server_ciphers on;

    # turn on the OCSP Stapling and verify
    ssl_stapling on;
    ssl_stapling_verify on;
    ssl_trusted_certificate /etc/nginx/ssl/*.example.com/fullchain.cer;

    # http compression method is not secure in https
    # opens you up to vulnerabilities like BREACH, CRIME
    gzip off;

    location / {
        root /www/example.com/;
        index index.html;
    }

    error_log  /var/log/nginx/error.log;
    access_log /var/log/nginx/access.log;
}
```
