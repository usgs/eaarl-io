# Use solution folders (MSVC)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Don't group predefined targets into a special folder
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

# Sets up some helpful variables
include(GNUInstallDirs)

# Reveal errors and warnings
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC detected, adding compiler flags")
    # -std=c99 specifies that we're targeting C99
    # -Wall enables "all" warnings, which is a misnomer because it isn't
    #   actually all of them
    # -Wextra enables some of the warnings that -Wall doesn't include
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c99 -pedantic -Wall -Wextra \
        -Werror -pedantic-errors")
endif()
if(${MSVC})
    message(STATUS "MSVC detected, adding compiler flags")
    # /W4 enables all warnings through "level 4"
    # The /wd options disable specific warnings:
    #   4127 is "conditional expression is constant", which trips on any macro
    #       of the form do { ... } while(0)
    #   4130 is "logical operation on address of string constant", which trips
    #       on one of SPUT's macros
    #   4204 is "nonstandard extension used: non-constant aggregate initializer",
    #       which is permitted because we're targetting C99
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W4 /wd4127 /wd4130 /wd4204")
endif()

# Build types
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES Debug Release Coverage)
    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
        "Reset the configurations to those needed by eaarl-io"
        FORCE)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_C_FLAGS_COVERAGE
    "${CMAKE_C_FLAGS_DEBUG} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
