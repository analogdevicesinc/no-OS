# Build CPU1 with its own no-OS configuration, then embed it in CPU0's ELF.
include_guard(GLOBAL)
include(ExternalProject)

function(maxim_add_riscv_coprocessor ARM_TARGET)
    cmake_parse_arguments(RV "DEBUG_INFO"
        "DEFCONFIG;FORCE_INCLUDE;OPTIMIZATION;SRAM2_RESERVE;TIME_TIMER"
        "SOURCES;INCLUDES;DEFINES;DEPENDS" ${ARGN})
    if(RV_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown coprocessor arguments: ${RV_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT RV_SOURCES OR NOT RV_DEFCONFIG)
        message(FATAL_ERROR "maxim_add_riscv_coprocessor requires SOURCES and DEFCONFIG")
    endif()
    if(NOT MAXIM_CORE STREQUAL "arm" OR NOT TARGET_NUM MATCHES "^(78000|32690)$")
        message(FATAL_ERROR
            "The embedded coprocessor requires a MAX78000 or MAX32690 ARM host")
    endif()
    get_filename_component(RV_DEFCONFIG "${RV_DEFCONFIG}" ABSOLUTE
        BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT EXISTS "${RV_DEFCONFIG}")
        message(FATAL_ERROR "CPU1 defconfig not found: ${RV_DEFCONFIG}")
    endif()
    foreach(_list SOURCES INCLUDES DEPENDS)
        set(_paths "")
        foreach(_path IN LISTS RV_${_list})
            get_filename_component(_path "${_path}" ABSOLUTE
                BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
            list(APPEND _paths "${_path}")
        endforeach()
        set(RV_${_list} "${_paths}")
    endforeach()
    if(RV_FORCE_INCLUDE)
        get_filename_component(RV_FORCE_INCLUDE "${RV_FORCE_INCLUDE}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    if(NOT DEFINED RV_SRAM2_RESERVE OR RV_SRAM2_RESERVE STREQUAL "")
        set(RV_SRAM2_RESERVE 0)
    endif()
    math(EXPR _reserve "${RV_SRAM2_RESERVE}")
    math(EXPR _reserve_alignment "${_reserve} % 16")
    if(_reserve LESS 0 OR _reserve GREATER_EQUAL 0xC000 OR _reserve_alignment)
        message(FATAL_ERROR "SRAM2_RESERVE must be a multiple of 16 in [0, 0xC000)")
    endif()
    if(_reserve AND NOT TARGET_NUM STREQUAL "78000")
        message(FATAL_ERROR "SRAM2_RESERVE is only supported on MAX78000")
    endif()
    if(NOT DEFINED RV_TIME_TIMER OR RV_TIME_TIMER STREQUAL "")
        set(RV_TIME_TIMER 3)
    endif()
    if(RV_DEBUG_INFO)
        set(RV_BUILD_TYPE RelWithDebInfo)
    else()
        set(RV_BUILD_TYPE MinSizeRel)
    endif()
    if(RV_OPTIMIZATION)
        set(RV_OPTIMIZATION "-${RV_OPTIMIZATION}")
    endif()

    set(_bin "${CMAKE_CURRENT_BINARY_DIR}/coprocessor")
    set(_gcc "${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC")
    set(_arm_ld "${NO_OS_DIR}/drivers/platform/maxim/${TARGET}/${TARGET}_dualcore_arm.ld")
    # CPU1 flash bounds and the linker scripts the CPU1 image depends on.
    if(TARGET_NUM STREQUAL "32690")
        math(EXPR RV_FLASH_END "${_RISCV_FLASH_ORIGIN} + ${_RISCV_FLASH_SIZE}"
            OUTPUT_FORMAT HEXADECIMAL)
        set(RV_FLASH_SIZE ${_RISCV_FLASH_SIZE})
        set(RV_LINK_DEPENDS "${_gcc}/max32690_riscv.ld")
        # The SDK CPU0 startup exports the CPU1 regions to max32690.ld and
        # defaults them to 0 (no CPU1 image) unless they are defined.
        target_compile_definitions(no-os PRIVATE
            RISCV_FLASH_ORIGIN=${_RISCV_FLASH_ORIGIN}
            RISCV_FLASH_SIZE=${_RISCV_FLASH_SIZE}
            RISCV_SRAM_ORIGIN=${_RISCV_SRAM_ORIGIN}
            RISCV_SRAM_SIZE=${_RISCV_SRAM_SIZE})
    else()
        set(RV_FLASH_END 0x10080000)
        set(RV_FLASH_SIZE 0x80000)
        set(RV_LINK_DEPENDS "\${CMAKE_BINARY_DIR}/buildrv/common_riscv.ld"
            "\${CMAKE_BINARY_DIR}/common.ld" "${_gcc}/max78000_riscv.ld")
    endif()
    file(MAKE_DIRECTORY "${_bin}/source")
    configure_file("${RV_DEFCONFIG}" "${_bin}/source/project.conf" COPYONLY)
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/coprocessor_spec.cmake.in"
        "${_bin}/source/spec.cmake" @ONLY)
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/coprocessor_CMakeLists.txt.in"
        "${_bin}/source/CMakeLists.txt" @ONLY)

    string(REPLACE "-T${_gcc}/max${TARGET_NUM}.ld"
        "-T${_arm_ld} -L${_gcc}" _flags "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${_flags}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${_flags}")

    # The premap and final executable must resolve exactly the same host code.
    set(_premap ${ARM_TARGET}_premap)
    get_target_property(_arm_sources ${ARM_TARGET} SOURCES)
    add_executable(${_premap} ${_arm_sources})
    foreach(_property INCLUDE_DIRECTORIES COMPILE_DEFINITIONS COMPILE_OPTIONS
            LINK_OPTIONS LINK_LIBRARIES)
        get_target_property(_value ${ARM_TARGET} ${_property})
        if(_value)
            set_property(TARGET ${_premap} PROPERTY ${_property} "${_value}")
        endif()
    endforeach()
    set(_premap_map "${_bin}/${_premap}.map")
    target_link_options(${_premap} PRIVATE "-Wl,-Map=${_premap_map}")
    add_custom_command(TARGET ${_premap} POST_BUILD
        BYPRODUCTS "${_premap_map}"
        COMMAND ${CMAKE_COMMAND} -E true)

    set(_layout "${_bin}/flash.cmake")
    add_custom_command(OUTPUT "${_layout}"
        COMMAND ${CMAKE_COMMAND} "-DMAP_FILE=${_premap_map}" "-DOUT_FILE=${_layout}"
            "-DFLASH_END=${RV_FLASH_END}" "-DFLASH_SIZE=${RV_FLASH_SIZE}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen_common_riscv_ld.cmake"
        DEPENDS ${_premap} "${_premap_map}"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen_common_riscv_ld.cmake"
        VERBATIM)
    add_custom_target(${ARM_TARGET}_coprocessor_layout DEPENDS "${_layout}")

    set(_rv_build "${_bin}/build")
    set(_rv_elf "${_rv_build}/riscv.elf")
    set(_rv_bin "${_rv_build}/riscv.bin")
    set(_external ${ARM_TARGET}_riscv)
    ExternalProject_Add(${_external}
        SOURCE_DIR "${_bin}/source"
        BINARY_DIR "${_rv_build}"
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
            "-DSOURCE_DIR=${_bin}/source" "-DBINARY_DIR=${_rv_build}"
            "-DLAYOUT_FILE=${_layout}" "-DSDK_DIR=${MAXIM_LIBRARIES}"
            "-DPYTHON=${Python3_EXECUTABLE}" "-DBUILD_TYPE=${RV_BUILD_TYPE}"
            "-DVENDOR_TOOLCHAIN=${USE_VENDOR_TOOLCHAIN}"
            "-DSRAM2_RESERVE=${RV_SRAM2_RESERVE}" "-DTIME_TIMER=${RV_TIME_TIMER}"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/configure_coprocessor.cmake"
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target riscv
        INSTALL_COMMAND ""
        BUILD_ALWAYS TRUE
        BUILD_BYPRODUCTS "${_rv_elf}" "${_rv_bin}"
        DEPENDS ${ARM_TARGET}_coprocessor_layout)
    ExternalProject_Add_StepDependencies(${_external} configure
        "${_layout}" "${_bin}/source/project.conf" "${_bin}/source/spec.cmake"
        "${_bin}/source/CMakeLists.txt"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/configure_coprocessor.cmake")

    set(RISCV_BIN "${_rv_bin}")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/riscv_loader.S.in"
        "${_bin}/riscv_loader.S" @ONLY)
    set(_embed_obj "${_bin}/riscv.o")
    separate_arguments(_embed_flags NATIVE_COMMAND "${COMMON_CPU_FLAGS}")
    add_custom_command(OUTPUT "${_embed_obj}"
        COMMAND ${CMAKE_C_COMPILER} ${_embed_flags}
            -x assembler-with-cpp -c "${_bin}/riscv_loader.S" -o "${_embed_obj}"
        DEPENDS ${_external} "${_rv_bin}" "${_bin}/riscv_loader.S"
        VERBATIM)
    set_property(SOURCE "${_embed_obj}" PROPERTY EXTERNAL_OBJECT TRUE)
    target_sources(${ARM_TARGET} PRIVATE "${_embed_obj}")
    add_dependencies(${ARM_TARGET} ${_external})

    add_custom_command(TARGET ${ARM_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} "-DARM_NM=${CMAKE_NM}"
            "-DRISCV_BUILD_DIR=${_rv_build}" "-DARM_ELF=$<TARGET_FILE:${ARM_TARGET}>"
            "-DRISCV_ELF=${_rv_elf}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/verify_coprocessor_layout.cmake"
        COMMENT "coprocessor: verifying final embed/link address"
        VERBATIM)
endfunction()
