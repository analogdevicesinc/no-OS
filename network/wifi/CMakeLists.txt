no_os_sources_ifdef(CONFIG_WIFI ${CMAKE_CURRENT_SOURCE_DIR}/wifi_net.c
				${CMAKE_CURRENT_SOURCE_DIR}/at_parser.c)
no_os_include_dir_ifdef(CONFIG_WIFI ${CMAKE_CURRENT_SOURCE_DIR})
