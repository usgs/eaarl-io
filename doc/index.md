# EAARL Input/Output Library {#index}

This library provides I/O routines for working with the raw data collected by
the EAARL-A and EAARL-B lidar systems.

There are two file formats: TLD and EDB. The TLD files contain the raw waveform
data. The EDB file is an index file that is generated to facilitate easy lookup
of individual raster records and also defines the raster numbers of those
records. The structure of these files are described at @ref md_tld_format and
@ref md_edb_format.

## Library Usage

The primary intended use case for this library is to support reading raster
data from TLD files that have an existing EDB index file. Several examples are
included to illustrate this use case:

* @ref example_flight_read_rasters.c shows how to open a dataset using
  ::eaarlio_file_flight and then read multiple rasters in any arbitrary order
  using ::eaarlio_flight_read_raster. It also provides an example of reading
  just the raster headers (excluding the pulse and waveform data) which is much
  faster if you only need access to fields from the header.
* @ref example_flight_read_raster_pulse.c is similar, except it provides an
  example of loading a full raster with its pulse and waveform data instead.
* @ref eaarlio_yaml.c provides a more involved and robust example of using the
  same functionality. This is the source code for the included eaarlio_yaml
  program.

For other use cases, please refer to the rest of the library API documentation
and the other included examples.

### Data Structures

The library defines 9 different data structures. They can be broken out into
four categories.

* Structures for data from TLD files (raw waveform data):
  * ::eaarlio_tld_header, a record header in the TLD file
  * ::eaarlio_raster, the data for a single raster from a TLD file
  * ::eaarlio_pulse, the data for a single pulse in a raster
* Structures for data from EDB files (index data):
  * ::eaarlio_edb, the contents of an EDB file
  * ::eaarlio_edb_record, a record entry corresponding to an indexed raster
* Structures for accessing data:
  * ::eaarlio_flight, which supports reading rasters from an indexed dataset
    via raster number
  * ::eaarlio_stream, which supports read/write access to storage such as files
  * ::eaarlio_tld_opener, which supports opening a stream for given a TLD file
    name
* Structure for memory management:
  * ::eaarlio_memory, which allows you to use define your own memory allocation
    functions if you do not want to use the C standard library implementation

However, for the primary use case of reading raster data using
::eaarlio_file_flight and ::eaarlio_flight_read_raster, you'll only need to be
familiar with 3 structures: ::eaarlio_raster, ::eaarlio_pulse, and
::eaarlio_flight.

In addition to the structures, the library also defines an enum for error
codes: ::eaarlio_error. Most library functions will return an ::eaarlio_error,
where ::EAARLIO_SUCCESS means the function worked as expected and anything else
indicates that it failed in some way. Refer to @ref example_errors.c for
examples of working with error codes.

## Program Usage

The library comes with three utility programs:

* **eaarlio_edb_create** allows you to create an EDB index file for a set of
  TLD files.
* **eaarlio_edb_offset** allows you to check or change the time offset applied
  to an EDB file.
* **eaarlio_yaml** allows you to export selected raster data in YAML format.

For usage on each, run the command with the "-h" option. For example,
`eaarlio_edb_create -h`.

## Building

You will need [CMake](https://cmake.org) to generate build files for your
platform. CMake is used to support cross-platform build configuration.

The primary target platforms tested and developed against are:
- Linux using gcc and make
- Windows using Visual Studio 2015

However, the library is also known to compile on these platforms:
- Linux using clang and make
- Windows under cygwin using gcc and make
- Windows under MinGW using gcc and make

### Linux with gcc and make

The basic steps to compile on Linux using gcc and make:

```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Windows with Visual Studio 2015

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

### Build Targets

The default build target will compile the library and the utility programs. Other
build targets are available:

| Make      | Visual Studio     | Purpose                                                       |
| --------- | ----------------- | ------------------------------------------------------------- |
| all       | ALL_BUILD         | Default; build library and programs                           |
| eaarlio   | eaarlio           | Build just the library                                        |
| programs  | programs/programs | Build just the programs                                       |
| examples  | examples/examples | Build the examples                                            |
| install   | INSTALL           | Install the library and programs                              |
| test      | RUN_TESTS         | Run the test suite; see @ref md_testing                       |
| check     | tests/check       | Run the test suite; see @ref md_testing                       |
| doc       | doc               | Build the public API documentation; requires Doxygen          |
| dev-doc   | dev-doc           | Build the internal developer documentation; requires Doxygen  |
