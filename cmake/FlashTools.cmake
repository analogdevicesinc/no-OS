find_package(Python3 COMPONENTS Interpreter)

# Find GDB for debug targets.  arm-none-eabi-gdb ships in the same toolchain
# bin directory as the cross compiler (e.g. the CubeIDE-bundled GCC), which is
# typically not on PATH, so hint the search at the compiler's directory before
# falling back to PATH / gdb-multiarch.
if(CMAKE_C_COMPILER)
	get_filename_component(_toolchain_bin_dir "${CMAKE_C_COMPILER}" DIRECTORY)
endif()
find_program(GDB_PATH
	NAMES arm-none-eabi-gdb gdb-multiarch
	HINTS "${_toolchain_bin_dir}" "${CROSS_COMPILER_BIN}"
	DOC "Path to GDB executable"
)
if(NOT GDB_PATH)
	message(STATUS "GDB not found; 'debug' target will be unavailable")
endif()

# Intel FPGA / Nios V download utility used by the Altera flash flow.
# ALTERA_NIOSV_HOME points at the niosv/ install directory; tools are found inside.
if(DEFINED ENV{ALTERA_NIOSV_HOME} AND NOT "$ENV{ALTERA_NIOSV_HOME}" STREQUAL "")
	set(_altera_niosv_hints "$ENV{ALTERA_NIOSV_HOME}/bin")
elseif(DEFINED ENV{NIOSV_DOWNLOAD_PATH} AND NOT "$ENV{NIOSV_DOWNLOAD_PATH}" STREQUAL "")
	set(NIOSV_DOWNLOAD_PATH "$ENV{NIOSV_DOWNLOAD_PATH}" CACHE FILEPATH "Path to the Intel Nios V downloader" FORCE)
	set(_altera_niosv_hints "")
else()
	set(_altera_niosv_hints "")
endif()

if(NOT DEFINED NIOSV_DOWNLOAD_PATH OR NIOSV_DOWNLOAD_PATH STREQUAL "")
	find_program(NIOSV_DOWNLOAD_PATH
		NAMES niosv-download niosv-download.exe
		HINTS
			${_altera_niosv_hints}
			"${_toolchain_bin_dir}"
			"${CROSS_COMPILER_BIN}"
		DOC "Path to the Intel Nios V downloader"
	)
	if(NIOSV_DOWNLOAD_PATH)
		set(NIOSV_DOWNLOAD_PATH "${NIOSV_DOWNLOAD_PATH}" CACHE FILEPATH "Path to the Intel Nios V downloader" FORCE)
	endif()
endif()

# Default GDB port (must match gdb_port in openocd.cfg)
if(NOT DEFINED GDB_PORT)
	set(GDB_PORT 50000)
endif()

# Default debug probe. openocd works with the on-board CMSIS-DAP debuggers on
# the supported boards; override with -DPROBE=jlink for a J-Link.
if(NOT PROBE)
	set(PROBE openocd)
endif()

function(add_jlink_flash_target TARGET_NAME)
	set(FLASH_SCRIPT "${NO_OS_DIR}/tools/scripts/jlink.py")
	set(HEX_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.hex")

	add_custom_target(flash
		COMMAND "${VENV_PYTHON_EXE}" "${FLASH_SCRIPT}"
			--device "${TARGET}"
			--file "${HEX_FILE}"
		DEPENDS ${TARGET_NAME}
		COMMENT "Flashing ${TARGET}..."
		VERBATIM
	)
endfunction()

function(add_openocd_flash_target TARGET_NAME)
	set(OPENOCD_CFG "${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg")
	set(GDB_INIT_FILE "${CMAKE_CURRENT_BINARY_DIR}/gdbinit")
	set(TARGET_ELF "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.elf")

	# Generate GDB init file
	configure_file(
		"${NO_OS_DIR}/cmake/ide/templates/gdbinit.in"
		"${GDB_INIT_FILE}"
		@ONLY
	)

	add_custom_target(flash
		COMMAND ${OPENOCD_PATH}
			-s ${OPENOCD_SCRIPTS}
			-f ${OPENOCD_CFG}
			-c "program ${TARGET_ELF} verify reset exit"
		DEPENDS ${TARGET_NAME}
		COMMENT "Flashing ${TARGET}..."
		VERBATIM
	)

	add_custom_target(erase
		COMMAND ${OPENOCD_PATH}
			-s ${OPENOCD_SCRIPTS}
			-f ${OPENOCD_CFG}
			-c "init\; halt\; flash erase_sector 0 0 last\; exit"
		DEPENDS ${TARGET_NAME}
		COMMENT "Erasing..."
		VERBATIM
	)

	# Debug server: starts OpenOCD and waits for GDB connection (blocking)
	# Run this in one terminal, then run 'debug' target in another
	add_custom_target(debug_server
		COMMAND ${OPENOCD_PATH}
			-s ${OPENOCD_SCRIPTS}
			-f ${OPENOCD_CFG}
			-c "init"
		DEPENDS ${TARGET_NAME}
		COMMENT "Starting OpenOCD debug server on port ${GDB_PORT}..."
		USES_TERMINAL
		VERBATIM
	)

	# Debug: starts OpenOCD in background and launches GDB
	if(CMAKE_HOST_UNIX)
		add_custom_target(debug
			COMMAND sh -c "${OPENOCD_PATH} -s ${OPENOCD_SCRIPTS} -f ${OPENOCD_CFG} -c init &"
			COMMAND ${GDB_PATH} -x ${GDB_INIT_FILE}
			DEPENDS ${TARGET_NAME}
			COMMENT "Starting debug session..."
			USES_TERMINAL
			VERBATIM
		)
	else()
		# On Windows, run debug_server in one terminal, then debug_gdb in another
		add_custom_target(debug
			COMMAND ${CMAKE_COMMAND} -E echo "On Windows, run 'debug_server' in one terminal, then 'debug_gdb' in another"
			VERBATIM
		)
		add_custom_target(debug_gdb
			COMMAND ${GDB_PATH} -x ${GDB_INIT_FILE}
			DEPENDS ${TARGET_NAME}
			COMMENT "Starting GDB (connect to running debug_server)..."
			USES_TERMINAL
			VERBATIM
		)
	endif()
endfunction()

function(add_niosv_download_flash_target TARGET_NAME)
	if(NOT NIOSV_DOWNLOAD_PATH)
		message(WARNING
			"niosv-download was not found; 'flash' target will be unavailable. "
			"Install it or set -DNIOSV_DOWNLOAD_PATH=...")
		return()
	endif()

	if(NOT DEFINED ALTERA_NIOSV_DOWNLOAD_CPU OR ALTERA_NIOSV_DOWNLOAD_CPU STREQUAL "")
		set(_altera_download_cpu "1")
	else()
		set(_altera_download_cpu "${ALTERA_NIOSV_DOWNLOAD_CPU}")
	endif()

	if(NOT DEFINED ALTERA_NIOSV_DOWNLOAD_CABLE OR ALTERA_NIOSV_DOWNLOAD_CABLE STREQUAL "")
		set(_altera_download_cable "")
	else()
		set(_altera_download_cable "--cable=${ALTERA_NIOSV_DOWNLOAD_CABLE}")
	endif()

	if(NOT DEFINED ALTERA_NIOSV_DOWNLOAD_DEVICE OR ALTERA_NIOSV_DOWNLOAD_DEVICE STREQUAL "")
		set(_altera_download_device "")
	elseif(ALTERA_NIOSV_DOWNLOAD_DEVICE STREQUAL "1")
		set(_altera_download_device "")
	else()
		set(_altera_download_device "--device=${ALTERA_NIOSV_DOWNLOAD_DEVICE}")
	endif()

	if(NOT DEFINED ALTERA_NIOSV_DOWNLOAD_INSTANCE OR ALTERA_NIOSV_DOWNLOAD_INSTANCE STREQUAL "")
		set(_altera_download_instance "")
	elseif(ALTERA_NIOSV_DOWNLOAD_INSTANCE STREQUAL "1")
		set(_altera_download_instance "")
	else()
		set(_altera_download_instance "--instance=${ALTERA_NIOSV_DOWNLOAD_INSTANCE}")
	endif()

	set(_altera_target_elf "$<TARGET_FILE:${TARGET_NAME}>")
	add_custom_target(flash
		COMMAND "${CMAKE_COMMAND}" -E echo "Flashing ELF: ${_altera_target_elf}"
		COMMAND "${NIOSV_DOWNLOAD_PATH}" -g "${_altera_target_elf}" -c "${_altera_download_cpu}" ${_altera_download_cable} ${_altera_download_device} ${_altera_download_instance}
		DEPENDS ${TARGET_NAME}
		COMMENT "Flashing ${TARGET_NAME} with niosv-download..."
		VERBATIM
	)
endfunction()

function(add_flash_target TARGET_NAME)
	# Xilinx uses its own JTAG flow (Vitis Python API), not OpenOCD/J-Link.
	if(PLATFORM STREQUAL "xilinx")
		include(${NO_OS_DIR}/cmake/xilinx/xilinx_flash.cmake)
		add_xilinx_flash_target(${TARGET_NAME})
		return()
	endif()

	# Altera/Nios V uses the Intel downloader rather than OpenOCD.
	if(PLATFORM STREQUAL "altera")
		add_niosv_download_flash_target(${TARGET_NAME})
		return()
	endif()

	if(NOT PROBE)
		message(WARNING
			"PROBE is empty; no 'flash'/'debug' targets will be created. "
			"Set -DPROBE=openocd or -DPROBE=jlink to enable flashing.")
	elseif("${PROBE}" STREQUAL "jlink")
		add_jlink_flash_target(${TARGET_NAME})
	elseif("${PROBE}" STREQUAL "openocd")
		if(NOT OPENOCD_PATH)
			message(WARNING
				"PROBE=openocd but OpenOCD was not found; 'flash'/'debug' targets "
				"will be unavailable. Install OpenOCD or set -DOPENOCD_PATH=...")
		else()
			add_openocd_flash_target(${TARGET_NAME})
		endif()
	else()
		message(WARNING
			"Unknown PROBE='${PROBE}'; no 'flash'/'debug' targets will be created. "
			"Supported values: openocd, jlink.")
	endif()
endfunction()
