add_executable(adaq8092_fmc)

target_sources(adaq8092_fmc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adaq8092_fmc.c
)

if(CONFIG_ADAQ8092_FMC_IIO_EXAMPLE)
    target_compile_definitions(adaq8092_fmc PRIVATE IIO_SUPPORT)
endif()

target_include_directories(adaq8092_fmc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(adaq8092_fmc no-os)

config_platform_sdk(adaq8092_fmc)
add_flash_target(adaq8092_fmc)
post_build_config(adaq8092_fmc)
