__attribute__机制，可以是这函数属性、变量属性、类型属性，

可以对结构体和共用体进行属性设置，有6个属性可以设置
- aligned   
- packed    
- transparent_union 
- unused
- deprecated
- may_alias

# align(alignment)
设定一个指定大小的对齐格式
```c
struct S {
    char name[7];
    uint32_t id;
    char subject[5];
} __attribute__ ((aligned (8)));
```
强制编译器确保变量类型struct s的变量在分配空间是采用8字节的对齐方式, 结构体大小为24个字节

#packed
使用该属性可以使得变量或者结构体成员使用最小的对齐方式，即对变量是一字节对齐，对域（field）是位对齐。
```c
struct S {
    char name[7];
    uint32_t id;
    char subject[5];
} __attribute__ ((__packed__));
```
强制编译器确保变量类型struct s的变量在分配空间是采用1字节的对齐方式, 结构体大小为15个字节

TODO:: 待补充