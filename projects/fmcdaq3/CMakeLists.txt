add_executable(fmcdaq3)

target_sources(fmcdaq3 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/fmcdaq3.c
)

# Bare #ifdef selectors in fmcdaq3.c — emit compile definitions per variant
if(CONFIG_FMCDAQ3_IIO_EXAMPLE)
    target_compile_definitions(fmcdaq3 PRIVATE IIO_SUPPORT)
endif()

# XILINX_PLATFORM is used in fmcdaq3.c for Xil_DCacheInvalidateRange
target_compile_definitions(fmcdaq3 PRIVATE XILINX_PLATFORM)

target_include_directories(fmcdaq3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/adi_hal
)

config_platform_sdk(fmcdaq3)
add_flash_target(fmcdaq3)
post_build_config(fmcdaq3)
target_link_libraries(fmcdaq3 no-os)
