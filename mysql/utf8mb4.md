---
Title: utf8mb4格式存储数据
Keywords: utf8mb4
Description: mysql支持输入表情符号的问题
Cover: 
Label: 
Date: 2019-02-25 23:02:12
LastEditTime: 2019-11-29 11:30:42
---
**mysql支持输入表情符号的问题**

1. 备份,备份所有需要升级的字符串编码的数据库

    - dump出来

2. 升级

    utf8mb4是MySQL5.5.3版本之后支持的字符集

3. 修改

    可以为一个database设置字符编码，可以为一张表设置字符编码，甚至可以为某一个字段设置字符编码

    - 查看当前系统默认的字符集设置

    ```
    mysql> SHOW VARIABLES WHERE Variable_name LIKE 'character\_set\_%' OR Variable_name LIKE 'collation%';
    +--------------------------+-----------------+
    | Variable_name            | Value           |
    +--------------------------+-----------------+
    | character_set_client     | utf8            |
    | character_set_connection | utf8            |
    | character_set_database   | utf8            |
    | character_set_filesystem | binary          |
    | character_set_results    | utf8            |
    | character_set_server     | utf8            |
    | character_set_system     | utf8            |
    | collation_connection     | utf8_general_ci |
    | collation_database       | utf8_general_ci |
    | collation_server         | utf8_general_ci |
    +--------------------------+-----------------+
    10 rows in set (0.01 sec)

    mysql>

    ```

    - 查看database的字符编码

    ```
    mysql> show create database polarsnow;
    +-----------+--------------------------------------------------------------------+
    | Database  | Create Database                                                    |
    +-----------+--------------------------------------------------------------------+
    | polarsnow | CREATE DATABASE `polarsnow` /*!40100 DEFAULT CHARACTER SET utf8 */ |
    +-----------+--------------------------------------------------------------------+
    1 row in set (0.00 sec)

    mysql>
    ```

    - 查看table的字符编码

    ```
    mysql> show create table ps;
    +-------+---------------------------------------------------------------------------------------------+
    | Table | Create Table                                                                                |
    +-------+---------------------------------------------------------------------------------------------+
    | ps    | CREATE TABLE `ps` (
      `name` varchar(100) DEFAULT NULL
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8 |
    +-------+---------------------------------------------------------------------------------------------+
    1 row in set (0.00 sec)
    ```

    - 查看column的字符编码

    ```
    mysql> show full columns from ps;
    +-------+--------------+-----------------+------+-----+---------+-------+---------------------------------+---------+
    | Field | Type         | Collation       | Null | Key | Default | Extra | Privileges                      | Comment |
    +-------+--------------+-----------------+------+-----+---------+-------+---------------------------------+---------+
    | name  | varchar(100) | utf8_general_ci | YES  |     | NULL    |       | select,insert,update,references |         |
    +-------+--------------+-----------------+------+-----+---------+-------+---------------------------------+---------+
    1 row in set (0.00 sec)
    ```

    - 修改database默认的字符集
    ```
    ALTER DATABASE database_name CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci;

    mysql> ALTER DATABASE polarsnow CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci;
    Query OK, 1 row affected (0.03 sec)

    mysql> show create database polarsnow;
    +-----------+--------------------------------------------------------------------------------------------------+
    | Database  | Create Database                                                                                  |
    +-----------+--------------------------------------------------------------------------------------------------+
    | polarsnow | CREATE DATABASE `polarsnow` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci */ |
    +-----------+--------------------------------------------------------------------------------------------------+
    1 row in set (0.00 sec)

    mysql> show tables;
    +---------------------+
    | Tables_in_polarsnow |
    +---------------------+
    | ps                  |
    +---------------------+
    1 row in set (0.00 sec)

    mysql> show create table ps;
    +-------+---------------------------------------------------------------------------------------------+
    | Table | Create Table                                                                                |
    +-------+---------------------------------------------------------------------------------------------+
    | ps    | CREATE TABLE `ps` (
      `name` varchar(100) DEFAULT NULL
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8 |
    +-------+---------------------------------------------------------------------------------------------+
    1 row in set (0.00 sec)

    mysql> show full columns from ps;
    +-------+--------------+-----------------+------+-----+---------+-------+---------------------------------+---------+
    | Field | Type         | Collation       | Null | Key | Default | Extra | Privileges                      | Comment |
    +-------+--------------+-----------------+------+-----+---------+-------+---------------------------------+---------+
    | name  | varchar(100) | utf8_general_ci | YES  |     | NULL    |       | select,insert,update,references |         |
    +-------+--------------+-----------------+------+-----+---------+-------+---------------------------------+---------+
    1 row in set (0.00 sec)

    mysql> create table test_tb2 (tb2 varchar(100) );
    Query OK, 0 rows affected (0.21 sec)

    mysql> show tables;
    +---------------------+
    | Tables_in_polarsnow |
    +---------------------+
    | ps                  |
    | test_tb2            |
    +---------------------+
    2 rows in set (0.00 sec)

    mysql> show create table test_tb2;
    +----------+-----------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Table    | Create Table                                                                                                                                              |
    +----------+-----------------------------------------------------------------------------------------------------------------------------------------------------------+
    | test_tb2 | CREATE TABLE `test_tb2` (
      `tb2` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci |
    +----------+-----------------------------------------------------------------------------------------------------------------------------------------------------------+
    1 row in set (0.00 sec)

    mysql>
    ```

    - 修改column默认的字符集

    ` ALTER TABLE table_name CHANGE column_name column_name VARCHAR(191) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci; `

4. 修改配置文件
```
SET NAMES utf8 COLLATE utf8_unicode_ci becomes SET NAMES utf8mb4 COLLATE utf8mb4_unicode_ci

> vim /etc/my.cnf
# 对本地的mysql客户端的配置
[client]
default-character-set = utf8mb4

# 对其他远程连接的mysql客户端的配置
[mysql]
default-character-set = utf8mb4

# 本地mysql服务的配置
[mysqld]
character-set-client-handshake = FALSE
character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
> service mysqld restart
```

5. 检查修改
    ```
    mysql> SHOW VARIABLES WHERE Variable_name LIKE 'character\_set\_%' OR Variable_name LIKE 'collation%';
    +--------------------------+--------------------+
    | Variable_name            | Value              |
    +--------------------------+--------------------+
    | character_set_client     | utf8mb4            |
    | character_set_connection | utf8mb4            |
    | character_set_database   | utf8mb4            |
    | character_set_filesystem | binary             |
    | character_set_results    | utf8mb4            |
    | character_set_server     | utf8mb4            |
    | character_set_system     | utf8               |
    | collation_connection     | utf8mb4_unicode_ci |
    | collation_database       | utf8mb4_unicode_ci |
    | collation_server         | utf8mb4_unicode_ci |
    +--------------------------+--------------------+
    10 rows in set (0.00 sec)
    ```

6. 修改 & 优化所有数据表

    ```
    mysql> SHOW VARIABLES WHERE Variable_name LIKE 'character\_set\_%' OR Variable_name LIKE 'collation%';
    +--------------------------+--------------------+
    | Variable_name            | Value              |
    +--------------------------+--------------------+
    | character_set_client     | utf8mb4            |
    | character_set_connection | utf8mb4            |
    | character_set_database   | utf8mb4            |
    | character_set_filesystem | binary             |
    | character_set_results    | utf8mb4            |
    | character_set_server     | utf8mb4            |
    | character_set_system     | utf8               |
    | collation_connection     | utf8mb4_unicode_ci |
    | collation_database       | utf8mb4_unicode_ci |
    | collation_server         | utf8mb4_unicode_ci |
    +--------------------------+--------------------+
    10 rows in set (0.00 sec)
    ```

7. 最重要的修改 php 的数据库连接,不用担心, `utf8mb4` 支持 `utf8` 的链接!

    ```
    'mysql' => [
        'driver' => 'mysql',
        'host' => env('DB_HOST'),
        'port' => env('DB_PORT'),
        'database' =>env('BYIDB_DATABASE'),
        'username' => env('DB_USERNAME'),
        'password' => env('DB_PASSWORD'),
        'charset' => 'utf8mb4',
        'collation' => 'utf8mb4_unicode_ci',
        'prefix' => '',
        'strict' => true,
        'engine' => null,
    ],
    ```