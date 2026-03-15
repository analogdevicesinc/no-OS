# Resolve the .ioc file path from CONFIG_STM32_IOC_PATH or fall back to ${BOARD}.ioc.
# Sets IOC_FILE in the caller's scope.
function(resolve_stm32_ioc_file)
        if(DEFINED CONFIG_STM32_IOC_PATH AND NOT "${CONFIG_STM32_IOC_PATH}" STREQUAL "")
                if(IS_ABSOLUTE "${CONFIG_STM32_IOC_PATH}")
                        set(IOC_FILE "${CONFIG_STM32_IOC_PATH}" PARENT_SCOPE)
                else()
                        set(IOC_FILE "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/${CONFIG_STM32_IOC_PATH}" PARENT_SCOPE)
                endif()
        else()
                set(IOC_FILE "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/${BOARD}.ioc" PARENT_SCOPE)
        endif()
endfunction()

# Derive OPENOCD variables from the .ioc file, falling back to the TARGET
# variable when the .ioc is unavailable or incomplete.
# Sets OPENOCD_INTERFACE, OPENOCD_CHIPNAME, and OPENOCD_TARGETCFG in the
# caller's scope.
function(resolve_stm32_openocd_config IOC_FILE)
        # Default interface for all STM32 boards
        if(NOT DEFINED OPENOCD_INTERFACE)
                set(OPENOCD_INTERFACE "interface/stlink.cfg" PARENT_SCOPE)
        endif()

        # Primary: parse the .ioc file
        if(EXISTS "${IOC_FILE}")
                file(READ "${IOC_FILE}" IOC_CONTENT)

                string(REGEX MATCH "Mcu\\.CPN=([^\r\n]+)" _match "${IOC_CONTENT}")
                if(_match)
                        set(_chipname "${CMAKE_MATCH_1}")
                        set(OPENOCD_CHIPNAME "${_chipname}" PARENT_SCOPE)
                endif()

                string(REGEX MATCH "Mcu\\.Family=([^\r\n]+)" _match "${IOC_CONTENT}")
                if(_match)
                        string(TOLOWER "${CMAKE_MATCH_1}" _family)
                        set(OPENOCD_TARGETCFG "target/${_family}x.cfg" PARENT_SCOPE)
                        message(STATUS "Extracted OpenOCD config from .ioc: chip=${_chipname}, target=target/${_family}x.cfg")
                        return()
                endif()
        endif()

        # Fallback: derive from TARGET (e.g. "stm32f756" → "stm32f7")
        set(OPENOCD_CHIPNAME "${TARGET}" PARENT_SCOPE)
        string(REGEX MATCH "^(stm32[a-z][0-9])" _family "${TARGET}")
        if(_family)
                set(OPENOCD_TARGETCFG "target/${_family}x.cfg" PARENT_SCOPE)
                message(STATUS "Using fallback OpenOCD config: chip=${TARGET}, target=target/${_family}x.cfg")
        else()
                set(OPENOCD_TARGETCFG "target/${TARGET}.cfg" PARENT_SCOPE)
                message(STATUS "Using fallback OpenOCD config: chip=${TARGET}, target=target/${TARGET}.cfg")
        endif()
endfunction()

function(config_stm32_sdk BUILD_TARGET)
        resolve_stm32_ioc_file()
        resolve_stm32_openocd_config("${IOC_FILE}")
        generate_openocd_config()

        # Extract IOC name without extension for directory naming
        get_filename_component(IOC_NAME "${IOC_FILE}" NAME_WE)
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
                        -D CMAKE_FILE_TO_PATCH=${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${IOC_NAME}/cmake/stm32cubemx/CMakeLists.txt
                        -D STM32_PROJECT_BUILD=${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${IOC_NAME}
                        -P "${NO_OS_DIR}/cmake/stm32_patch_cubemx.cmake"
                        COMMENT "Patching STM32CubeMX generated CMakeLists.txt..."
                )

                file(GLOB STARTUP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/*/startup_*.s)
                execute_process(
                        COMMAND ${VENV_PYTHON_EXE} ${EXTI_SCRIPT} ${STARTUP_FILE} ${EXTI_GEN_FILE}
                )
        endif()

        if(NOT TARGET STM32_Drivers)
                add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${IOC_NAME}/cmake/stm32cubemx ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${IOC_NAME}/cmake/stm32cubemx/build)
        endif()

        target_sources(no-os PRIVATE ${EXTI_GEN_FILE})
        file(GLOB LINKER_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/*/*_FLASH.ld)
        target_link_libraries(no-os STM32_Drivers)
        target_compile_definitions(no-os PRIVATE -DSTM32_PLATFORM=1)
        target_link_options(${BUILD_TARGET} PRIVATE -T${LINKER_SCRIPT_FILE})
endfunction()
