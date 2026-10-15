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

# The carrier board, as a macro the sources can test. The VCU118 and VCK190
# designs are not interchangeable: the clock chips hang off a PL AXI quad-SPI on
# one and PS SPI0 on the other (with different chip selects), only the VCU118
# instantiates an axi_adxcvr, and the Apollo reset sits on a different axi_gpio
# pin. parameters.h and common_data.h branch on this; see
# dev_doc/VCK190_ENABLEMENT.md. Mirrors how projects/ad9361 emits PLATFORM_MB.
if(BOARD)
    string(TOUPPER "${BOARD}" _ad9088_board)
    string(MAKE_C_IDENTIFIER "${_ad9088_board}" _ad9088_board)
    target_compile_definitions(ad9088 PRIVATE BOARD_${_ad9088_board})
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

# ad9088's app and the Apollo/xilinx drivers it uses are SDT-clean, so opt in
# to the Vitis 2025+ SDT ABI when the BSP is SDT. Other projects keep the
# classic ABI until individually validated (see config_xilinx_sdk).
set(NOOS_XILINX_SDT ON)

config_platform_sdk(ad9088)
add_flash_target(ad9088)
post_build_config(ad9088)
target_link_libraries(ad9088 no-os)
