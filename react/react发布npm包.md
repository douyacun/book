---
Title: "npm publish: react component/react hook component"
Keywords: "npm publish,react component,react hook component,发布,publish,package,libraryTarget用法,Minified React error #321"
Description: "为了发布一个react npm package，在此默默的抹了2把泪～"
Label: "npm package"
---


# npm publish: react component/react hook component

 [![npm](https://img.shields.io/npm/v/npm-publish-react-component)](https://www.npmjs.com/package/npm-publish-react-componentr) [![npm](https://img.shields.io/npm/l/npm-publish-react-component)](https://www.npmjs.com/package/npm-publish-react-component) ![react supported](https://img.shields.io/badge/React-%5E16.8.0-blue.svg)

已经成功发布 [react 天气组件](https://www.npmjs.com/package/react-tencent-weather) , 以及  [npm publis react demo ](https://www.npmjs.com/package/npm-publish-react-component) 简单实例，此文就是基于此demo

完整代码已经发到 Github:  [https://github.com/douyacun/npm-publish-react-component](https://github.com/douyacun/npm-publish-react-component) 

误区：

1. npm 发布的react包是源码还是编译后的代码？ 编译后的代码
2. npm 如何在本地测试已经打包的代码？npm link, 这个后续详细讲
3. npm 打包以后 css 文件如何处理？webpack配置，js文件和css文件打包到一个文件
4. npm 发布的react包如何支持服务端渲染？css单独打包，import

概要：

1. package.json 配置
2. webpack 配置, 支持服务端渲染
3. [npm](https://www.npmjs.com/package/npm-publish-react-component)  账号，npm link，npm publish，
4. ISSUE

体验:

```bash
git clone https://github.com/douyacun/npm-publish-react-component.git
npm install
npm start
```

目录：

```
.
├── LICENSE
├── babel.config.json
├── example
│   ├── index.html
│   ├── index.js
│   └── webpack.config.js
├── lib
│   ├── index.js
│   └── index.js.LICENSE.txt
├── package.json
├── src
│   ├── index.css
│   └── index.js
└── webpack.config.js
```



# package.json 配置

1. git init初始化，指定origin仓库，接下来npm会帮忙初始化号git相关的字段

2. `npm init` 初始化 package.json，里面的信息都填写一下，这是搜到这个包的关键，尤其是是keywords

3. 安装一下react打包需要的插件, react/babel/webpack

```
npm install react react-dom --save
npm install @babel/core @babel/preset-env @babel/preset-react babel-loader --save-dev
npm install webpack webpack-cli webpack-dev-server --save-dev
```

4. bable配置文件 `babel.config.json`

```json
{
  "presets": [
    "@babel/preset-env",
    "@babel/preset-react"
  ]
}
```

完整 的 `package.json` 

```json
{
  "name": "npm-publish-react-component", // 重要! npm install <?> 包名，全局唯一
  "version": "1.0.0", // 版本号，每次发布累加一
  "description": "npm发包流程", // seo 三剑客
  "main": "lib/index.js",// 重要！import Demo from "npm-publish-react-component" 就是引入main指定的文件～
  "keywords": [
    "npm publish",
    "npm发布",
    "react组件",
    "react component"
  ],// seo 三剑客
  "scripts": {
    "start": "webpack serve --config example/webpack.config.js",
    "build": "webpack"
  },
  "author": "douyacun",
  "license": "MIT",
  "dependencies": {
    "react": "^17.0.1",
    "react-dom": "^17.0.1"
  },
  "devDependencies": {
    "@babel/core": "^7.12.10",
    "@babel/preset-env": "^7.12.11",
    "@babel/preset-react": "^7.12.10",
    "babel-loader": "^8.2.2",
    "css-loader": "^5.0.1",
    "html-webpack-plugin": "^4.5.1",
    "style-loader": "^2.0.0",
    "webpack": "^5.11.1",
    "webpack-cli": "^4.3.1",
    "webpack-dev-server": "^3.11.1"
  },
  "directories": {
    "example": "example",
    "lib": "lib"
  },
  "repository": {
    "type": "git",
    "url": "git+https://github.com/douyacun/npm-publish-react-component.git"
  },
  "bugs": {
    "url": "https://github.com/douyacun/npm-publish-react-component/issues"
  },
  "homepage": "https://github.com/douyacun/npm-publish-react-component#readme"
}

```

- dependencies 会在npm install <your package>时自动安装
- devDependencies 本地安装时需要用到，打包用的肯定放在devDependencies里面
- peerDependencies 重要！我是用 react hook 写的组件，是react 16.8.4里提供的功能，这里react必须要大于16.8.4的版本

# webpack 配置

1. 配置入口文件 

```
entry: './src/index.js', // 打包哪个文件
```

2. 配置输出目录，文件名，这个要对应到 `package.json`  **main** 文件

```
output: {
    path: path.join(__dirname, '../lib/'), // 输出到哪
    filename: 'index.js', // 输出的文件名字
    libraryTarget: 'umd', // 重要！
}
"main": "lib/index.js"
```

3. `libraryTarget` :  很重要，意味着我们打包出来的js文件可以在什么环境使用～ <a name="libraryTarget"></a>

这里罗列一下各个选项的含义：

- var [默认]:  分配给一个变量值

```js
var MyLibrary = _entry_return_;
// 在一个单独的 script……
MyLibrary.doSomething();
```

- this： 分配给this变量

```
this["MyLibrary"] = _entry_return_;

// 在一个单独的 script……
this.MyLibrary.doSomething();
MyLibrary.doSomething(); // 如果 this 是 window
```

- window： 当 library 加载完成，入口起点的返回值将分配给 window 对象。

```js
window["MyLibrary"] = _entry_return_;

window.MyLibrary.doSomething();
```

- golbal: 分配给 `global` 对象的这个属性下。一般用在node环境中

```js
global["MyLibrary"] = _entry_return_;

global.MyLibrary.doSomething();
```

- commonjs： 分配给 exports 对象。这个名称也意味着，模块用于 CommonJS 环境：这是我们需要的～～～

```js
exports["MyLibrary"] = _entry_return_;

require("MyLibrary").doSomething();
```

- amd：必须是支持define/require的环境引入

```js
define([], function() {
  return _entry_return_; // 此模块返回值，是入口 chunk 返回的值
});
```

- umd：混编，支持 commonJs/global/amd的环境: 这是我们需要的～～～

```
(function webpackUniversalModuleDefinition(root, factory) {
  if(typeof exports === 'object' && typeof module === 'object')
    module.exports = factory();
  else if(typeof define === 'function' && define.amd)
    define([], factory);
  else {
    var a = factory();
    for(var i in a) (typeof exports === 'object' ? exports : root)[i] = a[i];
  }
})(typeof self !== 'undefined' ? self : this, function() {
  return _entry_return_; // 此模块返回值，是入口 chunk 返回的值
});
```

4. react/css 编译规则

完整 `webpack.config.json`

```js
const path = require('path');

module.exports = {
    entry: './src/index.js', // 打包哪个文件
    output: {
        path: path.join(__dirname, '../lib/'), // 输出到哪
        filename: 'index.js', // 输出的文件名字
        libraryTarget: 'commonjs2', // 重要！
    },
    externals: { // 重要！Minified React error #321 https://github.com/facebook/react/issues/16029
        react: {
            commonjs: 'react',
            commonjs2: 'react',
            amd: 'react',
            root: 'React',
        },
        'react-dom': {
            commonjs: 'react-dom',
            commonjs2: 'react-dom',
            amd: 'react-dom',
            root: 'ReactDOM',
        },
    },
    resolve: {
        extensions: ['.js', '.jsx']
    },
    mode: "production",
    module: {
        rules: [
            {
                test: /\.css$/,
                use: ["style-loader", "css-loader"] // style-loader 会把css与js文件打包在一个文件中
            },
            {
                test: /\.jsx?$/,
                exclude: /node_modules/,
                use: ["babel-loader"]
            }
        ]
    },
    performance: {
        hints: "warning", // 枚举
        hints: "error", // 性能提示中抛出错误
        hints: false, // 关闭性能提示
        maxAssetSize: 200000, // 整数类型（以字节为单位）
        maxEntrypointSize: 400000, // 整数类型（以字节为单位）
        assetFilter: function (assetFilename) {
            // 提供资源文件名的断言函数
            return assetFilename.endsWith('.css') || assetFilename.endsWith('.js');
        }
    }
}
```

服务端渲染:

```js
const path = require('path');
const TerserJSPlugin = require('terser-webpack-plugin');
const ExtractCssChunks = require('extract-css-chunks-webpack-plugin');
const OptimizeCSSAssetsPlugin = require('optimize-css-assets-webpack-plugin');

module.exports = {
    output: {
        path: path.join(__dirname, '../lib/ssr/'),
        filename: 'index.js',
        libraryTarget: 'commonjs2',
    },
    optimization: {
        minimizer: [new TerserJSPlugin({}), new OptimizeCSSAssetsPlugin({})],
    },
    plugins: [new ExtractCssChunks({filename: 'index.css',})],
    module: {
        rules: [
            {
                test: /\.css$/,
                use: [ExtractCssChunks.loader, "css-loader"]
            },
            {
                test: /\.jsx?$/,
                exclude: /node_modules/,
                use: ["babel-loader"]
            }
        ]
    }
}
```

其他配置复用上面的配置，使用了2个插件将css单独出一个文件出来，否则服务端渲染时会报：`document is not defined`,

- ExtractCssChunks
- OptimizeCSSAssetsPlugin

这样引入组件时就需要单独引入js文件和css文件：

```js
import React from 'react';
import ReactDOM from 'react-dom';
import Weather from 'react-tencent-weather/lib/ssr/index.js';
import 'react-tencent-weather/lib/ssr/index.css';

ReactDOM.render(
    <div><Weather province="上海" city="上海" /></div>,
    document.getElementById('root')
)
```



# [npm](https://www.npmjs.com/)  账号，npm link，npm publish，

**1. 注册npm账号**

去 [https://www.npmjs.com/](https://www.npmjs.com/) 注册一个账号就ok了

- 在项目目录下 npm login
- 输入 Username/Password/Email: (this IS public)

在此注意！： npm是否使用淘宝或其他镜像代理

```bash
npm config list
```

如果使用了的话，注视掉

```bash
vim .npmrc

//registry=https://registry.npm.taobao.org/
```

使用npm build打包一下出来js文件，npm包发布的是webpack babel编译后的文件～

```js
npm build
```

打包完成后肯定要测试一下的对吧

**2. 本地测试打包文件 npm link**

`npm link` 会把当前目录 软链 到全局路径下

```bash
➜  npm-publish-react-component git:(master) ✗ npm link
npm notice created a lockfile as package-lock.json. You should commit this file.
removed 262 packages in 3.534s

52 packages are looking for funding
  run `npm fund` for details

/Users/liuning/.nvm/versions/node/v12.19.0/lib/node_modules/npm-publish-react-component -> /Users/liuning/Documents/github/npm-publish-react-component
```

 `npm link npm-publish-react-component` 将 全局路径下的 `npm-publish-react-component` 包软链到当前目录下的 `node_modules` <a name="link"></a>

```bash
➜  npm-publish-react-component git:(master) ✗ npm link npm-publish-react-component
/Users/liuning/Documents/github/npm-publish-react-component/node_modules/npm-publish-react-component -> /Users/liuning/.nvm/versions/node/v12.19.0/lib/node_modules/npm-publish-react-component -> /Users/liuning/Documents/github/npm-publish-react-component
```

我们就可以直接在example/index.js中直接import使用

```react
import React from 'react';
import ReactDOM from 'react-dom';
import Demo from 'npm-publish-react-component';

ReactDOM.render(
    <div><Demo name="douyacun"/></div>,
    document.getElementById('root')
)
```

删除 `package-lock.json`  `npm start` 要确保 使用的webpack使用的配置文件是测试的配置，最好区分 测试/正式 环境



# ISSUE

**1. 引入包报错: `React.createElement: type is invalid -- expected a string (for built-in components) or a class/function (for composite components) but got: object.`**

```
react.development.js:220 Warning: React.createElement: type is invalid -- expected a string (for built-in components) or a class/function (for composite components) but got: object. You likely forgot to export your component from the file it's defined in, or you might have mixed up default and named imports.

Uncaught Error: Element type is invalid: expected a string (for built-in components) or a class/function (for composite components) but got: object. You likely forgot to export your component from the file it's defined in, or you might have mixed up default and named imports.
```

这个就是webpack没有指定:  `libraryTarget` 导致，[详细在看一下上面我们罗列的选项](#libraryTarget)



**2. 引入包报错：`Uncaught ReferenceError: module is not defined`**

```js
import React from 'react';
import ReactDOM from 'react-dom';
import Demo from 'douyacun-react-demo';

ReactDOM.render(
    <div><Demo name="douyacun"/></div>,
    document.getElementById('root')
)
```

报错：

![module is not defined](assert/image-20210109163631455.png)

1. 这里是因为package.json没有定义 main 入口文件，import 不知道默认引入哪个文件
2. example 本地测试环境配置的测试环境，没有入口文件，确认是否指向 example/index.js。



**3. npm run build 报错 Cannot find module 'pkg-dir'**

这个是因为:  npm link 在 node_module [创建软链导致的](#link)

解决方案：

```
npm install
```



**4. import react组件时报错：Uncaught Error: Minified React error #321**

如果react组件中使用了hook，useState/useEffect 就会导致这个问题，react官方给出的解决方案：[https://github.com/facebook/react/issues/16029](https://github.com/facebook/react/issues/16029)

原因是因为两次引入了react

```
➜  npm-publish-react-component git:(master) ✗ npm run build

> npm-publish-react-component@1.0.0 build /Users/liuning/Documents/github/npm-publish-react-component
> webpack

{ javascriptModule: false }
asset index.js 8.28 KiB [compared for emit] [minimized] (name: main) 1 related asset
runtime modules 931 bytes 4 modules
orphan modules 326 bytes [orphan] 1 module
cacheable modules 19.6 KiB
  modules by path ./node_modules/ 16.8 KiB
    modules by path ./node_modules/react/ 6.48 KiB
      ./node_modules/react/index.js 190 bytes [built] [code generated]
      ./node_modules/react/cjs/react.production.min.js 6.3 KiB [built] [code generated]    <----- 这里将 react 也打包进去了～
    ./node_modules/style-loader/dist/runtime/injectStylesIntoStyleTag.js 6.67 KiB [built] [code generated]
    ./node_modules/object-assign/index.js 2.06 KiB [built] [code generated]
    ./node_modules/css-loader/dist/runtime/api.js 1.57 KiB [built] [code generated]
  modules by path ./src/ 2.85 KiB
    ./src/index.js + 1 modules 2.49 KiB [built] [code generated]
    ./node_modules/css-loader/dist/cjs.js!./src/index.css 368 bytes [built] [code generated]
webpack 5.12.2 compiled successfully in 1092 ms
```

解决方法：

```json
externals: { // 重要！Minified React error #321 https://github.com/facebook/react/issues/16029
  react: {
    commonjs: 'react',
    commonjs2: 'react',
    amd: 'react',
    root: 'React',
  },
  'react-dom': {
    commonjs: 'react-dom',
    commonjs2: 'react-dom',
    amd: 'react-dom',
    root: 'ReactDOM',
  },
}
```



