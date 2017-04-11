add_definitions(-DDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions(-DTEMPDIR="${CMAKE_CURRENT_BINARY_DIR}")
# Disable use of LONGJMP in Greatest. We don't use it.
add_definitions(-DGREATEST_USE_LONGJMP=0)

include_directories(
    "${VENDOR_DIR}/greatest"
    "${EAARLIO_LIBRARY_SOURCE_DIR}/public"
    "${EAARLIO_LIBRARY_SOURCE_DIR}/private"
    "${EAARLIO_LIBRARY_SOURCE_DIR}/tests"
    )

add_library(eaarlio-test
    EXCLUDE_FROM_ALL
    assert_error.c
    data_int.c
    mock_memory.c
    mock_stream.c
    util_tempfile.c
    )
target_link_libraries(eaarlio-test eaarlio)
set_target_properties(eaarlio-test PROPERTIES FOLDER tests)

# All test files must be defined here.
set(EAARLIO_TEST_FILES
    test_edb.c
    test_edb_decode.c
    test_edb_encode.c
    test_edb_internals.c
    test_edb_read.c
    test_edb_write.c
    test_error.c
    test_file_flight.c
    test_file_stream.c
    test_file_tld_opener.c
    test_int_decode.c
    test_int_encode.c
    test_memory_support.c
    test_pulse.c
    test_raster.c
    test_tld.c
    test_tld_constants.c
    test_tld_decode.c
    test_tld_encode.c
    test_tld_pack.c
    test_tld_read.c
    test_tld_size.c
    test_tld_unpack.c
    test_tld_write.c
    test_units.c
    )

# Build tests. This is updated to depend on the executable for each test as it
# is created.
add_custom_target(build_tests)
add_dependencies(build_tests eaarlio-test)

# Run tests. This is the same as the "test" target except it depends on the
# "build_tests" target, which means they get compiled *outside* of the test
# environment (which means you see compile errors more easily if they occur).
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)
add_dependencies(check build_tests)

find_program(
    VALGRIND
    NAMES valgrind
    DOC "valgrind executable, used by target valgrind for unit tests"
    )

if(VALGRIND STREQUAL "VALGRIND-NOTFOUND")
    add_custom_target(valgrind
        COMMAND ${CMAKE_COMMAND} -E echo_append
        COMMENT "valgrind is not available on your platform, skipping")
else()
    add_custom_target(valgrind COMMAND ${CMAKE_CTEST_COMMAND} -C Valgrind)
    add_dependencies(valgrind build_tests)
endif()
set_target_properties(valgrind PROPERTIES FOLDER tests)

# Put them in a folder for IDEs
set_target_properties(
    check build_tests
    PROPERTIES FOLDER tests
    )

# Make the "test" target work. This test case builds the tests (if they aren't
# already built). All other tests depend on this so that they get built prior
# to running.
add_test(
    NAME build_tests
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR} --target build_tests --config $<CONFIG>
    )

# Configure each test file.
foreach(FILE ${EAARLIO_TEST_FILES})

    # Remove the suffix to derive a test name
    string(REPLACE ".c" "" NAME ${FILE})

    # Create the executable.
    add_executable(${NAME} EXCLUDE_FROM_ALL ${FILE})
    target_link_libraries(${NAME}
        eaarlio
        eaarlio-test
        $<$<CONFIG:Coverage>:gcov>)
    set_target_properties(
        ${NAME}
        PROPERTIES FOLDER tests
        )

    # Create the test
    add_test(NAME ${NAME} COMMAND ${NAME})
    add_dependencies(build_tests ${NAME})
    set_tests_properties(
        ${NAME}
        PROPERTIES DEPENDS build_tests
        )

    # Create a valgrind test if necessary
    if(NOT VALGRIND STREQUAL "VALGRIND-NOTFOUND")
        add_test(
            NAME "${NAME}_valgrind"
            COMMAND
                ${VALGRIND} --error-exitcode=1 --track-origins=yes
                    --read-var-info=yes --leak-check=full "./${NAME}"
            CONFIGURATIONS Valgrind
        )
        set_tests_properties(
            "${NAME}_valgrind"
            PROPERTIES DEPENDS ${NAME}
            )
    endif()

endforeach(FILE ${EAARLIO_TEST_FILES})

find_library(LIB_MATH NAMES m math)
if(NOT LIB_MATH STREQUAL "LIB_MATH-NOTFOUND")
    target_link_libraries(test_units ${LIB_MATH})
endif()

if(${MSVC})
    # Visual Studio complains about for fopen and sprintf and recommends using
    # MS-specific versions which aren't portable. This suppresses those
    # warnings.
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
endif(${MSVC})
