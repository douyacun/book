网上可能很多会教你用Adobe Acrobat、福昕风腾PDF去水印、迅捷PDF去水印、PDFelement ...就能直接去掉水印，其实很多时候是去不掉的。

因为它只能去除由其「福昕风腾PDF」、「迅捷PDF」、「PDFelement」软件本身加到PDF文件上水印。

比如：如果这个PDF的水印是用Adobe Acrobat加上的，那么你用Adobe Acrobat的去水印功能，才能把水印去掉。

## ▍一. 声明

本文的目的是帮助恢复那些*自己添加了水印*但是没有*做原始备份的PDF*。

**请不要利用去水印技术来侵犯别人的版权和作者的权利，从不属于你的PDF文件中去除水印可能构成侵权！**

下面阿虚用各种软件的试用版，来教一下如何用各个软件去除各种水印，演示用到的PDF已于之后完全删除。

## ▍二. 删不掉的

比如直接打印在文件上，再翻拍或扫描的，这种靠一般的软件删不掉。

- 当然，你可以把PDF导出为一张张图片后，用PS一张一张去水印
- 这个的前提当然是你PS技术够好
- 自然这个办法花费时间肯定多

## ▍三. 可以删的

这个就很多了，阿虚分几类来讲一下

## ●（1）傻瓜式水印

指水印只在页眉页脚或其他边缘位置



![img](https://pic2.zhimg.com/80/v2-83793820e2328c56f8f0bdabf79e91a9_1440w.webp)

### ▷ ① 解决办法一

使用软件：Adobe Acrobat DC

> [点击下载Acrobat DC 2020](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

解决途径：点击编辑PDF，通过裁切页面的方式，将其去掉

![img](https://pic1.zhimg.com/80/v2-3590928a299e94dac628908851867878_1440w.webp)

### ▷ ② 解决办法二

使用软件：PDF橡皮擦

> [点击下载PDF橡皮擦](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

解决途径1：就像使用橡皮擦一样，直接挨着擦掉水印，最后点击左上角的Save PDF保存即可。

![动图封面](https://pic2.zhimg.com/v2-29d4cd0ca1361c5419abe3669996c009_b.jpg)





解决途径2：先用Delete Area这个功能，框选住每个页面你要删除的部分，最后点击左上角的Save PDF，即可得到一个新的删除水印后的PDF。



![动图封面](https://pic1.zhimg.com/v2-1879d11956033670ae9f229e5d9f7a84_b.jpg)



### ▷ ③ 可能会遇到的问题

点击编辑PDF时，会提示**PDF为加密文档**

![动图封面](https://pic3.zhimg.com/v2-95c7c2e26b9af96b1fdd7baca39e2e72_b.jpg)





解决办法：尝试使用 PDF Password Remover 解密

> [点击下载PDF Password Remover](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

PDF Password Remover一款用来解除受保护的Adobe Acrobat PDF文件的软件。

- 解密速度快，解密后的文件可以用各种PDF阅读器打开，并没有修改打印的任何限制
- 支持所有版本的Adobe Acrobat
- 支持命令行和批处理破解
- PDF Password Remover也可以解除用FileOpen插件加密的文件

## ●（2）第一种稍微复杂点的

指添加在文字版的PDF文档上的水印或背景

这类水印属灰色的最常见，可能也有其他颜色的

但*只要和正文颜色差别明显*，去除起来就不算很难

![img](https://pic4.zhimg.com/80/v2-53fac8dfa821b42994cbe265674915ef_1440w.webp)

▲PDF只做演示作用



### ▷ ① 如果页数不多

使用软件：Adobe Acrobat DC

> [点击下载Acrobat DC 2020](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

操作：如果PDF页数不多，那就直接用Adobe Acrobat DC挨着删除即可。点击编辑PDF，按住Shift键多选中要删除的水印，按Delete键将其去删除。

![动图封面](https://pic3.zhimg.com/v2-a55967f417cdf17eea657bd2b2d08826_b.jpg)





### ▷ ② 如果页数较多（方法一）

使用软件：Adobe Acrobat DC

> [点击下载Acrobat DC 2020](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

如果PDF页数比较多，可以利用Adobe Acrobat DC的分色打印功能，批量删除，但是步骤较多，具体请看下面的详解。

### 步骤一

1. 点击Adobe Acrobat DC左上角「打印」
2. 打印机选择「Adobe PDF」
3. 「高级」设置「Adobe默认值」
4. 颜色改为「分色」
5. 叉掉「黑色」
6. 右边下拉选择「确定」
7. 右边下拉点击「打印」

![动图封面](https://pic2.zhimg.com/v2-112ba068f0b67bec44dfb44fad16e0c5_b.jpg)



### 步骤二

1、用Adobe Acrobat DC打开刚刚打印下来的PDF，你会发现页数变多了，变成了原来的3倍（大概就是原来的每一页都被复制了两份，所以我们要删掉多的）

2、视图——工具——打开「组织页面」

![img](https://pic3.zhimg.com/80/v2-268f87d8ffc7cbfe1b674c87938b5e2a_1440w.webp)



3、选择组织内容，调整每一排显示3个页面，然后删除多余的两排页面



![动图封面](https://pic1.zhimg.com/v2-74ac058c2965240abb3ca8e00466b7bc_b.jpg)





### ▷ ③ 如果页数较多（方法二）

使用软件：Foxit PDF Editor

> [点击下载Foxit PDF Editor](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

根据*水印的类型*（一般都是路径或者图片），在选择类型里面选择水印对应的类型（下面以路径型水印做演示）。

1. 选择对象类型选择为路径
2. 按Ctrl+A全选中页面中所有相同类型的部分
3. 然后按Delete键，一键删除
4. 接着点击下一页，重复Ctrl+A，Delete，下一页，Ctrl+A，Delete.....

手动的话大概4秒钟搞定一页吧，一分钟搞定15页。

![动图封面](https://pic2.zhimg.com/v2-3a4d5860ac57e3c72f5918df8e27c1f5_b.jpg)





### ▷ ④ 如果页数较多（方法三）

使用软件：Foxit PDF Editor + 按键精灵

> [点击下载自动化去水印（按键精灵）](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

还是上面的软件，不过阿虚再提供一个自动化的办法。既然是只有几步的重复性操作，我们可以利用按键精灵，把「Ctrl+A，Delete，点击下一页」这3步录制成一个脚本，然后就能实现自动化删除了。

### 步骤一

先在选择对象处，设置水印对应的类型（这里是水印是路径类型，所以选择路径），然后鼠标放到下一页上面。

![动图封面](https://pic3.zhimg.com/v2-17a75db53a948fb9569d2b49f11130c2_b.jpg)





### 步骤二

阿虚帮大家录制好了一个按键精灵小脚本，大家下载后直接用就行了。

启动按键精灵脚本，鼠标放到下一页上之后，按F10键，就会开始自动「Ctrl+A，Delete+点击下一页」进行水印删除了。*再次按F12，停止脚本。*

![动图封面](https://pic3.zhimg.com/v2-f2ffe496d2340d83bbd8ae82d26134ee_b.jpg)



▲无手动操作，全自动去除水印

## ●（3）第二种稍微简单一点的

某些水印能被某些软件直接检测到，然后就能一键批量删除，比如下面这种

![img](https://pic2.zhimg.com/80/v2-e2a3a45b10facb0195b29bead4ecbe7d_1440w.webp)

▲PDF只做演示作用

### ① 解决办法一

使用软件：PDF Watermark Remover

> [点击下载PDF-Watermark-Remover](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)

解决办法：使用PDF Watermark Remover打开PDF文件，软件会自动检测出疑似水印的图层，*如果能检测出来*则可以用软件批量去除水印。

![动图封面](https://pic1.zhimg.com/v2-7f21249f6bd873f80b55dcec262b3f78_b.jpg)





备注：

有一款免费的软件叫万彩办公大师，内置和PDF Watermark Remover同样的功能（界面都相同，我想你明白的）

万彩办公大师可以在其官网免费下载：[http://www.wofficebox.com/](https://link.zhihu.com/?target=http%3A//www.wofficebox.com/)

![img](https://pic1.zhimg.com/80/v2-a03c8721f8ed9460f666dacbeaf2c214_1440w.webp)



### ② 解决办法二

使用软件：Adobe Acrobat DC + PDF水印清理专家

> [点击下载Acrobat DC 2020](https://link.zhihu.com/?target=https%3A//flowus.cn/share/c53945f6-c7bb-461b-acc9-abca086f0ca4)、[点击下载PDF水印清理专家](https://link.zhihu.com/?target=http%3A//www.liyusoft.com/pdfwr)

### 步骤一

Adobe Acrobat DC用来拆分PDF（因为PDF水印清理专家试用版每个PDF只能免费去除3页水印，所以用Adobe Acrobat DC把原本的PDF按每3页一个，拆分开来）

> 备注：你使用其他PDF编辑软件也是可以的，只要能把PDF拆分成每3页为1个文件。

Adobe Acrobat DC打开PDF后，点击视图—工具—组织页面，把PDF拆分成每3页1个。

![动图封面](https://pic4.zhimg.com/v2-3a7e8c13cf171acd05d49d84853f6293_b.jpg)



示例：原本16页的PDF被分成6个PDF

![img](https://pic2.zhimg.com/80/v2-1f7c49bff682467ca86d388792e67721_1440w.webp)

### 步骤二

打开PDF水印清理专家试用版，勾选移除图片上浅灰色背景水印，点击批量处理有同样水印的其他文件，选中步骤一中拆分开来的6个PDF，然后等待软件去水印。

![img](https://pic3.zhimg.com/80/v2-d4ef1a6f83cdc61bea10aad3b9187786_1440w.webp)

### 步骤三

打开Adobe Acrobat DC，点击工具—合并文件—选中步骤二中去完水印的6个子PDF文件，点击右上角合并，把其合并成一个PDF。

![动图封面](https://pic1.zhimg.com/v2-b8bd52756190e070b46e38c20347f5b4_b.jpg)





### 步骤四

PDF水印清理专家试用版会在每个PDF页面底部添加水印，不过这种水印去除起来实在简单。

![img](https://pic4.zhimg.com/80/v2-bae235829cbe29d5fba57549a1a2b5ff_1440w.webp)



点击编辑PDF，通过裁切页面的方式，即可将其去掉。

> 版权声明：阿虚提供的软件均来源于互联网，请勿用于商业用途。软件下载后仅能作为学习研究目的使用，请您于24小时内自觉将其删除，如果喜欢请购买正版。