tldr cpp client
=============

A `C++` command line client for tldr


## Installing

The client can be installed through homebrew.
```
brew tap tldr-pages/tldr
brew install tldr
```

## Building

Building the `tldr` client is pretty straigh forward.

#### Requirements

- `clang`
- `libcurl` (`brew install curl` / `apt-get install libcurl-dev`)


#### Compiling

The default prefix for installation is `/usr/local/bin`.

```
make
make install
``` 

## Usage

`tldr <command>`
