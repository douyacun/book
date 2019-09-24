# 电脑重装成centos系统

## 下载iso镜像

[镜像下载](http://centos.01link.hk/)

```
[DIR] Parent Directory                                             -   
[TXT] 0_README.txt                            10-May-2018 04:16  2.4K  
[   ] CentOS-7-x86_64-DVD-1804.iso            04-May-2018 05:07  4.2G  
[   ] CentOS-7-x86_64-DVD-1804.torrent        11-May-2018 23:43   84K  
[   ] CentOS-7-x86_64-Everything-1804.iso     07-May-2018 20:55  8.8G  
[   ] CentOS-7-x86_64-Everything-1804.torrent 11-May-2018 23:43  176K  
[   ] CentOS-7-x86_64-LiveGNOME-1804.iso      03-May-2018 02:21  1.3G  
[   ] CentOS-7-x86_64-LiveGNOME-1804.torrent  11-May-2018 23:43   52K  
[   ] CentOS-7-x86_64-LiveKDE-1804.iso        03-May-2018 02:28  1.8G  
[   ] CentOS-7-x86_64-LiveKDE-1804.torrent    11-May-2018 23:43   71K  
[   ] CentOS-7-x86_64-Minimal-1804.iso        04-May-2018 05:07  906M  
[   ] CentOS-7-x86_64-Minimal-1804.torrent    11-May-2018 23:43   36K  
[   ] CentOS-7-x86_64-NetInstall-1804.iso     04-May-2018 04:34  495M  
[   ] CentOS-7-x86_64-NetInstall-1804.torrent 11-May-2018 23:43   20K     >>>>>>>>>
[TXT] sha1sum.txt                             10-May-2018 04:02  454   
[TXT] sha1sum.txt.asc                         11-May-2018 23:12  1.3K  
[TXT] sha256sum.txt                           10-May-2018 03:59  598   
[TXT] sha256sum.txt.asc                       11-May-2018 23:12  1.4K  
```
> 选择的是 上面标记的 网络安装版本 >>>>>>>>> ,一些基础软件没有, 需要联网下载

## 制作centos系统盘 (mac os)

----

1. 把ISO转成DMG，命令中的-o后面的linux是目标文件名，按自己需求改

    ```
    # hdiutil convert -format UDRW -o 输出 输入文件.iso
    # hdiutil convert /Users/os/Downloads/ylmf.iso -format UDRW -o ylmf.img    // windows
    ```


2. .把U盘插上，查看盘符

    ```
    # diskutil list
    ```

3. 卸载U盘，把命令里的disk2改成你自己的盘符，还要注意卸载不是弹出！

    ```
    # diskutil unmountDisk /dev/disk2
    ```

4. 开始把光盘写入到u盘

    ```
    # sudo dd if=linux.dmg of=/dev/disk2 bs=1m

    之后终端里就不动了，看着没反应了，其实正在后台运行呢，等着就行，写入完成以后终端有提示，然后再弹出U盘就可以正常使用了。

    408+0 records in
    408+0 records out
    427819008 bytes transferred in 3.608223 secs (118567786 bytes/sec)
    ```

## 装机 安装选择

- f12 进入bios
```
Boot Mode  : Legacy Support
Boot Priority : Legacy First
```
- 重启 f12 选 use 启动

- 设置启动盘, 配置目录google或baidu搜吧!

- 上面的关键点来了, 因为选择的netinstall, 需要设置安装源, 下面的源可能会失效 (https://opsx.alibaba.com/mirror) 自己选吧

- https://mirrors.aliyun.com/centos/7.5.1804/os/x86_64/

- 选择最小化安装就行


# 扩展
- linux 挂载FAT32系统
    ```
    # mount -t vfat /dev/sdb1 /package
    ```