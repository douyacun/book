
- 导出数据库结构和数据
 mysqldump -u root -pa123456 demodb > demo.sql

- 导出数据库结构
 mysqldump -u root -pa123456 -d demodb > demo.sql

 mysqldump -u wcnc -p -d --add-drop-table demodb > demo.sql
  -d 没有数据 --add-drop-table 在每个create语句之前增加一个drop table

- 导出数据表结构和数据
 mysqldump -u root -pa123456 -d demodb test > test.sql

- 导出数据表结构
 mysqldump -u root -pa123456 -d --add-drop-table demo test > test.sql

- 导入数据库,先选择数据库
mysql > use demo
mysql > source /root/demo.sql

- 授权
```
grant select, update, insert, delete, alter, create, trucate
->  on  demo.*                  # 授权liuning demo数据库中的所有表
->  to  'liuning'@'locahost'    # 只能本机登录
->  identified by '123456'
```

- 刷新权限
flush PRIVILEGES


- 创建数据库
CREATE DATABASE IF NOT EXISTS videos default charset utf8mb4 COLLATE utf8mb4_general_ci


- 删除数据库
mysqladmin -u root -p drop dahe;


- 去掉重复数据:
select * from `scrapy_proxy_ip` where ip IN (SELECT ip FROM `scrapy_proxy_ip` GROUP BY ip HAVING COUNT(*) > 1)

