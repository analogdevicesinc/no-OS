# lvgl.cmake - LVGL graphics library with hybrid caching
# Priority: Submodule -> Global Cache -> FetchContent
include(LibraryCacheUtils)

if(NOT DEFINED CONFIG_LVGL_VERSION OR "${CONFIG_LVGL_VERSION}" STREQUAL "")
    set(CONFIG_LVGL_VERSION "v8.3.0")
endif()

message(STATUS "LVGL requested version: ${CONFIG_LVGL_VERSION}")

resolve_library_source(
    lvgl
    "${CONFIG_LVGL_VERSION}"
    "${NO_OS_DIR}/libraries/lvgl/lvgl"
    "https://github.com/lvgl/lvgl.git"
    LVGL_SOURCE_DIR
    LVGL_BINARY_DIR
)

# LVGL requires a lv_conf.h configuration file provided by the project
if(NOT DEFINED CONFIG_LVGL_CONF_PATH OR "${CONFIG_LVGL_CONF_PATH}" STREQUAL "")
    message(FATAL_ERROR "CONFIG_LVGL_CONF_PATH must be set to the directory containing lv_conf.h")
endif()

set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON CACHE BOOL "" FORCE)
set(LV_CONF_BUILD_DISABLE_DEMOS ON CACHE BOOL "" FORCE)

# Resolve the project's lv_conf.h directory to an absolute path.
if(IS_ABSOLUTE "${CONFIG_LVGL_CONF_PATH}")
    set(_lvgl_conf_dir "${CONFIG_LVGL_CONF_PATH}")
else()
    set(_lvgl_conf_dir "${CMAKE_SOURCE_DIR}/${CONFIG_LVGL_CONF_PATH}")
endif()

set(LV_BUILD_CONF_DIR "${_lvgl_conf_dir}" CACHE PATH "" FORCE)

if(NOT TARGET lvgl)
    add_subdirectory("${LVGL_SOURCE_DIR}" "${LVGL_BINARY_DIR}" EXCLUDE_FROM_ALL)
endif()

target_compile_definitions(lvgl PUBLIC LV_CONF_INCLUDE_SIMPLE)

# Make the project's lv_conf.h visible to lvgl
target_include_directories(lvgl PUBLIC "${_lvgl_conf_dir}")

target_include_directories(lvgl PUBLIC
    ${LVGL_SOURCE_DIR}/src/misc
    ${LVGL_SOURCE_DIR}/src/display
)

target_link_libraries(no-os PUBLIC lvgl)

message(STATUS "LVGL configured from: ${LVGL_SOURCE_DIR}")
