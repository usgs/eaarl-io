set(EAARLIO_LIBRARY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(EAARLIO_LIBRARY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

if(${MSVC})
    # Visual Studio recommends fopen_s instead of fopen, but that's not
    # portable. This suppresses the warning.
    add_definitions (-D_CRT_SECURE_NO_DEPRECATE)
endif(${MSVC})

configure_file(
    "public/eaarlio/version.h.in"
    "public/eaarlio/version.h"
    @ONLY
    )

set(EAARLIO_LIBRARY_SRCS
    private/edb.c
    private/edb_decode.c
    private/edb_encode.c
    private/edb_read.c
    private/edb_write.c
    private/error.c
    private/file_flight.c
    private/file_stream.c
    private/file_tld_opener.c
    private/flight.c
    private/int_decode.c
    private/int_encode.c
    private/memory_stdlib.c
    private/memory_support.c
    private/misc_support.c
    private/pulse.c
    private/raster.c
    private/stream_support.c
    private/tld_decode.c
    private/tld_encode.c
    private/tld_pack.c
    private/tld_read.c
    private/tld_size.c
    private/tld_unpack.c
    private/tld_write.c
    private/units.c
    )

set(EAARLIO_LIBRARY_HDRS_PUB
    public/eaarlio/edb.h
    public/eaarlio/error.h
    public/eaarlio/file.h
    public/eaarlio/flight.h
    public/eaarlio/memory.h
    public/eaarlio/pulse.h
    public/eaarlio/raster.h
    public/eaarlio/stream.h
    public/eaarlio/tld.h
    public/eaarlio/tld_opener.h
    public/eaarlio/units.h
    )

set(EAARLIO_LIBRARY_HDRS_PRIV
    private/eaarlio/edb_decode.h
    private/eaarlio/edb_encode.h
    private/eaarlio/edb_internals.h
    private/eaarlio/edb_read.h
    private/eaarlio/edb_write.h
    private/eaarlio/int_decode.h
    private/eaarlio/int_encode.h
    private/eaarlio/memory_support.h
    private/eaarlio/misc_support.h
    private/eaarlio/stream_support.h
    private/eaarlio/tld_constants.h
    private/eaarlio/tld_decode.h
    private/eaarlio/tld_encode.h
    private/eaarlio/tld_pack.h
    private/eaarlio/tld_size.h
    private/eaarlio/tld_unpack.h
    )

source_group("Source Files"
    FILES ${EAARLIO_LIBRARY_SRCS})
source_group("Public Header Files"
    FILES ${EAARLIO_LIBRARY_HDRS_PUB} public/eaarlio/version.h.in)
source_group("Private Header Files"
    FILES ${EAARLIO_LIBRARY_HDRS_PRIV})

add_library(eaarlio ${EAARLIO_LIBRARY_SRCS})

set_property(
    TARGET eaarlio
    PROPERTY PUBLIC_HEADER ${EAARLIO_LIBRARY_HDRS_PUB}
        "${EAARLIO_LIBRARY_BINARY_DIR}/public/eaarlio/version.h"
)

install(
    TARGETS eaarlio
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/eaarlio"
    )

target_include_directories(eaarlio
    PUBLIC "${EAARLIO_LIBRARY_SOURCE_DIR}/public" "${EAARLIO_LIBRARY_BINARY_DIR}/public"
    PRIVATE "${EAARLIO_LIBRARY_SOURCE_DIR}/private")

add_subdirectory(tests)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    eaarlioConfig.cmake.in ${EAARLIO_LIBRARY_BINARY_DIR}/eaarlioConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/eaarlio/cmake
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)
write_basic_package_version_file(
    ${EAARLIO_LIBRARY_BINARY_DIR}/eaarlioConfigVersion.cmake
    VERSION ${EAARLIO_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${EAARLIO_LIBRARY_BINARY_DIR}/eaarlioConfig.cmake
        ${EAARLIO_LIBRARY_BINARY_DIR}/eaarlioConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/eaarlio/cmake
)
