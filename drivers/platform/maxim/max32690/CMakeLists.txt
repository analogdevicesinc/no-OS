target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_reva1.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_reva1.h)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_reva_regs.h)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_reva.h)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_reva_regs.h)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA/dma_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA/dma_me18.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_revb.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_common.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_delay.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/pins_me18.c)
# Exclude sys_me18.c from RISC-V builds due to SDK incompatibilities (missing MCR registers)
if(NOT (DEFINED CPU_CORE AND CPU_CORE STREQUAL "riscv"))
	target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/sys_me18.c)
endif()
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/nvic_table.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_lock.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/LP/lp_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_me18.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC/rtc_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC/rtc_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CTB/ctb_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CTB/ctb_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CTB/ctb_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_revb.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_me18.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_common.c)

target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/RTC)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/ICC)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/FLC)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/SYS)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/TMR)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/GPIO)

target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/CMSIS/Include)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/MAX32690)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Include)

# IPC support (inter-processor communication for dual-core)
# maxim_ipc.c is registered by drivers/platform/maxim/CMakeLists.txt via
# CONFIG_IPC_MAXIM. Chip-specific SEMA PeriphDrivers stay here.
if(CONFIG_IPC_MAXIM)
	target_sources(no-os PRIVATE
		${MAXIM_LIBRARIES}/PeriphDrivers/Source/SEMA/sema_me18.c
		${MAXIM_LIBRARIES}/PeriphDrivers/Source/SEMA/sema_reva.c
	)
	target_include_directories(no-os PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${MAXIM_LIBRARIES}/PeriphDrivers/Include/MAX32690
		${MAXIM_LIBRARIES}/PeriphDrivers/Source/SEMA
	)
endif()

# Detect RISC-V core build
if(DEFINED CPU_CORE AND CPU_CORE STREQUAL "riscv")
	set(RISCV_BUILD TRUE)

	# RISC-V-specific startup and system files
	target_sources(no-os PRIVATE
		${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/GCC/startup_riscv_max32690.S
		${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/system_riscv_max32690.c
	)

	# RISC-V compiler flags
	target_compile_options(no-os PRIVATE
		-march=rv32im_zicsr_zifencei
		-mabi=ilp32
		-ffunction-sections
		-fdata-sections
	)

	target_link_options(no-os PRIVATE
		-march=rv32im_zicsr_zifencei
		-mabi=ilp32
		-Wl,--gc-sections
		-nostartfiles
		--specs=nano.specs
		--specs=nosys.specs
	)

	# Gate ARM-only drivers
	target_compile_definitions(no-os PRIVATE __riscv)

else()
	# ARM Cortex-M4 build (default)
	target_sources(no-os PRIVATE
		${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/GCC/startup_max32690.S
		${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/heap.c
		${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/system_max32690.c
	)
endif()

target_compile_definitions(no-os PUBLIC -DTARGET_NUM=32690)
target_compile_definitions(no-os PUBLIC -DTARGET=32690)
