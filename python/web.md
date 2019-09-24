# nginx + uwsgi + python 搭建web服务

1. 编译安装python3.6, [官网下载](https://www.python.org/downloads/source/)
    ```
    # xz -d  Python-3.6.4.tar.xz
    # tar xvf Python-3.6.4.tar

    # ./configure --prefix=/usr/local/python3 --enable-optimizations
    # make && make install

    加入环境变量
    # ln -s /usr/local/python3/bin/python3.6 /usr/bin/python
    # ln -s /usr/local/python3/bin/pip3.6 /usr/bin/pip
    ```

2. 安装并配置uwsgi
    ```
    # pip install uwsgi

    # test.py
    def application(env, start_response):
        start_response('200 OK', [('Content-Type','text/html')])
        return [b"Hello World"] # python3

    直接搭建http服务:(无需经过nginx)
    # uwsgi --http :8000 --wsgi-file test.py


    ```
    > 调用 uwsgi myconf.ini 等价于 uwsgi --ini myconf.ini 。

3. 配置nginx
    ```
    server {
        listen 8000; #暴露给外部访问的端口
        server_name localhost;
            charset utf-8;
        location / {
            include uwsgi_params;
            uwsgi_pass 127.0.0.1:8778;
        }
        location /static/ {
            alias /home/www/myproject/myapp/static/; #项目静态路径设置
        }
    }
    ```