---
Title: git-查看历史-待删除
LegacyId: 49d1cda87efd18aab8fefd1685c5b787
topic: pdf
Keywords:
Label:
description: "**最后一次提交时间** git log --date-order -1 --format=\"%ad\" -- path/to/file **第一次提交时间** git log --reverse --format=\"%ci\" -- path"
Date: 2019-11-29T11:38:31+08:00
LastEditTime: 2019-11-29T11:38:31+08:00
Publish: false
Lang: zh
---

# git 查看历史

**最后一次提交时间**

git log  --date-order -1 --format="%ad" -- path/to/file

**第一次提交时间**

git log --reverse --format="%ci" -- path/to/file |awk 'NR == 1'
