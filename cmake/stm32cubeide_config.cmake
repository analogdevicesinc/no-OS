# --- STM32CubeIDE Project Generation ---
#
# This module generates .project and .cproject files in CMAKE_BINARY_DIR
# that can be imported into STM32CubeIDE. The project uses linked resources
# to reference no-OS source files, allowing them to be edited from the IDE.
#
# The build is configured to use cmake as an external builder, so the same
# build directory is used whether building from CLI or IDE.

function(generate_stm32cubeide_project PROJECT_NAME)
    if(NOT PLATFORM STREQUAL "stm32")
        return()
    endif()

    message(STATUS "Generating STM32CubeIDE project files...")

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

    # Build include paths for .cproject
    # These are the standard no-OS include directories
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

    # Build compile definitions for .cproject
    set(COMPILE_DEFS_LIST
        "USE_HAL_DRIVER"
        "STM32_PLATFORM=1"
    )

    # Add MCU-specific define (e.g., STM32F756xx from STM32F756ZGT6)
    # Extract family and number (e.g., STM32F756 from STM32F756ZGT6)
    string(REGEX MATCH "^(STM32[A-Z][0-9]+)" MCU_FAMILY "${MCU_NAME}")
    if(MCU_FAMILY)
        set(MCU_DEFINE "${MCU_FAMILY}xx")
        list(APPEND COMPILE_DEFS_LIST "${MCU_DEFINE}")
    endif()

    # Check for IIO support
    if(CONFIG_IIO)
        list(APPEND COMPILE_DEFS_LIST "IIO_SUPPORT")
    endif()

    # Format compile definitions for .cproject XML
    set(COMPILE_DEFINITIONS "")
    foreach(def ${COMPILE_DEFS_LIST})
        string(APPEND COMPILE_DEFINITIONS "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${def}\"/>\n")
    endforeach()

    # Create IDE project in source root (like .vscode/)
    set(IDE_PROJECT_DIR "${CMAKE_SOURCE_DIR}/.cubeide")
    file(MAKE_DIRECTORY "${IDE_PROJECT_DIR}")

    # Generate .project file
    configure_file(
        "${NO_OS_DIR}/cmake/templates/stm32cubeide_project.in"
        "${IDE_PROJECT_DIR}/.project"
        @ONLY
    )

    # Generate .cproject file
    configure_file(
        "${NO_OS_DIR}/cmake/templates/stm32cubeide_cproject.in"
        "${IDE_PROJECT_DIR}/.cproject"
        @ONLY
    )

    # Create .settings directory for additional IDE settings
    file(MAKE_DIRECTORY "${IDE_PROJECT_DIR}/.settings")

    # Generate language settings to help with indexing
    file(WRITE "${IDE_PROJECT_DIR}/.settings/language.settings.xml"
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>
<project>
\t<configuration id=\"com.st.stm32cube.ide.mcu.gnu.managedbuild.config.exe.debug\" name=\"Debug\">
\t\t<extension point=\"org.eclipse.cdt.core.LanguageSettingsProvider\">
\t\t\t<provider class=\"org.eclipse.cdt.managedbuilder.language.settings.providers.GCCBuiltinSpecsDetector\" console=\"false\" env-hash=\"0\" id=\"org.eclipse.cdt.managedbuilder.core.GCCBuiltinSpecsDetector\" keep-relative-paths=\"false\" name=\"CDT GCC Built-in Compiler Settings\" parameter=\"\${COMMAND} \${FLAGS} -E -P -v -dD &quot;\${INPUTS}&quot;\" prefer-non-shared=\"true\">
\t\t\t\t<language-scope id=\"org.eclipse.cdt.core.gcc\"/>
\t\t\t\t<language-scope id=\"org.eclipse.cdt.core.g++\"/>
\t\t\t</provider>
\t\t\t<provider class=\"org.eclipse.cdt.core.language.settings.providers.LanguageSettingsGenericProvider\" id=\"org.eclipse.cdt.ui.UserLanguageSettingsProvider\" name=\"CDT User Setting Entries\" prefer-non-shared=\"true\" store-entries-with-project=\"true\"/>
\t\t\t<provider class=\"org.eclipse.cdt.managedbuilder.language.settings.providers.CompileCommandsJsonParser\" id=\"org.eclipse.cdt.managedbuilder.core.CompileCommandsJsonParser\" name=\"Compile Commands JSON Parser\" prefer-non-shared=\"true\" store-entries-with-project=\"true\">
\t\t\t\t<language-scope id=\"org.eclipse.cdt.core.gcc\"/>
\t\t\t\t<language-scope id=\"org.eclipse.cdt.core.g++\"/>
\t\t\t</provider>
\t\t</extension>
\t</configuration>
</project>
")

    message(STATUS "Generated STM32CubeIDE project in ${IDE_PROJECT_DIR}")
    message(STATUS "  Import in STM32CubeIDE: File -> Import -> Existing Projects into Workspace")
    message(STATUS "  Browse to: ${IDE_PROJECT_DIR}")
endfunction()
