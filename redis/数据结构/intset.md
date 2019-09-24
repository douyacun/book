# 整数集合
是集合键的底层实现之一，一个集合只包含整数值元素，并且这个集合的元素数量不多时，redis就会使用整数集合作为集合键的底层实现

```c
typedef struct intset{
    // 编码方式
    uint32_t encoding;

    // 集合元素数量
    uint32_t lenght;

    // 保存元素的数组
    int8_t contents[];
}intset;
```