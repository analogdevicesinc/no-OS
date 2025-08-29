# Set Cordio root directory
set(CORDIO_ROOT ${MAXIM_LIBRARIES}/Cordio)

# Create cordio static library
# add_library(cordio STATIC)

# Add include directories
target_include_directories(no-os PUBLIC
    ${CORDIO_ROOT}/wsf/include
    ${CORDIO_ROOT}/ble-host/include
    ${CORDIO_ROOT}/ble-host/sources/stack/cfg
    ${CORDIO_ROOT}/ble-host/sources/stack/hci
    ${CORDIO_ROOT}/ble-host/sources/stack/l2c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm
    ${CORDIO_ROOT}/ble-host/sources/stack/smp
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle
    ${CORDIO_ROOT}/ble-host/sec/common
    ${CORDIO_ROOT}/controller/include/ble
    ${CORDIO_ROOT}/controller/include/common
    ${CORDIO_ROOT}/controller/sources/ble/include
    ${CORDIO_ROOT}/platform/include
    ${CORDIO_ROOT}/ble-profiles/include
    ${CORDIO_ROOT}/ble-profiles/sources/profiles
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/include
    ${CORDIO_ROOT}/ble-profiles/sources/services
    ${CORDIO_ROOT}/ble-profiles/sources/af
    ${CORDIO_ROOT}/thirdparty/uecc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal

    # ${MAXIM_LIBRARIES}/PeriphDrivers/Include/MAX32690
    # ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Include
)

# Collect all source files from specified directories
set(CORDIO_SRC_DIRS
    ${CORDIO_ROOT}/host/sources
    ${CORDIO_ROOT}/ble-profiles/sources
    ${CORDIO_ROOT}/controller/sources/ble
    ${CORDIO_ROOT}/controller/sources/common
    ${CORDIO_ROOT}/controller/sources/ble/bb
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle
    ${CORDIO_ROOT}/ble-host/sources/hci/common
    ${CORDIO_ROOT}/ble-host/sources/stack/att
    ${CORDIO_ROOT}/ble-host/sources/stack/dm
    ${CORDIO_ROOT}/ble-host/sources/stack/hci
    ${CORDIO_ROOT}/ble-host/sources/stack/smp
    ${CORDIO_ROOT}/ble-host/sources/stack/l2c
    ${CORDIO_ROOT}/ble-host/sources/stack/cfg
    ${CORDIO_ROOT}/ble-host/sources/sec/common
    ${CORDIO_ROOT}/wsf/sources/targets/baremetal
    ${CORDIO_ROOT}/wsf/sources/util
    ${CORDIO_ROOT}/thirdparty/uecc
)

set(CORDIO_SOURCES)
foreach(src_dir ${CORDIO_SRC_DIRS})
    if(EXISTS ${src_dir})
        file(GLOB_RECURSE dir_sources "${src_dir}/*.c")
        list(APPEND CORDIO_SOURCES ${dir_sources})
    endif()
endforeach()

# Add specific platform sources
if(EXISTS ${CORDIO_ROOT}/platform/targets/maxim/max32655/sources/pal_crypto.c)
    list(APPEND CORDIO_SOURCES ${CORDIO_ROOT}/platform/targets/maxim/max32655/sources/pal_crypto.c)
endif()
if(EXISTS ${CORDIO_ROOT}/platform/targets/maxim/max32655/sources/pal_timer.c)
    list(APPEND CORDIO_SOURCES ${CORDIO_ROOT}/platform/targets/maxim/max32655/sources/pal_timer.c)
endif()
if(EXISTS ${CORDIO_ROOT}/platform/targets/maxim/max32655/sources/pal_cfg.c)
    list(APPEND CORDIO_SOURCES ${CORDIO_ROOT}/platform/targets/maxim/max32655/sources/pal_cfg.c)
endif()

# Remove ignored files
list(REMOVE_ITEM CORDIO_SOURCES ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_ecc_debug.c)

# Add platform-dependent sources from libraries/cordio/src
set(CORDIO_PLATFORM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/cordio_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/periph_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/stack_periph.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_rtc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_sys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_uart.c
)
list(APPEND CORDIO_SOURCES ${CORDIO_PLATFORM_SOURCES})

# Add sources to cordio library
if(CORDIO_SOURCES)
    target_sources(no-os PRIVATE ${CORDIO_SOURCES})
endif()

# Add compile definitions
target_compile_definitions(no-os PRIVATE
    BT_ENABLED
    CHCI_TR_UART=1
    HCI_TR_EXACTLE=1
    BT_VER=11
    uECC_ASM=2
    PAL_TMR_IDX=4
    PAL_SLEEP_TMR_IDX=5
)

# Add compiler flags
target_compile_options(no-os PRIVATE -fomit-frame-pointer)

# Link cordio library to no-os target
# target_link_libraries(no-os cordio)