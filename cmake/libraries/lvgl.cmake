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

# Use simple include for the config file
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON CACHE BOOL "" FORCE)
set(LV_CONF_BUILD_DISABLE_DEMOS ON CACHE BOOL "" FORCE)

if(NOT TARGET lvgl)
    add_subdirectory("${LVGL_SOURCE_DIR}" "${LVGL_BINARY_DIR}" EXCLUDE_FROM_ALL)
endif()

target_compile_definitions(lvgl PUBLIC LV_CONF_INCLUDE_SIMPLE)

# Make the project's lv_conf.h visible to lvgl
if(IS_ABSOLUTE "${CONFIG_LVGL_CONF_PATH}")
    target_include_directories(lvgl PUBLIC "${CONFIG_LVGL_CONF_PATH}")
else()
    target_include_directories(lvgl PUBLIC "${CMAKE_SOURCE_DIR}/${CONFIG_LVGL_CONF_PATH}")
endif()

target_link_libraries(no-os lvgl)

message(STATUS "LVGL configured from: ${LVGL_SOURCE_DIR}")
