add_executable(ad9088)

target_sources(ad9088 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# main.c selects the example with a bare #ifdef, so the Kconfig choice is
# translated back into the macro the sources already use.
if(CONFIG_AD9088_BASIC_EXAMPLE)
    target_sources(ad9088 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad9088 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
    target_compile_definitions(ad9088 PRIVATE BASIC_EXAMPLE)
endif()

if(CONFIG_AD9088_DMA_EXAMPLE)
    target_sources(ad9088 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma/dma_example.c
    )
    target_include_directories(ad9088 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma
    )
    target_compile_definitions(ad9088 PRIVATE DMA_EXAMPLE)
endif()

# src/platform (not just src/platform/${PLATFORM}) is required:
# platform_includes.h reaches the parameters header as "xilinx/parameters.h".
target_include_directories(ad9088 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
)

# Device profile and signed core firmware, linked in as binary blobs. The
# driver owns the image list and the symbol names; PROFILE overrides the
# default use-case profile.
include(${NO_OS_DIR}/drivers/rf-transceiver/ad9088/ad9088_firmware.cmake)
ad9088_add_firmware(ad9088 PROFILE "${CONFIG_AD9088_PROFILE}")

config_platform_sdk(ad9088)
add_flash_target(ad9088)
post_build_config(ad9088)
target_link_libraries(ad9088 no-os)
