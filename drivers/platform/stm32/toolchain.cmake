message(STATUS "STM32")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_package(STM32CubeMX REQUIRED)

if(${USE_VENDOR_TOOLCHAIN})
  find_package(STM32CubeIDE REQUIRED)
endif()

if (${USE_VENDOR_TOOLCHAIN})

# Specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_LINKER arm-none-eabi-ld)

else()

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_LINKER arm-none-eabi-ld)

endif()

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -MD -g3")
set(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_EXE_LINKER_FLAGS "-mthumb -mcpu=cortex-m4 -specs=nosys.specs -Wl,--gc-sections -mfpu=fpv4-sp-d16 ${MCU_LINKER_FLAGS} \
                            --entry=Reset_Handler" CACHE STRING "Linker flags for MCU" FORCE)

set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")
