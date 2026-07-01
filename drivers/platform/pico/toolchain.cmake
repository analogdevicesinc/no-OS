message(STATUS "Pico Platform")

# The pico-sdk is vendored as the git submodule libraries/pico-sdk. Pin
# PICO_SDK_PATH to this in-tree copy (relative to this toolchain file:
# drivers/platform/pico -> repo root) and ignore any externally exported value,
# so a stale PICO_SDK_PATH left in the environment cannot break the build.
cmake_path(SET PICO_SDK_PATH NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../../..")
cmake_path(APPEND PICO_SDK_PATH "libraries" "pico-sdk")
set(PICO_SDK_PATH "${PICO_SDK_PATH}" CACHE PATH "Path to the vendored pico-sdk" FORCE)

# Surface a missing submodule here, with the fix, rather than failing later with
# a missing pico header during compilation.
if(NOT EXISTS "${PICO_SDK_PATH}/pico_sdk_init.cmake")
    message(FATAL_ERROR
        "pico-sdk not found at ${PICO_SDK_PATH}. Initialize the submodule from "
        "the repository root with:\n"
        "    git submodule update --init libraries/pico-sdk")
endif()

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
set(CMAKE_ASM_FLAGS "${COMMON_CPU_FLAGS} -x assembler-with-cpp" CACHE STRING "ASM compiler flags" FORCE)

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

# The pico-sdk replaces a number of libc/libgcc/soft-float/printf symbols with
# ROM-backed or instrumented implementations via the linker's --wrap mechanism.
# This list is ported verbatim from tools/scripts/pico.mk so the CMake build
# links the same set of wrapped symbols as the proven Make build.
set(PICO_WRAP_SYMBOLS
    sprintf snprintf vsnprintf
    __ctzsi2 __ctzdi2 __popcountsi2 __popcountdi2 __clz __clzl __clzll
    sqrt cos sin tan atan2 exp log ldexp copysign trunc floor ceil round
    sincos asin acos atan sinh cosh tanh asinh acosh atanh exp2 log2 exp10
    log10 pow powint hypot cbrt fmod drem remainder remquo expm1 log1p fma
    __aeabi_fadd __aeabi_fdiv __aeabi_fmul __aeabi_frsub __aeabi_fsub
    __aeabi_cfcmpeq __aeabi_cfrcmple __aeabi_cfcmple __aeabi_fcmpeq
    __aeabi_fcmplt __aeabi_fcmple __aeabi_fcmpge __aeabi_fcmpgt __aeabi_fcmpun
    __aeabi_i2f __aeabi_l2f __aeabi_ui2f __aeabi_ul2f __aeabi_f2iz __aeabi_f2lz
    __aeabi_f2uiz __aeabi_f2ulz __aeabi_f2d
    sqrtf cosf sinf tanf atan2f expf logf ldexpf copysignf truncf floorf ceilf
    roundf sincosf asinf acosf atanf sinhf coshf tanhf asinhf acoshf atanhf
    exp2f log2f exp10f log10f powf powintf hypotf cbrtf fmodf dremf remainderf
    remquof expm1f log1pf fmaf
    malloc calloc free
    _memcpy _memset __aeabi_memcpy __aeabi_memset __aeabi_memcpy4
    __aeabi_memset4 __aeabi_memcpy8 __aeabi_memset8
    printf vprintf puts putchar getchar
)
set(PICO_WRAP_FLAGS "")
foreach(_sym ${PICO_WRAP_SYMBOLS})
    string(APPEND PICO_WRAP_FLAGS " -Wl,--wrap=${_sym}")
endforeach()

# Linker flags common to all build types. The linker script (-T) and the -L
# search path for the generated pico_flash_region.ld are target-specific and are
# applied per-target in config_pico_sdk() (cmake/pico/pico_platform_sdk.cmake).
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_CPU_FLAGS} --specs=nosys.specs \
-Wl,--gc-sections -Wl,--build-id=none -Wl,-z,max-page-size=4096${PICO_WRAP_FLAGS}" CACHE STRING "Linker flags for MCU" FORCE)

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
