no_os_sources_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_SOURCE_DIR}/freertos_alloc.c)
no_os_sources_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_SOURCE_DIR}/freertos_delay.c)
no_os_sources_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_SOURCE_DIR}/freertos_mutex.c)
no_os_sources_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_SOURCE_DIR}/freertos_semaphore.c)