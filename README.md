# EAARL Input/Output Library

This library provides I/O routines for working with the raw data collected by
the EAARL-A and EAARL-B lidar systems.

## Building

You will need [CMake](https://cmake.org) to generate build files for your
platform. CMake is used to support cross-platform build configuration.

The basic steps to compile on Linux using gcc and make:

```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

The basic steps to compile on Windows using Visual Studio 2015:

1. Launch the CMake GUI
    1. Select the source code directory ("Where is the source code:")
    2. Select a separate directory for the build ("Where to build the binaries:")
    3. Click "Configure"
    4. Choose your version of Visual Studio under "Specify the generator for this project"
    5. Leave "Use default native compilers" enabled
    6. Click "Finish"
    7. If you plan to install the software, check that CMAKE_INSTALL_PREFIX is set to the destination directory you want to use.
    8. Click "Generate"
2. Launch Visual Studio 2015
    1. File > Open > Project/Solution and select the EAARLIO.sln file in the build directory.
    2. In the Solution Explorer, right-click on ALL_BUILD and select Build.

Refer to [doc/index.md](doc/index.md#building) for more details on building.

## Documentation with Doxygen

The library's documentation is generated using [Doxygen](http://www.stack.nl/~dimitri/doxygen/).
You'll need Doxygen installed in order to build the documentation.

On Linux, you can generate the documentation using make by specifying the doc target.

```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make doc
```

On Windows with Visual Studio, follow the instructions in the Building section above.
For the final step, right-click on doc instead of ALL_BUILD and then select Build.

The documentation will be created as an HTML website in your build directory
under doc/html. Open the file doc/html/index.html to begin browsing the documentation.

## Documentation without Doxygen

You can also directly browse the underlying sources used to generate the Doxygen
documentation.

- [doc/index.md](doc/index.md) is the introductory page.
- [library/public/eaarlio](library/public/eaarlio) contains the header files
  for the library. Each header file is thoroughly documented to provide the
  information that goes into the Doxygen documentation.
- [doc/examples.dox](doc/examples.dox) provides a short description for files that
  serve as good examples to help learn the library. The example files referenced
  can be found elsewhere in the repository.
- [doc/testing.md](doc/testing.md) provides instructions on how to run the test suite.
- [doc/tld_format.md](doc/tld_format.md) and [doc/edb_format.md](doc/edb_format.md)
  document the underlying file formats for TLD and EDB files.
- [doc/developer.md](doc/developer.md) provides some information directed towards
  individuals who wish to contribute to the library.
