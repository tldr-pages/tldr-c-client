tldr c client
=============

A `C` command line client for tldr

[![Build Status](https://travis-ci.org/tldr-pages/tldr-cpp-client.svg)](https://travis-ci.org/tldr-pages/tldr-cpp-client)

## Installing

On OS X, the client can be installed through [homebrew](http://brew.sh/).

To get the latest version:
```
brew tap tldr-pages/tldr
brew install tldr --HEAD
```

## Building

Building the `tldr` client is pretty straightforward.

#### Requirements

- `clang`
- `libcurl` (`brew install curl` / `apt-get install libcurl-dev` / `apt-get install libcurl4-openssl-dev`)
- `libzip` (`brew install libzip` / `apt-get install libzip-dev`)

#### Compiling

The default prefix for installation is `/usr/local/bin`.

```
cd src
make
make install
```

## Usage

```
usage: tldr [-v] [--version] [--help] [--update] <search>

available commands:
    --version    print version and exit
    --help       print help and exit
    --update     update local database
```

