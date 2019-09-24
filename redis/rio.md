# redis 文件写入

结构体
```c
struct _rio{

    // backend functions
    size_t (*read) (struct _rio *, void *buf, size_t len); // 从流中读取
    size_t (*write) (struct _rio *, void *buf, size_t len); // 写入到流中
    off_t (*tell) (struct _rio *);// 获取当前的偏移量
    
    // 检验和计算函数
    void (*update_cksum)(struct _rio*, const void *buf, size_t len);

    // 校验和
    uint64_t cksum;
    
    // 读写的字节数
    size_t process_bytes;

    // 单词读写的块大小
    size_t max_processing_chunk;

    union {
        struct {
            // 缓存指针
            sds ptr;
            // 偏移量
            off_t pos;
        } buffer;

        struct {
            FILE *fp;// 文件指针
            off_t bufferd;// 最近一次fsync以来写入的字节量
            off_t autosync;// 写入多少字节以后，才会自动执行一次fsync()
        } file;
    }io
};
```