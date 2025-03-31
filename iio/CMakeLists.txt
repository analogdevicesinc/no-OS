target_compile_definitions(capi_build PUBLIC -DIIO_SUPPORT)

no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio_trigger.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iiod.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio_app/iio_app.c)

target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/iio_app)