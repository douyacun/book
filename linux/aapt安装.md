---
Title: apktool、aapt安装
Keywords: aapt安装,apktool安装
Description: 
Cover: 
Label: 
Date: 2019-05-12 11:23:44
LastEditTime: 2019-11-29 11:53:05
---
# apktool安装

首先安装java环境
```shell
yum search java
yum install java-1.8.0-openjdk.i686
```

[安装方式](https://ibotpeaches.github.io/Apktool/install/)


# 解压aapt
```c
unzip apktool.jar -d apktool

[root@computer_name apktool]# ll
total 48
drwxr-xr-x 4 root root 4096 Aug  8 09:48 android
-rw-r--r-- 1 root root   25 Aug  7 11:04 baksmali.properties
drwxr-xr-x 7 root root 4096 Aug  8 09:48 brut
drwxr-xr-x 3 root root 4096 Aug  8 09:48 com
drwxr-xr-x 3 root root 4096 Aug  8 09:48 javax
-rw-r--r-- 1 root root 1509 Aug  7 11:04 LICENSE.txt
drwxr-xr-x 4 root root 4096 Aug  8 09:48 META-INF
drwxr-xr-x 8 root root 4096 Aug  8 09:48 org
drwxr-xr-x 3 root root 4096 Aug  8 09:48 prebuilt
drwxr-xr-x 2 root root 4096 Aug  8 09:48 properties
-rw-r--r-- 1 root root   25 Aug  7 11:04 smali.properties
drwxr-xr-x 2 root root 4096 Aug  8 09:48 templates
-rw-r--r-- 1 root root    0 Aug  7 11:04 XPP3_1.1.4c_VERSION
[root@computer_name apktool]# cd prebuilt/
[root@computer_name prebuilt]# ll
total 4
drwxr-xr-x 5 root root 4096 Aug  8 09:48 aapt
[root@computer_name prebuilt]# cd aapt
[root@computer_name aapt]# ll
total 12
drwxr-xr-x 2 root root 4096 Aug  8 09:48 linux
drwxr-xr-x 2 root root 4096 Aug  8 09:48 macosx
drwxr-xr-x 2 root root 4096 Aug  8 09:48 windows
[root@computer_name aapt]# cd linux/
[root@computer_name linux]# ll
total 1924
-rw-r--r-- 1 root root 1967712 Aug  7 11:04 aapt
```

全局变量
```
cp aapt /usr/bin
```