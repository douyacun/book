---
Title: "pyqt5 mac打包签名公证"
Keywords: "pyqt5,mac,打包,pyinstaller,签名,公证,证书"
Description: "解决安装时提示：安装包已损坏，无法打开 或 来自不明开发者的解决办法"
Label: "pyqt5签名公证"
---

mac签名打包的坑比较深：有需求的朋友可以加微信  私聊



## app打包

python打包工具用的pyinstaller

```shell
bundle="com.douyacun.pdftools"
appname="PDF工具箱"
echo ">>>> 清理"
sudo rm -rf dist build
echo ">>>> 打包"
sudo pyinstaller \
  --windowed \
  --onedir \
  --icon=assert/icon.icns \
  --osx-bundle-identifier=${bundle} \
  main.py \
  --name ${appname}
sudo pyinstaller --clean --noconfirm $appname.spec
```

介绍一下打包参数：

--onedir 打包成目录，启动速度明显提示。最后可以打包成dmg

--osx-bundle-identifier 指定应用包名，签名需要用到



## app 签名

 [https://idmsa.apple.com/IDMSWebAuth/signin](https://idmsa.apple.com/IDMSWebAuth/signin)

签名证书的申请流程参考知乎：[https://zhuanlan.zhihu.com/p/431634112](https://zhuanlan.zhihu.com/p/431634112)

```shell
#echo ">>>> 签名修复"
sudo python for_codesign.py dist/PDF工具箱.app

#echo ">>>> 签名"
sudo codesign -s "Developer ID Application: * Enterprise Management Co., Ltd (P*)" -f --entitlements entitlements.plist --timestamp --all-architectures  --deep --options=runtime ～/Documents/github.com/pdf-tools/dist/PDF工具箱.app
```

签名脚本报错：

```
bundle format unrecognized, invalid, or unsuitabl

Contents/MacOS/PyQt5/Qt5/qml/QtQml/Models.2
```

这个错误就是因为codesign不支持目录名带 点 `Models.2`

解决方案就是：将 `Models.2`  改成 `Models_2` , 然后再将 `Models_2` 创建软链到 `Models.2`



pyintaller 给出的解决方案是将 `Models.2` 转移到 `/Contents/Resources/` 下，然后软链过来

https://github.com/pyinstaller/pyinstaller/wiki/Recipe-OSX-Code-Signing-Qt

此方案已经不通了，mac公证时要求所有用到的文件都需要签名：

```
The signature of the binary is invalid.
```



签名修复脚本：

```python
# -*- coding: utf-8 -*-
import os
import shutil
import sys
from pathlib import Path
from typing import Generator, List, Optional

from macholib.MachO import MachO


def find_problematic_folders(folder: Path) -> Generator[Path, None, None]:
    for path in folder.iterdir():
        if not path.is_dir() or path.is_symlink():
            # Skip simlinks as they are allowed (even with a dot)
            continue
        if "." in path.name:
            yield path
        else:
            yield from find_problematic_folders(path)


def main(args: List[str]) -> int:
    """
    修复mac签名
    - 重命名.2目录为 _2
    - 创建软链
    """
    for app in args:
        name = os.path.basename(app)
        print(f">>> [{name}] Fixing Qt folder names")
        path = Path(os.path.abspath(app)) / "Contents" / "MacOS"
        for folder in find_problematic_folders(path):
            dest = Path(str(folder).replace(".2", "_2"))
            shutil.move(folder, dest)
            os.symlink(dest, folder)
            print(f" !! Fixed {folder}")


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

```



## app打包dmg

我是用的dmg canvas

https://www.jianshu.com/p/f07a8351a14f



## dmg公证

 [https://appleid.apple.com/sign-in](https://appleid.apple.com/sign-in)

需要对应用创建专用密码

![image-20230111232411436](assert/image-20230111232411436.png)

```
# 上传app
xcrun altool --notarize-app \
             --primary-bundle-id="com.douyacun.pdftools" \
             --username="账号" \
             --password="APP专用密码" \
             -t="osx" \
             --asc-provider="密钥证书编号" \
             --file "pdf工具箱.dmg"
# 查看公正结果
 xcrun altool --notarization-info "1669889c-3c11-45f3-ad0a-525e3565bdaf" -u "gqjk2021@163.com" --password "app专用密码"
```



No errors getting notarization info.

       Date: 2023-01-11 15:14:44 +0000
       Hash: 904cbbba016a86cf32cdfed8f3762da219d3f0230e7f4d5ac975264d8b34f319
       RequestUUID: 1669889c-3c11-45f3-ad0a-525e3565bdaf
       Status: in progress

