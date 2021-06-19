---
Title: "如何识别图片文字格式化导出为word"
Keywords: "ocr,图片文字识别,导出word,导出docx,image ocr"
Description: "识别图片上的文字，格式化导出word"
Cover: "./assert/免费文字识别.png"
---

网站地址：[https://www.douyacun.com/image/ocr-to-docx](https://www.douyacun.com/image/ocr-to-docx)

**功能特点：**

1. 识别图片上的文字
2. 按照图片文字样式进行格式化
3. 导出为word文档
4. 2算法实现：googel-tesseract-ocr / easyocr
5. 借助开源库实现，功能免费

**背景：**

几天前媳妇给了我一张图片，让我把图片上的内容导出来为可以编辑，识别图片文字的软件有挺多的了，不过能相对格式化的不多，接这次东风就顺便了解了一下文字识别（OCR）相关的技术，然后就自己实现了一下，顺便能格式化一下，经过一段时间摸底之后找到了3种比较靠谱的方式

最先接触到的第一个开源工具Tesseract，大家说的比较多是Tesseract对于中文不是很友好，识别率不高。在我多次调试配置之下发现他们都是没有用心学习Tessract，实践下来发现：中文识别率较高，中文标点准确。cpu识别速度可以接受。

第二个了解到的工具请教了一下搞算法的同学，他告诉我用百度云的接口，中文识别率较高，速度快，API可享每天1000次免费调用，对于我来说尝鲜足够了。要是做成一个功能就比较费钱了。

第三个工具是在逛Github途中发现的，easyocr，尝试下来总结：中文识别  率较高，中文符号识别率不高，有人提过issue，作者也在3.0.1的版本进行了修复，实验了几次还是不准。CPU速度较慢，没有GPU所以没有尝试GPU的速度如何。

测试用例1:

![douyacun-tesseract-测试](./assert/tesseract-demo1.png)

Tesseract-or识别结果：

![douyacun-免费文字识别](./assert/tesseract-demo1-result.png)

相应的代码实现在github：https://github.com/douyacun/ocr-to-docx
