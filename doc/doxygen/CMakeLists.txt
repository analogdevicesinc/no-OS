cmake_minimum_required(VERSION 3.1.3)
project(doc)

find_program(DOXYGEN_PATH doxygen)
if (NOT DOXYGEN_PATH)
    message(FATAL_ERROR "Doxygen not found!")
endif()

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)
set(DOXY_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen_doc)

set(PROJECT_NAME "no-OS")

#Source files paths to be included for documentation generation
set(DOCUMENTED_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/projects_page.dox \\
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers_page.dox \\
    ${CMAKE_CURRENT_SOURCE_DIR}/license_page.dox \\
    ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.dox \\
    ${SOURCES_DIR}/include \\
    ${SOURCES_DIR}/iio \\
    ${SOURCES_DIR}/drivers \\
    ${SOURCES_DIR}/projects \\
 ")

set(EXCLUDE_FILES
    "${SOURCES_DIR}/drivers/rf-transceiver/talise \\
    ${SOURCES_DIR}/drivers/rf-transceiver/navassa \\
    ${SOURCES_DIR}/drivers/rf-transceiver/madura \\
    ${SOURCES_DIR}/drivers/adc/ad9081/api \\
    ${SOURCES_DIR}/drivers/adc/ad9083/ad9083_api \\
    ${SOURCES_DIR}/drivers/afe/ad5940 \\
")

set(EXCLUDE_PATT
    "*/hal/stm32/* \\
")

configure_file(
    DoxygenLayoutIn.xml
    ${DOXY_WORKING_DIR}/DoxygenLayout.xml
)

configure_file(
    Doxyfile.in
    ${DOXY_WORKING_DIR}/Doxyfile.doxy
)

add_custom_target(doc
    COMMAND ${DOXYGEN_PATH} Doxyfile.doxy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXY_WORKING_DIR}/html
    WORKING_DIRECTORY ${DOXY_WORKING_DIR}
)
