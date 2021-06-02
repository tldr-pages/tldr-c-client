# Old style completion for zsh users.
# Its not using the new compdef syntax, which was introduced in 3.1
#
# Pull requests to update it to the new `compdef` style are welcome.
#
# Copyright (C) 2016 Arvid Gerstmann
#

_tldr_get_files() {
	local ret
	local files="$(find $HOME/.tldrc/tldr/pages/$1 -name '*.md' -exec basename {} .md \;)"

	IFS=$'\n\t'
	for f in $files; do
	    echo $f
	done
}

_tldr_complete() {
	local word="$1"
	local cmpl=""
    if [ "$word" = "-" ]; then
        cmpl=$(echo $'\n-v\n-h\n-u\n-c\n-p\n-r' | sort)
    elif [ "$word" = "--" ]; then
        cmpl=$(echo $'--version\n--help\n--update\n--clear-cache\n--platform\n--render' | sort)
    else
        if [ -d "$HOME/.tldrc/tldr/pages" ]; then
            local platform="$(uname)"
            cmpl="$(_tldr_get_files common | sort | uniq)"
            if [ "$platform" = "Darwin" ]; then
                cmpl="${cmpl}$(_tldr_get_files osx | sort | uniq)"
            elif [ "$platform" = "Linux" ]; then
                cmpl="${cmpl}$(_tldr_get_files linux | sort | uniq)"
            elif [ "$platform" = "SunOS" ]; then
                cmpl="${cmpl}$(_tldr_get_files sunos | sort | uniq)"
            fi
        fi
    fi
	reply=( "${(ps:\n:)cmpl}" )
}

compctl -K _tldr_complete tldr
