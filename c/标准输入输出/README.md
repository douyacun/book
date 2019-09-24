# 标准输入输出
1. 标准输入, 输出主要由缓冲区和操作区两部分组成
2. 缓冲区最大的特点是数据的一次性, 泼出去的水
3. 两个指针指向两个结构体, 键盘和屏幕在内存的抽象表示, 两个指针命名为 `stdin` 和 `stdout`
4. 标准输入输出缓冲区存储的字符ASCII码值


## getchar, putchar

putchar的作用主要是向输出缓冲区中写入一个字符
getchar的作用主要是想输入缓冲区中读取一个字符, 如果碰到文件结尾返回-1

getchar 源码:
```
int getchar(void)
{
    static char buf[BUFSIZ];
    static char *bb = buf;
    static int n = 0;
    if(n == 0){
        n = read(0, bug, BUFSIZ);
        bb = buf;
    }
    return(--n >= 0) ? (unsigned char)*bb++ : EOF;
}

调用read方法从键盘读取数据（该方法会导致阻塞）,并让指针指向数组的首地址
```

## gets, puts
puts函数主要向输出缓冲区写入一个字符串, 并再字符串结束后, 在额外输出一个换行符 '\n'
gets函数从输入缓冲区读取中读取字符到指定字符串中, 读取过程会忽略所有前导空白符, 读入的第一个字符为非空白符, 知道遇见换行符停止
结束的换行符'\n', 被 gets 从缓冲区读取存储在数组中, 然后替换成'\0'

```
char* gets(char *buf){
    int c;
    char *s;
    static int warned;
    static const char w[] = "warning: this program uses gets(), which is unsafe.\n";
    FLOCKFILE(stdin); ORIENT(stdin, -1);
    if (!warned) {
        (void)_write(STDERR_FILENO, w, sizeof(w) - 1);
        warned = 1;
    }
    for (s = buf; (c = __sgetc(stdin)) != '\n';)
        if (c == EOF)
            if (s == buf) {
                 FUNLOCKFILE(stdin); return (NULL);
            } else
                break;
        else
            *s++ = c; *s = 0;
        FUNLOCKFILE(stdin); return (buf);
}
```

## printf的使用

printf()会根据参数format字符串来转换并格式化数据，然后将结果写出到标准输出设备，直到出现字符串结束('\n')为止。
puts函数会自动添加换行;

1. 一般文本，伴随直接输出。
2. 转移字符, 如 `'\t'`, `'\n'`;
3. 格式转换字符, 格式转换为一个百分比

格式转换字符: `“%[符号][宽度][.精度]类型”`
- 宽度: 做少输出的字符个数
- `-`: 表示对齐方式, `%-8` ,左对齐，当显示字符不足8时，右补空格; `%08，右对齐，当显示字符不足8时，左补0`
- `.`: 对于小数表示, 小数点后的位数, 大于显示精度四舍五入, 小于显示精度补0

类型:
- `%d`: 显示十进制有符号数, char, short, int, long long
- `%u`: 显示十进制无符号数, unsigned short, unsigned int, unsigned long long
- `%x`: 显示十六进制整数, 所有有符号及无符号整数
- `%f`: 显示十进制浮点数, float, double
- `%c`: 显示字符
- `%s`: 显示字符串


## scanf的使用
scanf会将输入的数据根据参数format字符串来转换并格式化数据

格式: `%[宽度][数据所占字节]输入类型`
- 宽度: 最多输入的字符数
- 数据类型: h表示2个字节, l表示8字节, 什么都没有表示四字节

用scanf读取字符串时，忽略前导的空白符，再次遇到空白符会结束输入，并将再次遇到的空白符留在缓冲区内，自动添加字符串数组的结束标志'\n'。


## fprintf(): 输出函数(格式化输出数据至文件)

定义函数: int fprintf(FILE * stream, const char * format, ...);

函数说明: fprintf(); 会根据参数format字符串来转换并格式化数据, 然后将结果输出到参数stream指定的文件中,
直到出现字符串结束('\0')为止;

返回值: 成功则返回实际输出的字符串, 失败返回-1, 错误原因存于errno中.

## fscanf(): 将文件流中的数据格式化输入

定义函数: `int fscanf(FILE * stream, char *format [, argument ,... ] );`

其他见`scanf`

## fwrite(): 将数据流写入文件中

定义函数: fwrite(<#const void *restrict __ptr#>, <#size_t __size#>, <#size_t __nitems#>, <#FILE *restrict __stream#>)

函数说明: `fwrite()` 用来将数据写入文件流中, 参数stream为一打开的文件指针, 参数ptr指向欲写入的数据地址,
总共写入的字符数以参数 size *nmemb 来决定, `fwrite()`会返回实际写入的 `nmemb` 数目

返回值: 实际写入 `nmemb` 数目;

## fseek(): 移动文件流的读写位置

定义函数: `int fseek(FILE * stream, long offset, int whence);`

函数说明:
1. `stream` 为已打开的文件指针
2. `offset` 根据参数 `whence` 来移动读写位置的位移数:
    - `SEEK_END` 从距文件开头 offset 位移量为新的读写位置;
    - `SEEK_CUR` 从当前的读写位置往后增加 offset 个位移量;
    - `SEEK_END` 将读写位置指向文件尾后在增加 offset 个位移量;
    > `whence` 值为 `SEEK_CUR` 或  `SEEK_END`  时, 参数offset 允许负值的出现.

下列是较特别的使用方式：

1) 欲将读写位置移动到文件开头时:fseek(FILE *stream, 0, SEEK_SET);
2) 欲将读写位置移动到文件尾时:fseek(FILE *stream, 0, 0SEEK_END);

返回值: 当当调用成功是返回0, 若有错误返回-1, errno 会存放错误代码;

> fseek()不像lseek()会返回读写位置, 因此必须使用 `ftell()` 来取得目前读写的位置.

## fgets(): 从 `stream` 指向的文件 读取下一个输入行(包括换行符), 并把它存在 str 所指向的字符串内

定义函数: `char *fgets(char *str, int n, FILE *stream)`

函数说明: 当读取到 (n - 1)个字符是, 或者读取到换行符时, 或者达到文件末尾是, 它会停止, 具体视情况而定;

参数:
- str 指向一个字符数组的指针, 该数组存储了要读取的字符串
- n 这是要读取的最大字符数(包括最后的空字符), 通常是使用以 `str` 传递的数组长度
- stream 指向FILE对象的指针,FILE对象标识了吆从中读取字符的流

返回值: 如果成功, 该函数返回相同的str参数, 如果到达文件末尾或者没有读取到任何字符,
str的内容保持不变, 并返回一个空指针(EOF), 如果发生错误, 返回一个空指针;

## fputs(): 将一个字符串(不包含换行符) 写入到一个文件中

定义函数: `int fputs(char *line, FILE *fp)`

函数说明:  `gets` 和 `puts` 的功能与 `fputs` 和 `fgets` 函数类似, 他们是对 `stdin` 和 `stdout`
进行操作, `gets` 函数在读取字符串是将删除结尾的换行符('\n'), `puts` 函数在写入字符串是将在结尾添加一个换行符
