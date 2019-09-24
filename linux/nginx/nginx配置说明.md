---
Title: nginx基本配置_调试配置_进程配置_事件类配置
Keywords: 配置参数_调试_进程_事件_配置
Description: nginx配置理解与使用
Author: douyacun
Date: 2019-03-06 12:09:19
LastEditTime: 2019-04-26
---

# 调试进程和定位问题的配置项

## 是否以守护进程方式运行nginx，方便gdb调试
```
daemon on|off
deafult  daemon on
```
> 前台任务/后台任务：两者区别时是否继承标准输入STDIN\
> 守护进程：进程退出取决于是否是否会收到SIGHUP信号，发送SIGHUP信号在于Linux的huponexit（shopt|grep huponext）,linux默认是关闭的\
> disdown：可以将指定任务从后台任务列表（jobs可以查看）移除，一个后台任务只要不在这个任务列表中，session肯定不会向它发SIGHUP信号
> nohup：阻止SIGHUP信号发送给进程，关闭标准输入，重定向标准输入/输出到 nohup.out


## 是否以master|work方式运行
```
master_process on|off
master_process on
```
如果off关闭后，就不会fork出子进程来处理请求，由marster进程自身来处理

## error日志

```
error_log path level;
defualt error_log logs/error.log error
```
日志文件会比较大; /dev/null 可以关闭日志输出;
日志级别: debug notice info warn error crit alter emerg

## 特殊的调试点

```
debug_points stop|abort
```

## 仅对指定客户端输出debug日志
```
debug_connection IP|CID
```

这个配置只能放到events块中
```
events {
    debug_connection 192.168.2.1
    debug_connection 172.10.0.0/24
}
```
仅仅对上述ip地址才会生成debug级别的日志，其他请求仍然沿用error_log的配置

## 限制coredump转储文件
```
work_rlimir_core size
```
发生错误或受到信号终止时，系统会将内存内容写入一个文件(core)，内存越界被操作系统终止时，会生成core核心存储文件，可以从文件获取当时堆栈、寄存器等信息，core打印的信息很大，容易占满磁盘，需要限制大小

## 指定coredump文件的位置
```
working_directory path
```
设置coredump存储的目录，保证work进程对目录有写入权限


# 正常运行的配置项

## 定义环境变量
```
env var|var=value
```
让用户直接设置操作系统上的变量

# 嵌入其他配置文件
```
include path/file

include mime.type
include conf.d/*.conf
```
将其他配置文件嵌入到当前的nginx.conf中，相对路径是基于nginx.conf所在目录，可以是通配符，可以同时嵌入多个文件

## pid文件路径
```
pid path/file
pid log/nginx.pid
```

可以用`--pid-path`指定，确保nginx有权操作文件

## work进程的用户和用户组
```
user username [groupname]
user nobody nobody
```
设置master进程fork的子进程运行在那个用户和用户组下，当按照`username`设置时用户名和用户组名相同，可以用`--user=www --group=www`指定

## 指定work进程可以打开的最大句柄描述符
```
work_rlimit_nofile limit
```

## 限制信号队列
```
work_rlimit_sigpending limit
```
每个用户发给nginx的信号队列大小，超过就会被丢弃

## work进程数
```
work_process number
work_process 1
```
marter/work运行方式下，定义work进程的个数

每个work进程都是单线程进程，调用各个模块实现功能，确认模块不会出现阻塞式调用，又多少cpu内核就应该设置多少work进程

多work进程可以充分利用多核系统架构，work进程的数量多于cpu内核会增大进程间切换带来的消耗(linux抢占式内核)。

## 绑定work进程到指定内核
```
work_cpu_affinity cpumask
```

如果多个work进程都在抢占同一个cpu，就会出现同步问题，反之如果每一个work进程都独享一个cpu，就在内核的调度策略上实现了完全的开发
```
work_process 4;
work_cpu_affinity 1000 0100 0010 0001;
```

## ssl 硬件加速
```
ssl_engine device
```

## 系统调用gettimeofday的频率
```
timer_resolution t;
```
默认情况下，每次的内核的事件调用（如 epoll/select/poll/kqueue）, 都会执行gettimeofday,实现用内核时钟来更新nginx的缓存时钟，早期调用代价较大， 现在代价不大

## work进程的优先级设置
```
work_priority nice
```
在linux系统中，许多进程都处于可执行状态，将按照进程的优先级来决定本次内核选择哪一个执行，进程所分配的时间片大小也与优先级有关(最小5ms，最大800ms)

优先级由静态优先级和内核根据进程执行情况所做的动态调整(+-5)，nice是静态优先级，取值范围是-20/+19, +19是最低优先级，可以把优先级设置的小一些，不建议超过比内核进程的nice(-5)值还要小

# 事件类配置项

## 是否打开accept锁
```
accept_mutex on|off
```
accept_mutex可以让多个work进程轮流的、序列化的与新的客户端建立tcp连接，当某一个work进程的连接数量达到`work_connections`配置的最大连接数量的7/8时，会大大减少该work进程试图建立tcp连接的机会，以此实现让work进程处理的客户端请求接近

默认是打开的，

## lock 文件的路径
```
lock_file path/file;
lock_file logs/nginx.lock
```
accept锁可能需要这个lock文件，如果accept锁关闭，lock_file锁完全不生效，如果打开了accept锁，由于编译程序、操作系统架构等因素导致nginx不支持原子锁，这是才会用文件锁实现accept锁

## 使用accept到真正建立连接之间的延迟时间
```
accept_mutex_delay Nms;
accept_mutex_delay 500ms;
```
在使用accpet锁后，同一时间只有一个work进程能够取到accept锁，这个accept锁不是阻塞锁，如果另一个work进程没有取到，至少要等aceept_mutex_delay定义的间隔后才能再次试图取锁


## 批量建立新链接

```
multi_accept [on|off]
multi_accept off
```
当事件模型有新的通知时，尽可能的对本次调度所有客户端发起的tcp请求建立连接

## 选择事件模型
```
use [kqueue|rtsig|epoll|dev|poll|select]
```
linux就选择epoll

## 每个work的最大连接数
```
work_connections number
```
定义每个work可以同时处理的最大连接数


# http核心模块配置

## 虚拟主机请求转发

### 监听端口
```
listen address: port [default server] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=size] [defered] [bind] [ipv6=on|off] [ssl]; 
listen 80;
```
listen 决定nginx服务如何监听端口，listen参数的意义：
- default server: 将所在的server快作为整个web服务的默认server块，如果没有这个设置将会以nginx的conf找到的第一个server作为默认的server块
- backlog=num: 表示TCP中backlog队列的大小， 默认为-1，表示不予处理
> tcp backlog: 表示已连接但是未进行accept处理的socket队列大小，如果这个队列满了，将会发送一个ECONNREFUSED错误信息到客户端
- rcvbuf=size: 设置监听句柄SO_RCFBUF参数
- sndbuf=size: 设置监听句柄SO_SNDBUF参数
- accept_filter：设置accept过滤器，只对FreeBSD操作系统有用
- deferred: 设置该参数后， 如用户发起建立连接请求,并且完成了tcp的三次握手，内核也不会为了这次的连接调度work进程来处理，只有用户真的发送请求数据时（内核已经在网卡中收到请求数据包），内核才会唤醒work进程来处理这个连接
- bind： 绑定当前端口/地址对时，同时对一个端口监听多个地址时才会生效
- ssl： 当前监听的端口上建立的连接必须是基于SSL协议
- server name：可以绑定多个主机名称
  - 首先选择全等匹配
  - 其次选择统配符在前面的： *.testweb.com
  - 再次选择通配符在后面的： www.testweh.*
  - 最后选择正则表达式才匹配的server_name, ~^\.testweb\.com$
  - 与所有的server name 都不匹配时：
    - 优先选择在listen配置项后面加入[default server]的server块
    - 找到匹配listen端口的第一个server块

### server_name_hash_max_size
```
server_names_hash_max_size size;
server_name_hash_max_size 512;
```
server_names_hash_max_size 会影响散列表的冲突率，越大占用内存越多，但散列key的冲突率越低，检索速度越快

### 重定向主机名称的处理
```
server_name_in_redirect on|off;
server_name_in_redirect on;
```
该配置需要配合server_name使用，在使用on打开时表示在重定向请求时会使用server_name里配置的第一个主机名代替原先请求的host请求，off关闭，在重定向请求时使用请求本身的host头部

### location 用法
```
location [=|~|~*|^~|@] /uri?{ ... }
```
1. = 表示把uri作为字符串，以便于参数重的uri做完全匹配
2. ~ 表示匹配uri时字母时大小写敏感的
3. ~* 表示匹配uri字母大小写不敏感
4. ^～ 表示只需要前半部分与uri参数匹配即可
5. @ 仅用于ngnix内部重定向

注意，location 是有顺序的，当一个请求有可能匹配多个location时，实际上这个请求会被第一个location处理

location只能表达 如果匹配...则...., 如果表达 如果匹配...则...,可以在最后location使用 / 作为参数
```
location / {
    可以匹配所有
}
```

https://www.00h.tv/images/
```
location images {
    # 这样时匹配不进来的
}
location /images {
    # 这样时可以匹配进来的
}

需要从 / 开始匹配
```

## 文件路径的定义

### 以root方式设置资源路径
```
root path;
root html;
http server location if
```

### 以alias方式设置资源路径
```
alias path;
location;
```
alias 与 root 以不同的方式将用户请求映射到真正的磁盘文件上，例如

URI：/conf/nginx.conf, 实际文件/usr/local/nginx/conf/nginx.conf

alias：
```
location /conf {
    alias /usr/local/nginx/conf/;
    # 必须以 / 结尾
}
```
root:
```nginx
location /conf {
    root /usr/loca/nginx;
    # 可以以 / 结尾，也可以不用 /
}
```
使用alias时，在uri向时间文件路径的映射过程中，已经把location后配置的/conf这部分字符串丢弃掉， root则不会，它会根据完整的uri请求来映射

alias后面可以添加正则表达式：
```nginx
location ~ ^/test/(\w+).(\w+)$ {
    alias /usr/location/nginx/$2/$1.$2;
}
```
### 访问首页
```nginx
index file...;
index index.html index.php;
```

### http返回码重定向页面
```nginx
error_page code [code...][=|=answer-code]uri|@named_location
http server location if
```
当对某个请求返回错误码时，如果匹配上了error_page中设置的code，则重定向到新的URI中
```nginx
error_page 404  404.html;
error_page 502 503 504 50x.html;
error_page 403  https://www.00h.html;

error_page 404  = @fetch;
location @fetch {
    
}
```
虽然重定向了uri，但返回的http错误码还是与原来的相同，可以通过=来更改返回的错误码
```
error_page 404 =200 empty.gif;

location /favicon.ico {
    error_page 404 =200 /favicon.ico;
}

error_page 404 =403 fobidden.gif;
```

也可以不指定确切的返回错误码， 由重定向后世纪出的真实结果来决定，只要把‘=’后面的错误码去掉就行
```
error_page 404 = /empty.gif;
```

如果不想修改uri， 只想让这样的请求重定向到另一个location中进行处理，可以这样设置：
```nginx
location / {
    error_page 404 @fallback;
}
location @fallback {
    proxy_pass http://backend;
}
```

### 是否允许递归使用error_page
```nginx
recursive_error_page [on|off];
recursize_error_page off;
http server location;
```

### try_files
```nginx
try_files path1[path2]uri;
server location;
```
try_files后面跟若干路径如：path1、path2....，而且最有必须要有uri参数，意义如下: 尝试按照顺序访问每一个path，如果可以有效的读取，就直接向用户返回这个path对应的文件结束请求，就继续向下访问，所有重定向到最后的参数uri上，因此最后这个参数uri必须存在，它应该时可以有效重定向的
```nginx
try_files systeminstan.html $uri $uri/index.html $uri.html @other;
location @other {
    proxy_pass http://www.00h.tv;
}
```

## 内存及磁盘资源的分配

### http包体置存储到磁盘文件中
```nginx
client_body_in_file_only on|clean|off;
client_body_in_file_only off;
http server location
```
值不是off时，用户请求中的http包体一律存储到磁盘文件中，即使只有0字节也会存储为文件，如果配置为on，这个文件不会被删除（一般用于调试和定位问题），如果配置为clean，会删除该文件

### http包体尽量写入到一个内存buffer中
```nginx
client_body_in_single_buffer on|off;
client_body_in_single_buffer off;
http server location
```
用户请求的包体一律存储到内存buffer中，当然，如果http包体的大小超过了client_body_buffer_size设置的值，包体还是会写入到磁盘文件中


### 存储http头部的内存buffer大小
```nginx
client_header_buffer_size size;
client_header_buffer_size 1k;
http、server
```
定义了正常情况下nginx接受用户请求中HTTP header部分分配的内存buffer大小， 超过这个大小，large_client_header_buffer定义的buffer会生效

### 存储超大HTTP头部的内存buffer大小
```nginx
large_client_header_buffers number size;
large_client_header_buffers 48k;
http、server
```
large_client_header_buffers 定义nginx接受一个超大HTTP头部请求的buffer个数和每个buffer的大小，如果HTTP请求行的大小超过上面的单个buffer，返回“Request URI too large”(414),请求中一般会有取多header，每个header的大小也不能超过打个buffer的大小，否则返回“Bad Request”(400)

### 存储http包体的内存buferr大小
```nginx
client_body_buffer_size size;
client_body_buferr_size 8k/16k;
http、server、location
```
定义ngnx接收包体的内存缓冲区大小，也就是说，http包体会先接收到指定的这块缓存中，之后才决定是否写入磁盘，如果content-length小于定义的buffer大小，会自动降低本次请求使用的内存buffer，降低内存消耗

### http包体的临时存放目录
```nginx
client_body_temp_path dir-path[level1|[level2[leve3]]]
client_body_temp_path client_body_temp;
http、server、location
```
接受http包体时，如果包体的大小大于client_body_buffer_size, 则会一个递增的证书明明并存放到client_body_temp_path指定的目录中，level参数为了防止一个目录下的文件数量太多，从而导致性能下降，按照临时文件名最多再加三层目录

```nginx
client_body_temp_path /opt/nginx/client_temp/ 1 2;
```

如果文件名是0000123456作为临时文件名，就会存放在这个目录中
```nginx
/opt/nginx/client_temp/6/45/0000123456
```

### connection_pool_size
```nginx
connection_pool_size size;
connection_pool_size 256;
```
nginx对于每个建立连接成功的tcp连接会预先分配一个内存池（ngx_connection_t结构体中pool内存池中初始大小），减少内核对于小内存的分配次数，更大会使消耗的内存增多，更小引发更多的内存次数

### request_pool_size
```nginx
request_pool_size size;
request_pool_size 4k;
http、server
```
nginx开始处理HTTP请求时，将会为每个请求都分配一个内存池，size配置项指定这个内存池的初始大小，因为一个tcp连接可能被复用于多个http请求

## 网络连接设置

### 读取http头部的超时时间
```nginx
client_header_timeout time;
client_header_timeout 60;
http、server、location
```
客户端和服务端建立连接后开始接受http头部，在次期间如果没有读取到客户端发来的字节，则认为超时，并向客户端返回408(request timeout)响应

### 读取http包体的超时时间
```nginx
client_body_timeout time;
client_body_timeout 60;
http、server、location
```

### 发送响应的超时时间
```nginx
send_timeout time;
send_timeout 60;
http、server、location
```
nginx向客户端发送了数据包客户端发送了数据包，但客户端一直没有去接收这个数据包

### reset_timeout_connection
```nginx
reset_timeout_connection on|off;
reset_timeout_connection off;
http、server、location
```
连接超时后将通过向客户端发送RST包来直接重置连接，nginx会在某个连接超时后，不使用正常情况下的四次握手关闭TCP连接，直接向客户端发送rest重置包，直接释放关于这个套接字使用的所有缓存，相比正常模式下，使得服务器产生许多处于FIN_WAIT_1、FIN_WAIT_2、TIME_WAIT; RST重置包关闭连接会带来一些问题，默认情况下不会开启

### lingering_close
```nginx
lingering_close on|always|off;
lingering_close on;
```
nginx 关闭用户连接方式：
- always: 关闭用户连接前必须无条件的处理连接上所有用户发送的数据
- on：一般情况下，都会处理连接上用户发送数据，除了业务上认定这之后的数据是不必要的
- off：关闭连接时，完全不管是否已经有准备就绪的来自用户的数据

### lingering_time
```nginx
lingering_time time;
lingering_time 30s;
```
lingering_close开启后，对于上传大文件超过max_client_body_size配置时，会返回413（Request entity too large）,但是很多客户端可能不管413返回值，仍然持续不断上传，经过linering_time后，nginx都会把连接关掉

### lingering_timeout
```nginx
lingering_close 生效后，关闭连接前，会检测是否有客户端发送的数据到达服务器，如果超过lingering_timeout时间后还没有数据可读直接关闭连接
```

### 对某些浏览器禁用keepalive
```nginx
keepalive_disable [msie6|safari|none];
keepalive_disable msie6 safari;
http、server、location
```

### keepalive 超时时间
```nginx
keepalive_timeout time(s);
keepalive_timeout 75;
http、server、location
```
一个keepalive在闲置超过一段时间后，服务器和客户端都会去关闭这条连接，这是预约nginx的；

### keepalive允许承载的最大连接数
```
keepalive_requests n;
keepalive_requests 100;
http、server、location
```
一个keepalive连接上默认最多只能发送100个请求

### tcp_nodelay
对keepalive使用TCP_NODELAY

### tcp_nopush
打开tcp_nopush后,将会在发送响应时把整个响应包放到一个TCP包中发送

## MIME类型的设置
MIME type于文件扩展的映射
```nginx
type{
    text/html html;
    text/html conf;
    image/gif gif;
    image/png png;
}
```
默认的MIME-type
```nginx
default_type MIME-type;
default_type test/plain;
```

## 对客户端请求的限制

### http请求方法限制
```nginx
limit_except method {};
location;
```
limit_except 后面指定方法名来限制用户请求，GET、POST、HEAD、PUT、DELETE...
```nginx
limit_except GET{
    allow 192.168.1.2;
    deny all;
}
```
允许GET方法也允许HEAD方法，但其他http方法是允许的

### http请求包体的最大值
```
client_max_body_size size;
client_max_body_size 1m;
```

### 对请求的限速
```nginx
limit_rate speed;
limit_rate 0;
http server location if;
```
对客户端请求限制美妙传输的字节数，默认0，表示不限速
```nginx
server{
    if ($slow) {
        set $limit_rate 4k;
    }
}
```
### limit_rate_after
```nginx
limit_rate_after size;
limit_rate_after 1m;
```
nginx向客户端发送的响应长度超过limit_rate_after后才开始限速
```nginx
limit_rate_after 1m;
limit_rate 100k
```

## 文件操作的优化

### sendfile
```nginx
sendfile on|off;
sendfile off;
```
可以启用linux上的sendfile系统调用来发送文件，减少内核态到用户态之间的两次内存复制，这样就会从磁盘读取文件后直接在内核态发送到网卡设备

### AIO系统调用
```nginx
aio on|off
aio off;
```
表示是否在linux系统上启用内核级别的异步文件i/o功能，它与sendfile功能是互斥的

### directio
```
directio on|off;
directio off;
http、server、location
```
在linux系统上使用O_DIRECT选项去读取文件，缓冲区大小为size，通常对大文件的读取速度有优化作用

### directio_alignment
```
于directio配合使用，指定以directio方式读取文件时的对其方式，一般情况，512B足够了，针对一些高性能文件系统，linux的XFS系统需要设置4kb
```

### 打开文件缓存
```
open_file_cache max=N [inactive=time] | off;
open_file_cache off;
```
文件会在内存中存储一下3种信息：
- 文件句柄、文件大小、上次修改时间
- 已经打开过的目录结构
- 没有找到或者没有权限操作的文件信息

参数：
- max：在内存种存储元素的最大个数，达到最大个数后采用LRU算法淘汰最近使用的元素
- inactive：指定时间段内没有被访问过的元素将会被淘汰，默认时间是60秒
- off：关闭缓存功能
```nginx
open_file_cache max=1000 inactive=20s;
```

### 是否缓存文件打开错误的信息
```
open_file_cache_errors on|off;
open_file_cache_errors off;
```
是否在文件缓存中缓存打开文件时找不到的路径、没有权限等错误信息

### 不被淘汰的最小访问次数
```
open_file_cache_min_uses number;
open_file_cache_min_uses 1;
```
在inactive指定的时间段内，访问次数超过了open_file_cache_min_uses指定的次数，就不会被淘汰

### 检查缓存中元素有效性的频率
```
open_file_cache_valid time;
open_file_cache_valid 60;
```
默认每60秒检查一次元素是否依旧有效







## 对客户端请求的特殊处理

### 忽略不合法的http请求头部
```
ignore_invalid_headers on|off;
ignore_invalid_headers on;
http、server
```
- off：当出现不合法的http头部时，会直接400（Bad Request）,
- on：会忽略此http头部

### http头部是否允许下划线_
```
underscores_in_headers on|off;
underscores off;
http、server
```
表示HTTP头部的名称中不允许待下划线_

### if-modified-since头部处理策略
```
if-modified-since off|exact|before;
if-modified-since exact;
http、server、location
```
web客户端会在本地缓存一些文件，并存储当时获取的时间，下次向web服务器获取缓存过的资源时，可以用if-modified-since头部把上次获取的时间捎带上，
- off： 忽略if-modified-since,如果获取一个文件，那么正常返回文件内容，HTTP响应码通常是200
- exact：将if-modified-since头部包含的时间将要返回的文件上次修改的时间做精确比较，如果没有匹配上，则返回200和文件的实际内容，如果匹配上返回304（not modified）,浏览器收到后会直接读取自己的本地缓存
- befor：是比exact更宽松的笔记，只要文件的修改时间等于或早于if-modified-since头部的时间，就会向客户端304

### 文件未找到记录到文件中
```
log_not_found on|off;
log_not_found on;
http、server、location
```
处理用户请求且需要访问文件时，如果没有找到文件，是否将错误日志记录到error.log文件，仅用于定位问题

### merge_slashes
表示是否合并相邻的"",例如`/test///a.txt`,在配置为on时，会将其匹配为`/location/test/a.txt`, 如果配置为off，则不会匹配uri仍然是`/test///a.txt`

### DNS解析地址
```
resolver address;
http、server、location
```
设置dns名字解析服务器的地址,例如：
```
resolver 127.0.0.1 192.0.2.1
```

### DNS 解析的超时时间
```
resolver_timeout time;
resolver_timeout 30s;
http、server、location
```

### 返回错误时是否注明Nginx版本
```
server_tokens on|off;
server_tokens on;
http、server、location
```
请求出错时是否在响应的server头部中著名nginx版本，这是为了方便定位问题

### [ngx_http_core_module](http://nginx.org/en/docs/http/ngx_http_core_module.html#variables)
