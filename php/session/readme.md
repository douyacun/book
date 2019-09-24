/**
* session 笔记
* @author 刘宁
* @date 2017/04/06
**/

会话:
    会话支持在 PHP 中是在并发访问时由一个方法来保存某些数据.从而使你能够构建更多的定制程序 从而提高你的 web 网站的吸引力.

    一个访问者访问你的 web 网站将被分配一个唯一的 id, 就是所谓的会话 id. 这个 id 可以存储在用户端的一个 cookie 中，也可以通过 URL 进行传递.

    会话支持允许你将请求中的数据保存在超全局数组$_SESSION中. 当一个访问者访问你的网站，PHP 将自动检查(如果 session.auto_start 被设置为 1）或者在你要求下检查(明确通过 session_start() 或者隐式通过 session_register()) 当前会话 id 是否是先前发送的请求创建. 如果是这种情况， 那么先前保存的环境将被重建.

    $_SESSION (和所有已注册得变量) 将被 PHP 使用内置的序列化方法在请求完成时 进行序列化.序列化方法可以通过 session.serialize_handler 这个 PHP 配置选项中来设置一个指定的方法.注册的变量未定义将被标记为未定义.在并发访问时,这些变量不会被会话模块 定义除非用户后来定义了它们.

    默认情况下，所有与特定会话相关的数据都被存储在由 INI 选项 session.save_path 指定的目录下的一个文件中。对每个会话会建立一个文件（不论是否有数据与该会话相关）。这是由于每打开一个会话即建立一个文件，不论是否有数据写入到该文件中。注意由于和文件系统协同工作的限制，此行为有个副作用，有可能造成用户定制的会话处理器（例如用数据库）丢失了未存储数据的会话。

运行时配置:

    1    session.save_handler = files         默认为files
        定义了来存储和获取与会话关联的数据的处理器的名字
        session_set_save_handler
        设置用户自定义 会话存储函数。 如果想使用 PHP 内置的会话存储机制之外的方式， 可以使用本函数。

    2    session.svae_path       string
        定义了传递给存储处理器的参数
        如果选择了默认的 files 文件处理器，则此值是创建文件的路径。默认为 /tmp。
        session_save_path
         读取/设置当前会话的保存路径
         必须在调用 session_start() 函数之前调用 session_save_path() 函数。
         ini_set('session.save_path',realpath(dirname($_SERVER['DOCUMENT_ROOT']) . '/../session'));

    3    ini_set( string $varname , string $newvalue )
        设置指定配置选项的值。这个选项会在脚本运行时保持新的值，并在脚本结束时恢复。
        $varname    选项
        $newvalue   选项新的值

    4   session.name
        指定会话名以用做 cookie 的名字。只能由字母数字组成，默认为 PHPSESSID

    5   session.auto_start
        指定会话模块是否在请求开始时自动启动一个会话。默认为 0（不启动）。
        如果为 1,当session_start()时会提示错误

    6   session.serialize_handler
        session.serialize_handler 定义用来序列化／解序列化的处理器名字;

    7   session.gc_probability
        session.gc_probability 与 session.gc_divisor 合起来用来管理 gc（garbage collection 垃圾回收）进程启动的概率。默认为 1。

    8   session.gc_divisor
        session.gc_divisor 与 session.gc_probability 合起来定义了在每个会话初始化时启动 gc（garbage collection 垃圾回收）进程的概率。
        此概率用 gc_probability/gc_divisor 计算得来。例如 1/100 意味着在每个请求中有 1% 的概率启动 gc 进程。session.gc_divisor 默认为 100。

    9   session.gc_maxlifetime
        session.gc_maxlifetime 指定过了多少秒之后数据就会被视为“垃圾”并被清除;

    10  session.referer_check
        session.referer_check 包含有用来检查每个 HTTP Referer 的子串。
        如果客户端发送了 Referer 信息但是在其中并未找到该子串，则嵌入的会话 ID 会被标记为无效。默认为空字符串。

    11  session.entropy_file
        session.entropy_length 指定了从上面的文件中读取的字节数。默认为 0（禁用）。

    12  session.use_strict_mode

    13  session.use_cookies
        session.use_cookies 指定是否在客户端用 cookie 来存放会话 ID。默认为 1（启用）。

    14  session.use_only_cookies
        指定是否在客户端仅仅使用 cookie 来存放会话 ID。启用此设定可以防止有关通过 URL 传递会话 ID 的攻击

    15  session.cookie_lifetime
         以秒数指定了发送到浏览器的 cookie 的生命周期.值为 0 表示“直到关闭浏览器”。默认为 0

         session_get_cookie_params()
         获取会话cookie参数
         返回一个包含当前会话 cookie 信息的数组：
        "lifetime" - cookie 的生命周期，以秒为单位。
        "path" - cookie 的访问路径。
        "domain" - cookie 的域。
        "secure" - 仅在使用安全连接时发送 cookie。
        "httponly" - 只能通过 http 协议访问 cookie

        session_set_cookie_params()
        设置会话 cookie 参数
        "lifetime" - cookie 的生命周期，以秒为单位。
        "path" - cookie 的访问路径。
        "domain" - cookie 的域。
        "secure" - 仅在使用安全连接时发送 cookie。
        "httponly" - 只能通过 http 协议访问 cookie

    16  session.cookie_path
        指定了要设定会话 cookie 的路径。默认为 /

    17  session.cookie_domain
        指定了要设定会话 cookie 的域名。默认为无，表示根据 cookie 规范产生 cookie 的主机名。

    18  session.cookie_secure
        指定是否仅通过安全连接发送 cookie。默认为 off

    19  session.cookie_httponly
        "httponly" - 只能通过 http 协议访问 cookie

    20  session.cache_limiter
        指定会话页面所使用的缓冲控制方法（none/nocache/private/private_no_expire/public）。默认为 nocache。

        session_cache_limiter()
        返回当前缓存限制器的名称。
        缓存限制器定义了向客户端发送的 HTTP 响应头中的缓存控制策略。
        客户端或者代理服务器通过检测这个响应头信息来 确定对于页面内容的缓存规则
        设置缓存限制器为:

        nocache 会禁止客户端或者代理服务器缓存内容;
                    Expires: Thu, 19 Nov 1981 08:52:00 GMT
                    Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0
                    Pragma: no-cache

        public 表示允许客户端或代理服务器缓存内容;
                    Expires：（根据 session.cache_expire 的设定计算得出）
                    Cache-Control： public, max-age=（根据 session.cache_expire 的设定计算得出）
                    Last-Modified：（会话最后保存时间）

        private 表示允许客户端缓存， 但是不允许代理服务器缓存内容。
                    Expires: Thu, 19 Nov 1981 08:52:00 GMT
                    Cache-Control: private, max-age=（根据 session.cache_expire 的设定计算得出）, pre-check=（根据 session.cache_expire 的设定计算得出）
                    Last-Modified: （会话最后保存时间）

        private_no_expire:在 private 模式下， 包括 Mozilla 在内的一些浏览器可能无法正确处理 Expire 响应头， 通过使用 private_no_expire 模式可以解决这个问题：在这种模式下， 不会向客户端发送 Expire 响应头
                    Cache-Control: private, max-age=（根据 session.cache_expire 的设定计算得出）
                    pre-check=（根据 session.cache_expire 的设定计算得出）
                    Last-Modified: （会话最后保存时间）

    21  session.cache_expire
        以分钟数指定缓冲的会话页面的存活期，此设定对 nocache 缓冲控制方法无效。默认为 180 (分钟)

        session_cache_expire()
        返回当前缓存的到期时间
        /* 设置缓存过期时间为 30 分钟 */
        session_cache_expire(30);
        需要在 session_start() 函数调用之前 调用

    22  session.use_trans_sid
        定是否启用透明 SID 支持。默认为 0（禁用）。

    23  session.hash_function
         允许用户指定生成会话 ID 的散列算法。'0' 表示 MD5（128 位），'1' 表示 SHA-1（160 位）

函数:
    1  session_id()
        获取/设置当前会话 ID
        * 为了能够将会话 ID 很方便的附加到 URL 之后， 你可以使用常量 SID 获取以字符串格式表达的会话名称和 ID;
        * 如果指定了 id 参数的值， 则使用指定值作为会话 ID。 必须在调用 session_start() 函数之前调用 session_id() 函数;
        * 不同的会话管理器对于会话 ID 中可以使用的字符有不同的限制。 例如文件会话管理器仅允许会话 ID 中使用以下字符：a-z A-Z 0-9 , （逗号）和 - （减号）
        * 如果使用 cookie 方式传送会话 ID，并且指定了 id 参数， 在调用 session_start() 之后都会向客户端发送新的 cookie， 无论当前的会话 ID 和新指定的会话 ID 是否相同。

    2  session_name()
        读取/设置会话名称
        * 函数会更新会话名称， 并返回 原来的 会话名称。
        * 请求开始的时候，会话名称会被重置并且存储到 session.name 配置项。 因此，要想设置会话名称，那么对于每个请求，都需要在 调用 session_start() 或者 session_register() 函数 之前调用 session_name() 函数。

    3  session_status()
        返回当前会话状态;
        PHP_SESSION_DISABLED 会话是被禁用的。
        PHP_SESSION_NONE 会话是启用的，但不存在当前会话。
        PHP_SESSION_ACTIVE 会话是启用的，而且存在当前会话。

    4  session_cache_limiter()
        返回当前缓存限制器的名称。
        缓存限制器定义了向客户端发送的 HTTP 响应头中的缓存控制策略。
        客户端或者代理服务器通过检测这个响应头信息来 确定对于页面内容的缓存规则
        设置缓存限制器为:

        nocache 会禁止客户端或者代理服务器缓存内容;
                    Expires: Thu, 19 Nov 1981 08:52:00 GMT
                    Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0
                    Pragma: no-cache

        public 表示允许客户端或代理服务器缓存内容;
                    Expires：（根据 session.cache_expire 的设定计算得出）
                    Cache-Control： public, max-age=（根据 session.cache_expire 的设定计算得出）
                    Last-Modified：（会话最后保存时间）

        private 表示允许客户端缓存， 但是不允许代理服务器缓存内容。
                    Expires: Thu, 19 Nov 1981 08:52:00 GMT
                    Cache-Control: private, max-age=（根据 session.cache_expire 的设定计算得出）, pre-check=（根据 session.cache_expire 的设定计算得出）
                    Last-Modified: （会话最后保存时间）

        private_no_expire:在 private 模式下， 包括 Mozilla 在内的一些浏览器可能无法正确处理 Expire 响应头， 通过使用 private_no_expire 模式可以解决这个问题：在这种模式下， 不会向客户端发送 Expire 响应头
                    Cache-Control: private, max-age=（根据 session.cache_expire 的设定计算得出）
                    pre-check=（根据 session.cache_expire 的设定计算得出）
                    Last-Modified: （会话最后保存时间）

    5   session_get_cookie_params()
        获取会话cookie参数
        返回一个包含当前会话 cookie 信息的数组：
        "lifetime" - cookie 的生命周期，以秒为单位。
        "path" - cookie 的访问路径。
        "domain" - cookie 的域。
        "secure" - 仅在使用安全连接时发送 cookie。
        "httponly" - 只能通过 http 协议访问 cookie

    6  session_set_cookie_params()
        设置会话 cookie 参数
        "lifetime" - cookie 的生命周期，以秒为单位。
        "path" - cookie 的访问路径。
        "domain" - cookie 的域。
        "secure" - 仅在使用安全连接时发送 cookie。
        "httponly" - 只能通过 http 协议访问 cookie

    7  ini_set( string $varname , string $newvalue )
        设置指定配置选项的值。这个选项会在脚本运行时保持新的值，并在脚本结束时恢复。
        $varname    选项
        $newvalue   选项新的值
