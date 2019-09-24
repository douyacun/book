# 引用同一个文件中的变量

在函数内定义的变量是局部变量，在函数之外定义的变量则称为外部变量，也是全局变量，它的存储在静态区域，生存周期为整个程序的生存周期，全局变量可以为本文件中的其他函数所共用，它的有效范围从定义变量的位置开始到本源文件结束

如果全局变量不再文件的开头定义，有效的错用范围将只限于其定义处到文件结束，如果在定一点之前的函数想引用该全局变量，则应该在引用之前用关键字对改变量做 “外部变量声明”， 表示变量是一个已经定义的外部变量，有了此声明，就可以从声明处起，合法的使用该外部变量

```c
#include <stdio.h>
int max(int x,int y);
int main(void)
{
    int result;
    // 外部变量声明
    extern int g_X;
    extern int g_Y;
    result = max(g_X,g_Y);
    printf("the max value is %d\n",result);
    return 0;
}
/*定义两个全局变量*/
int g_X = 10;
int g_Y = 20;
int max(int x, int y)
{
    return (x>y ? x : y);
}
```

# 饮用另一个文件中的变量
整个工程由多个源文件组成，在一个源文件中想饮用另外一个源文件中已经定义的外部变量，同样只需在饮用变量的文件中用extern关键字加以声明
```c
/****max.c****/
#include <stdio.h>
/*外部变量声明*/
extern int g_X ;
extern int g_Y ;
int max()
{
    return (g_X > g_Y ? g_X : g_Y);
}
/***main.c****/
#include <stdio.h>
/*定义两个全局变量*/
int g_X=10;
int g_Y=20;
int max();
int main(void)
{
    int result;
    result = max();
    printf("the max value is %d\n",result);
    return 0;
}
```

> 注意: 
由于用extern引用外部变量，可以在饮用的模块内修改其变量的值，因此，如果有多个文件同时要对应用的变量进行操作，而且可能会修改该变量，就会影响其他模块的使用

# 引用用另一个文件中的函数

对函数而言，如果你想在本源文件中使用另一个源文件的函数，就需要在使用前用声明该函数，声明函数加不加extern都没关系，所以在头文件中函数可以不用加extern。