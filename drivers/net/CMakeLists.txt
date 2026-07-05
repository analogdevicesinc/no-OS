no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/adin1110/adin1110.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1110)

if (CONFIG_OA_TC6)
        add_subdirectory(oa_tc6)
endif()