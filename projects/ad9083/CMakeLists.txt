add_executable(ad9083)

target_sources(ad9083 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_ad9083.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_clocking.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_jesd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uc/uc_settings.c
)

if(CONFIG_AD9083_IIO_EXAMPLE)
    target_compile_definitions(ad9083 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9083 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uc
)

# The ad9083 driver (compiled into the no-os library, not this target) includes
# the project-provided "uc_settings.h", so src/uc must be on the no-os include
# path too. The legacy Make build put it there via the project INCS list.
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/uc)

config_platform_sdk(ad9083)
add_flash_target(ad9083)
post_build_config(ad9083)
target_link_libraries(ad9083 no-os)
