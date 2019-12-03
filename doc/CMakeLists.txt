cmake_minimum_required(VERSION 3.1.3)
project(doc)

find_program(DOXYGEN_PATH doxygen)
if (NOT DOXYGEN_PATH)
    message(FATAL_ERROR "Doxygen not found!")
endif()


set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(DOXY_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen_doc)


set(PROJECT_NAME "No-OS")
set(DOCUMENTED_FILES "${SOURCES_DIR}/drivers/platform/generic/spi.c \\
                      "
)


configure_file(
    Doxyfile_API.in
    ${DOXY_WORKING_DIR}/Doxyfile.doxy
)


add_custom_target(doc
    COMMAND ${DOXYGEN_PATH} Doxyfile.doxy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXY_WORKING_DIR}/html
    WORKING_DIRECTORY ${DOXY_WORKING_DIR}
)
