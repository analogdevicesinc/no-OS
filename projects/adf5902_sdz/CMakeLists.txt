add_executable(adf5902_sdz)

target_sources(adf5902_sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adf5902_sdz.c
)

if(CONFIG_ADF5902_SDZ_IIO_EXAMPLE)
    target_compile_definitions(adf5902_sdz PRIVATE IIO_SUPPORT)
endif()

target_include_directories(adf5902_sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(adf5902_sdz no-os)

config_platform_sdk(adf5902_sdz)
add_flash_target(adf5902_sdz)
post_build_config(adf5902_sdz)
