# tldr c client

[![Build Status](https://img.shields.io/github/actions/workflow/status/tldr-pages/tldr-c-client/.github/workflows/tests.yml?branch=main)](https://github.com/tldr-pages/tldr-c-client/actions)

A command line client for tldr, written in plain ISO C90.

## Installing

On OS X, the client can be installed through [Homebrew](http://brew.sh/).

```shell
# To install the latest development version
brew install tldr --HEAD

# To install the latest stable release
brew install tldr
```

On Arch Linux, the client can be installed through [the AUR](https://aur.archlinux.org/packages/tldr-git) using an AUR helper such as yay.

```shell
yay -S tldr-git
```

To build the latest version from the source:

```shell
git clone https://github.com/tldr-pages/tldr-c-client.git
cd tldr-c-client

sudo ./deps.sh           # install dependencies
make                # build tldr
sudo make install        # install tldr
```

To remove the version installed from the source:

```shell
sudo make uninstall
```

The default prefix for installation is `/usr/local/bin`.


## Building

Building the `tldr` client is pretty straightforward.

### Requirements

- `clang`/`gcc`
- `libcurl` (`brew install curl` / `apt-get install libcurl-dev` / `apt-get install libcurl4-openssl-dev`)
- `libzip` (`brew install libzip` / `apt-get install libzip-dev`)
- `pkg-config` (`brew install pkg-config` / `apt-get install pkg-config`)

### Compiling

The [`Makefile`](https://github.com/tldr-pages/tldr-c-client/blob/master/Makefile)
in the root directory has all you need for building the project.

Just call `make` and `tldr` will build itself.

```shell
make
```

## Autocompletion

Autocompletion is supported for `bash`, `zsh`, and `fish` and can be added by sourcing
the correct autocompletion file.

The files `autocomplete.zsh`, `autocomplete.bash`, and `autocomplete.fish` can be found in the `autocomplete`
folder in the root of the repository.

### Installation

To install the autocompletion, just move the script for your shell to an easy
to access the directory (like your home directory), and source it in your `.bashrc` or `.zshrc`.

Example for zsh:

```shell
mv autocomplete/complete.zsh ~/.tldr.complete
echo "source ~/.tldr.complete" >> ~/.zshrc
```

## Usage

```shell
usage: tldr [-v] [OPTION]... SEARCH

available commands:
    -v                   print verbose output          
    --version            print version and exit        
    -h, --help           print this help and exit      
    -u, --update         update local database         
    -c, --clear-cache    clear local database          
    -l, --list           list all entries in the local database
    -p, --platform=PLATFORM select platform, supported are linux / osx / sunos / windows / common
    --linux              show command page for Linux   
    --osx                show command page for OSX     
    --sunos              show command page for SunOS   
    -r, --render=PATH    render a local page for testing purposes
    -C, --color          force color display
```

## Configuration

To prevent `tldr` from automatically updating its database, set the environment variable `TLDR_AUTO_UPDATE_DISABLED`.

## Contributing

Please read the [CONTRIBUTING.md](https://github.com/tldr-pages/tldr-c-client/blob/master/CONTRIBUTING.md) for details.

## License

The MIT License (MIT) - see [LICENSE](https://github.com/tldr-pages/tldr-c-client/blob/master/LICENSE) for details.
