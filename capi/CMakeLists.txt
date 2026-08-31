no_os_sources_ifdef(CONFIG_CAPI_MAXIM
  ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_alloc.c)
no_os_sources_ifdef(CONFIG_CAPI_MAXIM
  ${CMAKE_CURRENT_SOURCE_DIR}/platform/maxim/maxim_capi_alloc.c)
no_os_sources_ifdef(CONFIG_CAPI_COPROCESSOR_MAXIM
  ${CMAKE_CURRENT_SOURCE_DIR}/platform/maxim/maxim_capi_coprocessor.c)

if(CONFIG_CAPI_MAXIM)
  target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/platform/maxim)
endif()
