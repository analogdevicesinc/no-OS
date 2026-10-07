add_subdirectory(common)

if (${TARGET} STREQUAL "max32657")
	add_subdirectory(max32657)
elseif (${TARGET} STREQUAL "max78000")
	add_subdirectory(max78000)
endif()
