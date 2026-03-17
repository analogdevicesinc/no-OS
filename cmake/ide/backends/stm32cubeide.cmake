# --- STM32CubeIDE IDE Backend ---
#
# Generates .cubeide/ project files that can be imported into STM32CubeIDE:
#   - .project                         (project description with linked resources)
#   - .cproject                        (build/toolchain configuration)
#   - .settings/language.settings.xml  (indexer/language settings)

set(CUBEIDE_TEMPLATES_DIR "${CMAKE_CURRENT_LIST_DIR}/../templates/stm32cubeide")

# Configure-time generation of STM32CubeIDE project files.
function(ide_stm32cubeide_configure PROJECT_TARGET)
    if(NOT PLATFORM STREQUAL "stm32")
        return()
    endif()

    message(STATUS "Generating STM32CubeIDE project files...")

    set(PROJECT_NAME "${PROJECT_TARGET}")

    # Get project source directory
    if(DEFINED NO_OS_PROJECT_NAME)
        set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}")
    else()
        set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
    endif()

    # Extract MCU name from IOC file or use fallback
    set(MCU_NAME "${TARGET}")
    if(DEFINED IOC_FILE AND EXISTS "${IOC_FILE}")
        file(READ "${IOC_FILE}" IOC_CONTENT)
        string(REGEX MATCH "Mcu\\.CPN=([^\r\n]+)" MCU_CPN_MATCH "${IOC_CONTENT}")
        if(MCU_CPN_MATCH)
            set(MCU_NAME "${CMAKE_MATCH_1}")
        endif()
    endif()

    # Find linker script
    file(GLOB LINKER_SCRIPT "${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}/${BOARD}_build/*/*_FLASH.ld")
    if(NOT LINKER_SCRIPT)
        set(LINKER_SCRIPT "")
    endif()

    # --- Build linked resources XML from IDE_SOURCE_GROUPS ---
    set(LINKED_RESOURCES "")
    foreach(_group ${IDE_SOURCE_GROUPS})
        string(REGEX MATCH "^([^=]+)=(.+)$" _match "${_group}")
        if(_match)
            set(_name "${CMAKE_MATCH_1}")
            set(_path "${CMAKE_MATCH_2}")
            if(EXISTS "${_path}")
                string(APPEND LINKED_RESOURCES "\t\t<link>\n")
                string(APPEND LINKED_RESOURCES "\t\t\t<name>${_name}</name>\n")
                string(APPEND LINKED_RESOURCES "\t\t\t<type>2</type>\n")
                string(APPEND LINKED_RESOURCES "\t\t\t<location>${_path}</location>\n")
                string(APPEND LINKED_RESOURCES "\t\t</link>\n")
            endif()
        endif()
    endforeach()
    # Always add build output link
    string(APPEND LINKED_RESOURCES "\t\t<link>\n")
    string(APPEND LINKED_RESOURCES "\t\t\t<name>build_output</name>\n")
    string(APPEND LINKED_RESOURCES "\t\t\t<type>2</type>\n")
    string(APPEND LINKED_RESOURCES "\t\t\t<location>${CMAKE_BINARY_DIR}/build</location>\n")
    string(APPEND LINKED_RESOURCES "\t\t</link>\n")

    # --- Build include paths for .cproject ---
    set(INCLUDE_PATHS_LIST
        "${NO_OS_DIR}/include"
        "${NO_OS_DIR}/drivers/platform/stm32"
        "${NO_OS_DIR}/iio"
        "${NO_OS_DIR}/iio/iio_app"
        "${PROJECT_SOURCE_DIR}/src/common"
        "${PROJECT_SOURCE_DIR}/src/platform/stm32"
    )

    # Add STM32 HAL include paths
    file(GLOB STM32_CORE_INC "${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}/${BOARD}_build/*/Core/Inc")
    file(GLOB STM32_HAL_INC "${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}/${BOARD}_build/*/Drivers/STM32*_HAL_Driver/Inc")
    file(GLOB STM32_CMSIS_DEV_INC "${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}/${BOARD}_build/*/Drivers/CMSIS/Device/ST/STM32*/Include")
    file(GLOB STM32_CMSIS_INC "${CMAKE_BINARY_DIR}/projects/${PROJECT_NAME}/${BOARD}_build/*/Drivers/CMSIS/Include")

    list(APPEND INCLUDE_PATHS_LIST
        ${STM32_CORE_INC}
        ${STM32_HAL_INC}
        ${STM32_CMSIS_DEV_INC}
        ${STM32_CMSIS_INC}
    )

    # Format include paths for .cproject XML
    set(INCLUDE_PATHS "")
    foreach(inc_path ${INCLUDE_PATHS_LIST})
        if(EXISTS "${inc_path}")
            string(APPEND INCLUDE_PATHS "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${inc_path}\"/>\n")
        endif()
    endforeach()

    # --- Build compile definitions for .cproject ---
    set(COMPILE_DEFS_LIST
        "USE_HAL_DRIVER"
        "STM32_PLATFORM=1"
    )

    # Add MCU-specific define (e.g., STM32F756xx from STM32F756ZGT6)
    string(REGEX MATCH "^(STM32[A-Z][0-9]+)" MCU_FAMILY "${MCU_NAME}")
    if(MCU_FAMILY)
        set(MCU_DEFINE "${MCU_FAMILY}xx")
        list(APPEND COMPILE_DEFS_LIST "${MCU_DEFINE}")
    endif()

    if(CONFIG_IIO)
        list(APPEND COMPILE_DEFS_LIST "IIO_SUPPORT")
    endif()

    # Format compile definitions for .cproject XML
    set(COMPILE_DEFINITIONS "")
    foreach(def ${COMPILE_DEFS_LIST})
        string(APPEND COMPILE_DEFINITIONS "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${def}\"/>\n")
    endforeach()

    # --- Generate project files ---
    set(IDE_PROJECT_DIR "${CMAKE_SOURCE_DIR}/.cubeide")
    file(MAKE_DIRECTORY "${IDE_PROJECT_DIR}")

    configure_file(
        "${CUBEIDE_TEMPLATES_DIR}/project.in"
        "${IDE_PROJECT_DIR}/.project"
        @ONLY
    )

    configure_file(
        "${CUBEIDE_TEMPLATES_DIR}/cproject.in"
        "${IDE_PROJECT_DIR}/.cproject"
        @ONLY
    )

    # Generate language settings for indexing
    file(MAKE_DIRECTORY "${IDE_PROJECT_DIR}/.settings")
    configure_file(
        "${CUBEIDE_TEMPLATES_DIR}/language_settings.xml.in"
        "${IDE_PROJECT_DIR}/.settings/language.settings.xml"
        @ONLY
    )

    message(STATUS "Generated STM32CubeIDE project in ${IDE_PROJECT_DIR}")
    message(STATUS "  Import in STM32CubeIDE: File -> Import -> Existing Projects into Workspace")
    message(STATUS "  Browse to: ${IDE_PROJECT_DIR}")
endfunction()

# No post-build steps needed for STM32CubeIDE.
function(ide_stm32cubeide_post_build PROJECT_TARGET)
endfunction()
