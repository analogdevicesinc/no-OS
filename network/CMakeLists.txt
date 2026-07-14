no_os_include_dir_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR})

if(CONFIG_LWIP)
    if(CONFIG_NO_OS_NET)
        add_subdirectory(lwip)
    else()
        no_os_sources_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/tcp_socket.c)
        no_os_sources_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/lwip_raw_socket/lwip_socket.c)
        no_os_include_dir_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/lwip_raw_socket)
        add_subdirectory(lwip_raw_socket/netdevs)
    endif()
endif()

if(CONFIG_W5500)
    add_subdirectory(w5500)
endif()

if(CONFIG_WIFI)
    add_subdirectory(wifi)
endif()

if(CONFIG_NO_OS_NET AND NOT CONFIG_LWIP)
    target_compile_definitions(no-os PUBLIC -DNO_OS_NET=1)
endif()
