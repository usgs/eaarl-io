cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
enable_testing()

project(EAARLIO
    VERSION 0.1.0
    LANGUAGES C)

include(Common)
set(VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")

# For release versions, comment out EAARLIO_VERSION_PRERELEASE so that it is
# undefined. If this is not a release version, then this should be defined.
set(EAARLIO_VERSION_PRERELEASE "alpha")

if(EAARLIO_VERSION_PRERELEASE)
    set(EAARLIO_VERSION_FULL "${EAARLIO_VERSION}-${EAARLIO_VERSION_PRERELEASE}")
else(EAARLIO_VERSION_PRERELEASE)
    set(EAARLIO_VERSION_FULL "${EAARLIO_VERSION}")
endif(EAARLIO_VERSION_PRERELEASE)

add_subdirectory("library")
add_subdirectory("programs")
add_subdirectory("examples")
add_subdirectory("doc")
