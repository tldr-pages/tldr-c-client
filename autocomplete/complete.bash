# I don't use bash, but I remember this works.
# If anyone has an improved, and better version, go ahead, open a pull-request.
#
# Copyright (C) 2016 Arvid Gerstmann
#

_tldr_get_files() {
	local ret
	local files="$(find $HOME/.tldrc/tldr-master/pages/$1 -name '*.md' -exec basename {} .md \;)"

	IFS=$'\n\t'
	for f in $files; do
	    echo $f
	done
}

_tldr_complete() {
    COMPREPLY=()
	local word="${COMP_WORDS[COMP_CWORD]}"
	local cmpl=""
    if [ "$word" = "-" ]; then
        cmpl=$(echo $'\n-v\n-h\n-u\n-c\n-p\n-r' | sort)
    elif [ "$word" = "--" ]; then
        cmpl=$(echo $'--version\n--help\n--update\n--clear-cache\n--platform\n--render' | sort)
    else
        if [ -d "$HOME/.tldrc/tldr-master/pages" ]; then
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
    COMPREPLY=( $(compgen -W "$cmpl" -- "$word") )
}

complete -F _tldr_complete tldr
