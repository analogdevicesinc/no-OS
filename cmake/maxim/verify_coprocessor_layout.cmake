# Verify that CPU0 embeds the CPU1 image at the address used to link CPU1.

foreach(_required ARM_NM RISCV_NM ARM_ELF RISCV_ELF)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR
            "verify_coprocessor_layout: ${_required} was not provided")
    endif()
endforeach()

execute_process(
    COMMAND "${ARM_NM}" -n "${ARM_ELF}"
    RESULT_VARIABLE _arm_result
    OUTPUT_VARIABLE _arm_symbols
    ERROR_VARIABLE _arm_error)
if(NOT _arm_result EQUAL 0)
    message(FATAL_ERROR
        "verify_coprocessor_layout: ARM nm failed: ${_arm_error}")
endif()

execute_process(
    COMMAND "${RISCV_NM}" -n "${RISCV_ELF}"
    RESULT_VARIABLE _riscv_result
    OUTPUT_VARIABLE _riscv_symbols
    ERROR_VARIABLE _riscv_error)
if(NOT _riscv_result EQUAL 0)
    message(FATAL_ERROR
        "verify_coprocessor_layout: RISC-V nm failed: ${_riscv_error}")
endif()

string(REGEX MATCH "(^|\n)([0-9a-fA-F]+)[ \t]+[A-Za-z][ \t]+_riscv_boot(\n|$)"
    _arm_match "${_arm_symbols}")
if(NOT _arm_match)
    message(FATAL_ERROR
        "verify_coprocessor_layout: _riscv_boot missing from ${ARM_ELF}")
endif()
set(_arm_address "${CMAKE_MATCH_2}")

string(REGEX MATCH "(^|\n)([0-9a-fA-F]+)[ \t]+[A-Za-z][ \t]+__isr_vector(\n|$)"
    _riscv_match "${_riscv_symbols}")
if(NOT _riscv_match)
    message(FATAL_ERROR
        "verify_coprocessor_layout: __isr_vector missing from ${RISCV_ELF}")
endif()
set(_riscv_address "${CMAKE_MATCH_2}")

string(TOLOWER "${_arm_address}" _arm_address)
string(TOLOWER "${_riscv_address}" _riscv_address)
if(NOT _arm_address STREQUAL _riscv_address)
    message(FATAL_ERROR
        "CPU1 address mismatch: ARM embeds at 0x${_arm_address}, but RISC-V "
        "was linked at 0x${_riscv_address}")
endif()

message(STATUS
    "coprocessor: verified ARM embed == RISC-V link == 0x${_arm_address}")
