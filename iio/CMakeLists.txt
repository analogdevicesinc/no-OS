target_compile_definitions(no-os PUBLIC -DIIO_SUPPORT)

no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio_trigger.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iiod.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio_app/iio_app.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/iio_app)