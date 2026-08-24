add_executable(ad3552r_fmcz)

target_sources(ad3552r_fmcz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_AD3552R_FMCZ_AXI_QSPI_EXAMPLE OR CONFIG_AD3552R_FMCZ_AXI_QSPI_IIO_EXAMPLE)
    target_sources(ad3552r_fmcz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/axi_qspi/axi_qspi_example.c
    )
    target_include_directories(ad3552r_fmcz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/axi_qspi
    )
endif()

if(CONFIG_AD3552R_FMCZ_GENERIC_SPI_EXAMPLE OR CONFIG_AD3552R_FMCZ_GENERIC_SPI_IIO_EXAMPLE)
    target_sources(ad3552r_fmcz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/generic_spi/generic_spi_example.c
    )
    target_include_directories(ad3552r_fmcz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/generic_spi
    )
endif()

if(CONFIG_AD3552R_FMCZ_AXI_QSPI_IIO_EXAMPLE OR CONFIG_AD3552R_FMCZ_GENERIC_SPI_IIO_EXAMPLE)
    target_compile_definitions(ad3552r_fmcz PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad3552r_fmcz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad3552r_fmcz no-os)

config_platform_sdk(ad3552r_fmcz)
add_flash_target(ad3552r_fmcz)
post_build_config(ad3552r_fmcz)
