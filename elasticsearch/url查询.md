---
Title: elasticsearch url查询的常用方法
Keywords: url查询,query string
Description: 看到这里才知道其实阿里云的sls查询，也是elasticsearch做了一次封装
Label:
Cover:
Date: 2019-12-24 00:17:00
LastEditTime: 2019-12-24 00:18:00
---

千里之行，始于足下。一点点的去积累，es的查询不止这些。



- 指定字段 VS 泛查询
  - `q=title:2012`
  - `q=2012`
- Term vs Pharse
  - `q=title:beautiful mind` 等效于 beautiful OR mind   Term查询
  - `q=title:"beautiful mind"` 等效于 beautiful AND mind，Pharse查询还要求前后顺序一致
- 分组 vs 引号
  - `q=title:(day AND night) `
    - Day Night Day Night
    - Day & Night
    - Day and Night
    - Night and Day
  - `q=title:"Night day"`
    - Day Night Day Night
  - `+title:night +title:day`
    - `+` 表示 must
    - `-` 表示 must not
- Bool
  - `AND` (`&&`)、`OR` (`||`)、`NOT `(`!`)  
    - 这里在url上 `&`有特殊含义，最好用`AND`
    - `AND`必须大写
    - `q=title:(night AND day)`
- 范围查询
  - `[]` 闭区间、`{}`开区间
    - `q=year:{2016 TO 2019]` TO 必须大写
    - `q=year:[2016 TO 2019]`
    - `q=year:[* TO 2019]`
- 算数符号
  - `q=year:>2019`
  - `q=year:(>2016 AND <=2019)`

- 通配符查询，查询效率低，占用内存大，不易放在前面
  - `?` 占一个字符，`*`  占0个或多个字符
    - `q=title:da?`
      - title:dad title:dae title:dan title:dao title:das title:day
    - `q=title:da*`

- regrex 正则
  - `q=title:[bt]oy`
  - todo: 这里待查看用法
- 模糊匹配近似匹配
  - `q=title:beautifl~1`
  - `q=title:"lord rings"~2`