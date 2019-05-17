<!-- TOC -->

- [1. 说明](#1-说明)

<!-- /TOC -->


# 1. 说明

```bash
# (-i 打开stdin, -t 交互tty, --rm 退出时删除容器)

# 策略1: -it tty并交互  -dt tty守护运行
# 策略2: --rm 结束后删除镜像 --restart=always 结束后重新开启
# 另外: --name 制定container运行的名称

# 构建
docker build -t yqsy/testdockerecho:1.0 .

# 前端运行
docker run -it --rm  -p 5555:5555  --name testdockerecho yqsy/testdockerecho:1.0

# 守护运行
docker run  -dt --rm -p 5555:5555 --name testdockerecho yqsy/testdockerecho:1.0

#　bash
docker run -it --rm yqsy/testdockerecho:1.0 bash

# 附加bash
docker exec -it yqsy/testdockerecho:1.0 bash

# 列出文件大小
du -h --max-depth=1 | sort -hr

# 私有服务器 配置修改
/etc/docker/daemon.json
"insecure-registries": ["192.168.0.213:5000"]

# private hub
docker run -dt --restart=always -p 5000:5000 --name registry registry:2

# 本地标记
docker tag yqsy/testdockerecho:1.0 192.168.0.213:5000/yqsy/testdockerecho:1.0

# 推送
docker push 192.168.0.213:5000/yqsy/testdockerecho:1.0

# 拉取
docker pull 192.168.0.213:5000/yqsy/testdockerecho:1.0

# 拉取后运行
docker run -it --rm -p 5555:5555 --name testdockerecho 192.168.0.213:5000/yqsy/testdockerecho:1.0

# 拉取后运行守护
docker run -dt --rm -p 5555:5555 --name testdockerecho 192.168.0.213:5000/yqsy/testdockerecho:1.0

```
