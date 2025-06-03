# PR Description Generator

A standalone JavaScript utility that replaces the Claude automated PR description action with direct Anthropic API calls.

## Overview

This utility generates PR descriptions by calling the Anthropic API directly, avoiding the GitHub MCP server access issues with the Claude action. It extracts PR context (diff, commits, title, author) and generates a structured description that gets updated directly to the PR body.

## Files

- `generate-pr-description.js` - Main utility script
- `../.github/workflows/claude-pr-assistant-new.yml` - New GitHub workflow

## Environment Variables

The script requires these environment variables:

- `ANTHROPIC_API_KEY` - Your Anthropic API key (set as GitHub secret)
- `GITHUB_TOKEN` - GitHub token (automatically provided)
- `PR_NUMBER` - Pull request number
- `REPO_OWNER` - Repository owner
- `REPO_NAME` - Repository name  
- `BASE_SHA` - Base commit SHA
- `HEAD_SHA` - Head commit SHA
- `PR_TITLE` - Pull request title
- `PR_AUTHOR` - Pull request author

## Features

- ✅ Direct Anthropic API calls (no Claude action)
- ✅ No GitHub MCP server access
- ✅ Updates PR description without leaving comments
- ✅ Comprehensive error handling
- ✅ Uses only built-in Node.js modules (no dependencies)
- ✅ Structured PR description format

## Usage

### Automated (via GitHub Actions)

The workflow runs automatically on PR events. To activate:

1. Replace the existing workflow:
   ```bash
   mv .github/workflows/claude-pr-assistant.yml .github/workflows/claude-pr-assistant-old.yml
   mv .github/workflows/claude-pr-assistant-new.yml .github/workflows/claude-pr-assistant.yml
   ```

2. Ensure `ANTHROPIC_API_KEY` is set as a repository secret

### Manual Testing

```bash
export ANTHROPIC_API_KEY="your-api-key"
export GITHUB_TOKEN="your-github-token"
export PR_NUMBER="123"
export REPO_OWNER="owner"
export REPO_NAME="repo"
export BASE_SHA="base-commit-sha"
export HEAD_SHA="head-commit-sha"
export PR_TITLE="Your PR Title"
export PR_AUTHOR="username"

node scripts/generate-pr-description.js
```

## How It Works

1. **Extract Context**: Uses git commands to get PR diff and commit messages
2. **Generate Description**: Calls Anthropic API with structured prompt
3. **Update PR**: Uses GitHub API to update PR description directly

## Benefits vs Claude Action

- **No MCP Access**: Completely avoids GitHub MCP server access
- **No Comments**: Updates PR body directly, no action comments
- **Faster**: No Claude action overhead
- **Controllable**: Full control over API requests and responses
- **Self-contained**: No external dependencies beyond Node.js

## Error Handling

The script handles various error scenarios:

- Missing environment variables
- Git command failures
- Anthropic API errors
- GitHub API errors
- Invalid response formats

All errors are logged with clear messages for debugging.