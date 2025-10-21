find_package(Python3 COMPONENTS Interpreter)

function(add_jlink_flash_target TARGET_NAME)
	set(FLASH_SCRIPT "${NO_OS_DIR}/tools/scripts/jlink.py")
	set(HEX_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.hex")

	add_custom_target(flash
		COMMAND "${VENV_PYTHON_EXE}" "${FLASH_SCRIPT}"
			--device "${TARGET}"
			--file "${HEX_FILE}"
		DEPENDS ${TARGET_NAME} # Ensures the elf and hex are built first
		COMMENT "Flashing ${TARGET}..."
		VERBATIM
	)
endfunction()

function(add_openocd_flash_target TARGET_NAME)
	if(${PLATFORM} STREQUAL "maxim")
		add_custom_target(flash
			COMMAND ${OPENOCD_PATH}
				-s ${OPENOCD_SCRIPTS}
				-f interface/cmsis-dap.cfg
				-f target/${TARGET}.cfg
				-c "program ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.elf verify reset exit"
			DEPENDS ${TARGET_NAME} # Ensures the elf and hex are built first
			COMMENT "Flashing ${TARGET}..."
			VERBATIM
		)

		add_custom_target(erase
			COMMAND ${OPENOCD_PATH}
				-s ${OPENOCD_SCRIPTS}
				-f interface/cmsis-dap.cfg
				-f target/${TARGET}.cfg
				-c "init; halt; flash erase_sector 0 0 last; exit"
			DEPENDS ${TARGET_NAME} # Ensures the elf and hex are built first
			COMMENT "Erasing..."
			VERBATIM
		)
	elseif(${PLATFORM} STREQUAL "stm32")
		add_custom_target(flash
			COMMAND ${OPENOCD_PATH}
				-s ${OPENOCD_SCRIPTS}
				-f ${CMAKE_BINARY_DIR}/openocd.cfg
				-c "program ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.elf verify reset exit"
			DEPENDS ${TARGET_NAME} # Ensures the elf and hex are built first
			COMMENT "Flashing ${TARGET}..."
			VERBATIM
		)

		add_custom_target(erase
			COMMAND ${OPENOCD_PATH}
				-s ${OPENOCD_SCRIPTS}
				-f ${CMAKE_BINARY_DIR}/openocd.cfg
				-c "init; halt; flash erase_sector 0 0 last; exit"
			DEPENDS ${TARGET_NAME} # Ensures the elf and hex are built first
			COMMENT "Erasing..."
			VERBATIM
		)
	endif()
endfunction()

function(add_flash_target TARGET_NAME)
	if(${PROBE} STREQUAL "jlink")
		add_jlink_flash_target(${TARGET_NAME})
	elseif(${PROBE} STREQUAL "openocd")
		add_openocd_flash_target(${TARGET_NAME})
	endif()
endfunction()
