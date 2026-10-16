cmake_minimum_required(VERSION 3.10)
project(ad7291 C)

set(NO_OS ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_compile_options(-Wall -DLINUX_PLATFORM)

add_executable(ad7291
    src/main.c
    ${NO_OS}/drivers/adc/ad7291/ad7291.c
    ${NO_OS}/drivers/api/no_os_i2c.c
    ${NO_OS}/drivers/platform/linux/linux_i2c.c
    ${NO_OS}/drivers/platform/linux/linux_delay.c
    ${NO_OS}/util/no_os_util.c
    ${NO_OS}/util/no_os_alloc.c
    ${NO_OS}/util/no_os_mutex.c
)

target_include_directories(ad7291 PRIVATE
    src
    ${NO_OS}/include
    ${NO_OS}/drivers/adc/ad7291
    ${NO_OS}/drivers/platform/linux
    ${NO_OS}/drivers/api
)

target_link_libraries(ad7291 m pthread)
