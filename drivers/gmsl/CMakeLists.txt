# gmsl common register access
no_os_sources_ifdef(CONFIG_GMSL_REG_ACCESS ${CMAKE_CURRENT_SOURCE_DIR}/common/gmsl_reg_access.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)
