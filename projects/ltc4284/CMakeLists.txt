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

# Build the RISC-V coprocessor firmware and embed it into the ARM ELF. The
# platform helper owns the two-pass link, linker script, and embed; the project
# just names the RISC-V sources. Placed after the ARM target's sources/includes
# are set so the helper's premap twin inherits them.
if(CONFIG_LTC4284_DUAL_CORE_EXAMPLE)
	include(maxim_coprocessor)
	maxim_add_riscv_coprocessor(ltc4284
		SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/dual_core_example_riscv.c
			${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/maxim_sys_riscv_stubs.c
		INCLUDES
			${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core
			${CMAKE_SOURCE_DIR}/include
			${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}
		NOOS_INCLUDES
			${CMAKE_SOURCE_DIR}/drivers/power/ltc4284
		NOOS_DRIVERS
			# no-OS API layer for RISC-V core
			${CMAKE_SOURCE_DIR}/drivers/api/no_os_gpio.c
			${CMAKE_SOURCE_DIR}/drivers/api/no_os_irq.c
			${CMAKE_SOURCE_DIR}/drivers/api/no_os_i2c.c
			${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/no_os_mutex_stubs.c
			${CMAKE_SOURCE_DIR}/util/no_os_util.c
			${CMAKE_SOURCE_DIR}/util/no_os_alloc.c
			# LTC4284 driver (runs on RISC-V)
			${CMAKE_SOURCE_DIR}/drivers/power/ltc4284/ltc4284.c
			# Maxim platform drivers for RISC-V
			${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_gpio.c
			${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_i2c.c
			${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_delay.c
			# MXC PeriphDrivers (RV32-compiled) — me18 variants match MAX32690
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_me18.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_reva.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_common.c
			# sys_me18.c excluded - ARM-only MCR/MRS instructions; stubs in maxim_sys_riscv_stubs.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/pins_me18.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_delay.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_lock.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_assert.c
			# I2C PeriphDrivers for RISC-V
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_me18.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_reva.c
			# ICC PeriphDrivers for RISC-V (instruction cache)
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_me18.c
			${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_reva.c)
endif()

# Platform SDK configuration
config_platform_sdk(ltc4284)
add_flash_target(ltc4284)
post_build_config(ltc4284)
