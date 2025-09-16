if(DEFINED ENV{MAXIM_LIBRARIES})
    set(MAXIM_LIBRARIES $ENV{MAXIM_LIBRARIES})
else()
    message(FATAL_ERROR "MAXIM_LIBRARIES environment variable is not defined")
endif()

message(STATUS "MAXIM_LIBRARIES: ${MAXIM_LIBRARIES}")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
endif()

if(USE_VENDOR_TOOLCHAIN)
    cmake_path(SET CROSS_COMPILER_BIN NORMALIZE "${MAXIM_LIBRARIES}/../Tools/GNUTools/*/bin")

    find_program(CMAKE_C_COMPILER arm-none-eabi-gcc HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++ HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_LINKER arm-none-eabi-ld HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_SIZE arm-none-eabi-size HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy HINTS ${CROSS_COMPILER_BIN})
else()
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
    set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
    set(CMAKE_LINKER arm-none-eabi-ld)
    set(CMAKE_SIZE arm-none-eabi-size)
    set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
endif()

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -MD -g3")
set(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "-mthumb -mcpu=cortex-m4 -specs=nosys.specs -Wl,--gc-sections -mfpu=fpv4-sp-d16 ${MCU_LINKER_FLAGS} \
    -T${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC/${TARGET}.ld --entry=Reset_Handler" CACHE STRING "Linker flags for MCU" FORCE)

file(GLOB_RECURSE OPENOCD_PATH ${MAXIM_LIBRARIES}/../Tools/*/openocd)
if(OPENOCD_PATH)
    list(GET OPENOCD_PATH 0 OPENOCD_PATH)
    cmake_path(SET OPENOCD_SCRIPTS NORMALIZE "${MAXIM_LIBRARIES}/../Tools/OpenOCD/scripts")

    set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
    set(OPENOCD_CHIPNAME ${TARGET})
    set(OPENOCD_TARGETCFG "target/${TARGET}.cfg")
endif()