FROM ubuntu:18.04 as build

COPY . /tmp/helloworld

COPY ./sources.list /tmp/sources.list

RUN set -ex; \
    rm /etc/apt/sources.list; cp /tmp/sources.list /etc/apt/sources.list; \
    apt-get update; \
    apt-get install -y autoconf automake autotools-dev build-essential checkinstall; \
    rm -rf /var/lib/apt/lists/*

RUN set -ex; \
    cd /tmp/helloworld; \
    ./autogen.sh; \
    ./configure; \
    make; \
    checkinstall --pkgversion 0.1.0 --pkgname=helloworld -y --install=no --fstrans=yes;

FROM ubuntu:18.04

COPY --from=build /tmp/helloworld/helloworld_0.1.0-1_amd64.deb /tmp/

COPY ./sources.list /tmp/sources.list

RUN set-ex; \
    rm /etc/apt/sources.list; cp /tmp/sources.list /etc/apt/sources.list; \
    apt-get update; \
    apt-get install /tmp/helloworld_0.1.0-1_amd64.deb; \
    rm -rf /var/lib/apt/lists/*
