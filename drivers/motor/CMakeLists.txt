# tmc5240
no_os_sources_ifdef(CONFIG_MOTOR_TMC5240
    ${CMAKE_CURRENT_SOURCE_DIR}/tmc5240/tmc5240.c
    ${CMAKE_SOURCE_DIR}/libraries/tmc/TMC-API/tmc/ic/TMC5240/TMC5240.c)
no_os_sources_ifdef(CONFIG_MOTOR_IIO_TMC5240
    ${CMAKE_CURRENT_SOURCE_DIR}/tmc5240/iio_tmc5240.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tmc5240)
if(CONFIG_MOTOR_TMC5240)
    target_include_directories(no-os PUBLIC ${CMAKE_SOURCE_DIR}/libraries/tmc/TMC-API)
endif()

# tmc7300
no_os_sources_ifdef(CONFIG_MOTOR_TMC7300 ${CMAKE_CURRENT_SOURCE_DIR}/tmc7300/tmc7300.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tmc7300)
