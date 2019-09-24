# locale 系统语言，地域，字符集设置,分为12类：

- 语言符号及其分类(LC_CTYPE)
- 数字(LC_NUMERIC)
- 比较和排序习惯(LC_COLLATE)
- 时间显示格式(LC_TIME)
- 货币单位(LC_MONETARY)
- 信息主要是提示信息,错误信息, 状态信息, 标题, 标签, 按钮和菜单等(LC_MESSAGES)
- 姓名书写方式(LC_NAME)
- 地址书写方式(LC_ADDRESS)
- 电话号码书写方式(LC_TELEPHONE)
- 度量衡表达方式(LC_MEASUREMENT)
- 默认纸张尺寸大小(LC_PAPER)
- 对locale自身包含信息的概述(LC_IDENTIFICATION)

还有两个变量：LC_ALL和LANG。它们之间有一个优先级的关系：LC_ALL > LC_* > LANG ，高优先级的设置会覆盖低优先级的设置。

# setlocal函数
函数原型：
```
#include <locale.h>

char *setlocale(int category, const char *locale);
```
第一个参数category是一个整型值，可取的值为上边locale的十二个类；

第二个参数locale是一个char型的字符串，可取的值为locale -a命令的输出：

- 如果locale参数为NULL，则返回当前指定类别的locale值，即只查询；
- 如果locale为一个合法的参数值（对于某个特定系统，其合法的值即为系统上面locale -a命令的输出），则将指定的locale类别设定为该值
- 如果locale为空字符串""，则每个类别会被设置成系统环境变量所设置的值，依次设置LC_ALL、LC_*、LANG。如果某个参数非法，则函数返回NULL。

对于C程序来说，程序刚启动的时候，“C”会被设置为默认的locale。为了程序的通用性和可移植性，我们调用setlocale(LC_ALL, ""); 语句，将locale设置为当前系统上面locale相关环境变量所设置的值。

在Linux中输入`locale`可以查看常量配置