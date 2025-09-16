include(FlashTools)
include(ColoredOutput)

set(GENERATED_SOURCES_CMAKE "${CMAKE_CURRENT_BINARY_DIR}/generated_sources.cmake")
include(${GENERATED_SOURCES_CMAKE} OPTIONAL)

function(generate_openocd_config)
        # For STM32 platform, try to extract OpenOCD config from project .ioc file
        if(PLATFORM STREQUAL "stm32" AND DEFINED NO_OS_PROJECT_NAME)
                # Look for .ioc files in the project directory
                file(GLOB IOC_FILES "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/*.ioc")

                if(IOC_FILES)
                        list(GET IOC_FILES 0 IOC_FILE)  # Use first .ioc file found
                        message(STATUS "Found .ioc file: ${IOC_FILE}")

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
                        "${NO_OS_DIR}/cmake/templates/openocd.in"
                        "${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg"
                        @ONLY
                )
                message(STATUS "Generated OpenOCD config: ${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg")
        else()
                message(STATUS "OpenOCD config variables not set, skipping openocd.cfg generation")
        endif()
endfunction()

function(post_build_config PROJECT_TARGET)
        set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
        
        # Generate OpenOCD config file
        generate_openocd_config()
        
        add_custom_command(
                TARGET ${PROJECT_TARGET}
                POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_TARGET}> ${PROJECT_TARGET}.hex
                COMMAND ${CMAKE_COMMAND}
                        -DELF_PATH=$<TARGET_FILE:${PROJECT_TARGET}>
                        -DNO_OS_DIR=${NO_OS_DIR}
                        -DPROJECT_HOME=${PROJECT_DIR}
                        -DTARGET=${TARGET}
                        -P "${NO_OS_DIR}/cmake/vscode_config.cmake"
                COMMENT "Generating ${PROJECT_TARGET}.hex"
        )
endfunction()

function(config_platform_sdk BUILD_TARGET)
        if(${NATIVE_BUILD})
                set(PROJECT_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
        else()
                set(PROJECT_RELATIVE_PATH ${CMAKE_SOURCE_DIR})
        endif()

        if(${PLATFORM} STREQUAL "stm32")
                set(EXTI_SCRIPT ${NO_OS_DIR}/tools/scripts/platform/stm32/exti_script.py)
                set(EXTI_GEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/stm32_gpio_irq_generated.c)
                if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build")
                        execute_process(
                                COMMAND ${CMAKE_COMMAND}
                                -D STM32_TEMPLATE_FILE=${NO_OS_DIR}/cmake/stm32_cubemx_template.cubemx
                                -D STM32_TARGET_BUILD=${CMAKE_CURRENT_BINARY_DIR}
                                -D STM32_IOC_PATH=${CONFIG_STM32_IOC_PATH}
                                -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                                -D BOARD=${BOARD}
                                -P "${NO_OS_DIR}/cmake/stm32_patch_cubemx_config.cmake"
                                COMMENT "Patching STM32CubeMX config file..."
                        )

                        execute_process(
                                COMMAND ${STM32CUBEMX_JAVA} -jar ${STM32CUBEMX_EXECUTABLE} -q "${CMAKE_CURRENT_BINARY_DIR}/stm32cubemx_config.cubemx"
                                COMMENT "Generating STM32CubeMX project..."
                        )

                        execute_process(
                                COMMAND ${CMAKE_COMMAND}
                                -D CMAKE_FILE_TO_PATCH=${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}/cmake/stm32cubemx/CMakeLists.txt
                                -D STM32_PROJECT_BUILD=${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}
                                -P "${NO_OS_DIR}/cmake/stm32_patch_cubemx.cmake"
                                COMMENT "Generating STM32CubeMX project..."
                        )

                        file(GLOB STARTUP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/*/startup_*.s)
                        execute_process(
                                COMMAND ${VENV_PYTHON_EXE} ${EXTI_SCRIPT} ${STARTUP_FILE} ${EXTI_GEN_FILE}
                        )
                endif()
                        
                if(NOT TARGET STM32_Drivers)
                        add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}/cmake/stm32cubemx ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}/cmake/stm32cubemx/build)
                endif()

                target_sources(no-os PRIVATE ${EXTI_GEN_FILE})
                file(GLOB LINKER_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/*/*_FLASH.ld)
                target_link_libraries(no-os STM32_Drivers)
                target_compile_definitions(no-os PRIVATE -DSTM32_PLATFORM=1)
                target_link_options(${BUILD_TARGET} PRIVATE -T${LINKER_SCRIPT_FILE})
        endif()
endfunction()