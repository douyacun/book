# 联合体

当多个数据需要共享内存或多个数据每次只取其一时, 可以使用联合体(union);

1. 联合体是一个结构;
2. 它的所有成员相对基地址的偏移量都为0;
3. 此结构空间要大到足够容纳最宽的成员;
4. 其对齐方式要适合其中所有的成员;

> 联合体所有成员, 共享一段内存, 因此每个成员的存放首地址相对于联合体变量的基地址的偏移量为0,即所有成员的首地址都是一样的
为了使得所有成员能够共享一段内存, 该空间要能容纳最宽的成员, 因为共享, 所以取数据时只能取一, 联合体不同于结构体

```
#include <stdio.h>
union var{
    long int l;
    int i;
};

int main(int argc, const char * argv[]) {

    // 联合体
    union var v;

    v.l = 5;
    printf("v.l is %ld\n", v.l);

    v.i = 6;
    printf("now v.l is %ld! the address is %p\n",v.l,&v.l);
    printf("now v.i is %d! the address is %p\n",v.i,&v.i);

    printf("\n");
    return 0;
}
```

#### 本质
根据 union 固定首地址和 union 按最大需求开辟一段内存空间, 都是虚的, 就是内存化一块足够用的空间;

