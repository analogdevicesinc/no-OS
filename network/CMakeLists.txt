# Generic no_os_net / no_os_socket interface (api layer). Pulled in whenever a
# networking backend (currently lwip) is enabled.
no_os_sources_ifdef(CONFIG_LWIP ${NO_OS_DIR}/drivers/api/no_os_net.c)
no_os_sources_ifdef(CONFIG_LWIP ${NO_OS_DIR}/drivers/api/no_os_socket.c)

no_os_sources_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/lwip_raw_socket/lwip_socket.c)
no_os_include_dir_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR}/lwip_raw_socket)
no_os_include_dir_ifdef(CONFIG_LWIP ${CMAKE_CURRENT_SOURCE_DIR})

# Composable TLS socket (no_os_secure_socket). Only meaningful with mbedTLS;
# the source is fully guarded by DISABLE_SECURE_SOCKET otherwise.
no_os_sources_ifdef(CONFIG_MBEDTLS ${CMAKE_CURRENT_SOURCE_DIR}/no_os_secure_socket.c)
no_os_include_dir_ifdef(CONFIG_MBEDTLS ${CMAKE_CURRENT_SOURCE_DIR})

if(${CONFIG_LWIP})
        add_subdirectory(lwip_raw_socket/netdevs)
endif()
