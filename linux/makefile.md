# 依赖关系

```makefile
app : main.o
    cc -o app main.o
main.o : main.c
    cc -c main.c
clean :
    rm app main.o
```
生成app依赖于main.o文件

# 定义变量，代替目标文件
```makefile
objects = main.o person.o other.o
app : $(objects)
    cc -o app $(objects)
main.o : main.c
    cc -c main.c a.h
person.o : person.c a.h b.h
    cc -c person.c
other.o : person.c b.h
    cc -c person.c -o other.o
clean :
    rm app $(objects)
```

# makefile自动推导机制
```makefile
objects = main.o person.o 

app : $(objects)
    cc -o app $(objects)

main.o : main.c
person.o : person.c a.h b.h

clean :
    rm app $(objects)
```

# vpath 如果文件不再一个目录，只需要子啊第一行添加vpath(:分隔), makefile会自动按照路径顺序，依次查找文件
```makefile
VPATH = path1:path2:path3
```

# eg.
```makefile
VPATH = ./subfile:./otherPath
#这是一个注释
CC = gcc #设置编译器

objects = main.o person.o

app : $(objects)
    $(CC) -o app $(objects) #使用设定的编译器编译
    @echo "输出信息：编译完成"

main.o : main.c

person.o : person.c a.h b.h

.PHONY : clean #说明虚拟标号，表示clean不出现在依赖树中

clean :
    rm app $(objects)
```