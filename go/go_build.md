---
Title: go build/参数/选项
Keywords: 
Description: 
Author: douyacun
Date: 2019-07-12 17:46:21
LastEditTime: 2019-07-12 17:47:20
---

- -a
    完全编译，不理会-i产生的.a文件(文件会比不带-a的编译出来要大？)
- -n
    仅打印输出build需要的命令，不执行build动作（少用）。
- -p n
    开多少核cpu来并行编译，默认为本机CPU核数（少用）。
- -race
    同时检测数据竞争状态，只支持 linux/amd64, freebsd/amd64, darwin/amd64 和 windows/amd64.
- -msan
    启用与内存消毒器的互操作。仅支持linux / amd64，并且只用Clang / LLVM作为主机C编译器（少用）。
- -v
    打印出被编译的包名（少用）.
- -work
    打印临时工作目录的名称，并在退出时不删除它（少用）。
- -x
    同时打印输出执行的命令名（-n）（少用）.
- -asmflags 'flag list'
    传递每个go工具asm调用的参数（少用）
- -buildmode mode
    编译模式（少用）
    'go help buildmode'
- -compiler name
    使用的编译器 == runtime.Compiler
    (gccgo or gc)（少用）.
- -gccgoflags 'arg list'
    gccgo 编译/链接器参数（少用）
- -gcflags 'arg list'
    垃圾回收参数（少用）.
- -installsuffix suffix

    a suffix to use in the name of the package installation directory,
    in order to keep output separate from default builds.
    If using the -race flag, the install suffix is automatically set to race
    or, if set explicitly, has _race appended to it.  Likewise for the -msan
    flag.  Using a -buildmode option that requires non-default compile flags
    has a similar effect.
- -ldflags 'flag list'
    '-s -w': 压缩编译后的体积
    -s: 去掉符号表
    -w: 去掉调试信息，不能gdb调试了
- -linkshared
    链接到以前使用创建的共享库
    -buildmode=shared.
- -pkgdir dir
    从指定位置，而不是通常的位置安装和加载所有软件包。例如，当使用非标准配置构建时，使用-pkgdir将生成的包保留在单独的位置。
- -tags 'tag list'
    构建出带tag的版本.
- -toolexec 'cmd args'
    ？？？？？？不明白
    a program to use to invoke toolchain programs like vet and asm.
    For example, instead of running asm, the go command will run
    'cmd args /path/to/asm <arguments for asm>'.