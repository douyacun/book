## ungetc(): 用于讲一个字符退回输入流

定义函数: `int ungetc(char c,  FILE *stream)`

函数说明: c为要退回的字符，stream为要退回的输入流。

返回值: 若该函数执行成功，返回非零值；否则，返回0。

## system(): 命令执行函数

头文件: ` #include <stdlib.h> `

定义函数: `int system(const char *command)`

## 存储管理函数

#### malloc(): 分配所需的内存空间, 返回一个指向他的指针

头文件: `#include <stdlib.h>`

定义函数: `void *malloc(size_t size)`

函数说明:
- size  -- 内存块大小, 以字节为单位

返回值: 当分配成功时, 返回一个指针, 指针指向n字节长度的未初始化存储空间, 否则返回 `NULL`

#### calloc():分配所需的内存空间, 返回一个指向他的指针

定义函数: void *calloc(size_t n, size_t size);

返回值: 分配成功时, 返回一个指针, 该指针指向的空闲空间足以容纳由n个指定长度对象组成的数组, 否则为null, 该存储空间被初始化为0

#### free(p): 函数释放p指向的存储空间

定义函数: p是调用malloc或calloc函数得到的指针, 存储空间的释放顺序没有限制

## 随机数函数

#### rand(): 伪随机数

定义函数: `int rand(void)`

函数说明: c 库函数  `rand` 返回一个 0 到 RAND_MAX 之间的伪随机数, RAND_MAX是一个常量, 默认值在不同的实现中会有所不同,但是
值至少是32767

#### srand(): 初始化随机数发生器

定义函数: `void srand(unsigned int seed)`

函数说明: C 库函数 `void srand(unsigned int seed)` 播种由函数 rand 使用的随机数发生器。

实例: 0 - max 之间的随机数
```
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    int i, n, max = 10;
    time_t t;

    n = 5;

    srand(((unsigned) time(&t)));

    i = rand() % max;

    printf("%d\n", i);

    return 0;
}

```
