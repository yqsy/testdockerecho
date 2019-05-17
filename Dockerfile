FROM ubuntu:18.04 as build

COPY ./sources.list /tmp/sources.list

# -e, 遇到错误时退出
# -x, 打印执行语句 

RUN set -ex; \
    rm /etc/apt/sources.list; cp /tmp/sources.list /etc/apt/sources.list; \
    apt-get update; \
    apt-get install -y autoconf automake libtool autotools-dev \
    build-essential checkinstall wget libgoogle-glog-dev; \
    rm -rf /var/lib/apt/lists/*

# libevent 2.1.9
RUN set -ex; \
    cd /tmp; \
    wget https://github.com/libevent/libevent/releases/download/release-2.1.9-beta/libevent-2.1.9-beta.tar.gz; \
    tar zxf libevent-2.1.9-beta.tar.gz; \
    cd libevent-2.1.9-beta; \
    ./autogen.sh; \
    ./configure --disable-shared; \
    make; \
    make install; \
    rm -rf /tmp/libevent-2.1.9-beta

COPY . /tmp/testdockerecho

# compile testdockerecho
RUN set -ex; \
    cd /tmp/testdockerecho; \
    ./autogen.sh; \
    ./configure; \
    make; \
    checkinstall --pkgversion 0.1.0 --pkgname=testdockerecho -y --install=no --fstrans=yes;

FROM ubuntu:18.04

COPY --from=build /tmp/testdockerecho/testdockerecho_0.1.0-1_amd64.deb /tmp/

COPY ./sources.list /tmp/sources.list

RUN set-ex; \
    rm /etc/apt/sources.list; cp /tmp/sources.list /etc/apt/sources.list; \
    apt-get update; \
    apt-get install /tmp/testdockerecho_0.1.0-1_amd64.deb; \
    rm -rf /var/lib/apt/lists/*

EXPOSE 5555

CMD testdockerecho
