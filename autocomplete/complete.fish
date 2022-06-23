set -l os_arg_condition "__fish_not_contain_opt linux osx sunos"

complete -c tldr -f                                       -s v                -d "print verbose output"
complete -c tldr -x  -n __fish_use_subcommand                  -l version     -d "print version and exit"
complete -c tldr -x  -n __fish_use_subcommand             -s l -l list        -d "list all entries in the local database"
complete -c tldr -x  -n __fish_use_subcommand             -s h -l help        -d "print this help and exit"
complete -c tldr -x  -n __fish_use_subcommand             -s u -l update      -d "update local database"
complete -c tldr -x  -n __fish_use_subcommand             -s c -l clear-cache -d "clear local database"
complete -c tldr -x  -n $os_arg_condition                 -s p -l platform    -d "select platform" -a "linux osx sunos common"
complete -c tldr -f  -n "$os_arg_condition -s p platform"      -l linux       -d "show command page for Linux"
complete -c tldr -f  -n "$os_arg_condition -s p platform"      -l osx         -d "show command page for macOS"
complete -c tldr -f  -n "$os_arg_condition -s p platform"      -l sunos       -d "show command page for SunOS"
complete -c tldr -rF -n __fish_use_subcommand             -s r -l render      -d "render a local page for testing purpose"

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

    complete -c tldr -f -a "$cmpl" -n __fish_use_subcommand
end

complete -c tldr -f

functions -e __tldr_get_files
