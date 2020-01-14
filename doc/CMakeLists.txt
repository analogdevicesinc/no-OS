cmake_minimum_required(VERSION 3.1.3)
project(doc)

find_program(DOXYGEN_PATH doxygen)
if (NOT DOXYGEN_PATH)
    message(FATAL_ERROR "Doxygen not found!")
endif()

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
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
    ${SOURCES_DIR}/drivers/accel \\
    ${SOURCES_DIR}/drivers/adc \\
    ${SOURCES_DIR}/drivers/adc-dac  \\
    ${SOURCES_DIR}/drivers/afe \\
    ${SOURCES_DIR}/drivers/axi_core \\
    ${SOURCES_DIR}/drivers/cdc \\
    ${SOURCES_DIR}/drivers/dac \\
    ${SOURCES_DIR}/drivers/frequency \\
    ${SOURCES_DIR}/drivers/gyro \\
    ${SOURCES_DIR}/drivers/impedance-analyzer \\
    ${SOURCES_DIR}/drivers/io-expander \\
    ${SOURCES_DIR}/drivers/mux \\
    ${SOURCES_DIR}/drivers/platform \\
    ${SOURCES_DIR}/drivers/photo-electronic \\
    ${SOURCES_DIR}/drivers/potentiometer \\
    ${SOURCES_DIR}/drivers/rf-transceiver/adf7023 \\
    ${SOURCES_DIR}/drivers/sd-card \\
    ${SOURCES_DIR}/drivers/temperature \\
    ${SOURCES_DIR}/projects \\
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
