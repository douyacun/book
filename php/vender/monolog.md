# laravel 日志问题:

1.  将不同的日志信息存放到不同的日志中去
2.  laravel的错误日志堆栈
3.  laravel日志分割
4.  sql日志
5.  异常日志分析: Exception
6.  日志格式,  对接ELK日志系统

## laravel内置:

#### log 存储级别:
- single:     单一文件；
- daily:      没天一个文件;
- syslog:     日志服务器;
- errorlog:   错误日志;

#### 最大日志文件数:
默认存储5天的日志文件 \
log_max_files=30

#### 日志存储级别:

debug、 info、notice、 warning、error、critical、alert、emergency;

- Log::emergency($error);     //紧急状况，比如系统挂掉
- Log::alert($error);     //需要立即采取行动的问题，比如整站宕掉，数据库异常等，这种状况应该通过短信提醒
- Log::critical($error);     //严重问题，比如：应用组件无效，意料之外的异常
- Log::error($error);     //运行时错误，不需要立即处理但需要被记录和监控
- Log::warning($error);    //警告但不是错误，比如使用了被废弃的API
- Log::notice($error);     //普通但值得注意的事件
- Log::info($error);     //感兴趣的事件，比如登录、退出
- Log::debug($error);     //详细的调试信息

#### 常用助手函数:

- `info('User login attempt failed.', ['id' => $user->id]);` 记录info级别
- `logger('User has logged in.', ['id' => $user->id]);` 记录debug级别

> :  数组, 资源, 对象... 等非字符串数据格式 print_r($var, true);

## 将不同的日志信息存放到不同的日志中去, 日志分割

```
Log::useDailyFiles(storage_path().'/logs/laravel.log', config('app.log_max_files'. 5));
```

## 定义日志格式:
```
Log::useErrorLog("[%datetime%] %level_name% : %message% %context% %extra%\n");
```

## Api Exception 处理, 接口500错误返回json, 修改 `app/Exceptions/Handler.php`

```
public function render($request, Exception $exception)
{
    if($request->is('api/*')){
        $response = [];
        $error = $this->convertExceptionToResponse($exception);
        $response['status'] = $error->getStatusCode();
        $response['msg'] = 'something error';
        if(config('app.debug')) {
            $response['msg'] = empty($exception->getMessage()) ? 'something error' : $exception->getMessage();
            if($error->getStatusCode() >= 500) {
                if(config('app.debug')) {
                    //$response['meta'] = $exception->getTraceAsString();
                    $response['code'] = $exception->getCode();
                }
            }
        }
        $response['data'] = [];
        return response()->json($response, $error->getStatusCode());
    }else{
        return parent::render($request, $exception);
    }
}
```


## 自定义函数, 不同级别日志放到不同目录中, 可自定义日志路径

```
if (! function_exists('__log')) {
	/**
	 * 不同级别日志放到不同目录中
	 * @param string $message
	 * @param array  $context
	 * @param null   $path      自定义存储路径
	 * @param string $level     日志级别
	 */
	function __log(string $message,array $context, $level = 'info' , $path = null)
	{
		$path = $path ?? $level;
		# 日志分割
		app('log')->useDailyFiles(storage_path().'/logs/'. $level .'/'. $path .'.log', config('app.log_max_files'), $level);
		app('log')->$level($message, $context);
	}
}
```

## 记录sql日志:

添加时间监听:  `app/Providers/EventServiceProvider.php`

```
protected $listen = [
    'App\Events\Event' => [
        'App\Listeners\EventListener',
    ],
    'Illuminate\Database\Events\QueryExecuted' => [
        'App\Listeners\SqlListener',
    ],
];
```

`php artisan event:generate` 生成 `App\Listeners\SqlListener`

```
    /**
     * Handle the event.
     *
     * @param  QueryExecuted  $event
     * @return void
     */
    public function handle(QueryExecuted $event)
    {
	    if($event->sql){
		    // 把sql  的日志独立分开
		    app('log')->useDailyFiles(storage_path().'/logs/sql/sql.log', config('app.log_max_files',5));
		    $sql = str_replace("?", "'%s'", $event->sql);
		    $log = vsprintf($sql, $event->bindings);
		    __log($log);
	    }
    }
```

## 使用 `unique_id` 标记每条请求,统一日志格式,记录入参出参

```
//# 自定义日志处理:
$app->configureMonologUsing(function ($monolog) {
	# 统一日志格式
	$dateFormat = "[%datetime%] %channel%.%level_name%: %extra% %message% %context% \n";
	$handler = new Monolog\Handler\RotatingFileHandler(storage_path('logs/laravel.log'), 0, Monolog\Logger::DEBUG);
	$handler->setFormatter(new Monolog\Formatter\LineFormatter($dateFormat, 'Y-m-d H:i:s'));

	$request = request()->all();
	$serverData = &$_SERVER;
	$serverData['REQUEST'] = $request;
	$serverData['UNIQUE_ID'] = substr(hash('md5', uniqid('', true)), 0, 16);
	$extraFields = array(
		'url'         => 'REQUEST_URI',
		'ip'          => 'REMOTE_ADDR',
		'request'     => 'REQUEST',
	);

	$monolog->pushHandler($handler);
	$monolog->pushProcessor(new Monolog\Processor\WebProcessor($serverData, $extraFields));
});
```




