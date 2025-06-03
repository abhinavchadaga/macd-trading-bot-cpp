# shellcheck disable-file
ZINIT_HOME="${XDG_DATA_HOME:-${HOME}/.local/share}/zinit/zinit.git"
if [ ! -d "$ZINIT_HOME" ]; then
  mkdir -p "$(dirname $ZINIT_HOME)"
  git clone https://github.com/zdharma-continuum/zinit.git "$ZINIT_HOME"
fi

typeset -A ZINIT
ZINIT[NO_ALIASES]=1
source "${ZINIT_HOME}/zinit.zsh"

zinit ice depth=1

# Add in zsh plugins
zinit light zsh-users/zsh-syntax-highlighting
zinit light zsh-users/zsh-completions
zinit light zsh-users/zsh-autosuggestions
zinit light Aloxaf/fzf-tab

# Add in snippets
zinit snippet OMZP::git
[[ "$(uname -a)" == *"Ubuntu"* ]] && zinit snippet OMZP::ubuntu
zinit snippet OMZP::command-not-found

# Load completions
autoload -Uz compinit && compinit
autoload -U promptinit
promptinit
prompt pure

# History
HISTSIZE=5000
HISTFILE=~/.zsh_history
SAVEHIST=$HISTSIZE
HISTDUP=erase
setopt appendhistory
setopt sharehistory
setopt hist_ignore_space
setopt hist_ignore_all_dups
setopt hist_save_no_dups
setopt hist_ignore_dups
setopt hist_find_no_dups

# Enable fzf-tab fuzzy path completion
export FZF_COMPLETION_TRIGGER='**'

# completion styling
zstyle ':completion:*' matcher-list \
  'm:{a-z}={A-Z}' \
  'r:|=*' \
  'l:|=* r:|=*'
zstyle ':completion:*' list-colors "{(s.:.)LS_COLORS}"
zstyle ':completion:*' menu no

typeset -gx EXPORTED_ALIASES
EXPORTED_ALIASES=$(alias)

# Preview can then parse this
zstyle ':fzf-tab:complete:*:*' fzf-preview \
  'if [[ -n $word ]] && echo "$EXPORTED_ALIASES" | grep -q "^$word="; then
     echo "📋 Alias: $word"
     echo "$EXPORTED_ALIASES" | grep "^$word=" | sed "s/^[^=]*=//"
   elif [[ -n $word ]] && command -v $word >/dev/null 2>&1; then
     echo "📋 Executable: $word"
     which $word
   elif [[ -d $realpath ]]; then
     tree -a -C -L 2 -I ".git|node_modules|.DS_Store|venv|.venv" $realpath 2>/dev/null || ls -la --color=always $realpath
   elif [[ -f $realpath ]]; then
     bat --style=numbers --color=always $realpath 2>/dev/null || cat $realpath
   else
     echo "Preview: ${realpath:-$word}"
   fi'

zstyle ':fzf-tab:*' fzf-min-height 20
zstyle ':fzf-tab:*' fzf-preview-window 'right:70%:wrap'

# ls aliases
alias ls='ls --color'
alias l='ls'
alias la="ls -a"
alias ll='ls -l'
alias lla="ls -al"
alias lt='tree -a -C -L 2 -I ".git|node_modules|.DS_Store|venv|.venv"'

# Ensure globstar and no match errors are handled
setopt globstarshort
unsetopt nomatch

# fzf options
export FZF_DEFAULT_COMMAND="fd --type file --color=always"
export FZF_DEFAULT_OPTS="--ansi"
export FZF_PREVIEW_COMMAND="if [[ -d {} ]]; then tree -C -L 2 {} 2>/dev/null || ls -la --color=always {}; elif [[ -f {} ]]; then bat --style=numbers --color=always {} 2>/dev/null || cat {}; else echo {}; fi"
export FZF_CTRL_T_OPTS="--preview 'if [[ -d {} ]]; then tree -C -L 2 {} 2>/dev/null || ls -la --color=always {}; elif [[ -f {} ]]; then bat --style=numbers --color=always {} 2>/dev/null || cat {}; else echo {}; fi' --preview-window=right:60%"

source <(fzf --zsh)
