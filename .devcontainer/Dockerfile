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
    # Python tools
    python3-devel \
    python3-pip \
    mypy \
    ruff \
    # Node.js and npm
    nodejs \
    # locale stuff
    glibc-langpack-en \
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

# Install claude code
RUN npm install -g @anthropic-ai/claude-code

# Set locale
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# Install dotnet 6 for CMake Language Support
RUN wget https://dot.net/v1/dotnet-install.sh -O dotnet-install.sh && \
    chmod +x dotnet-install.sh && \
    ./dotnet-install.sh --runtime dotnet --channel 6.0 && \
    rm -rf ./dotnet-install.sh

WORKDIR /workspaces
