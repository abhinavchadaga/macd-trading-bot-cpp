#!/bin/bash

# Smart git commit that handles pre-commit hook auto-fixes
# Usage: ./git-commit-with-hooks.sh "your commit message"

if [ $# -eq 0 ]; then
    echo "Usage: $0 'commit message'"
    echo "Example: $0 'feat: add new feature'"
    exit 1
fi

commit_message="$1"

echo "🔄 Staging files..."
git add .

echo "🎯 Attempting commit with pre-commit hooks..."
if git commit -m "$commit_message"; then
    echo "✅ Commit successful!"
else
    echo "🔧 Pre-commit hooks made changes. Re-staging and committing..."
    git add .
    if git commit --amend --no-edit; then
        echo "✅ Commit successful with auto-fixes applied!"
    else
        echo "❌ Commit failed. Please check the errors above."
        exit 1
    fi
fi

echo ""
echo "📋 Recent commits:"
git log --oneline -3
