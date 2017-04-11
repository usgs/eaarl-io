# Testing {#md_testing}

## Unit tests

Unit tests are configured to run on all platforms.

To run the tests on Linux:

```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make check
```

Note that you can also run tests using `make test`. `make check` and `make
test` are nearly identical. The only difference is that `make check` compiles
the test programs prior to running the tests, whereas `make test` compiles the
test programs as part of the tests. This means that `make check` lets you see
the compilation output whereas `make test` hides it.

Note that you can also run tests using `make test`. There are two differences
between `make test` and `make check`:

1. For `make test`, the test programs are built as the first test to ensure
   they are present and up to date. This hides any error output that occur
   during compilation. With `make check`, the compilation happens before the
   tests are run so the output is visible.

2. With `make test`, failing tests programs are noted but you do not see the
   failing test output. With `make check`, tests that fail will have their full
   output displayed so that you can see which tests in the test program failed.

## Memory tests

Memory testing is only configured to run on Linux using valgrind.

To run valgrind tests:

```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make valgrind
```

## Code coverage

Code coverage is only configured to run on Linux using gcov.

Commands to generate coverage data:

```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Coverage
make check
```

To view line coverage using gcovr:

```sh
gcovr -r ../library/private --object-directory=. -s
```

Similarly, branch coverage:

```sh
gcovr -r ../library/private --object-directory=. -s -b
```

To generate an HTML report in a subdirectory called cover:

```sh
gcovr -r ../library/private --object-directory=. --html --html-details -o cover/cover.html
```
