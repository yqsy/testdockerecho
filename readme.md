<!-- TOC -->

- [1. 说明](#1-说明)
- [2. 普通操作](#2-普通操作)
- [3. 容器操作](#3-容器操作)
- [4. 基于docker-compose](#4-基于docker-compose)

<!-- /TOC -->


# 1. 说明

这是一个docker实例项目,容器内跑了一个基于c++写的简单的echo服务. 服务一般

目的是为了实践:

* 容器内配置文件的读取  - 通过映射系统文件来进行读取.
* 容器日志的输出 - 通过Lograte切分stdout/stderr并持久化到日志文件
* image推送到私有服务器, 并在另一端从私有服务器上拉取下来
* 日志索引 - elk elasticsearch logstash kibana

# 2. 普通操作

输入/输出说明
* 配置文件读取: /etc/testdockerecho/testdockerecho.cfg
* 日志文件输出: /var/log/testdockerecho/testdockerecho.log

```bash
# 生成配置目录
sudo mkdir -p /etc/testdockerecho/ && sudo chown `whoami`:`id -g -n` /etc/testdockerecho

# 拷贝配置
cat > /etc/testdockerecho/testdockerecho.cfg <<  EOF
server = {
    listenaddr = "0.0.0.0";
    listenport = 5555;
};
EOF

# 生成日志目录
sudo mkdir -p /var/log/testdockerecho/ && sudo chown `whoami`:`id -g -n` /var/log/testdockerecho/

# 运行程序
testdockerecho 2>&1 | rotatelogs -ve /var/log/testdockerecho/testdockerecho.log-%Y%m%d 86400
```

# 3. 容器操作

映射宿主机文件:
* /env/testdockerecho/cfg/testdockerecho.cfg
* /env/testdockerecho/log/testdockerecho.log

容器内输入/输出说明: 
* 配置文件读取: /etc/testdockerecho/testdockerecho.cfg
* 日志文件输出: /var/log/testdockerecho/testdockerecho.log

```bash

# (-i 打开stdin, -t 交互tty, --rm 退出时删除容器)

# 策略1: -it tty并交互  -dt tty守护运行
# 策略2: --rm 结束后删除镜像 --restart=always 结束后重新开启
# 另外: --name 制定container运行的名称

# 构建
docker build -t yqsy/testdockerecho:1.0 .

sudo mkdir -p /env && sudo chown `whoami`:`id -g -n` /env

# 配置文件
mkdir -p /env/testdockerecho/cfg

# 拷贝配置
cat > /env/testdockerecho/cfg/testdockerecho.cfg <<  EOF
server = {
    listenaddr = "0.0.0.0";
    listenport = 5555;
};
EOF

# 日志目录
mkdir -p /env/testdockerecho/log 

# 前端运行
docker run -it --rm  -p 5555:5555 \
    -v /env/testdockerecho/cfg/:/etc/testdockerecho/ \
    -v /env/testdockerecho/log/:/var/log/testdockerecho/ \
    --name testdockerecho \
    yqsy/testdockerecho:1.0

# 守护运行
docker run -dt --rm  -p 5555:5555 \
    -v /env/testdockerecho/cfg/:/etc/testdockerecho/ \
    -v /env/testdockerecho/log/:/var/log/testdockerecho/ \
    --name testdockerecho \
    yqsy/testdockerecho:1.0

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

# 重新标记
docker tag 192.168.0.213:5000/yqsy/testdockerecho:1.0  yqsy/testdockerecho:1.0
```

# 4. 基于docker-compose

```bash

# 前端运行
docker-compose up

# 后端运行
docker-compose up -d 

# 关闭
docker-compose

# 删除
docker-compose rm -f

```
