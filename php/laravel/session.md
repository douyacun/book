# laravel session问题:
- api session 不生效问题;
- Auth 模块 remember me session有效期问题;

# api session 不生效问题;

laravel 并没有使用php内置的session, 使用自有session处理方式来存储的session:
1. 路由api的中间件为: ThrottleRequests(限制请求速率), SubstituteBindings(路由参数变量，我们还想要把它转化为特定的对象), 并没有涉及到session,所以你懂了
2. 路由web使用的中间件: EncryptCookies(加密cookie),  AddQueuedCookiesToResponse(将cookie添加到response中去), StartSession(开启session), ShareErrorsFromSession(error信息闪存),SubstituteBindings(路由参数变量，我们还想要把它转化为特定的对象),

所以: api并没有集成session中间件, 所以导致session无法记录, 只能怪自己没有好好看

# Auth 模块 remember me session有效期问题

```
if (Auth::attempt(['email' => $email, 'password' => $password, 'is_active'=>1], $remember)) {
    // set remember me expire time 7 days
    $rememberTokenExpireMinutes = 604800;

    // first we need to get the "remember me" cookie's key, this key is generate by laravel randomly
    // it looks like: remember_web_59ba36addc2b2f9401580f014c7f58ea4e30989d
    $rememberTokenName = Auth::getRecallerName();

    // reset that cookie's expire time
    Cookie::queue($rememberTokenName, Cookie::get($rememberTokenName), $rememberTokenExpireMinutes);

    return ['code'=>0, 'msg'=>'success', 'data'=>['location'=>url('/dashboard.html')]];
} else {
    return ['code'=>400, 'msg'=>'email or password error', 'data'=>[]];
}
```