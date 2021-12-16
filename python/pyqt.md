[toc]

# 准备

qt-designer : https://build-system.fman.io/qt-designer-download

教程：

1. [【PyQt5中文教程】](https://www.bookstack.cn/read/PyQt5-Chinese-tutoral/hello_world.md)
2. https://segmentfault.com/a/1190000038814109

打包：

1. [PyInstaller](https://www.cxyzjd.com/article/lxt610/111169402)

# 设置图标

## mac

mac图标是显示在dock中，mac设置窗口图标就需要设置 `QApplication.setWindowIcon`

```python
·app = QApplication(sys.argv)
path = os.path.join(os.path.dirname(sys.modules[__name__].__file__), 'assert/icon.icns')
app.setWindowIcon(QIcon(path))
```

## windows

暂时没有windows运行

# 窗口位置

主窗口大小：

```python
from PyQt5.QtWidgets import QWidget, QDesktopWidget, QApplication
QDesktopWidget().availableGeometry().center()
```

## 居中

1. 获取桌面中心点
2. 获取当前窗口的图形矩阵（rect）
3. 图形矩阵（rect）移至桌面中心点，获取左边界，右边界
4. 当前窗口移动

```python
qr = self.frameGeometry()
cp = QDesktopWidget().availableGeometry().center()
qr.moveCenter(cp)
self.move(qr.topLeft())
```

# 结束进程

```python
from PyQt5.QtCore import QCoreApplication
from PyQt5.QtWidgets import ( QToolButton )

btn = QPushButton('Button', self)
btn.clicked.connect(QApplication.instance().quit)
```

1. PyInstaller: https://www.cxyzjd.com/article/lxt610/111169402
