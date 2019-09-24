* 刘宁
* 2017/09/07

# sql原始语句

## 增
- insert插入多条语句
```
mysql> insert into test(name) values('a'),('b'),('c'),('d');
```

## 查
- distinct 查询不重复的结果集
```
mysql> select distinct name from test;
```
- 使用正则查询
```
mysql> select * from t1 where name regexp '^a'
```
- 使用用户变量查询
```
mysql> select @min_price:=price, @max_price:= from shop;
mysql> select @min_prcie;
mysql> select * from shop where price=@max_price or price=@min_price;
```
## 改


## 删