no_os_sources_ifdef(CONFIG_CAPI_ALLOC ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_alloc.c)

if (${TARGET} IN_LIST MAXIM_CAPI_COMMON_TARGETS)
	no_os_sources_ifdef(CONFIG_CAPI_TIME  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_time.c)

	target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
