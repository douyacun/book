* laravel5.4 安装 JWT,使用JWT作为api接口验证 
* 只是测试5.4,5.3没有实践,但是也可以
* [文档地址,英文文档](https://github.com/tymondesigns/jwt-auth/issues/860)
* @date 2017-07-30

# 第一步 composer.json `
```
'require' ={
        ...
        "tymon/jwt-auth": "dev-develop", //1.0.* 版本
    }
```
# 第二步 配置
- config/app.php
```php
'providers'=>[
        ...
        Tymon\JWTAuth\Providers\LaravelServiceProvider::class
    ]

'aliases' => [
        ...
        'JWTAuth' => 'Tymon\JWTAuth\Facades\JWTAuth',
        'JWTFactory' => 'Tymon\JWTAuth\Facades\JWTFactory'
    ]
```
- config/auth.php
```php
<?php

return [

    'defaults' => [
        'guard' => 'api',//使用jwt作为api接口认证
        'passwords' => 'users',
    ],

    'guards' => [
        'web' => [
            'driver' => 'session',
            'provider' => 'users',
        ],
        'api' => [
            'driver' => 'jwt',
            'provider' => 'users',
        ],
    ],
    'providers' => [
        'users' => [
            'driver' => 'eloquent',
            'model' => App\model\user\User::class,// 注意修改模型
        ],
        // 'users' => [
        //     'driver' => 'database',
        //     'table' => 'users',
        // ],
    ],
    ...
];

```


# 第三步 设置用户模型  
- use Illuminate\Foundation\Auth\User as Authenticatable;
- 注意实现两个方法返回的 `key` 和 `user`,否则会报错 ` array_merge(): Argument #2 is not an array in JWT.php 243`
```php
<?php

namespace Illuminate\Foundation\Auth;

use Illuminate\Auth\Authenticatable;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Auth\Passwords\CanResetPassword;
use Illuminate\Foundation\Auth\Access\Authorizable;
use Illuminate\Contracts\Auth\Authenticatable as AuthenticatableContract;
use Illuminate\Contracts\Auth\Access\Authorizable as AuthorizableContract;
use Illuminate\Contracts\Auth\CanResetPassword as CanResetPasswordContract;
use Tymon\JWTAuth\Contracts\JWTSubject as AuthenticatableUserContract;// 添加JWTSubject接口

class User extends Model implements
    AuthenticatableContract,
    AuthorizableContract,
    CanResetPasswordContract,
    AuthenticatableUserContract
{
    use Authenticatable, Authorizable, CanResetPassword;
    
    /**
    * key 
    */
    public function getJWTIdentifier(){
        return $this->getKey();  // Eloquent model method
    }
    
    /**
    * token 中设置的字段
    **/
    public function getJWTCustomClaims(){
        return [
            'user' => [
                'id' => $this->id,
                'phone' => $this->phone,
                'nickname' => $this->nickname,
                'picture' => $this->picture,
                'email' => $this->email
             ]
        ];
    }
}

```
# 第四步 配置 `Providers/AuthServiceProvider.php`   
```php
<?php

namespace App\Providers;
use Illuminate\Support\Facades\Auth;
use Tymon\JWTAuth\JwtGuard;
use Illuminate\Foundation\Support\Providers\AuthServiceProvider as ServiceProvider;

class AuthServiceProvider extends ServiceProvider
{
    /**
     * The policy mappings for the application.
     *
     * @var array
     */
    protected $policies = [
        'App\Model' => 'App\Policies\ModelPolicy',
    ];

    /**
     * Register any authentication / authorization services.
     *
     * @return void
     */
    public function boot()
    {
        $this->registerPolicies();
    
        Auth::extend('jwt', function ($app, $name, array $config) {
            // Return an instance of Illuminate\Contracts\Auth\Guard...
            return new JwtGuard(Auth::createUserProvider($config['provider']));
        });
    }
}
_接下来 就按照 Auth 文档进行验证就可以了,Auth的方法 JwtGuard已经实现了_

