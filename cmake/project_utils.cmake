include(FlashTools)
include(ColoredOutput)
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
        set(PLATFORM_SDK_FILE "${NO_OS_DIR}/cmake/${PLATFORM}_platform_sdk.cmake")
        if(EXISTS "${PLATFORM_SDK_FILE}")
                include(${PLATFORM_SDK_FILE})
                string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" PLATFORM_FUNC "${PLATFORM}")
                cmake_language(CALL "config_${PLATFORM_FUNC}_sdk" ${BUILD_TARGET})
        endif()
endfunction()