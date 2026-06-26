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

# Default GDB port (must match gdb_port in openocd.cfg)
if(NOT DEFINED GDB_PORT)
	set(GDB_PORT 50000)
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

function(add_flash_target TARGET_NAME)
	if(NOT PROBE)
		message(WARNING
			"PROBE is not set; no 'flash'/'debug' targets will be created. "
			"Set -DPROBE=openocd or -DPROBE=jlink to enable flashing.")
	elseif(${PROBE} STREQUAL "jlink")
		add_jlink_flash_target(${TARGET_NAME})
	elseif(${PROBE} STREQUAL "openocd")
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
