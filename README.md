
# HoI4 Map Normalizer Tool [![CMake](https://github.com/AFlyingCar/HoI4-Map-Normalizer-Tool/actions/workflows/OnPullRequest.yml/badge.svg)](https://github.com/AFlyingCar/HoI4-Map-Normalizer-Tool/actions/workflows/OnPullRequest.yml)

## Description

This program is a tool for creating total overhaul HoI4 mods. You can use it to 
build total overhauls of the ingame map, provided an input map which lays out
the shapes of the provinces. Doing so will allow the tool to generate a
`provinces.bmp` and `definition.csv` file containing precalculated information
about your map. After the map has been imported, you can select the detected
provinces and edit information about them.

An imported image should be lines of `rgb:000000` to separate each province out.
The provinces may be given a single solid color to specify the province type
ahead of time so that they do not have to be specified by hand later.

See [the sample images](tests/bin/) for examples on how a input image should be
constructed for the importing algorithm to work.

### Currently Supported Platforms

* Linux

## Building

### Linux

```
$ apt install -y python3.6 libgtkmm-3.0-dev googletest libgtest-dev
$ ./tools/setupGTest.linux.sh
$ mkdir Binaries
$ cd Binaries
$ cmake -G"Unix Makefiles" ..
$ make
```

The resulting executables and libraries will be placed in `$PROJECT_ROOT/bin`

This project has been tested and is known to work with the clang-8 compiler. It
may work with other compilers, but I have not tested those yet.

## Completed Features

* Detecting provinces given an input map
* Selecting and editing data about a given province
* Generating unique colors for every province
* Checking imported maps for common errors HoI4 would fail with

## Planned Features

* Windows support
* Exporting mods to load in HoI4 
* Building custom research trees
* Building custom focus trees
* Creating and editing countries
* Creating and editing states
* Generating/importing normal maps given a heightmap
* Generating/importing river maps
* Importing existing mods/maps
* Re-importing map files
* Calculate coastal provinces
* Render the map as it would be seen ingame
* Importing state information

## Credits

Libraries used are [gtkmm](https://gtkmm.org/), [nlohmann::json](https://github.com/nlohmann/json), [nlohmann::fifo_map](https://github.com/nlohmann/fifo_map), and [gtest](https://github.com/google/googletest)

