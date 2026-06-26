no_os_sources_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/tcp_socket.c)

no_os_sources_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/lwip_raw_socket/lwip_socket.c)
no_os_include_dir_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/lwip_raw_socket)
no_os_include_dir_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR})

if(${CONFIG_LWIP})
        add_subdirectory(lwip_raw_socket/netdevs)
endif()