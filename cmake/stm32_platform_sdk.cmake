function(config_stm32_sdk BUILD_TARGET)
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
