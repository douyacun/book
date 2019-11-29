---
Title: mysql函数整理
Keywords: 函数,func,case when
Description: 聚合函数、符串函数、控制流、日期函数
Cover: 
Label: 
Date: 2019-02-25 23:02:12
LastEditTime: 2019-11-29 11:34:43
---

# 聚合函数
- avg(),        求取平均值
- count(),      计算表中的行数
- instr('mysql','sql')       计算子字符在字符串中第一次出现的位置
- sum()         计算总和
- min()         求取最小值
- max()         计算最大值
- group_concat()    函数将组中的字符串连接成为具有各种选项的单个字符串。
    ```
        GROUP_CONCAT(DISTINCT expression
            ORDER BY expression
            SEPARATOR sep);

    mysql> INSERT INTO t(v) VALUES('A'),('B'),('C'),('B');

    mysql> select group_concat(
                distinct v
                order by v desc
                separator ';')
            from t;

    mysql> d;c;b;a
    ```
    + 相当于:select v from t order by v desc;
    + 结果整合成字符串,PHP implode(',',$res);

# 字符串函数
- concat()  不多说
- length() char_length() 以字节或字符 字符串长度
- replace(str,old_string,new_string)     搜索并替换字符串中的子字符串
- substring(string,position)            截取字符串
- trim([{BOTH|LEADING|TRAILING} [removed_str]] FROM str) 和php作用相同
- find_in_set('a','a,b,c')     允许您在逗号分隔的字符串列表中查找指定字符串的位置。
- format(1111.22222,2);         相当于PHP number_format();

# 控制流
- CASE表达式 
```
CASE value
WHEN compare_value_1 THEN result_1
WHEN compare_value_2 THEN result_2
…
ELSE result 
END

CASE
    WHEN state IS NULL THEN country
    ELSE state
END

mysql> select
case phone
when 15258157582 then 1234567
else phone end
from ln_users;
```
- if语句
```
IF expression THEN
   statements;
END IF;
```
- ifnull语句
```
SELECT IFNULL(1,0); -- returns 1

SELECT IFNULL('',1); -- returns ''

select ifnull(email,concat(phone,'@abc.com') as email from ln_users;
```
- nullif语句
    + NULLIF(expression_1,expression_2);
    + 如果expression_1 = expression_2为true，则NULLIF函数返回NULL，否则返回expression_1 。
```
mysql> select nullif(phone,15258157582) as phone from ln_users;
```

# 日期函数
- current_date()    返回当前日期
- datediff()        计算两个日期之间的时间差
- day()             获取指定日期的天数   2017-09-05  5天
- [date_add()](http://www.yiibai.com/mysql/date_add.html)   将时间值添加到日起值
- [date_sub()](http://www.yiibai.com/mysql/date_sub.html)   减日期
- date_format()     格式化日期
```
select date_format(create_at,'%Y-%m-%d') from ln_users;
```
- from_unixtime(unixtime(),'%Y-%m-%d %H:%i:%s')   格式化时间戳
- uninx_timestamp(now());    获取当前时间戳
- now()             获取当前时时间
- month()           获取给定日期的整数月份
- str_to_date()     函数将字符串转换为日期时间值
```
SELECT STR_TO_DATE('21,5,2018','%d,%m,%Y');
```
- timediff()        返回两个TIME或DATETIME值之间的差值。
```
SELECT TIMEDIFF('12:00:00','10:00:00') diff;
```
- timestampdiff()   计算两个DATE或DATETIME值之间的差值。
- week()            返回一个日期的星期数值。
- weekday()         返回一个日期表示为工作日/星期几的索引。
- year()            放回年份
