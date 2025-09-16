message(STATUS "STM32 Platform")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(STM32CubeMX REQUIRED)

if(USE_VENDOR_TOOLCHAIN)
    find_package(STM32CubeIDE REQUIRED)
endif()

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_LINKER arm-none-eabi-ld)

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

set(CMAKE_C_FLAGS "${STM32_MCU_FLAGS} -ffunction-sections -fdata-sections -MD -g3")
set(CMAKE_CXX_FLAGS "${STM32_MCU_FLAGS} -ffunction-sections -fdata-sections -MD -g3")
set(CMAKE_ASM_FLAGS "${STM32_MCU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${STM32_MCU_FLAGS} -specs=nosys.specs -Wl,--gc-sections ${MCU_LINKER_FLAGS} --entry=Reset_Handler" CACHE STRING "Linker flags for MCU" FORCE)

# OpenOCD interface configuration
set(OPENOCD_INTERFACE "interface/stlink.cfg")

