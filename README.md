# EAARL Input/Output Library

This library provides a C API that you can use within your own programs for I/O
access to raw data collected by the EAARL-A and EAARL-B lidar systems. As such,
the primary intended audience for this library are users who are need to add
EAARL I/O capabilities to their existing applications.

A few small standalone utility programs are also included that can be used to
inspect EAARL data. One of the program exports the data as YAML, a common
human-readable data format. This provides users another avenue for getting
EAARL data into their existing applications.

The EAARL sensors were operational between 2001 and 2014. This library is part
of a final effort to make EAARL data acquisitions more accessible to the
public. The USGS has no plans for further development or support of EAARL or of
its associated software (including this library).

## Building

You will need [CMake](https://cmake.org) to generate build files for your
platform. CMake is used to support cross-platform build configuration.

The basic steps to compile on Linux using gcc and make:

```sh
mkdir build
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

## License

This software is licensed under [CC0 1.0] and is in the [public domain] because
it contains materials that originally came from the [U.S. Geological Survey
(USGS)], an agency of the [United States Department of Interior]. For more
information, see the [official USGS copyright policy].

[CC0 1.0]: http://creativecommons.org/publicdomain/zero/1.0/
[public domain]: https://en.wikipedia.org/wiki/Public_domain
[U.S. Geological Survey (USGS)]: https://www.usgs.gov/
[United States Department of Interior]: https://www.doi.gov/
[official USGS copyright policy]: http://www.usgs.gov/information-policies-and-instructions/copyrights-and-credits

## Disclaimer

This software is preliminary or provisional and is subject to revision. It is
being provided to meet the need for timely best science. The software has not
received final approval by the U.S. Geological Survey (USGS). No warranty,
expressed or implied, is made by the USGS or the U.S. Government as to the
functionality of the software and related material nor shall the fact of
release constitute any such warranty. The software is provided on the condition
that neither the USGS nor the U.S. Government shall be held liable for any
damages resulting from the authorized or unauthorized use of the software.

The USGS provides no warranty, expressed or implied, as to the correctness of
the furnished software or the suitability for any purpose. The software has
been tested, but as with any complex software, there could be undetected
errors. Users who find errors are requested to report them to the USGS.

References to non-USGS products, trade names, and (or) services are provided
for information purposes only and do not constitute endorsement or warranty,
express or implied, by the USGS, U.S. Department of Interior, or U.S.
Government, as to their suitability, content, usefulness, functioning,
completeness, or accuracy.

Although this program has been used by the USGS, no warranty, expressed or
implied, is made by the USGS or the United States Government as to the accuracy
and functioning of the program and related program material nor shall the fact
of distribution constitute any such warranty, and no responsibility is assumed
by the USGS in connection therewith.

This software is provided "AS IS."
