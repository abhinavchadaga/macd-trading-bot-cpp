#!/usr/bin/env node

const fs = require('fs');
const { spawn } = require('child_process');

const ANTHROPIC_API_KEY = process.env.ANTHROPIC_API_KEY;
const GITHUB_TOKEN = process.env.GITHUB_TOKEN;
const PR_NUMBER = process.env.PR_NUMBER;
const REPO_OWNER = process.env.REPO_OWNER;
const REPO_NAME = process.env.REPO_NAME;
const BASE_SHA = process.env.BASE_SHA;
const HEAD_SHA = process.env.HEAD_SHA;
const PR_TITLE = process.env.PR_TITLE;
const PR_AUTHOR = process.env.PR_AUTHOR;

function runCommand(command, args = []) {
  return new Promise((resolve, reject) => {
    const child = spawn(command, args, { 
      stdio: ['pipe', 'pipe', 'pipe'],
      shell: true 
    });
    
    let stdout = '';
    let stderr = '';
    
    child.stdout.on('data', (data) => {
      stdout += data.toString();
    });
    
    child.stderr.on('data', (data) => {
      stderr += data.toString();
    });
    
    child.on('close', (code) => {
      if (code === 0) {
        resolve(stdout);
      } else {
        reject(new Error(`Command failed with code ${code}: ${stderr}`));
      }
    });
  });
}

async function getPRContext() {
  try {
    const diff = await runCommand(`git diff ${BASE_SHA}..${HEAD_SHA}`);
    const commits = await runCommand(`git log --pretty=format:"%h %s" ${BASE_SHA}..${HEAD_SHA}`);
    
    return {
      title: PR_TITLE,
      author: PR_AUTHOR,
      commits: commits.trim(),
      diff: diff.trim()
    };
  } catch (error) {
    console.error('Error getting PR context:', error);
    throw error;
  }
}

async function generateDescription(context) {
  const prompt = `Read the pull request context below and write a clear, concise PR description.

Return ONLY the PR description content in GitHub Markdown format - no meta-commentary, no explanations about what you did, just the actual description that will become the PR body.

Structure the description with:
## Summary
Brief overview of what this PR does

## Changes Made
- Key changes in bullet points

## Impact
What this improves or fixes

Pull Request Title: ${context.title}
Author: ${context.author}

Commit Messages:
${context.commits}

Complete Code Diff:
${context.diff}`;

  const requestBody = {
    model: "claude-3-5-sonnet-20241022",
    max_tokens: 1000,
    messages: [
      {
        role: "user",
        content: prompt
      }
    ]
  };

  try {
    const response = await fetch('https://api.anthropic.com/v1/messages', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'x-api-key': ANTHROPIC_API_KEY,
        'anthropic-version': '2023-06-01'
      },
      body: JSON.stringify(requestBody)
    });

    if (!response.ok) {
      throw new Error(`Anthropic API error: ${response.status} ${response.statusText}`);
    }

    const data = await response.json();
    
    if (!data.content || !data.content[0] || !data.content[0].text) {
      throw new Error('Invalid response format from Anthropic API');
    }

    return data.content[0].text.trim();
  } catch (error) {
    console.error('Error calling Anthropic API:', error);
    throw error;
  }
}

async function updatePRDescription(description) {
  const url = `https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}/pulls/${PR_NUMBER}`;
  
  try {
    const response = await fetch(url, {
      method: 'PATCH',
      headers: {
        'Authorization': `token ${GITHUB_TOKEN}`,
        'Accept': 'application/vnd.github.v3+json',
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        body: description
      })
    });

    if (!response.ok) {
      throw new Error(`GitHub API error: ${response.status} ${response.statusText}`);
    }

    console.log('PR description updated successfully');
  } catch (error) {
    console.error('Error updating PR description:', error);
    throw error;
  }
}

async function main() {
  try {
    if (!ANTHROPIC_API_KEY) {
      throw new Error('ANTHROPIC_API_KEY environment variable is required');
    }
    
    if (!GITHUB_TOKEN) {
      throw new Error('GITHUB_TOKEN environment variable is required');
    }
    
    if (!PR_NUMBER || !REPO_OWNER || !REPO_NAME || !BASE_SHA || !HEAD_SHA) {
      throw new Error('Missing required environment variables: PR_NUMBER, REPO_OWNER, REPO_NAME, BASE_SHA, HEAD_SHA');
    }

    console.log('Getting PR context...');
    const context = await getPRContext();
    
    console.log('Generating description with Anthropic API...');
    const description = await generateDescription(context);
    
    console.log('Updating PR description...');
    await updatePRDescription(description);
    
    console.log('✅ PR description generation complete');
  } catch (error) {
    console.error('❌ Error:', error.message);
    process.exit(1);
  }
}

if (require.main === module) {
  main();
}