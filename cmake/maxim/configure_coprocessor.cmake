# Configure CPU1 after the CPU0 premap has determined its flash origin.
include("${LAYOUT_FILE}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${SOURCE_DIR}" -B "${BINARY_DIR}"
    -G "${GENERATOR}"
    -DMAXIM_CORE=riscv "-DMAXIM_LIBRARIES=${SDK_DIR}"
    "-DUSE_VENDOR_TOOLCHAIN=${VENDOR_TOOLCHAIN}"
    "-DPython3_EXECUTABLE=${PYTHON}" "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
    "-DMAXIM_RISCV_FLASH_ORIGIN=${RISCV_FLASH_ORIGIN}"
    "-DMAXIM_RISCV_FLASH_LENGTH=${RISCV_FLASH_LENGTH}"
    "-DMAXIM_RISCV_SRAM2_RESERVE=${SRAM2_RESERVE}"
    "-DMAXIM_RISCV_TIME_TIMER=${TIME_TIMER}"
    RESULT_VARIABLE _result)
if(NOT _result EQUAL 0)
    message(FATAL_ERROR "CPU1 CMake configuration failed (${_result})")
endif()
