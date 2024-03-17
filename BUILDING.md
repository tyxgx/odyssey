# Building with CMake

## Dependencies
The project depends on `LLVM`, and thus you must have `LLVM` installed on your system
such that the build system can locate it.
For building in Developer mode, `cppcheck` is required.

## Build

This project doesn't require any special command-line flags to build to keep
things simple.

Here are the steps for building in release mode with a single-configuration
generator, like the Unix Makefiles one:

```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```
The CMake build systems supports Ninja as a back-end, and thus the recommended
way to build is to use Ninja:

```sh
cmake -G Ninja -S . -B build
cd build && ninja
```

### Building on Windows

The tests are only run in Unix environments, and there is no ongoing plan for
Windows support implementation. Thus, *the code might not behave as expected for
some possible edge cases when run on Windows*.
The CI does not run tests for Windows, thus there can be some unexpected behavior.

**Note**: You can expect the code to at least _build_ on Windows if you have LLVM headers installed.

### Building on Apple Silicon

CMake supports building on Apple Silicon properly since 3.20.1. Make sure you
have the [latest version][1] installed.

## Install

This project doesn't require any special command-line flags to install to keep
things simple. As a prerequisite, the project has to be built with the above
commands already.

The below commands require at least CMake 3.15 to run, because that is the
version in which [Install a Project][2] was added.

Here is the command for installing the release mode artifacts with a
single-configuration generator, like the Unix Makefiles one:

```sh
cmake --install build
```

[1]: https://cmake.org/download/
[2]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project
