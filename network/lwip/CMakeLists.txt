no_os_sources_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/lwip_net.c)
no_os_include_dir_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(netdevs)
