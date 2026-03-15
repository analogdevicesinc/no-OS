include(FlashTools)
include(ColoredOutput)
include(${NO_OS_DIR}/cmake/ide/ide_config.cmake)

set(GENERATED_SOURCES_CMAKE "${CMAKE_CURRENT_BINARY_DIR}/generated_sources.cmake")
include(${GENERATED_SOURCES_CMAKE} OPTIONAL)

function(generate_openocd_config)
        # For STM32 platform, try to extract OpenOCD config from project .ioc file
        if(PLATFORM STREQUAL "stm32" AND DEFINED NO_OS_PROJECT_NAME)
                # Determine which .ioc file to use (following same pattern as CubeMX config)
                set(IOC_FILE "" PARENT_SCOPE)
                if(DEFINED CONFIG_STM32_IOC_PATH AND NOT "${CONFIG_STM32_IOC_PATH}" STREQUAL "")
                        # Use user-specified .ioc file
                        message(STATUS "DEBUG: Using CONFIG_STM32_IOC_PATH=${CONFIG_STM32_IOC_PATH}")
                        if(IS_ABSOLUTE "${CONFIG_STM32_IOC_PATH}")
                                set(IOC_FILE "${CONFIG_STM32_IOC_PATH}" PARENT_SCOPE)
                        else()
                                set(IOC_FILE "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/${CONFIG_STM32_IOC_PATH}" PARENT_SCOPE)
                        endif()
                        message(STATUS "DEBUG: Resolved IOC_FILE=${IOC_FILE}")
                else()
                        # Fallback to board-named .ioc file
                        set(IOC_FILE "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/${BOARD}.ioc" PARENT_SCOPE)
                        set(IOC_FILE "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/${BOARD}.ioc")
                        message(STATUS "DEBUG: Using fallback IOC_FILE=${IOC_FILE}")
                endif()

                if(EXISTS "${IOC_FILE}")
                        message(STATUS "Found .ioc file: ${IOC_FILE}")
                else()
                        message(STATUS "DEBUG: .ioc file not found at: ${IOC_FILE}")
                endif()

                # Read .ioc file and extract MCU info using CMake
                file(READ "${IOC_FILE}" IOC_CONTENT)

                # Extract Mcu.CPN (Complete Part Number) for chip name
                string(REGEX MATCH "Mcu\\.CPN=([^\r\n]+)" MCU_CPN_MATCH "${IOC_CONTENT}")
                if(MCU_CPN_MATCH)
                        set(OPENOCD_CHIPNAME "${CMAKE_MATCH_1}")
                endif()

                # Extract Mcu.Family for target config mapping
                string(REGEX MATCH "Mcu\\.Family=([^\r\n]+)" MCU_FAMILY_MATCH "${IOC_CONTENT}")
                if(MCU_FAMILY_MATCH)
                        set(MCU_FAMILY "${CMAKE_MATCH_1}")
                        string(TOLOWER "${MCU_FAMILY}" MCU_FAMILY_LOWER)

                        # Map family to OpenOCD target config
                        if(MCU_FAMILY_LOWER MATCHES "stm32f4")
                                set(OPENOCD_TARGETCFG "target/stm32f4x.cfg")
                        elseif(MCU_FAMILY_LOWER MATCHES "stm32f7")
                                set(OPENOCD_TARGETCFG "target/stm32f7x.cfg")
                        elseif(MCU_FAMILY_LOWER MATCHES "stm32h7")
                                set(OPENOCD_TARGETCFG "target/stm32h7x.cfg")
                        elseif(MCU_FAMILY_LOWER MATCHES "stm32l4")
                                set(OPENOCD_TARGETCFG "target/stm32l4x.cfg")
                        elseif(MCU_FAMILY_LOWER MATCHES "stm32g4")
                                set(OPENOCD_TARGETCFG "target/stm32g4x.cfg")
                        else()
                                set(OPENOCD_TARGETCFG "target/${MCU_FAMILY_LOWER}x.cfg")
                        endif()
                endif()

                if(OPENOCD_CHIPNAME AND OPENOCD_TARGETCFG)
                        message(STATUS "Extracted OpenOCD config from .ioc: chip=${OPENOCD_CHIPNAME}, target=${OPENOCD_TARGETCFG}")
                endif()
        endif()

        # Set fallback defaults if not already set (for STM32 or when .ioc parsing fails)
        if(PLATFORM STREQUAL "stm32" AND NOT DEFINED OPENOCD_CHIPNAME)
                set(OPENOCD_CHIPNAME ${TARGET})

                if(${TARGET} MATCHES "^stm32f4")
                        set(OPENOCD_TARGETCFG "target/stm32f4x.cfg")
                elseif(${TARGET} MATCHES "^stm32f7")
                        set(OPENOCD_TARGETCFG "target/stm32f7x.cfg")
                elseif(${TARGET} MATCHES "^stm32l4")
                        set(OPENOCD_TARGETCFG "target/stm32l4x.cfg")
                elseif(${TARGET} MATCHES "^stm32h7")
                        set(OPENOCD_TARGETCFG "target/stm32h7x.cfg")
                else()
                        set(OPENOCD_TARGETCFG "target/${TARGET}.cfg")
                endif()

                message(STATUS "Using fallback OpenOCD config: chip=${OPENOCD_CHIPNAME}, target=${OPENOCD_TARGETCFG}")
        endif()

        # Generate OpenOCD configuration file for debugging
        if(DEFINED OPENOCD_INTERFACE AND DEFINED OPENOCD_CHIPNAME AND DEFINED OPENOCD_TARGETCFG)
                configure_file(
                        "${NO_OS_DIR}/cmake/ide/templates/openocd.in"
                        "${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg"
                        @ONLY
                )
                message(STATUS "Generated OpenOCD config: ${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg")
        else()
                message(STATUS "OpenOCD config variables not set, skipping openocd.cfg generation")
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