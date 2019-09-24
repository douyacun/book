# Eloquent ORM

### 读写分离的实现:
```
'mysql' => [
    'read' => [
        'host' => '192.168.1.1',
    ],
    'write' => [
        'host' => '196.168.1.2'
    ],
    'sticky'    => true,
    'driver'    => 'mysql',
    'database'  => 'database',
    'username'  => 'root',
    'password'  => '',
    'charset' => 'utf8mb4',
    'collation' => 'utf8mb4_unicode_ci',
    'prefix'    => '',
],
```

### 全局作用域:

- 查询作用域
- 匿名的全局作用域
- 删除全局作用域

### 观察器
- 监控 增删改查


### 关联
```
User::find(1)->phone;
```