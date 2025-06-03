FROM fedora:42

# Install system packages
RUN dnf update -y && dnf install -y \
    # Package groups for basic tools
    @standard \
    @c-development \
    @development-tools \
    @development-libs \
    # C/C++ specific tools
    cmake \
    ninja-build \
    valgrind \
    clang-tools-extra \
    clang \
    cppcheck \
    # Project dependencies
    boost-devel \
    gtest-devel \
    nlohmann_json-devel \
    pkgconf \
    perl-FindBin \
    perl-IPC-Cmd \
    openssl-devel \
    openssl \
    # Python tools
    python3-devel \
    python3-pip \
    mypy \
    ruff \
    # Node.js and npm
    nodejs \
    # locale stuff
    glibc-langpack-en \
    # Rust
    cargo \
    rust \
    # Modern shell/dev utilities
    zsh \
    bat \
    fd-find \
    tree \
    ripgrep \
    fzf \
    zoxide \
    # Linters and formatters
    pre-commit \
    shfmt \
    ShellCheck \
    pre-commit \
    yamllint \
    jq && \
    dnf clean all

# Copy zshrc and set zsh as default shell
COPY .zshrc /root/.zshrc
RUN chsh -s $(which zsh)

# Install npm packages
RUN npm install -g @anthropic-ai/claude-code pure-prompt

# Set locale
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# Install dotnet 6 for CMake Language Support
RUN wget https://dot.net/v1/dotnet-install.sh -O dotnet-install.sh && \
    chmod +x dotnet-install.sh && \
    ./dotnet-install.sh --runtime dotnet --channel 6.0 && \
    rm -rf ./dotnet-install.sh

# Install file watcher for cmake configure task
RUN cargo install --locked watchexec-cli

# Install TA-Lib from source
RUN wget https://github.com/ta-lib/ta-lib/releases/download/v0.6.4/ta-lib-0.6.4-src.tar.gz && \
    tar -xzf ta-lib-0.6.4-src.tar.gz && \
    cd ta-lib-0.6.4 && \
    ./configure && \
    make -j $(nproc) && \
    make install

ENV PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"

WORKDIR /workspaces
