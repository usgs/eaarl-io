include(FindDoxygen)

if(DOXYGEN_FOUND)

    configure_file(
        public.doxygen.in
        ${CMAKE_CURRENT_BINARY_DIR}/public.doxygen
        @ONLY)

    add_custom_target(
        doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/public.doxygen
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation using doxygen" VERBATIM)

    set(INSTALL_DOC_DIR ${PROJECT_BINARY_DIR}/doc/html)
    install(DIRECTORY ${INSTALL_DOC_DIR}
        DESTINATION share/eaarlio-${EAARLIO_VERSION}
        OPTIONAL)

    configure_file(
        private.doxygen.in
        ${CMAKE_CURRENT_BINARY_DIR}/private.doxygen
        @ONLY)

    add_custom_target(
        dev-doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/private.doxygen
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating developer API documentation using doxygen" VERBATIM)

endif()
