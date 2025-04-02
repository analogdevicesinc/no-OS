add_executable(eval-adxl355-pmdz-dummy ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c)
target_link_libraries(eval-adxl355-pmdz-dummy capi_build)

add_executable(eval-adxl355-pmdz-iio ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c)
target_link_libraries(eval-adxl355-pmdz-iio capi_build)

add_executable(eval-adxl355-pmdz-iio-trigger ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c)
target_link_libraries(eval-adxl355-pmdz-iio-trigger capi_build)

target_sources(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c)
target_sources(capi_build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/maxim/main.c)
target_sources(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/maxim/parameters.c)

# Just because iio_app.c includes parameters.h ...
# target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/maxim)
target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform)
# And parameters.h includes common_data.h
target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/common)
# And common_data.h includes platform_includes.h
target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM})

target_compile_definitions(eval-adxl355-pmdz-dummy PRIVATE -DADXL355_DEV=1)
target_compile_definitions(eval-adxl355-pmdz-iio PRIVATE -DADXL355_DEV=1)
target_compile_definitions(eval-adxl355-pmdz-iio-trigger PRIVATE -DADXL355_DEV=1)

target_include_directories(eval-adxl355-pmdz-dummy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy)
target_include_directories(eval-adxl355-pmdz-iio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example)
target_include_directories(eval-adxl355-pmdz-iio-trigger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example)

set(CAPI_CURRENT_PROJECT eval-adxl355-pmdz CACHE INTERNAL "Current project")
list(APPEND CONFIG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/project.conf)
