message(STATUS "STM32 Platform")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(STM32CubeMX REQUIRED)

if(USE_VENDOR_TOOLCHAIN)
    find_package(STM32CubeIDE REQUIRED)
endif()

# Find OpenOCD — prefer CubeIDE-bundled version, then system
if(DEFINED CUBEIDE_DIR)
    file(GLOB_RECURSE _cubeide_openocd ${CUBEIDE_DIR}/plugins/*/openocd)
    if(_cubeide_openocd)
        list(GET _cubeide_openocd 0 OPENOCD_PATH)
        # Scripts dir sits next to the binary's parent
        file(GLOB_RECURSE _cubeide_scripts ${CUBEIDE_DIR}/plugins/*/mem_helper.tcl)
        if(_cubeide_scripts)
            list(GET _cubeide_scripts 0 _scripts_file)
            cmake_path(GET _scripts_file PARENT_PATH OPENOCD_SCRIPTS)
        endif()
        message(STATUS "Found CubeIDE-bundled OpenOCD: ${OPENOCD_PATH}")
    endif()
endif()

if(NOT OPENOCD_PATH)
    find_program(OPENOCD_PATH
        NAMES openocd
        PATH_SUFFIXES bin
        DOC "Path to OpenOCD executable"
    )
    if(OPENOCD_PATH)
        # Resolve scripts dir relative to the binary
        cmake_path(GET OPENOCD_PATH PARENT_PATH _ocd_bin_dir)
        cmake_path(GET _ocd_bin_dir PARENT_PATH _ocd_prefix)
        cmake_path(SET _ocd_scripts NORMALIZE "${_ocd_prefix}/share/openocd/scripts")
        if(EXISTS "${_ocd_scripts}")
            set(OPENOCD_SCRIPTS "${_ocd_scripts}")
        endif()
        message(STATUS "Found system OpenOCD: ${OPENOCD_PATH}")
    else()
        message(STATUS "OpenOCD not found")
    endif()
endif()

if(OPENOCD_PATH)
    set(OPENOCD_INTERFACE "interface/stlink.cfg")
endif()

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_LINKER arm-none-eabi-ld)
set(CMAKE_SIZE arm-none-eabi-size)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# Detect MCU type and set appropriate flags
if(${TARGET} MATCHES "^stm32f7")
    # Cortex-M7 MCUs (STM32F7xx)
    set(STM32_MCU_FLAGS "-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16")
    message(STATUS "Detected Cortex-M7 MCU: ${TARGET}")
elseif(${TARGET} MATCHES "^stm32h7")
    # Cortex-M7 MCUs (STM32H7xx)
    set(STM32_MCU_FLAGS "-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16")
    message(STATUS "Detected Cortex-M7 MCU: ${TARGET}")
elseif(${TARGET} MATCHES "^stm32f4")
    # Cortex-M4 MCUs (STM32F4xx)
    set(STM32_MCU_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
    message(STATUS "Detected Cortex-M4 MCU: ${TARGET}")
elseif(${TARGET} MATCHES "^stm32l4")
    # Cortex-M4 MCUs (STM32L4xx)
    set(STM32_MCU_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
    message(STATUS "Detected Cortex-M4 MCU: ${TARGET}")
else()
    # Default to Cortex-M4 for unknown MCUs
    set(STM32_MCU_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
    message(STATUS "Unknown MCU ${TARGET}, defaulting to Cortex-M4 flags")
endif()

# Common flags for all build types
set(CMAKE_C_FLAGS "${STM32_MCU_FLAGS} -ffunction-sections -fdata-sections -MD" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "${STM32_MCU_FLAGS} -ffunction-sections -fdata-sections -MD" CACHE STRING "C++ compiler flags" FORCE)
set(CMAKE_ASM_FLAGS "${STM32_MCU_FLAGS} -x assembler-with-cpp" CACHE STRING "ASM compiler flags" FORCE)

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

# Linker flags (common for all build types)
set(CMAKE_EXE_LINKER_FLAGS "${STM32_MCU_FLAGS} -specs=nosys.specs -Wl,--gc-sections ${MCU_LINKER_FLAGS} --entry=Reset_Handler" CACHE STRING "Linker flags for MCU" FORCE)

if (NOT PROBE)
    set(PROBE "openocd")
endif()