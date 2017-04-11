include_directories("${EAARLIO_LIBRARY_SOURCE_DIR}/public")
include_directories("${EAARLIO_LIBRARY_BINARY_DIR}/public")

set(EAARLIO_EXAMPLES
    example_edb_read
    example_edb_write
    example_errors
    example_flight_read_raster_pulse
    example_flight_read_rasters
    example_file_tld_opener)

add_custom_target(examples)
set_target_properties(examples PROPERTIES FOLDER examples)

foreach(EXAMPLE ${EAARLIO_EXAMPLES})
    add_executable(${EXAMPLE} EXCLUDE_FROM_ALL "${EXAMPLE}.c")
    target_link_libraries(${EXAMPLE} eaarlio)
    add_dependencies(examples ${EXAMPLE})
    set_target_properties(${EXAMPLE} PROPERTIES FOLDER examples)
endforeach()
