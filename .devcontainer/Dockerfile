FROM ubuntu:24.04

# install system packages and deps
RUN apt-get update && apt-get -y install  \
    build-essential \
    gdb \
    cmake \
    ninja-build \
    valgrind \
    clang-tidy \
    clangd \
    libboost-all-dev \
    libgtest-dev \
    git \
    python3-dev \
    python3-venv \
    nlohmann-json3-dev \
    libssl-dev \
    curl \
    man-db \
    clangd \
    clang-tidy \
    clang-format

# install node
ENV NVM_DIR=/root/.nvm
ENV NODE_VERSION="--lts"

RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash
RUN bash -c "source ${NVM_DIR}/nvm.sh && nvm install ${NODE_VERSION} && ln -sf ${NVM_DIR}/versions/node/\$(nvm current)/bin/* /usr/local/bin/"

# install claude code
RUN npm install -g @anthropic-ai/claude-code

# install pipx
RUN apt-get install -y pipx
RUN pipx ensurepath

# unminimize
RUN apt-get update -y && \
DEBIAN_FRONTEND=noninteractive apt-get install -y unminimize \
&& rm -rf /var/lib/apt/lists/* \
&& yes | unminimize

# fix locales
RUN apt-get install locales
RUN locale-gen en_US.UTF-8

# install dotnet runtimes for cmake vscode extenstions
RUN apt-get -y install software-properties-common
RUN add-apt-repository ppa:dotnet/backports
RUN apt-get install -y dotnet-sdk-6.0
