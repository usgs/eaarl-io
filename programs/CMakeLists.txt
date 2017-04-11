if(${MSVC})
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
endif(${MSVC})

include_directories("${VENDOR_DIR}/argtable3")

include_directories("${EAARLIO_LIBRARY_SOURCE_DIR}/public")
include_directories("${EAARLIO_LIBRARY_BINARY_DIR}/public")

set(EAARLIO_PROGRAMS
    eaarlio_edb_create
    eaarlio_edb_offset
    eaarlio_yaml)

add_custom_target(programs)
set_target_properties(programs PROPERTIES FOLDER programs)

foreach(PROGRAM ${EAARLIO_PROGRAMS})
    add_executable(
        ${PROGRAM}
        "${PROGRAM}.c"
        "${VENDOR_DIR}/argtable3/argtable3.c")
    target_link_libraries(${PROGRAM} eaarlio)
    add_dependencies(programs ${PROGRAM})
    set_target_properties(${PROGRAM} PROPERTIES FOLDER programs)
endforeach()

install(
    TARGETS ${PROGRAMS}
    DESTINATION bin
    OPTIONAL)
