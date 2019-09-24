# gdb 调试工具

调试redis设置:
```c
// 相当于 gcc -g
make CFLAGS="-g -O0"

gdb src/redis-server
```

> gdb 支持tab补全

# 运行程序
- `run` 可指定运行参数
  - `set args` 指定运行是参数
- 运行环境
  - `path` 设定程序的运行路径
  - `set environment varname[=value]` 设置环境变量 ：`set env USER=hchen`

# 设置断点: breakpointer

- 函数断点: `break <function>`
- 行号断点：`break <linenum>`
- 在程序运行内存地址处停止：`break *address`

# 查看断点: info
`info break[n]`

# 设置捕捉点：catchpoint

`cactch event`, event如下:
1. throw 一个 C++抛出的异常(throw 为关键字) ，
2. catch 一个 C++捕捉到的异常(catch 为关键字) ，
3. exec 调用系统调用 exec 时(exec 为关键字， 目前此功能只在HP-UX 下有用) 
4. fork 调用系统调用 fork 时。 (fork 为关键字， 目前此功能只在HP-UX 下有用)
5. vfork 调用系统调用vfork时。(vfork为关键字，目前此功能只在HP-UX下有用)
6. load 或 load <libname> 载入共享库(动态链接库)时。 (load 为关键字，目前此功能只在 HP-UX 下有用)
7. unload 或 unload<libname> 卸载共享库(动态链接库)时。 (unload为关键字，目前此功能只在 HP-UX 下有用)

只设置一次断点，程序停住后，断点自动删除

# 维护断点

- `clear` 清除所有的断点
- `clear <function>` 清除函数断点
- `clear <linenum>` 清除指定行上的断点
- `delete [<breakpointe>][range]` 清除指定断点，范围断点

# 恢复程序运行
- `continue`
- `c`
- `fg`
以上3个是同一个意思

# 单步跟踪
- `step<count>`: 如果有函数会进入函数，count：每次运行几条指令,不加每次运行一条
- `next<count>`: 如果有函数不会进入函数，count：每次运行几条指令,不加每次运行一条
- `finish`: 运行程序，知道函数完成，并打印函数返回时的堆栈地址和返回值及参数值信息
- `until`|`u`:推出循环体

打开单步跟踪模式: `set step-mode [on][off]` 在进行单步跟踪时，程序不会因为没有debug而停住

# 信号
unix定义了很多信号，比如SIGINT表示中断字符信号，可以要求GDB收到你锁指定的信号时，马上挺住正在运行的程序，以供进行调试

`handle <single> <keywords...>`

- single: 信号，all表明要处理的所有信号
- keywords:
  - nostop: 不会停住程序的运行
  - stop: 停住
  - print: 回显示出一条信息
  - noprint: 不会显示信息
  - pass/noignore: GDB不处理信号，交给调试程序处理
  - nopass/ignore: GDB不会让调试程序处理这个信号
  - info singles/info handle: 哪些信号被GDB检测中

# 线程
可以定义断点是否在所有线程上还是某个特定的线程
```gdb
break <linespec> thread <threadno>
break <linespec> thread <threadno> if...

break frik.c:13 thread 28 if bartab>lim
```
- linespec 行号
- threadno 线程id，`info threads`可以查看正在运行中的线程

不指定`threadno`表示断点设在所有的线程上面

# 查看栈信息
当程序被停住了，需要做的第一件事就是查看程序是在哪里停住了，当程序调用了一个函数：函数的地址、参数、局部变量都会被压入“栈中”
- `backtrace<n>`
- `bt<n>`
> n 只打栈顶上n层的信息
> -n 栈底n层的栈信息
打印当前函数调用栈的所有信息

切换当前栈
- `f<n>` | `frame<n>` 打印某一层的栈: 栈的层编号、当前函数名、函数参数值，函数所在的文件及行号
- `up<n>` 向上移动n层
- `down<n>` 移动n层

# 打印函数栈信息

- `info f` 打印更详细的栈层信息，大多是运行内存地址：函数地址、调用函数地址、被调用函数的地址、程序语言、函数参数地址及值、局部变量的地址
- `info args` 打印当前函数的参数名、值
- `info locals` 当前函数中所有局部变量、值
- `info catch` 当前函数中异常处理信息


# 显示源代码
`set listsize <count>` 一次显示源代码的行数

- `list<linenum>` 显示程序第`linenum`行周围的源程序
- `list<function>` 显示函数名为function的函数的源程序
- `list` 当前行后面的源程序
- `list -` 当前行前面的源程序
- `list <first>,<last>` 从first到last行之间的源代码
- `list <last>` 从当前行到last行之间的源代码
- `list +`往后显示源代码

list 后面可以跟的参数:
- `<linenum>`行号。 
- `<+offset>`当前行号的正偏移量。 
- `<-offset>`当前行号的负偏移量。 
- `<filename:linenum>`那个文件的那一行。 
- `<funtion>`函数名 
- `<filename:funtion>`那个文件中的那个函数。 
- `<*address>`程序运行时的语句在内存中的地址。

# 搜索源代码
- 向前搜索
  - `forward-search <regexp>`
  - `search <regexp>` 
- 全部搜索
  - `reverse-search<regexp>`

# 指定源文件的路径
- `directory<dirname...>`加一个源文件路径到当前路径的前面，指定多个路径“:”
- `directory`清除所有的自定义的源文件搜索路径信息
- `show directories`显示定义了的源文件搜索路径

# 打印数据
`print/<f><expr>`

- `@` 一个数组有关的操作符，在后面会有更详细的说明
- `::`指定一个文件中或是一个函数中的变量
- `{<type>}<addr>` 表示一个指向内存地址`<addr>`的类型为type的一个对象

gdb中可以随时查看一下三种变量的值：
- 全局变量（所有文件可见）
- 静态全局变量（当前文件可见）
- 局部变量（当前scope可见的）

数组：
```c
int *array = (int*)malloc(len*sizeof(int))

// 动态数组
(gdb) p *array@len
$1 = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40}

// 静态数组就可以查看
print array
```

# 输出格式
- x 按十六进制格式显示变量。
- d 按十进制格式显示变量。
- u 按十六进制格式显示无符号整型。 o 按八进制格式显示变量。
- t 按二进制格式显示变量。
- a 按十六进制格式显示变量。
- c 按字符格式显示变量。
- f 按浮点数格式显示变量。

# 查看内存
`examine`(简写 x), 查看内存地址中的值

`x<n/f/u><addr>`

n、f、u 是可选参数。
- n 是一个正整数，表示显示内存的长度，也就是说从当前地址向后显示几个地址的内容。 
- f 表示显示格式，参加上面。如果地址所指的是字符串，那么格式是 s，如果地址是指令
地址，那么格式也可以是 i。
- u 表示从当前的地址往后请求的字节数，如果不指定的话，GDB 默认是 4 个 bytes。u 参数可以用下面的字符来代替：
  - b 表示单字节
  - h 表示双字节
  - w 表示四字节
  - g 表示八字节

`x/3uh 0x54320` 表示，从内存地址`0x54320`读取内容

# 自动显示
可以设置一些自动显示的变量，当程序停住时，或是在单步跟踪时，这些变量回自动显示：
- `display<expr>`
- `display<fmt><expr>`
- `display/<fmt><expr>`

> expr: 表达式
> fmt: 显示的格式
> addr: 内存地址

删除自动显示，dnums设置好了的自动显示的编号，同时删除多个，用空格分隔，范围删除，用`-`分隔
- `undisplay<dnums...>`
- `delete display<dnums...>`

`info display` 查看display自动显示的信息

# 设置选项
- `set print address <on/off>` 打开地址输出,系统默认打开，等同于`frame/f`显示地址信息
- `set print array <on/off>` 打开数组显示，每个元素栈占一行，不打开每个以逗号分隔
- `set print elements <on/off>` print elemets的选项信息
- `set print null-stop<on/off>` 显示字符串时，遇到`\0`就停止显示
- `set print pretty <on/off>` gdb显示结构体就比较漂亮
- `set print union <on/off>` 显示结构体时，是否显示其内的联合体数据
- `set listsize <n>` 一次显示源代码的行数
- `set step-mode [on][off]` 在进行单步跟踪时，程序不会因为没有debug而停住
- `set print array-indexes on` 打印数组索引下标


# 修改变量值
`print x=4`

# 跳转执行
`jump<linespect>`执行下一条语句运行点，`file:line`

# 查生信号量
`single 命令`

# 强制函数返回
`return`

# 强制调用函数
`call`

- https://www.kancloud.cn/wizardforcel/gdb-tips-100/146752

# gdb 启动脚本
gdb启动时，会读取HOME目录和当前目录下的的配置文件，执行里面的命令。这个文件通常为“.gdbinit”。

```
# 保存历史命令
set history filename ~/.gdb_history
set history save on

# 退出时不显示提示信息
set confirm off

# 按照派生类型打印对象
set print object on

# 打印数组的索引下标
set print array-indexes on

# 每行打印一个结构体成员
set print pretty on

set step-mode on
```
