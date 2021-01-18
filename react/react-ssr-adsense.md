---
Title: "react接入google广告: 支持服务端渲染"
Keywords: "react,google adsense,ssr,nextjs,服务端渲染"
Description: "昨天成功将google adsense申请下来了，博客时nextjs服务端渲染框架，使用react-adsense不支持服务端渲染，解决以后出了一个react-ssr-adsense"
Label: "react-ssr-adsense"
Cover: "assert/react-ssr-adsense-cover.png"
---

 @hustcc react-adsense  不支持服务端渲染框架，受 [react-adsense](https://www.npmjs.com/package/react-adsense) 启发写了 react-ssr-adsense

# react-ssr-adsense

react-ssr-adsense is a React-ssr-component for Google AdSense。support [#nextjs](https://nextjs.org/)

[![npm](https://img.shields.io/npm/v/react-ssr-adsense.svg)](https://www.npmjs.com/package/react-ssr-adsense)
[![npm](https://img.shields.io/npm/dm/react-ssr-adsense.svg)](https://www.npmjs.com/package/react-ssr-adsense)
[![npm](https://img.shields.io/npm/l/react-ssr-adsense.svg)](https://www.npmjs.com/package/react-ssr-adsense)

# 1. Install

> **npm install --save react-ssr-adsense**

Before use Google AdSense, you should add the `script` at the end of HTML.

```html
<script async src="//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js"></script>
```


# 2. Usage

```jsx
import React from 'react';
import AdSense from 'react-ssr-adsense';

// ads with no set-up
<AdSense
  client='ca-pub-7292810486004926'
  slot='7806394673'
/>

// ads with custom format
<AdSense
  client='ca-pub-7292810486004926'
  slot='7806394673'
  style={{ width: 500, height: 300, float: 'left' }}
  format=''
/>

// responsive and native ads
<AdSense
  client='ca-pub-7292810486004926'
  slot='7806394673'
  style={{ display: 'block' }}
  layout='in-article'
  format='fluid'
/>

// auto full width responsive ads
<AdSense
  client='ca-pub-7292810486004926'
  slot='7806394673'
  style={{ display: 'block' }}
  format='auto'
  responsive='true'
  layoutKey='-gw-1+2a-9x+5c'
/>
```


# 3. Props

 - Required props:
   - `client`
   - `slot`
 - Optional props:
   - `className`:
   - `style`:
   - `layout`:
   - `layoutKey`:
   - `format`:
   - `responsive`:


# LICENSE

MIT@[douyacun](https://github.com/douyacun).



