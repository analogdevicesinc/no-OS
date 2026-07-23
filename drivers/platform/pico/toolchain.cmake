message(STATUS "Pico Platform")

# Resolve PICO_SDK_PATH with clone-once semantics. This toolchain file runs before
# project(), so FetchContent isn't available; resolve directly here.
# Priority: NO_OS_PICO_SDK_DIR override -> in-tree libraries/pico-sdk (cloned once
# by default) -> version-keyed shared cache when NO_OS_CACHE_DIR is set.
cmake_path(SET _no_os_root NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(_pico_version "1.5.1")

if(DEFINED NO_OS_PICO_SDK_DIR AND NOT "${NO_OS_PICO_SDK_DIR}" STREQUAL "")
    set(PICO_SDK_PATH "${NO_OS_PICO_SDK_DIR}")
elseif(DEFINED ENV{NO_OS_PICO_SDK_DIR} AND NOT "$ENV{NO_OS_PICO_SDK_DIR}" STREQUAL "")
    set(PICO_SDK_PATH "$ENV{NO_OS_PICO_SDK_DIR}")
else()
    if(DEFINED ENV{NO_OS_CACHE_DIR} AND NOT "$ENV{NO_OS_CACHE_DIR}" STREQUAL "")
        set(PICO_SDK_PATH "$ENV{NO_OS_CACHE_DIR}/pico-sdk/${_pico_version}")
    else()
        set(PICO_SDK_PATH "${_no_os_root}/libraries/pico-sdk")
    endif()
    if(NOT EXISTS "${PICO_SDK_PATH}/pico_sdk_init.cmake")
        find_package(Git REQUIRED)
        cmake_path(GET PICO_SDK_PATH PARENT_PATH _pico_parent)
        file(MAKE_DIRECTORY "${_pico_parent}")
        # No --recurse-submodules: an RP2040 no-OS build does not need the SDK's
        # own submodules (tinyusb, cyw43, lwip, ...); mirrors the documented
        # `git submodule update --init libraries/pico-sdk` (non-recursive).
        message(STATUS "Cloning pico-sdk ${_pico_version} to ${PICO_SDK_PATH}")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone --depth 1 --branch ${_pico_version}
                    https://github.com/raspberrypi/pico-sdk.git "${PICO_SDK_PATH}"
            RESULT_VARIABLE _pico_clone_res)
        if(NOT _pico_clone_res EQUAL 0)
            message(FATAL_ERROR "Failed to clone pico-sdk to ${PICO_SDK_PATH}")
        endif()
    endif()
endif()

set(PICO_SDK_PATH "${PICO_SDK_PATH}" CACHE PATH "Path to the pico-sdk" FORCE)

if(NOT EXISTS "${PICO_SDK_PATH}/pico_sdk_init.cmake")
    message(FATAL_ERROR "pico-sdk not found at ${PICO_SDK_PATH}")
endif()

# no-OS drives the toolchain itself (Generic bare-metal), rather than letting the
# pico-sdk pick its own. Because CMAKE_TOOLCHAIN_FILE is already set when
# pico_sdk_init() runs, the SDK's pre-load leaves our toolchain in place. This
# also means the SDK's cmake/Platform/PICO.cmake (loaded only for
# CMAKE_SYSTEM_NAME=PICO) is intentionally bypassed; the few things it would set
# (e.g. the .elf suffix) are already configured below.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# For bare-metal toolchains, only compile (not link) during CMake's compiler
# test. This avoids linker failures caused by missing startup files, linker
# scripts and syscall stubs during the detection phase.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# The pico cross toolchain is plain arm-none-eabi-* from PATH (the pico-sdk is
# vendored in-tree, there is no bundled vendor toolchain to discover).
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_LINKER arm-none-eabi-ld)
set(CMAKE_SIZE arm-none-eabi-size)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# Common flags for all build types (RP2040 is a Cortex-M0+).
set(COMMON_CPU_FLAGS "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_C_FLAGS "${COMMON_CPU_FLAGS} -ffunction-sections -fdata-sections -std=gnu11 -MD" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "${COMMON_CPU_FLAGS} -ffunction-sections -fdata-sections -MD" CACHE STRING "C++ compiler flags" FORCE)
# No -x assembler-with-cpp: all pico asm is uppercase .S, which gcc preprocesses
# automatically by extension. Forcing the language also leaks onto CMake's ASM
# *link* line for the SDK's boot_stage2 executable, making gcc try to re-assemble
# the linked object and fail.
set(CMAKE_ASM_FLAGS "${COMMON_CPU_FLAGS}" CACHE STRING "ASM compiler flags" FORCE)

# Debug build flags - Full debug info, no optimization
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG" CACHE STRING "C compiler flags for Debug" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG" CACHE STRING "C++ compiler flags for Debug" FORCE)
set(CMAKE_ASM_FLAGS_DEBUG "-g3" CACHE STRING "ASM compiler flags for Debug" FORCE)

# Release build flags - Optimize for speed, disable assertions
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C compiler flags for Release" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C++ compiler flags for Release" FORCE)
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE STRING "ASM compiler flags for Release" FORCE)

# RelWithDebInfo build flags - Optimize with debug info
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "C compiler flags for RelWithDebInfo" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "C++ compiler flags for RelWithDebInfo" FORCE)
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO "-g" CACHE STRING "ASM compiler flags for RelWithDebInfo" FORCE)

# MinSizeRel build flags - Optimize for size
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C compiler flags for MinSizeRel" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C++ compiler flags for MinSizeRel" FORCE)
set(CMAKE_ASM_FLAGS_MINSIZEREL "" CACHE STRING "ASM compiler flags for MinSizeRel" FORCE)

# The libc/libgcc/soft-float/printf --wrap options are supplied transitively by
# the pico-sdk libraries linked in drivers/platform/pico/CMakeLists.txt: each
# owning lib (pico_printf, pico_malloc, pico_mem_ops, pico_float, pico_double,
# pico_bit_ops, pico_int64_ops, pico_divider, pico_stdio) adds its own
# --wrap=<sym> via pico_wrap_function, so no hand-maintained wrap list is needed.

# Linker flags common to all build types. The linker script (-T) and the -L
# search path for the flash region come from the SDK's pico_standard_link library,
# linked onto the executable in config_pico_sdk() (cmake/pico/pico_platform_sdk.cmake).
#
# These global flags are inherited by every executable, including the SDK's own
# internal boot_stage2 ELF, so they must be safe for that tiny freestanding link:
#   - nosys.specs is intentionally NOT set: the pico-sdk provides its own crt0,
#     syscall stubs and libc interface (pico_crt0 / pico_clib_interface /
#     pico_runtime), and adding it breaks the boot2 link with a duplicate spec.
#   - --gc-sections is intentionally NOT set here: it would strip every section
#     from the boot2 ELF (its linker script has no ENTRY/KEEP). The application
#     still gets --gc-sections from the no-os target (root CMakeLists) and from the
#     SDK's pico_standard_link library.
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_CPU_FLAGS} \
-Wl,--build-id=none -Wl,-z,max-page-size=4096" CACHE STRING "Linker flags for MCU" FORCE)

# Find OpenOCD for the default cmsis-dap (picoprobe) flash/debug probe.
find_program(OPENOCD_PATH
    NAMES openocd
    PATH_SUFFIXES bin
    DOC "Path to OpenOCD executable"
)
if(OPENOCD_PATH)
    # add_openocd_flash_target() always passes "-s ${OPENOCD_SCRIPTS}", so
    # derive the scripts directory from the OpenOCD install prefix to avoid a
    # bare "-s" on the command line.
    cmake_path(GET OPENOCD_PATH PARENT_PATH _ocd_bin_dir)
    cmake_path(GET _ocd_bin_dir PARENT_PATH _ocd_prefix)
    cmake_path(SET _ocd_scripts NORMALIZE "${_ocd_prefix}/share/openocd/scripts")
    if(EXISTS "${_ocd_scripts}")
        set(OPENOCD_SCRIPTS "${_ocd_scripts}")
    endif()
    message(STATUS "Found OpenOCD: ${OPENOCD_PATH}")
else()
    message(STATUS "OpenOCD not found")
endif()

if(NOT PROBE)
    set(PROBE "openocd")
endif()
