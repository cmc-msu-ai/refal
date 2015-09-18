[![Refal2 Logo](http://refal2.github.io/images/refal2_logo.png)](http://refal2.github.io)

[![Build Status](https://travis-ci.org/cmc-msu-ai/refal.svg?branch=master)](https://travis-ci.org/cmc-msu-ai/refal)

## Supported Platforms

MS Windows, Mac OS X, Linux

Other UNIX-like operating systems may work too out of the box.

## Building Refal2

- Download and install las version of [CMake](http://www.cmake.org/download/)
- Download the source code from this repository
- Run command line shell and navigate up to the root folder of downloaded data
- Create project for your IDE (or just makefile)
```sh
# Create `build` folder
mkdir build; cd build

# You can use all cmake generators available on your platform
# For example, next command create solution for "Microsoft Visual Studio 2010"
cmake -G "Visual Studio 10 2010" ../
```
- Create executable file with your IDE (`Release` mode highly recommended)

## Unsing

Just run executable file `refal2`
```sh
# Run Refal2 with stdin for enter source code
./refal2

# Learn more about available command line options
./refal2 --help
```

## Reporting Bugs

Please report unacceptable behavior to refal2@yandex.ru.
