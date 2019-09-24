# 字符类别测试和转换函数

头文件: `<ctype.h>`
> c 是一个可表示为 `unsigned char`类型或 `EOF`的 `int` 对象, 该函数的返回值类型为 `int`

- isalpha(c)        若c是字母, 则返回一个非0值, 否则返回0
- isupper(c)        若c是大写字母, 则返回一个非0值, 否则返回0
- islower(c)        若c小写字母, 则返回一个非0值. 否则返回0
- isdigit(c)        若c是数字,则返回一个非0值, 否则返回0
- isalnum(c)        若`isalpha(c)` 或 `isdigit(c)`, 则返回一个非0值, 否则返回0
- isspace(c)        若c是空格, 横向制表符, 换行符, 回车符, 换页符或纵向制表符则返回一个非0值
- toupper(c)        返回c的大写形式
- tolower(c)        返回c的小写形式
