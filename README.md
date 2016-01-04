tldr cpp client
=============

A `C++` command line client for tldr

[![Build Status](https://travis-ci.org/tldr-pages/tldr-cpp-client.svg)](https://travis-ci.org/tldr-pages/tldr-cpp-client)

## Installing

On OS X, the client can be installed through [homebrew](http://brew.sh/).
```
brew install tldr-pages/tldr/tldr
```

## Building

Building the `tldr` client is pretty straightforward.

#### Requirements

- `clang`
- `libcurl` (`brew install curl` / `apt-get install libcurl-dev` / `apt-get install libcurl4-openssl-dev`)

#### Compiling

The default prefix for installation is `/usr/local/bin`.

```
cd src
make
make install
```

## Usage

`tldr <command>`
