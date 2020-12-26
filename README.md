# tldr c client

[![Build Status](https://img.shields.io/github/workflow/status/tldr-pages/tldr-c-client/Tests.svg?style=flat-square)](https://github.com/tldr-pages/tldr-c-client/actions)

A command line client for tldr, written in plain ISO C90.


## Installing

On OS X, the client can be installed through [homebrew](http://brew.sh/).

```
# To install latest development version
brew install tldr --HEAD

# To install the latest stable release
brew install tldr
```

To build the latest version from source:
```
git clone https://github.com/tldr-pages/tldr-c-client.git
cd tldr-c-client

./deps.sh           # install dependencies
make                # build tldr
make install        # install tldr
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

The [`Makefile`](https://github.com/tldr-pages/tldr-c-client/blob/master/Makefile)
in the root directory has all you need for builing the project.

Just call `make` and `tldr` will build itself.

```
make
```


## Autocompletion

Autocompletion is supported for `bash`, `zsh`, and `fish` and can be added by sourcing
the correct autocompletion file.

The files `autocomplete.zsh`, `autocomplete.bash`, and `autocomplete.fish` can be found in the `autocomplete`
folder in the root of the repository.

#### Installation

To install the autocompletion, just move the script for your shell to a an easy
to access directory (like your home directory), and source it in your `.bashrc` or `.zshrc`.

Example for zsh:

```
mv autocomplete/complete.zsh ~/.tldr.complete
echo "source ~/.tldr.complete" >> ~/.zshrc
```


## Usage

```
usage: ./tldr [-v] [OPTION]... SEARCH

available commands:
    -v                   print verbose output
    --version            print version and exit
    -h, --help           print this help and exit
    -u, --update         update local database
    -c, --clear-cache    clear local database
    -p, --platform=PLATFORM select platform, supported are linux / osx / sunos / common
    -r, --render=PATH    render a local page for testing purposes
```


## Contributing

Please read the [CONTRIBUTING.md](https://github.com/tldr-pages/tldr-c-client/blob/master/CONTRIBUTING.md) for details.


## License

The MIT License (MIT) - see [LICENSE](https://github.com/tldr-pages/tldr-c-client/blob/master/LICENSE) for details.
