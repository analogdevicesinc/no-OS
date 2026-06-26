# Only define IIO_SUPPORT when IIO is enabled. Many drivers switch behavior on
# #ifdef IIO_SUPPORT, so defining it unconditionally pulls IIO code paths into
# non-IIO builds. This mirrors the Make build, which gates -DIIO_SUPPORT behind
# the IIO toggle (tools/scripts/libraries.mk).
if(CONFIG_IIO)
  target_compile_definitions(no-os PUBLIC -DIIO_SUPPORT)
endif()

no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio_trigger.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iiod.c)
no_os_sources_ifdef(CONFIG_IIO ${CMAKE_CURRENT_SOURCE_DIR}/iio_app/iio_app.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/iio_app)