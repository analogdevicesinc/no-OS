add_executable(ltc4284)

# Common sources (required for all examples)
target_sources(ltc4284 PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_LTC4284_BASIC_EXAMPLE)
	target_sources(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
	)
	target_include_directories(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
	)
endif()

# IIO example
if(CONFIG_LTC4284_IIO_EXAMPLE)
	target_sources(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
	)
	target_include_directories(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
	)
endif()

# Overcurrent example
if(CONFIG_LTC4284_OVERCURRENT_EXAMPLE)
	target_sources(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/overcurrent/overcurrent_example.c
	)
	target_include_directories(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/overcurrent
	)
endif()

# Dual-core example (ARM + RISC-V)
if(CONFIG_LTC4284_DUAL_CORE_EXAMPLE)
	target_sources(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/dual_core_example.c
	)
	target_include_directories(ltc4284 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core
	)
endif()

# Include directories
target_include_directories(ltc4284 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/common
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Link to no-os library
target_link_libraries(ltc4284 no-os)

# RISC-V (CPU1) image, embedded in the ARM firmware. SRAM2_RESERVE keeps the
# shared IPC table (LTC4284_IPC_TABLE_ADDR/SIZE) out of CPU1's own RAM.
if(CONFIG_LTC4284_DUAL_CORE_EXAMPLE)
	include(maxim_coprocessor)
	maxim_add_riscv_coprocessor(ltc4284
		DEFCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/riscv/dual_core.conf
		SRAM2_RESERVE 0x100
		SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/dual_core_example_riscv.c
		INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core)
endif()

# Platform SDK configuration
config_platform_sdk(ltc4284)
add_flash_target(ltc4284)
post_build_config(ltc4284)
