include(FlashTools)
include(${NO_OS_DIR}/cmake/ide/ide_config.cmake)

set(GENERATED_SOURCES_CMAKE "${CMAKE_CURRENT_BINARY_DIR}/generated_sources.cmake")
include(${GENERATED_SOURCES_CMAKE} OPTIONAL)

function(generate_openocd_config)
        if(DEFINED OPENOCD_INTERFACE AND DEFINED OPENOCD_CHIPNAME AND DEFINED OPENOCD_TARGETCFG)
                if(NOT DEFINED OPENOCD_EXTRA_COMMANDS)
                        set(OPENOCD_EXTRA_COMMANDS "")
                endif()
                if(NOT DEFINED OPENOCD_POST_COMMANDS)
                        set(OPENOCD_POST_COMMANDS "")
                endif()
                configure_file(
                        "${NO_OS_DIR}/cmake/ide/templates/openocd.in"
                        "${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg"
                        @ONLY
                )
                message(STATUS "Generated OpenOCD config: ${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg")
        endif()
endfunction()

function(post_build_config PROJECT_TARGET)
        # Generate OpenOCD config file
        generate_openocd_config()

        # Binary generation (hex/bin/size)
        add_custom_command(
                TARGET ${PROJECT_TARGET}
                POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_TARGET}> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_TARGET}.hex
                COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_TARGET}> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_TARGET}.bin
                COMMAND ${CMAKE_COMMAND} -E echo "Binary size:"
                COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_TARGET}> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_TARGET}.hex || ${CMAKE_COMMAND} -E true
                COMMENT "Generating ${PROJECT_TARGET}.hex"
        )

        # IDE project file generation (replaces cmake -P vscode_config.cmake
        # and generate_stm32cubeide_project calls)
        ide_generate(${PROJECT_TARGET})
endfunction()

function(config_platform_sdk BUILD_TARGET)
        # Check cmake root first, then platform-specific subdirectory
        set(PLATFORM_SDK_FILE "${NO_OS_DIR}/cmake/${PLATFORM}_platform_sdk.cmake")
        if(NOT EXISTS "${PLATFORM_SDK_FILE}")
                set(PLATFORM_SDK_FILE "${NO_OS_DIR}/cmake/${PLATFORM}/${PLATFORM}_platform_sdk.cmake")
        endif()
        if(EXISTS "${PLATFORM_SDK_FILE}")
                include(${PLATFORM_SDK_FILE})
                string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" PLATFORM_FUNC "${PLATFORM}")
                if(COMMAND "config_${PLATFORM_FUNC}_sdk")
                        cmake_language(CALL "config_${PLATFORM_FUNC}_sdk" ${BUILD_TARGET})
                else()
                        message(WARNING "Platform SDK file ${PLATFORM_SDK_FILE} does not define config_${PLATFORM_FUNC}_sdk()")
                endif()
        else()
                # Not all platforms ship a *_platform_sdk.cmake (e.g. maxim configures
                # its SDK entirely from the toolchain file), so this is informational
                # rather than fatal — but surface it so an unsupported/typo'd PLATFORM
                # doesn't silently skip SDK configuration.
                message(STATUS "No platform SDK file for PLATFORM=${PLATFORM} "
                        "(looked for ${NO_OS_DIR}/cmake/${PLATFORM}_platform_sdk.cmake and "
                        "${NO_OS_DIR}/cmake/${PLATFORM}/${PLATFORM}_platform_sdk.cmake); skipping SDK configuration.")
        endif()
endfunction()