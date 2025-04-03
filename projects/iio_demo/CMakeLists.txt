add_executable(iio_demo src/examples/iio_example/iio_example.c)
target_link_libraries(iio_demo capi_build)

target_sources(iio_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c)
# target_sources(iio_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c)
target_sources(iio_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c)
target_sources(iio_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c)

target_include_directories(iio_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example)

# Just because iio_app.c includes parameters.h ...
target_include_directories(iio_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM})
# And parameters.h includes common_data.h
target_include_directories(iio_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/common)
# And common_data.h includes platform_includes.h
target_include_directories(iio_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform)

target_compile_definitions(iio_demo PRIVATE -DIIO_SUPPORT=1)
target_compile_definitions(iio_demo PRIVATE -DIIO_EXAMPLE=1)
# target_compile_definitions(iio_demo PRIVATE -DIIO_TRIGGER_EXAMPLE=0)
# target_compile_definitions(iio_demo PRIVATE -DIIO_TIMER_TRIGGER_EXAMPLE=0)

set(CAPI_CURRENT_PROJECT iio_demo CACHE INTERNAL "Current project")
list(APPEND CONFIG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/project.conf)