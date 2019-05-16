<!-- TOC -->

- [1. 说明](#1-说明)

<!-- /TOC -->


# 1. 说明

```bash
# 构建
docker build -t yqsy/testdockerecho:1.0 .
# 运行
docker run -it yqsy/testdockerecho:1.0
#　查看
docker run -it yqsy/testdockerecho:1.0 bash
# 列出文件大小
du -h --max-depth=1 | sort -hr
```
