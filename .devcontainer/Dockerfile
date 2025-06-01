FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    cmake \
    ninja-build \
    valgrind \
    clang-tidy \
    clangd \
    clang-format \
    libboost-all-dev \
    libgtest-dev \
    git \
    python3-dev \
    python3-venv \
    nlohmann-json3-dev \
    libssl-dev \
    curl \
    man-db \
    zsh \
    bat \
    fd-find \
    tree \
    software-properties-common \
    locales \
    pipx \
    unminimize && rm -rf /var/lib/apt/lists/*

# Copy zshrc and set zsh as default shell
COPY .zshrc /root/.zshrc
RUN chsh -s $(which zsh)

# Install fzf from git (newer version)
RUN git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf && \
    ~/.fzf/install --all

# Install zoxide
RUN curl -sSfL https://raw.githubusercontent.com/ajeetdsouza/zoxide/main/install.sh | sh

# Install node via nvm (should auto-detect zsh and update .zshrc)
ENV NVM_DIR=/root/.nvm
ENV NODE_VERSION="--lts"
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash && \
    bash -c "source ${NVM_DIR}/nvm.sh && \
    nvm install ${NODE_VERSION} && \
    ln -sf ${NVM_DIR}/versions/node/\$(nvm current)/bin/* /usr/local/bin/"

# Install claude code
RUN npm install -g @anthropic-ai/claude-code

# Setup pipx (should auto-detect zsh and update .zshrc)
RUN pipx ensurepath

# Unminimize Ubuntu
RUN DEBIAN_FRONTEND=noninteractive yes | unminimize

# Fix locales
RUN locale-gen en_US.UTF-8

# Install .NET SDK for CMake VSCode extensions
RUN add-apt-repository ppa:dotnet/backports && \
    apt-get update && \
    apt-get install -y dotnet-sdk-6.0 && \
    rm -rf /var/lib/apt/lists/*

# Install shell tools (should auto-detect shell and update profiles)
RUN curl -sS https://webi.sh/shfmt | sh
RUN curl -sS https://webi.sh/shellcheck | sh

WORKDIR /workspaces
