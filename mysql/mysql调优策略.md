# 磁盘io
- 使用ssd硬盘
- 使用磁盘阵列存储，建议阵列同时配备cache和BBU模块,可明显提升IOPS
- raid，建议使用raid10，不要使用raid5

# 文件系统层优化
- 使用deadline/noop这两种I/O调度器
- 使用xfs文件系统
- 文件系统参数中增加：noatime,nodiratime,nobarrier几个选项（不同修改时间）

# 内核参数优化
- vm.swappiness参数，降低swap使用率，建议5-10
- vm.dirty_background_ratio/vm.dirty_ratio 确保能持续将脏数据刷新到磁盘，避免瞬间i/o写，产生等待
- 调整net.ipv4.tcp_tw_recycle/net.ipv4.tcp_tw_reuse都设置为1，减少time_wait,提高tcp效率

# mysql参数优化
- default-storage-engine=innodb
- innodb_buffer_pool_size, 可以设置为物理内存的50%-70%
- innodb_file_per_table = 1 独立表空间
- innodb_data_file_path = ibdata1:1G:autoextend 数据文件路径 1G: 初始化大小 autoextend：超过1G时自动扩展，不要使用默认10M，否则在有高并发事务时，会受到不小的影响；
- innodb_log_file_size = 256M innodb_log_file_in_group=2
- max_connection（最大链接数）、max_connection_error（最大错误数）根据业务量大小进行设置
- open_files_limit、innodb_open_fiels、table_open_cache
- key_buffer_size 32M左右，关闭query cache
