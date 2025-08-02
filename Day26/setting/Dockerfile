FROM ubuntu:22.04

# 確保 non-interactive 模式，避免安裝過程卡住
ENV DEBIAN_FRONTEND=noninteractive

# 更新並安裝必要工具
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        git \
        clang-format \
        cppcheck \
        aspell \
        colordiff \
        vim \
        curl \
        gdb \
        htop \
        valgrind && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update && \
    apt-get install -y curl ca-certificates && \
    cd /usr/local/src && \
    curl -LO https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.tar.gz && \
    tar -xvf cmake-3.22.2-linux-x86_64.tar.gz && \
    mv cmake-3.22.2-linux-x86_64 /usr/local/cmake

# 設定 CMake 到 PATH（對所有 shell 生效）
ENV PATH="/usr/local/cmake/bin:$PATH"
