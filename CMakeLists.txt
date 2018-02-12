cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
enable_testing()

project(EAARLIO
    VERSION 1.0.0
    LANGUAGES C)

include(Common)
set(VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE EAARLIO_VERSION_GIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
endif()

# For release versions, comment out EAARLIO_VERSION_PRERELEASE so that it is
# undefined. If this is not a release version, then this should be defined.
#set(EAARLIO_VERSION_PRERELEASE "alpha")

if(EAARLIO_VERSION_PRERELEASE)
    set(EAARLIO_VERSION_FULL "${EAARLIO_VERSION}-${EAARLIO_VERSION_PRERELEASE}")
else(EAARLIO_VERSION_PRERELEASE)
    set(EAARLIO_VERSION_FULL "${EAARLIO_VERSION}")
endif(EAARLIO_VERSION_PRERELEASE)

if(EAARLIO_VERSION_GIT)
    set(EAARLIO_VERSION_FULL "${EAARLIO_VERSION_FULL}+${EAARLIO_VERSION_GIT}")
endif()

add_subdirectory("library")
add_subdirectory("programs")
add_subdirectory("examples")
add_subdirectory("doc")
