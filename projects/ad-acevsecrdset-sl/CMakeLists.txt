add_executable(ad-acevsecrdset-sl)

target_sources(ad-acevsecrdset-sl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/parameters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_machine/state_machine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pilot/pilot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inter/inter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/supply/supply.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rcd/rcd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/relay/relay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/self_test/self_test.c
)

if(CONFIG_AD_ACEVSECRDSET_SL_STOUT_APPLICATION_EXAMPLE)
    target_compile_definitions(ad-acevsecrdset-sl PRIVATE REV_D)
    target_compile_definitions(ad-acevsecrdset-sl PRIVATE NO_OS_LOG_LEVEL=NO_OS_LOG_DEBUG)
endif()

target_include_directories(ad-acevsecrdset-sl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_machine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pilot
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inter
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/src/supply
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rcd
    ${CMAKE_CURRENT_SOURCE_DIR}/src/relay
    ${CMAKE_CURRENT_SOURCE_DIR}/src/self_test
)

target_link_libraries(ad-acevsecrdset-sl no-os)

config_platform_sdk(ad-acevsecrdset-sl)
add_flash_target(ad-acevsecrdset-sl)
post_build_config(ad-acevsecrdset-sl)
