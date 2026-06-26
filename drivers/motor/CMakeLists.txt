# tmc7300
no_os_sources_ifdef(CONFIG_MOTOR_TMC7300 ${CMAKE_CURRENT_SOURCE_DIR}/tmc7300/tmc7300.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tmc7300)
