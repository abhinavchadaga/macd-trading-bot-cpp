FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt-get update && apt-get -y install  \
    # basic dev tools
    build-essential \
    software-properties-common \
    apt-transport-https \
    ca-certificates \
    curl \
    wget \
    gnupg \
    lsb-release \
    git \
    man-db \
    less \
    vim \
    # c/c++
    cmake \
    ninja-build \
    clang \
    clangd \
    clang-format \
    gdb \
    valgrind \
    # project deps
    libboost-all-dev \
    libgtest-dev \
    nlohmann-json3-dev \
    libssl-dev \
    pkg-config \
    # python
    python3-dev \
    python3-venv \
    # misc
    zsh \
    tree \
    jq && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# install rust tools
RUN curl -fsSL https://apt.cli.rs/pubkey.asc | tee -a /usr/share/keyrings/rust-tools.asc && \
    curl -fsSL https://apt.cli.rs/rust-tools.list | tee /etc/apt/sources.list.d/rust-tools.list && \
    apt-get update && \
    apt-get install -y bat \
    fd \
    ripgrep \
    watchexec-cli \
    zoxide

COPY .zshrc /root/.zshrc
RUN chsh -s $(which zsh)

# install fzf from source
RUN git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf && \
    ~/.fzf/install

# install pure prompt
RUN mkdir -p "$HOME/.zsh" && \
    git clone https://github.com/sindresorhus/pure.git "$HOME/.zsh/pure"


# install pipx
RUN apt-get install -y pipx
RUN pipx ensurepath

# Install additional Python packages via pipx
RUN pipx install ruff pre-commit

# unminimize
RUN apt-get update && \
    apt-get install -y unminimize \
    && rm -rf /var/lib/apt/lists/* \
    && yes | unminimize

# Generate and set locale
RUN apt-get update && apt-get install locales
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# Install dotnet 6 sdk for vscode cmake extension
RUN add-apt-repository ppa:dotnet/backports && \
    apt-get update && \
    apt-get install -y dotnet-sdk-6.0

# Install TA-Lib from source
RUN wget https://github.com/ta-lib/ta-lib/releases/download/v0.6.4/ta-lib-0.6.4-src.tar.gz && \
    tar -xzf ta-lib-0.6.4-src.tar.gz && \
    cd ta-lib-0.6.4 && \
    ./configure && \
    make -j $(nproc) && \
    make install && \
    cd .. && \
    rm -rf ta-lib-0.6.4 ta-lib-0.6.4-src.tar.gz

WORKDIR /workspace
