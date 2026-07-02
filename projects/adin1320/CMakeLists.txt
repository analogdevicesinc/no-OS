add_executable(adin1320)

# Common sources
target_sources(adin1320 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# basic_init example
if(CONFIG_ADIN1320_BASIC_INIT_EXAMPLE)
    target_sources(adin1320 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_init/basic_init.c
    )
    target_include_directories(adin1320 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_init
    )
endif()

# basic_init_2devices example
if(CONFIG_ADIN1320_BASIC_INIT_2DEVICES_EXAMPLE)
    target_sources(adin1320 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_init_2devices/basic_init_2devices.c
    )
    target_include_directories(adin1320 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_init_2devices
    )
endif()

# cmd example
if(CONFIG_ADIN1320_CMD_EXAMPLE)
    target_sources(adin1320 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/cmd/cmd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/cmd/cmd_list.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/cmd/cmd_mode.c
    )
    target_include_directories(adin1320 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/cmd
    )
endif()

# wake_on_lan example
if(CONFIG_ADIN1320_WAKE_ON_LAN_EXAMPLE)
    target_sources(adin1320 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/wake_on_lan/wake_on_lan.c
    )
    target_include_directories(adin1320 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/wake_on_lan
    )
endif()

# Optional: force the PHY to a fixed link speed (basic_init examples).
if(CONFIG_ADIN1320_FORCE_SPEED)
    target_compile_definitions(adin1320 PRIVATE FORCE_SPEED)
endif()

target_include_directories(adin1320 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adin1320 no-os)

config_platform_sdk(adin1320)
add_flash_target(adin1320)
post_build_config(adin1320)
