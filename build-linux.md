# Build guide for linux systems

As the integration code is very lightweight, prebuilt binaries are not supplied with the SDK. There are two integration options:

1. Copy the source code from one/arcus or one/ping directly into the game-server or game-client and compile them as part of a project build.

2. Build the complete repository first using the instructions below, and then copy required headers and libraries. All binaries are placed into the build folder, e.g. `/build/lib/one_arcus.lib` or `/build/bin/tests`.

Building the repository is recommended in either case, as the build will also create a fake agent executable. The fake agent can be used to test your game server before deploying it to the remote ONE Platform. For more information about the fake agent see [one/fake](/one/fake/readme.md)

## Build requirements

### Build tools

Ubuntu and Debian are supported

- build-essential package (gcc/g++/make)
- cmake
- libssl-dev (openssl)
- git

Dependencies can be installed using apt:

```sh
sudo apt update
sudo apt install build-essential cmake libssl-dev git
```

The following Linux configurations are tested and supported:

- Ubuntu20.04
  - GCC/G++ (v4.9.3) / make (v4.2.1)
  - cmake 3.16.3 or higher
  - libssl-dev (v1.1.1)

- Ubuntu22.04
  - GCC/G++ (v4.11.2) / make (v4.3.4)
  - cmake 3.22.1 or higher
  - libssl-dev (v3.0.2)

- Debian 12
  - GCC/G++ (v4.12.2) / make (v4.3.4)
  - cmake 3.25.1 or higher
  - libssl-dev (v3.0.13)

### Optional build tools

For automatic documentation generation:

- Doxygen
- Graphviz

```bash
sudo apt update
sudo apt-get install doxygen graphviz`
```

### Preparation

After the initial repository clone, make sure to initialize the git submodules.

```git-bash
git submodule update --init --recursive
```

## Build

Build scripts can be found in the `/tools/linux` folder. Scripts are provided with different build options, according to the following naming convention:

`build_<debug_or_release>_<static_or_shared_library>.sh`

- Debug or release build:
`_debug or _release` options specify whether to build the SDK in debug or release mode.

- Static or dynamic Library:
`_a or _so` options specify whether to build the SDK as a static library (`.a`) or a dynamic library (`.so`).

Example: `/tools/linux/build_debug_a.sh`
Builds the SDK as a static library in debug mode.

## Clean

A clean of the build folder is needed before switching to a script with different build settings. The clean script `clean.sh` can be also found in and should be run from the `/tools/linux` folder.

## Test

Several tests-cases and test-collections are available after a successful build of the SDK as a static library. Test scripts are provided for the three most common used collections of tests and are located in the  `/tools/linux` folder.

- short test
`test_short.sh` or `test_short.sh`

- long test (continuous test maintaining connection)
`test_long.sh` or `test_long.sh`

- soak test (very high usage over a very long period on multiple threads)
`test_soak.sh` or `test_soak.sh`

Its also possible to run single tests or other test groups. After compilation the `tests` program is available in the `/build/bin` directory.

`tests -?` displays help

`tests -l` lists all test cases

`test hello` runs a single test case (e.g. hello test)

## Alternative build methods

### Documentation build

The Doxygen source generated documentation is built automatically from every build script if CMake is able to find an installed version of Doxygen on your system.
