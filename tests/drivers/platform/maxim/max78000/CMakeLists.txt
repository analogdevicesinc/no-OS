cmake_minimum_required(VERSION 3.16)
project(max78000_time_test C)
enable_testing()
get_filename_component(NO_OS_ROOT "../../../../.." ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(time_test test_time.c
    ${NO_OS_ROOT}/drivers/platform/maxim/max78000/riscv/maxim_delay.c
    ${NO_OS_ROOT}/drivers/platform/maxim/max78000/riscv/maxim_init.c)
target_include_directories(time_test PRIVATE support ${NO_OS_ROOT}/include
    ${NO_OS_ROOT}/drivers/platform/maxim/max78000/riscv)
target_compile_options(time_test PRIVATE -Wall -Wextra -Werror
    -include ${CMAKE_CURRENT_SOURCE_DIR}/support/maxim_core.h)
add_test(NAME riscv_time COMMAND time_test)
