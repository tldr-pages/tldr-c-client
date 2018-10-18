#!/bin/sh

UNAME=$(uname -s)
ARCH=$(uname -m)

if [ "$UNAME" = "Darwin" ]; then
    HAS_BREW=$(command -v brew > /dev/null 2>&1 && echo 1 || echo 0)
    if [ "$HAS_BREW" -eq 1 ]; then
        brew update
        brew install curl libzip pkg-config || exit 1
    fi
elif [ "$UNAME" = "Linux" ]; then
    HAS_APT=$(command -v apt-get > /dev/null >&1 && echo 1 || echo 0)
    HAS_PACMAN=$(command -v pacman > /dev/null >&1 && echo 1 || echo 0)

    if [ "$(id -u)" != "0" ]; then
        echo "Please run this script as root"
        exit 1
    fi

    if [ "$HAS_APT" -eq 1 ]; then
        apt-get update
        apt-get install -y libcurl4-openssl-dev libzip-dev pkg-config || exit 1
    elif [ "$HAS_PACMAN" -eq 1 ]; then
        pacman -Syy
        pacman -S libzip libcurl-gnutls pkg-config || exit 1
    fi
else
    echo "Unknown platform"
    exit 1
fi

