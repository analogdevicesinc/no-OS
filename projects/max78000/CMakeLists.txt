add_executable(max78000)

# Common sources
target_sources(max78000 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
)

# Basic example
if(CONFIG_MAX78000_BASIC_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# Dual-core example (ARM + RISC-V)
if(CONFIG_MAX78000_DUAL_CORE_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/dual_core_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core
    )
endif()

# Dual-core IMU example (ADIS16577 on RISC-V, ADXL345 on ARM)
if(CONFIG_MAX78000_IMU_DUAL_CORE_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core/imu_dual_core_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core
    )

endif()

# Single-core dual-IMU example (ADIS + ADXL345, ARM only)
if(CONFIG_MAX78000_IMU_SINGLE_CORE_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_single_core/imu_single_core_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_single_core
    )

endif()

target_include_directories(max78000 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(max78000 no-os)

if(CONFIG_MAX78000_DUAL_CORE_EXAMPLE)
    include(maxim_coprocessor)
    maxim_add_riscv_coprocessor(max78000
        DEFCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/riscv/dual_core.conf
        SRAM2_RESERVE 0x2000
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/dual_core_example_riscv.c
        INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core)
endif()

if(CONFIG_MAX78000_IMU_DUAL_CORE_EXAMPLE)
    include(maxim_coprocessor)
    maxim_add_riscv_coprocessor(max78000
        DEFCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/riscv/imu_dual_core.conf
        SRAM2_RESERVE 0x2000
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core/imu_dual_core_example_riscv.c
        INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core)
endif()

config_platform_sdk(max78000)
add_flash_target(max78000)
post_build_config(max78000)
