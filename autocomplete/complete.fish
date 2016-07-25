complete -c tldr -xf -s v -d "print verbose output"
complete -c tldr -xf -l version -d "print version and exit"
complete -c tldr -xf -s h -l help -d "print this help and exit"
complete -c tldr -xf -s u -l update -d "update local database"
complete -c tldr -xf -s c -l clear-cache -d "clear local database"
complete -c tldr -rf -f -s p -l platform -a "linux osx sunos common" -d "select platform, supported are linux / osx / sunos / common"
complete -c tldr -r -s r -l render -a PATH -d "render a local page for testing purpose"

function __tldr_get_files
    set -l files (basename -s .md (find $HOME/.tldrc/tldr-master/pages/$argv[1] -name '*.md'))
    for f in $files
        echo $f
    end
end

if test -d "$HOME/.tldrc/tldr-master/pages"
    set -l cmpl (__tldr_get_files common)

    switch (uname)
    case Darwin
        set cmpl $cmpl (__tldr_get_files osx)
    case Linux
        set cmpl $cmpl (__tldr_get_files linux)
    case SunOS
        set cmpl $cmpl (__tldr_get_files sunos)
    end

    complete -c tldr -a (echo $cmpl | sort | uniq)
end

functions -e __tldr_get_files
