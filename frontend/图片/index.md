---
Title: 图片
LegacyId: b485646770c02d77ae5da9a28f300c0f
Slug: 图片
Category: frontend
Summary: 1. 从本地加载图片 ```python img = QImage("/Users/admin/Desktop/hello.png") pimg
  = QPixmap.fromImage(image) self.imgLabel.setPix
SeoTitle: 图片
SeoDescription: 1. 从本地加载图片 ```python img = QImage("/Users/admin/Desktop/hello.png")
  pimg = QPixmap.fromImage(image) self.imgLabel.setPix
Date: 2023-01-11T16:03:00+08:00
LastEditTime: 2023-01-11T16:03:00+08:00
Publish: true
---

1. 从本地加载图片

```python
img = QImage("/Users/admin/Desktop/hello.png")
pimg = QPixmap.fromImage(image)
self.imgLabel.setPixmap(pimg)
```

2. 图片铺满QLabel

```python
img = QImage("/Users/admin/Desktop/hello.png")
pimg = QPixmap.fromImage(image)
self.imgLabel.setPixmap(pimg)
self.imgLabel.setScaledContents(True)
```

3. 等比例显示未知大小的图片

```python
img = QImage("/Users/admin/Desktop/hello.png")
pimg = QPixmap.fromImage(image)

fw = self.imgLabel.width() / width
fh = self.imgLabel.height() / height

factor = min(fw, fh)

width = int(width * factor)
height = int(height * factor)
nimg = img.scaled(width, height, QtCore.Qt.KeepAspectRatio, QtCore.Qt.SmoothTransformation)
self.imgLabel.setPixmap(nimg)
```
