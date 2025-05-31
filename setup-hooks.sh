#!/bin/bash
set -e

echo "Setting up pre-commit hooks for C++ project..."

# Install pre-commit if not already installed
if ! command -v pre-commit &>/dev/null; then
	echo "Installing pre-commit..."
	pipx install pre-commit
fi

# Install hooks
echo "Installing pre-commit hooks..."
pre-commit install
pre-commit install --hook-type commit-msg

# Generate compilation database for clang-tidy
echo "Generating compilation database..."
if [ ! -d "cmake-build-debug" ]; then
	mkdir -p cmake-build-debug
fi
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B cmake-build-debug

echo "Setup complete!"
echo ""
echo "Usage:"
echo "  - Hooks will run automatically on git commit"
echo "  - Formatting issues (clang-format, whitespace, newlines) are auto-fixed"
echo "  - If fixes are applied, re-run 'git add .' and commit again"
echo "  - Use 'pre-commit run --all-files' to run on all files"
echo "  - Use conventional commit format: type(scope): description"
echo "    Examples: feat: add new trading strategy"
echo "             fix(websocket): handle connection timeouts"
echo "             docs: update README"
echo ""
echo "Pro tip: To commit with auto-fixes in one step:"
echo "  git add . && git commit -m 'your message' || (git add . && git commit --amend --no-edit)"
