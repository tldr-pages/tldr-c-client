# Changelog

All notable changes of this project will be documented in this file.
This project is following [semantic versioning](http://semver.org).

Deprecated features will be kept for any following maintenance release, and
will be removed after two major releases.

## [Unreleased]

## v1.5.0 - 2022-07-03
### Added
- Add fish completion for `--list`, `--linux`, `--osx`, and `--sunos` flags

### Fixed
- Fix typo of "database" in the usage output
- Fix fish completion not escaping characters
- Make fish completion reflect actual usage of `tldr` better

## v1.4.3 - 2022-04-11
### Fixed
- Fix segfault on trying to update cache
- Fix segfault when failing to open cache directory
- Bubble error code appropriately when using `--list`

## v1.4.2 - 2021-11-13
### Fixed
- Fix version not being updated in source code

## v1.4.1 - 2021-11-12
### Fixed
- Fix wrong directory name when extracting files from zip cache download

## v1.4.0 - 2021-11-07
### Added
- Add fish completion (see README.md for details)
- Improve output messages for various options
- Add `--list option`
- Add Windows as available platform
- Allow using `TLDR_CACHE_DIR` env var to specify cache directory
- Add `--linux`, `--osx`, and `--sunos` flags as shorthand for `--platform=<platform>`
- Add yum support to the `./deps.sh` script

### Fixed
- Do not check last cache update date when updating cache
- Add yes flag to apt install in ./deps.sh script
- Add blank line at end of output
- Remove `-ansi` flag when compiling
- Use `tldr` instead of `tldr-pages` as name for cache folder

### Miscellaneous
- Rename repository from `tldr-cpp-client` to `tldr-c-client`
- Move from Travis-CI to GitHub actions

## v1.3.0 - 2016-09-09
### Added
- Initial release in core homebrew repository
- Add zsh / bash completion (see README.md for details)

### Fixed
- Fix compiling on old Linux distributions


## v1.2.0 - 2016-04-06
### Added
- Download local database at first run
- Add SunOS as supported tldr platform
- Add --clear-cache option, to clear the local database


## v1.1.0 - 2016-01-18
### Added
- Error messages

### Fixed
- Typos


## v1.0.0 - 2015-12-30
- Initial release


<!-- This is an example how a section should look like:

### [0.1.0-beta1] - YYYY-MM-DD
### Added
- New features

### Changed
- Changes in existing functionality

### Deprecated
- Once-stable features, to be removed in upcoming releases

### Removed
- Deprecated features removed in this release

### Fixed
- Bug fixes

### Security
- Mentioning any security vulnarabilities

-->

