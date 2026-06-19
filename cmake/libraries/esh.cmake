# esh.cmake - Embedded Shell library with hybrid caching
# Priority: Submodule -> Global Cache -> FetchContent
#
# esh has no CMakeLists.txt, so we compile the source files directly.
include(LibraryCacheUtils)

if(NOT DEFINED CONFIG_ESH_VERSION OR "${CONFIG_ESH_VERSION}" STREQUAL "")
    set(CONFIG_ESH_VERSION "master")
endif()

message(STATUS "esh requested version: ${CONFIG_ESH_VERSION}")

resolve_library_source(
    esh
    "${CONFIG_ESH_VERSION}"
    "${NO_OS_DIR}/libraries/esh/esh"
    "https://github.com/google/esh.git"
    ESH_SOURCE_DIR
    ESH_BINARY_DIR
)

set(ESH_WRAPPER_DIR "${NO_OS_DIR}/libraries/esh")

file(GLOB_RECURSE ESH_SHELL_SRCS
    "${ESH_SOURCE_DIR}/shell/shell/*.c"
    "${ESH_SOURCE_DIR}/shell/lib/a2i/*.c"
    "${ESH_SOURCE_DIR}/shell/lib/memlog/*.c"
    "${ESH_SOURCE_DIR}/shell/lib/printf/*.c"
    "${ESH_SOURCE_DIR}/shell/lib/regs/*.c"
    "${ESH_SOURCE_DIR}/shell/lib/utils/*.c"
)

target_sources(no-os PRIVATE
    ${ESH_WRAPPER_DIR}/no_os_esh.c
    ${ESH_SHELL_SRCS}
)

target_include_directories(no-os PUBLIC
    ${ESH_WRAPPER_DIR}
    ${ESH_SOURCE_DIR}/shell/shell
    ${ESH_SOURCE_DIR}/shell/lib/a2i
    ${ESH_SOURCE_DIR}/shell/lib/memlog
    ${ESH_SOURCE_DIR}/shell/lib/printf
    ${ESH_SOURCE_DIR}/shell/lib/regs
    ${ESH_SOURCE_DIR}/shell/lib/utils
)

target_compile_definitions(no-os PRIVATE
    __PROMPT__="esh>"
    ECHO_INIT_VALUE=1
)

# esh keeps its command/auto-load tables in dedicated sections (.cmd_list/.auto_list)
# whose boundary symbols (__CMD_TABLE_START__/__AUTO_TABLE_START__) are defined by a
# supplemental linker script. GNU ld treats additional -T scripts as additive, so this
# augments the platform's main linker script rather than replacing it. Propagated as an
# INTERFACE option so it reaches the final executable link (no-os is a static library).
target_link_options(no-os INTERFACE
    "-T${ESH_WRAPPER_DIR}/esh_linker.ld"
)
set_target_properties(no-os PROPERTIES
    INTERFACE_LINK_DEPENDS "${ESH_WRAPPER_DIR}/esh_linker.ld"
)

message(STATUS "esh configured from: ${ESH_SOURCE_DIR}")
