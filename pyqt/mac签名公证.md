---
Title: "pyqt5 mac打包签名公证"
Keywords: "pyqt5,mac,打包,pyinstaller,签名,公证,证书"
Description: "解决安装时提示：安装包已损坏，无法打开 或 来自不明开发者的解决办法"
Label: "pyqt5签名公证"
---

## app签名

 [https://idmsa.apple.com/IDMSWebAuth/signin](https://idmsa.apple.com/IDMSWebAuth/signin)

签名证书的申请流程参考知乎：[https://zhuanlan.zhihu.com/p/431634112](https://zhuanlan.zhihu.com/p/431634112)







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


