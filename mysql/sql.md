* sql 积累
* @author 刘宁
* @date 2017-04-13

1.根据 path    0-1  0-1-2 0-1 0-1-3 ....进行排序

```mysql
    model('BsmOrganization')->field('*,length(path)-length(replace(path,"-","")) as sort')->where('id','>','1')->order('sort')->select();
```
