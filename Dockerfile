FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y \
    cmake  \
    clang  \
    build-essential  \
    gdb \
    libssl-dev  \
    nlohmann-json3-dev \
    libgtest-dev \
    ninja-build  \
    pkg-config \
    software-properties-common \
    wget

RUN wget https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.gz && \
          tar -xzf boost_1_88_0.tar.gz && \
          cd boost_1_88_0 && \
          ./bootstrap.sh && \
          ./b2 install --prefix=/usr/local

RUN wget https://github.com/ta-lib/ta-lib/releases/download/v0.6.4/ta-lib-0.6.4-src.tar.gz && \
          tar -xzf ta-lib-0.6.4-src.tar.gz && \
          cd ta-lib-0.6.4 && \
          ./configure && \
          make -j $(nproc) && \
          make install
