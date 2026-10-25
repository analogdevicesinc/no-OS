add_executable(ad413x)

target_sources(ad413x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.c
)

if(CONFIG_AD413X_IIO_EXAMPLE)
    target_compile_definitions(ad413x PRIVATE IIO_SUPPORT)
endif()

if(CONFIG_AD413X_CORAZ7S_CARRIER)
    target_compile_definitions(ad413x PRIVATE AD413X_CORAZ7S_CARRIER)
endif()

# Device variant define consumed by main.c. The default (AD4130-8) needs no
# macro: the sources use #if, so an undefined symbol evaluates to 0 and falls
# through to the AD4130-8 branch.
if(CONFIG_AD413X_DEV_AD4129_4)
    target_compile_definitions(ad413x PRIVATE AD4129_4_DEV)
elseif(CONFIG_AD413X_DEV_AD4129_8)
    target_compile_definitions(ad413x PRIVATE AD4129_8_DEV)
elseif(CONFIG_AD413X_DEV_AD4130_4)
    target_compile_definitions(ad413x PRIVATE AD4130_4_DEV)
elseif(CONFIG_AD413X_DEV_AD4131_4)
    target_compile_definitions(ad413x PRIVATE AD4131_4_DEV)
elseif(CONFIG_AD413X_DEV_AD4131_8)
    target_compile_definitions(ad413x PRIVATE AD4131_8_DEV)
endif()

target_include_directories(ad413x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(ad413x no-os)

config_platform_sdk(ad413x)
add_flash_target(ad413x)
post_build_config(ad413x)
