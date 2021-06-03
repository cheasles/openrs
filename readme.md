# OpenRS

OpenRS is an open-source re-implementation of the RuneScape server written in C++.

# Table of Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Building](#building)
* [Testing](#testing)

# Usage

[(Back to top)](#table-of-contents)

OpenRS requires a copy of the original cache files to serve clients.
From the [RuneScape Wiki](https://runescape.fandom.com/wiki/Jagex_cache):

> The Jagex cache, also known as the RuneScape cache, is a bundle of files downloaded in the background of the game allowing players to quickly load and play RuneScape.
> Critical cache files are generally streamed to the player's computer the first time they load RuneScape, with the full cache becoming completely installed over time as the player experiences more of the game.

Currently, only the following version of the cache is supported:

* 718

The project is designed in such a way that adding support for more cache versions is possible, with an eventual goal of supporting many different versions.

## Cache Location

The location OpenRS will search for the cache is configured at generation time.
By default, this will be `/share/openrs`, but can be changed at build time.
Please see (Configuring Cache Location)[#configuring-cache-location] to find out how.

# Dependencies

[(Back to top)](#table-of-contents)

Building OpenRS is relatively straightforward, the only requirements are the MySQL, SQLite and LZMA binaries.
You can install those with the following:

## Debian/Ubuntu

```bash
apt install libsqlite3-dev libmysqlclient-dev liblzma-dev -y
```

## Arch

```bash
pacman -S sqlite3 libmysqlclient
```

# Building

[(Back to top)](#table-of-contents)

To get started building OpenRS, enter the following:

```bash
git clone https://github.com/cheasles/openrs.git
cd openrs
mkdir build
cd build
cmake ..
cmake --build .
```

## Configuring Cache Location

To configure where OpenRS will look for the cache files to serve at runtime, try the following after cloning the repositry and making the build directory:

```bash
cmake .. -DCACHE_DIR=./cache
cmake --build .
```

This will cause OpenRS to search for a folder named `cache` in the current working directory when executed, and serve this to clients.

## Configuring Cache Archive Keys

Each cache can be encrypted with different keys.
OpenRS does not hardcode any of these values, instead they can be provided by creating a `config.json` file within the cache directory.
An example of such a file is below:

```json
{
    "map_archive_keys": {
        "286": [
            840468102,
            457414925,
            -1741404111,
            291119064
        ],
        "287": [
            618027434,
            -1860700829,
            -1238327866,
            1124182670
        ],
        "288": [
            1716543613,
            145912601,
            -1824634140,
            559145923
        ],
        ...
    }
}
```

# Testing

[(Back to top)](#table-of-contents)

OpenRS comes with comprehensive unit testing.
You can execute them via CMake and CTest with the following:

```bash
mkdir build
cd build
cmake ..
cmake --build .
cmake --build . --target test
```
