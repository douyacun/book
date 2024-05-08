---

---



1. 开启pprof

```
import (
 _ "net/http/pprof"
)

go func() {
  log.Println(http.ListenAndServe(":6060", nil))
}()
```

2. 使用`go tool pprof` 分析

```
go tool pprof -inuse_space http://47.103.156.111:6060/debug/pprof/heap

-inuse_space 已分配但尚未释放的内存空间
-inuse_objects 已分配但尚未释放的对象数量
-alloc_space 分配的内存总量(已释放的也会统计)
-alloc_objects 分配的对象总数(无论是否释放)

(pprof) top
Showing nodes accounting for 11606.66kB, 65.25% of 17789.31kB total
Showing top 10 nodes out of 135
      flat  flat%   sum%        cum   cum%
 3618.02kB 20.34% 20.34%  3618.02kB 20.34%  google.golang.org/protobuf/internal/filedesc.(*File).initDecls
 2338.70kB 13.15% 33.48%  2338.70kB 13.15%  github.com/modern-go/reflect2.loadGoTypes
 1294.30kB  7.28% 40.76%  1294.30kB  7.28%  google.golang.org/protobuf/reflect/protoregistry.(*Files).RegisterFile.func2
 1024.16kB  5.76% 46.52%  1024.16kB  5.76%  google.golang.org/protobuf/internal/impl.fieldInfoForScalar
  655.29kB  3.68% 50.20%   655.29kB  3.68%  runtime.itabsinit
  574.34kB  3.23% 53.43%   574.34kB  3.23%  google.golang.org/protobuf/internal/strs.(*Builder).grow
  536.37kB  3.02% 56.45%   536.37kB  3.02%  github.com/twitchyliquid64/golang-asm/asm/arch.archX86
  528.17kB  2.97% 59.41%   528.17kB  2.97%  gitlab.aiera.tech/cloud/protogen/kitex_gen/cangjinge/app/payment.init
  518.65kB  2.92% 62.33%   518.65kB  2.92%  gitlab.aiera.tech/cloud/protogen/kitex_gen/hotdog/ecommerce/shellmarket.init
  518.65kB  2.92% 65.25%   518.65kB  2.92%  gitlab.aiera.tech/cloud/protogen/kitex_gen/hotdog/metaverse/land.init
(pprof) list google.golang.org/protobuf/internal/filedesc.(*File).initDecls
```

大部分都是引用库的函数，想办法追踪到调用的地方，

```
go tool pprof -inuse_space -cum -svg http://47.103.156.111:6060/debug/pprof/heap > heap_inuse.svg
```

如果报错，需要安装 Graphviz

```
failed to execute dot. Is Graphviz installed? Error: exec: "dot": executable file not found in $PATH

brew install Graphviz

==> Installing graphviz
==> Pouring graphviz-10.0.1.arm64_ventura.bottle.tar.gz
🍺  /opt/homebrew/Cellar/graphviz/10.0.1: 282 files, 7MB
```



