no_os_sources_ifdef(CONFIG_CAPI_ALLOC
  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_alloc.c)
no_os_sources_ifdef(CONFIG_IPC_MAXIM
  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_ipc.c)
no_os_sources_ifdef(CONFIG_COPROCESSOR_MAXIM
  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_coprocessor.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
