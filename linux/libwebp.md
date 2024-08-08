---
Title: 图片转webp
Keywords: webp,cwebp,png转webp,jpg转webp,jpeg转webp
Description: png转webp,jpg转webp,jpeg转webp
Cover: assert/webp-cover.jpeg
Label: 图片转webp
Date: 2019-12-01 20:10:05
LastEditTime: 2020-01-21 17:01:28
---

# 安装

**mac**

mac直接使用brew安装就好，brew请求的github，慢的话可以ss开启全局模式
```shell
$ brew install webp
```

**centos7**
因为是png、gif、jpeg转webp，所以这里需要libpng,libjpeg,giflib

libpng,libjpeg
```shell
sudo yum install libjpeg-turbo-devel libjpeg-turbo libpng-devel -y
```

webp还需要依赖LibTIFF, [http://download.osgeo.org/libtiff/](http://download.osgeo.org/libtiff/)这里可以下载最新的安装包
```shell
$ cd /opt
$ wget http://download.osgeo.org/libtiff/tiff-4.1.0.tar.gz
$ tar -zxvf tiff-4.1.0.tar.gz && cd tiff-4.1.0
$ ./configure
$ make && make install 
```

giflib安装, 这里可以获取最新的giflib [https://sourceforge.net/projects/giflib/files/latest/download](https://sourceforge.net/projects/giflib/files/latest/download)
```shell
$ cd /opt
$ wget https://nchc.dl.sourceforge.net/project/giflib/giflib-5.2.1.tar.gz
$ tar -zxvf giflib-5.2.1.tar.gz
$ cd giflib-5.2.1
$ make && make install
```

libwebp安装, 从github获取即可 [http://www.linuxfromscratch.org/blfs/view/svn/general/libwebp.html](http://www.linuxfromscratch.org/blfs/view/svn/general/libwebp.html)
```shell
$ cd /opt
$ wget https://github.com/webmproject/libwebp/archive/v1.0.3.tar.gz
$ tar -zxvf v1.0.3.tar.gz
$ cd v1.0.3
$ ./configure --prefix=/usr           \
            --enable-libwebpmux     \
            --enable-libwebpdemux   \
            --enable-libwebpdecoder \
            --enable-libwebpextras  \
            --enable-swap-16bit-csp \
            --disable-static
$ make && make install
```

安装完libwebp之后，执行dwebp或者cwebp命令报 dwebp: error while loading shared libraries: libwebpdemux.so.2: cannot open shared object file: No such file or directory 错误的解决办法：
```shell
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/lib
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/lib" >> ~/.bashrc
```

添加环境变量： 

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
echo $LD_LIBRARY_PATH
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib" >> ~/.bashrc



# libwebp提供的功能

**cwebp**

compresses an image using the WebP format.

**dwebp**

decompresses WebP files into PNG, PAM, PPM or PGM images.

**gif2webp**

converts a GIF image to a WebP image.

**vwebp**

decompress a WebP file and display it in a window.

**webpmux**

creates animated WebP files from non-animated WebP images, extracts frames from animated WebP images, and manages XMP/EXIF metadata and ICC profile.

**libwebp.so**

contains the API functions for WebP encoding and decoding.


这里主要是用到cwebp的功能
```shell
$ cwebp
Usage:

   cwebp [options] -q quality input.png -o output.webp

where quality is between 0 (poor) to 100 (very good).
Typical value is around 80.
```

我这里测试了一下quality 10 80 100的效果，当前使用webp的主要目的是为了减小图片的大小节省流量。



## quality 10
**stack.png**
![](assert/stack.10.webp)

**stack.10.webp**

```shell
$ cwebp -q 10 stack.png -o stack.10.webp
cwebp -q 10 a.png -o a.10.webp
cwebp -q 8 a.png -o a.8.webp
cwebp -q 5 a.png -o a.5.webp
$ Desktop ll stack.*
-rw-r--r--  1 douyacun  staff   8.3K 12  1 23:09 stack.10.webp
-rw-r--r--@ 1 douyacun  staff   127K 12  1 23:08 stack.png
```
可以看到减少了118.7k, 然后在看下效果:
![](assert/stack.png)


可以看到10的这个效果也是非常吃惊，像一些清晰度要求不高的场景使用webp绝对超值

**stack.80.webp**
```shell
$ cwebp -q 80 stack.png -o stack.80.webp
Saving file 'stack.80.webp'

$ ll stack.*
-rw-r--r--@ 1 douyacun  staff   8.3K 12  1 23:09 stack.10.webp
-rw-r--r--@ 1 douyacun  staff    19K 12  1 23:15 stack.80.webp
-rw-r--r--@ 1 douyacun  staff   127K 12  1 23:08 stack.png
```

![](assert/stack.80.webp)

quality 10和80的区别也只是一些颜色上的区别，没有出现锯齿的效果，而且80也是只有19k

**stack.100.webp**

```shell
$ cwebp -q 100 stack.png -o stack.100.webp
Saving file 'stack.100.webp'
-rw-r--r--@ 1 douyacun  staff   8.3K 12  1 23:09 stack.10.webp
-rw-r--r--@ 1 douyacun  staff    36K 12  1 23:20 stack.100.webp
-rw-r--r--@ 1 douyacun  staff    19K 12  1 23:15 stack.80.webp
-rw-r--r--@ 1 douyacun  staff   127K 12  1 23:08 stack.png
```

100压缩有36K
![](assert/stack.100.webp)