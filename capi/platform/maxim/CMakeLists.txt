set(MAXIM_CAPI_COMMON_TARGETS max32650 max32690)

add_subdirectory(common)

if (${TARGET} STREQUAL "max32657")
	add_subdirectory(max32657)
endif()
