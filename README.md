tldr c client
=============

[![Build Status](https://travis-ci.org/tldr-pages/tldr-cpp-client.svg)](https://travis-ci.org/tldr-pages/tldr-cpp-client)

A `C` command line client for tldr

## Installing

On OS X, the client can be installed through [homebrew](http://brew.sh/).

```
brew tap tldr-pages/tldr

# To install the latest stable release
brew install tldr

# To install latest development version
brew install tldr --HEAD
```

To build the latest version from source:
```
if [[ "`uname`" == "Darwin"  ]]; then brew install curl libzip pkg-config; fi
if [[ "`uname`" == "Linux" ]]; then apt-get install libcurl4-openssl-dev libzip-dev pkg-config; fi
git clone https://github.com/tldr-pages/tldr-cpp-client.git tldr-c-client
cd tldr-c-client/src
make
make install
```

The default prefix for installation is `/usr/local/bin`.


## Building

Building the `tldr` client is pretty straightforward.

#### Requirements

- `clang`/`gcc`
- `libcurl` (`brew install curl` / `apt-get install libcurl-dev` / `apt-get install libcurl4-openssl-dev`)
- `libzip` (`brew install libzip` / `apt-get install libzip-dev`)
- `pkg-config` (`brew install pkg-config` / `apt-get install pkg-config`)

#### Compiling

The [`Makefile`](https://github.com/tldr-pages/tldr-cpp-client/blob/master/src/Makefile) in the `src` directory has all you need
for builing the project.

```
cd src
make
```

## Usage

```
usage: tldr [-v] [<command>] <search>

available commands:
    -v                   print verbose output
    --version            print version and exit
    -h, --help           print this help and exit
    -u, --update         update local database
    -c, --clear-cache    clear local database
    -p, --platform=<platform> select platform, supported are linux / osx / common
```

