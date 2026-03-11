cmake_minimum_required(VERSION 3.10)
project(sphinx-doc NONE)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(SPHINX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(SPHINX_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(SPHINX_REQS_FILE "${SPHINX_SOURCE_DIR}/requirements.txt")

# Set up a Python venv for Sphinx dependencies
set(VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/.sphinx_venv")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  set(VENV_PYTHON "${VENV_DIR}/Scripts/python.exe")
  set(VENV_SPHINX_BUILD "${VENV_DIR}/Scripts/sphinx-build.exe")
else()
  set(VENV_PYTHON "${VENV_DIR}/bin/python3")
  set(VENV_SPHINX_BUILD "${VENV_DIR}/bin/sphinx-build")
endif()

# Stamp file to track whether pip install has been run
set(VENV_STAMP "${VENV_DIR}/.stamp")

add_custom_command(
  OUTPUT "${VENV_STAMP}"
  COMMAND ${Python3_EXECUTABLE} -m venv "${VENV_DIR}"
  COMMAND "${VENV_PYTHON}" -m pip install --quiet -r "${SPHINX_REQS_FILE}"
  COMMAND ${CMAKE_COMMAND} -E touch "${VENV_STAMP}"
  DEPENDS "${SPHINX_REQS_FILE}"
  COMMENT "Creating Python venv and installing Sphinx dependencies"
)

add_custom_target(sphinx-html
  COMMAND "${VENV_SPHINX_BUILD}" -M html
    "${SPHINX_SOURCE_DIR}" "${SPHINX_BUILD_DIR}" -j auto
  DEPENDS "${VENV_STAMP}"
  WORKING_DIRECTORY "${SPHINX_SOURCE_DIR}"
  COMMENT "Building Sphinx HTML documentation"
)
