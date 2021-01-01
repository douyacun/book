---
Title: react移动边框
Keywords: react,移动边框,ew-resize
Description: react实现拖拽移动边框思路
Lable: ew-resize
Date: 2020-12-28 11:02:00
LastEditTime: 2020-12-28 11:02:00
---

[TOC]

# 左右布局

定义父框`root`  左视图 `left`，点击移动区 `resizer`，右试图 `right`，建议使用flex布局

![image-20201228233232732](./assert/image-20201228233232732.png)

```css
root: {
  	height: "100%",
  	width: "100%",
}
left: {
    width: "70%"
},
resizer: {
  	userSelect: "none",
    display: "flex",
    backgroundColor: "#fafbfd",
    width: 14,
    cursor: "ew-resize",
    alignItems: "center",
    justifyContent: "center",
    border: "0.5px solid rgba(0, 0, 0, 0.07)",
},
right: {
  	width: "30%",
}
```

```html
<div className={classes.root} onMouseMove={mouseMoveHandler}>
  <div className={classes.left} style={{ width: leftWidth + "%", userSelect: userSelect }} ref={leftRef}>
		左
  </div>
  <div className={classes.resizer} onMouseDown={mousedownHandler}>︙</div>
  <div className={classes.right} style={{ width: rightWidth + "%" }}>
		右
  </div>
</div>
```

`resizer` 鼠标悬浮时一定是这个样式  <--> 的

如何实现左右移动呢？

1. 只有在`resizer` 区域内鼠标左键按下时，才可以左右移动
2. 鼠标左键抬起时，结束左右移动
3. 左视图宽度，鼠标移动时： [left.width = e.clientX / screen.width * 100] 鼠标x轴 -> 浏览器左边 的距离/窗口的宽度  * 100% 就是 `left` 的百分比
4. 右视图宽度，[right.width = 100 - left.width - resizer.width/screen.width] 窗口宽度 - 左视图宽度 - 移动区宽度 = 右视图宽度



### resizer 订阅 `mouseDown`

在resizer按下鼠标时，才可以左右移动，加锁:

```react
const [canMove, setCanMove] = React.useState(false) // 聊天框：是否移动
```

 `mouseDown`时，容易选中左右的文字体验感很差, 避免鼠标按下时选中文字，加锁：

```react
const [userSelect, setUserSelect] = React.useState("text")
```

处理函数:

```react
// 聊天框：开启移动
const mousedownHandler = (e) => {
  setCanMove(true)
  setUserSelect("none");
}
```

### document 订阅 `mouseup`

只能在document上订阅，不能确定最后鼠标的位置是在哪个元素上

```react
// 订阅聊天框宽度
React.useEffect(() => {
  document.addEventListener("mouseup", mouseUpHandler);
  return () => {
    document.removeEventListener("mouseup", mouseUpHandler);
  }
}, [])
```

首先`React.useEffect`,第二个参数空代表，不管state如何变化，此函数只会加载一次，类似init state。返回闭包组件卸载时执行移除document.onMouseUp的订阅

鼠标抬起后本次调整结束，持久化当前边框位置，下次再进来就不需要重新调整了:

```react
// 聊天框：结束移动
const mouseUpHandler = () => {
  setCanMove(false)
  setUserSelect("text");
  // 持久化聊天宽度
  localStorage.setItem(DiscussLeftWidth, leftWidth);
  localStorage.setItem(DiscussRightWidth, rightWidth);
}
```

### root 订阅 `mousemove`

首先获取窗口的宽度:

```react
const [screenWidth, setScreenWidth] = React.useState(100); // 聊天框：当前窗口宽度，计算左右比例

React.useEffect(() => {
 	 setScreenWidth(document.body.clientWidth);
}, [])
```

鼠标在`root` 元素内移动时，计算左右边框的百分比，为了避免鼠标移动重载次数过多导致的卡顿，加锁：

```react
const [lockResize, setLockResize] = React.useState(false); // 聊天框：加载锁，不要设置太快了
```

root 订阅鼠标移动

```react
const mouseMoveHandler = (e) => {
  if (canMove) {
    let lw = (e.clientX / screenWidth * 100).toFixed(3)
    let rw = (100 - lw - 1).toFixed(3);
    if (!lockResize && lw > 50 && (screenWidth - e.clientX) > 300) {
      setLeftWidth(lw)
      setRightWidth(rw)
      setLockResize(true)
      // 10毫秒以后释放该锁
      setTimeout(() => { setLockResize(false) }, 10)
    }
  }
}
```

# 滚动条样式

默认的滚动条样式时很粗的，中间加了 `resizer`以后就太丑了，滚动条必须瘦下来才行：

```css
left: {
  overflowY: "scroll",
  padding: 10,
  "&::-webkit-scrollbar": {
    width: 4,
  	// height: 16 只能设置宽度，长度浏览器会自动根据div长度调整
  },
  "&::-webkit-scrollbar-thumb": {
    backgroundColor: "rgba(105, 112, 125, 0.5)" // 滚动条背景色
  }
},
```

- `::-webkit-scrollbar` — 整个滚动条.
- `::-webkit-scrollbar-button` — 滚动条上的按钮 (上下箭头).
- `::-webkit-scrollbar-thumb` — 滚动条上的滚动滑块.
- `::-webkit-scrollbar-track` — 滚动条轨道.
- `::-webkit-scrollbar-track-piece` — 滚动条没有滑块的轨道部分.
- `::-webkit-scrollbar-corner` — 当同时有垂直滚动条和水平滚动条时交汇的部分.
- `::-webkit-resizer` — 某些元素的corner部分的部分样式(例:textarea的可拖动按钮).

![image-20201229003702633](./assert/image-20201229003702633.png)























































































