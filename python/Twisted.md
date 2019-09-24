# 国内镜像源:
```
http://pypi.douban.com/ 豆瓣
http://pypi.hustunique.com/ 华中理工大学
http://pypi.sdutlinux.org/ 山东理工大学
http://pypi.mirrors.ustc.edu.cn/ 中国科学技术大学
http://mirrors.aliyun.com/pypi/simple/ 阿里云
https://pypi.tuna.tsinghua.edu.cn/simple/ 清华大学
```

# 设置pip源
```
[global]
trusted-host =  mirrors.aliyun.com
index-url = http://mirrors.aliyun.com/pypi/simple
```

# pip 手动指定某个源下载
```
pip -i http://pypi.douban.com/simple install dnspython
```

# 安装 twisted
```
wget https://twistedmatrix.com/Releases/Twisted/18.7/Twisted-18.7.0.tar.bz2

yum install bzip2 -y
tar jxvf Twisted-18.7.0.tar.bz2
python setup.py install
```

#