find_package(Python3 COMPONENTS Interpreter)

function(add_jlink_flash_target TARGET_NAME)
	set(FLASH_SCRIPT "${NO_OS_DIR}/tools/scripts/jlink.py")
	set(HEX_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.hex")

	add_custom_target(flash_${TARGET_NAME}
	COMMAND "${Python3_EXECUTABLE}" "${FLASH_SCRIPT}"
		--device "${TARGET}"
		--file "${HEX_FILE}"
	DEPENDS ${TARGET_NAME} # Ensures the elf and hex are built first
	COMMENT "Flashing ${TARGET}..."
	VERBATIM
	)
endfunction()

function(add_flash_target TARGET_NAME)
	# This function reads the FLASH_METHOD variable and calls the correct helper.
	# if(NOT DEFINED FLASH_METHOD OR FLASH_METHOD STREQUAL "")
	# 	message(STATUS "No FLASH_METHOD specified, flash target for '${TARGET_NAME}' will not be created.")
	# 	return()
	# endif()

	# if(FLASH_METHOD STREQUAL "JLINK")
	add_jlink_flash_target(${TARGET_NAME})
	# else()
		# message(WARNING "Unsupported FLASH_METHOD: '${FLASH_METHOD}'. No flash target created for '${TARGET_NAME}'.")
	# endif()
endfunction()