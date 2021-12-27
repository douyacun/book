---
Title: "pyqt5 windows/mac打包教程"
Keywords: pyqt5,pyinstall,innosetup,dmgcanvas,qtwebengineview
Description: 如何打包windows/mac桌面安装程序，如果用到qtwebengineview又该如何处理
Lable: pyqt5打包
---

背景：

本文是已经假设你已经可以运行在本地运行软件了，只是受困于不知道如何将软件打包成dmg/exe安装包

[toc]

# pyinstaller打包软件

为啥用pyinstaller，因为一搜"pyqt5打包", 都是pyinstaller如何打包的教程，这里贴一下参数下面会用到，如果需要更详细的教程可以多搜搜。

```
-F, –onefile 打包成一个exe文件。
-D, –onedir 创建一个目录，包含exe文件，但会依赖很多文件（默认选项）。
-c, –console, –nowindowed 使用控制台，无界面(默认)
-w, –windowed, –noconsole 使用窗口，无控制台
--paths, 一同打包此目录下的文件
--icon, icon
```

建议大家使用 -D（多目录的方式）来打包软件，如果项目比较大或者用到Qt Web Engineview的话，你就会发现打包出来的软件每次启动至少20s以上～，多目录发布就要用到dmg或者set up 安装程序。

贴一下 PDF工具箱的打包命令

**mac**

```
	sudo pyinstaller \
		--windowed \
		--onedir \
		--icon=assert/icon.icns \
		--clean \
		--noconfirm \
		--osx-bundle-identifier=org.qt-project.Qt.QtWebEngineCore \
		--paths=.venv/lib/python3.8/site-packages/cv2 \
		--paths=.venv/lib/python3.8/site-packages/PyQt5/Qt5/lib/QtWebEngineCore.framework/Resources \
		main.py \
		--name pdf工具箱
```



**windows:**

```
ROOT=$(bash cd "$(dirname "$0")"; pwd)

pyinstaller \
  --windowed \
  -D \
  --icon=${ROOT}/assert/icon.ico \
  --paths=${ROOT}/.venv/Lib/site-packages/cv2 \
  --name pdf工具箱 \
  main.py &&
  cp ${ROOT}/.venv/Lib/site-packages/PyQt5/Qt5/resources/* ${ROOT}/dist/pdf工具箱/ &&
  cp ${ROOT}/.venv/Lib/site-packages/PyQt5/Qt5/bin/QtWebEngineProcess.exe ${ROOT}/dist/pdf工具箱/
```

这里cp等价于上面的--path



# Qt Web Engineview如何打包

官方文档：[https://doc.qt.io/qt-5/qtwebengine-deploying.html](https://doc.qt.io/qt-5/qtwebengine-deploying.html)

贴一下文档上关键部分: 

```
Deploying Resources
Qt WebEngine requires the following resource files:

qtwebengine_resources.pak contains the resources needed by Chromium.
qtwebengine_devtools_resources.pak contains tools for remote debugging.
qtwebengine_resources_100p.pak contains images suitable for low resolution displays.
qtwebengine_resources_200p.pak contains images suitable for high DPI displays.
icudtl.dat provides support for International Components for Unicode (ICU). It is the Chromium version of ICU, which is not needed if Qt WebEngine was configured to use the system ICU.
Resources are searched from the following locations:

On Linux and Windows: the resources directory in the directory specified by QLibraryInfo::location(QLibraryInfo::DataPath)
On macOS: .app/Content/Resources
```







# mac app 打包dmg - dmg canvas



# windows exe 打包为set up 安装引导程序  - innosetup

 





1. 
