file(READ ${STM32_TEMPLATE_FILE} OLD_CONTENT)

if(${STM32_IOC_PATH} AND (STM32_IOC_PATH MATCHES ""))
        if(IS_ABSOLUTE ${STM32_IOC_PATH})
                set(CUBEMX_CONFIG_NAME "config load ${STM32_IOC_PATH}\n")
        else()
                set(CUBEMX_CONFIG_NAME "config load ${SOURCE_DIR}/${STM32_IOC_PATH}\n")
        endif()
else()
        set(CUBEMX_CONFIG_NAME "config load ${SOURCE_DIR}/${BOARD}.ioc\n")
endif()

set(CUBEMX_PROJECT_PATH "project path ${STM32_TARGET_BUILD}/${BOARD}_build\n")

file(WRITE "${STM32_TARGET_BUILD}/stm32cubemx_config.cubemx" "${CUBEMX_CONFIG_NAME}${CUBEMX_PROJECT_PATH}${OLD_CONTENT}")
