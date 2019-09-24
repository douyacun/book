linux内核和其他一些开源的代码中，经常会遇到do{}while(0)
```c
#define update_zmalloc_stat_alloc(__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    if (zmalloc_thread_safe) { \
        update_zmalloc_stat_add(_n); \
    } else { \
        used_memory += _n; \
    } \
} while(0)
```

# 辅助定义复杂的宏,避免引用的时候出错
```
#define DOSOMETHING() \
                foo1();\
                foo2();

// 如果这样调用就是出错
if(a > 0)
    DOSOMETHING();

// 会被编译成
if (a) {
    foo1();
}
foo2();
```

也可以这样定义宏
```c
#define DOSOMETHING() ({\
        foo1();\
        foo2();\
})
```
# 避免使用goto对程序流进行统一的控制

在函数return之前我们经常会进行一些收尾的工作，比如free掉一块函数开始malloc的内存，goto一直都是一个比较简便的方法：
```c
int foo()
{
    somestruct* ptr = malloc(...);
 
    dosomething...;
    if(error)
    {
        goto END;
    }
 
    dosomething...;
    if(error)
    {
        goto END;
    }
    dosomething...;
 
END:
    free(ptr);
    return 0;
 
}
```
可以使用do{}while(0)进行统一管理
```c
int foo()
{
 
    somestruct* ptr = malloc(...);
 
    do{
        dosomething...;
        if(error)
        {
            break;
        }
 
        dosomething...;
        if(error)
        {
            break;
        }
        dosomething...;
    }while(0);
 
    free(ptr);
    return 0;
}
```

# 避免空宏一起的warning
```c
#define EMPTYMICRO do{}while(0)
```