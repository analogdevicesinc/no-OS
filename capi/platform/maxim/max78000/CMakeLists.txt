no_os_sources_ifdef(CONFIG_CAPI_MAILBOX
  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_ipc.c)
if(CONFIG_CAPI_COPROCESSOR)
  if(NOT MAXIM_CORE STREQUAL "arm")
    message(FATAL_ERROR "MAX78000 coprocessor lifecycle control requires the ARM host")
  endif()
  target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_coprocessor.c)
endif()

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
