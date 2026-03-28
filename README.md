# Book

`book` 现在只保留正式内容结构，文章统一放在：

`<category>/<slug>/index.md`

图片和附件统一放在：

`<category>/<slug>/images/*`

当前一级分类：

- `backend`
- `database`
- `devops`
- `architecture`
- `frontend`
- `pdf`
- `growth`
- `notes`

辅助目录：

- `assert/`：站点共享资源，例如订阅号二维码
- `migration/`：历史文章迁移映射和校验结果

迁移产物：

- `migration/article-map.csv`：旧文件到新文章目录映射
- `migration/url-map.csv`：旧 `/article/<legacy_id>` 到新 canonical URL 映射
- `migration/errors.csv`：迁移异常清单，当前已清空

发布控制：

- 文章默认发布
- 如需跳过发布，可在文章 `index.md` front matter 中增加 `Publish: false`
