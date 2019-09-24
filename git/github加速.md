原理是使用shadowsocks提供的socks5代理

`~/.gitconfig`
```conf
[user]
        name = name
        email = email
[http "https://github.com"]
        proxy = socks5://127.0.0.1:1080
[https "https://github.com"]
        proxy = socks5://127.0.0.1:1080
[core]
        autocrlf = input
```