# 解除 
```
git config http.postBuffer 1048576000
```

# 清除大文件 
```
git filter-branch -f --index-filter 'git rm --cached --ignore-unmatch 大文件'
git filter-branch --tree-filter 'rm -rf library/深入理解计算机系统.pdf' HEAD
```

# 突破github 100M限制, 最大1G，超过限制需要付费
上传超过100M大小的文件回遇到一下错误
```
remote: error: GH001: Large files detected. You may want to try Git Large File Storage - https://git-lfs.github.com.
remote: error: Trace: 10bdf6f87a2ec7d20bea6bd0a4648546
remote: error: See http://git.io/iEPt8g for more information.
remote: error: File library/深入理解计算机系统.pdf is 500.61 MB; this exceeds GitHub's file size limit of 100.00 MB
```

1. 先清除之前提交到版本库中的大文件，不要手动去删除
```
git filter-branch --tree-filter 'rm -rf library/深入理解计算机系统.pdf' HEAD
```
2. 使用 [git lfs](https://git-lfs.github.com/)
3. 可以查看500M大文件[深入理解计算机系统.pdf](https://github.com/douyacun/book/blob/master/library/%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A3%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%B3%BB%E7%BB%9F.pdf)
- Linux
```
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash

sudo apt-get install git-lfs
git lfs install
```

- Mac
```
安装HomeBrew /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

brew install git-lfs
git lfs install
```
- Windows
```
下载安装 windows installer

运行 windows installer
在命令行执行 git lfs install
```

使用

1. 执行 git lfs install 开启lfs功能
2. 使用 git lfs track 命令进行大文件追踪 例如git lfs track "*.png" 追踪所有后缀为png的文件
3. 使用 git lfs track 查看现有的文件追踪模式
4. 提交代码需要将gitattributes文件提交至仓库. 它保存了文件的追踪记录
5. 提交后运行git lfs ls-files 可以显示当前跟踪的文件列表
6. 将代码 push 到远程仓库后，LFS 跟踪的文件会以『Git LFS』的形式显示:
7. clone 时 使用'git clone' 或 git lfs clone均可