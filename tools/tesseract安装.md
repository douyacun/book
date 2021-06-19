---
Title: "centos7 安装 tesseract4.1.1 tesseract5.0"
Keywords: "tesseract,centos7,chinese,中文"
Description:  "tesseract安装教程"
---


# 安装依赖

```shell
yum install autoconf automake libtool pkgconfig.x86_64 libpng12-devel.x86_64 libjpeg-devel libtiff-devel.x86_64 zlib-devel.x86_64 gcc gcc++
```

- Leptonica-1.79

```shell
wget http://www.leptonica.org/source/leptonica-1.79.0.tar.gz
tar -zxvf leptonica-1.79.0.tar.gz
cd leptonica-1.79.0
./configure --prefix=/usr/local/leptonica-1.79.0
make && make install

export PKG_CONFIG_PATH=/usr/local/leptonica-1.79.0/lib/pkgconfig
```

# 安装 tesseract 4.1.1 稳定版本

```shell
wget https://github.com/tesseract-ocr/tesseract/archive/refs/tags/4.1.1.zip

./autogen.sh
./configure --prefix=/usr/local/tesseract-4.1.1
make && make install
```

看下tesseract支持的语言
```
tesseract --list-lang
```

如果报错：
```
Error in pixReadMemJpeg: function not present
Error in pixReadMem: jpeg: no pix returned
Error during processing.

Error in pixReadMemTiff: function not present
Error in pixReadMem: tiff: no pix returned
Error in pixaGenerateFontFromString: pix not made
Error in bmfCreate: font pixa not made
Error opening data file /usr/local/share/tessdata/eng.traineddata
```
说明 jpeg 的依赖库没有安装， 执行:
```
yum install libpng12-devel.x86_64 libjpeg-devel libtiff-devel.x86_64 -y 
```
然后重新编译 Leptonica-1.79

安装训练文件：eng + chi_sim

```shell
wget https://github.com/tesseract-ocr/tessdata_best/blob/master/chi_sim.traineddata /usr/local/share/tessdata
wget https://github.com/tesseract-ocr/tessdata_best/blob/master/eng.traineddata /usr/local/share/tessdata

export TESSDATA_PREFIX=/usr/local/share/tessdata
```

# 安装Tesseract 5.0

```shell
git clone https://github.com/tesseract-ocr/tesseract.git
cd tesseract
./autogen.sh
./configure --prefix=/usr/local/tesseract-5.0 
make && make install
```

训练文件安装步骤 和 4.0 相同


