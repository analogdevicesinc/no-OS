cmake_minimum_required(VERSION 3.28)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5 # release-1.16.0
  )

FetchContent_MakeAvailable(googletest)

message(STATUS "Args ${CMAKE_TOOLCHAIN_FILE}")
  
project(eval-adxl355-pmdz C ASM)


add_library(eval-adxl355-pmdz-common STATIC)

add_executable(eval-adxl355-pmdz-dummy ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c)
target_link_libraries(eval-adxl355-pmdz-dummy eval-adxl355-pmdz-common)

add_executable(eval-adxl355-pmdz-iio ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c)
target_link_libraries(eval-adxl355-pmdz-iio eval-adxl355-pmdz-common)

add_executable(eval-adxl355-pmdz-iio-trigger ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c)
target_link_libraries(eval-adxl355-pmdz-iio-trigger eval-adxl355-pmdz-common)
target_link_libraries(eval-adxl355-pmdz-common capi_build)

add_custom_target(eval-adxl355-pmdz DEPENDS eval-adxl355-pmdz-dummy eval-adxl355-pmdz-iio eval-adxl355-pmdz-iio-trigger)

target_sources(eval-adxl355-pmdz-common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c)
target_sources(eval-adxl355-pmdz-common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c)
target_sources(eval-adxl355-pmdz-common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c)

target_include_directories(eval-adxl355-pmdz-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform)
target_include_directories(eval-adxl355-pmdz-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/common)
target_include_directories(eval-adxl355-pmdz-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM})

target_compile_definitions(eval-adxl355-pmdz-dummy PRIVATE -DADXL355_DEV=1)
target_compile_definitions(eval-adxl355-pmdz-iio PRIVATE -DADXL355_DEV=1)
target_compile_definitions(eval-adxl355-pmdz-iio-trigger PRIVATE -DADXL355_DEV=1)

target_include_directories(eval-adxl355-pmdz-dummy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy)
target_include_directories(eval-adxl355-pmdz-iio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example)
target_include_directories(eval-adxl355-pmdz-iio-trigger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example)

message(STATUS "Running make for eval-adxl355-pmdz")