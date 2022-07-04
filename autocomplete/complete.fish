function __tldr_not_contain_standalone_opt
        __fish_not_contain_opt -s v
    and __fish_not_contain_opt      version
    and __fish_not_contain_opt -s l list
    and __fish_not_contain_opt -s h help
    and __fish_not_contain_opt -s u update
    and __fish_not_contain_opt -s c clear-cache
end

function __tldr_no_os_choice_opt
        __fish_not_contain_opt      linux osx sunos
    and __fish_not_contain_opt -s r render
    and __tldr_not_contain_standalone_opt
end

function __tldr_no_os_choice_opt_nor_p
        __tldr_no_os_choice_opt
    and __fish_not_contain_opt -s p platform
end

function __tldr_positional
        __fish_use_subcommand
    and __tldr_not_contain_standalone_opt
end

function __tldr_positional_no_os
        __tldr_positional
    and __fish_not_contain_opt      linux osx sunos
    and __fish_not_contain_opt -s p platform
end

complete -c tldr -f  -n "__fish_not_contain_opt -s v"  -s v                -d "print verbose output"
complete -c tldr -f  -n  __fish_is_first_arg                -l version     -d "print version and exit"
complete -c tldr -f  -n  __fish_is_first_arg           -s l -l list        -d "list all entries in the local database"
complete -c tldr -f  -n  __fish_is_first_arg           -s h -l help        -d "print help and exit"
complete -c tldr -f  -n  __fish_is_first_arg           -s u -l update      -d "update local database"
complete -c tldr -f  -n  __fish_is_first_arg           -s c -l clear-cache -d "clear local database"
complete -c tldr -x  -n  __tldr_no_os_choice_opt       -s p -l platform    -d "select platform" -a "linux osx sunos common"
complete -c tldr -f  -n  __tldr_no_os_choice_opt_nor_p      -l linux       -d "show command page for Linux"
complete -c tldr -f  -n  __tldr_no_os_choice_opt_nor_p      -l osx         -d "show command page for macOS"
complete -c tldr -f  -n  __tldr_no_os_choice_opt_nor_p      -l sunos       -d "show command page for SunOS"
complete -c tldr -rF -n  __tldr_positional_no_os       -s r -l render      -d "render a local page for testing purposes"

function __tldr_get_files
    basename -s .md (find $HOME/.tldrc/tldr/pages/$argv[1] -name '*.md') | string escape
end

if test -d "$HOME/.tldrc/tldr/pages"
    set -l cmpl (__tldr_get_files common)

    switch (uname)
    case Darwin
        set cmpl $cmpl (__tldr_get_files osx)
    case Linux
        set cmpl $cmpl (__tldr_get_files linux)
    case SunOS
        set cmpl $cmpl (__tldr_get_files sunos)
    end

    complete -c tldr -f -a "$cmpl" -n __tldr_positional
end

complete -c tldr -f

functions -e __tldr_get_files
