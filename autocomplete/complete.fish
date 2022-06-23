complete -c tldr -f -s v -d "print verbose output"
complete -c tldr -x -l version -d "print version and exit" -n __fish_use_subcommand
complete -c tldr -x -s h -l help -d "print this help and exit" -n __fish_use_subcommand
complete -c tldr -x -s u -l update -d "update local database" -n __fish_use_subcommand
complete -c tldr -x -s c -l clear-cache -d "clear local database" -n __fish_use_subcommand
complete -c tldr -x -s p -l platform -a "linux osx sunos common" -d "select platform, supported are linux / osx / sunos / common"
complete -c tldr -r -s r -l render -F -d "render a local page for testing purpose" -n __fish_use_subcommand

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
    complete -c tldr -f
end

functions -e __tldr_get_files
