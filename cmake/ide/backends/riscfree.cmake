# --- RiscFree IDE Backend (Altera / Nios V) ---
#
# Generates .riscfree/ project files that can be imported into the RiscFree IDE
# (the Eclipse-CDT-based IDE shipped with the Intel/Altera Nios V toolchain):
#   - .project                         (project description with linked resources)
#   - .cproject                        (build/toolchain configuration)
#   - .settings/language.settings.xml  (indexer/language settings)
#
# Scope: build + indexing only. The project builds the app through the real
# no-OS CMake build (external builder: `cmake --build`, managedBuildOn=false),
# and the indexer is fed by compile_commands.json. Hardware debug is NOT wired
# here: the Nios V flow uses the Ashling ash-riscv-gdb-server, for which no-OS
# has no CMake target, and which cannot lock the USB-Blaster over WSL2 usbip.
#
# Modeled on cmake/ide/backends/stm32cubeide.cmake (RiscFree is Eclipse CDT,
# same family as STM32CubeIDE) but stripped of ST-specific ids/natures and
# repointed at the RISC-V toolchain and the Nios V BSP.

set(RISCFREE_TEMPLATES_DIR "${CMAKE_CURRENT_LIST_DIR}/../templates/riscfree")

# Configure-time generation of RiscFree project files.
function(ide_riscfree_configure PROJECT_TARGET)
    if(NOT PLATFORM STREQUAL "altera")
        return()
    endif()

    message(STATUS "Generating RiscFree project files...")

    set(PROJECT_NAME "${PROJECT_TARGET}")

    # Project source directory
    if(DEFINED NO_OS_PROJECT_NAME)
        set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}")
    else()
        set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
    endif()

    # Device label (display only). config_altera_sdk() caches ALTERA_CPU_NAME.
    if(DEFINED ALTERA_CPU_NAME AND NOT ALTERA_CPU_NAME STREQUAL "")
        set(DEVICE_NAME "${ALTERA_CPU_NAME}")
    else()
        set(DEVICE_NAME "niosv")
    endif()

    # The real build/link is delegated to CMake; these flags are display-only in
    # the .cproject (they mirror drivers/platform/altera/toolchain.cmake).
    set(CMAKE_ARCH_FLAGS "-march=rv32im_zicbom -mabi=ilp32")

    # Linker script produced by the BSP (config_altera_sdk links with -T on it).
    if(DEFINED ALTERA_BSP_DIR)
        set(LINKER_SCRIPT "${ALTERA_BSP_DIR}/linker.x")
    else()
        set(LINKER_SCRIPT "")
    endif()

    # --- Build linked resources XML from IDE_SOURCE_GROUPS ---
    # (same idiom as the CubeIDE backend: each name=path becomes a linked folder)
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

    # --- Build include paths for .cproject (indexer fallback; the primary index
    #     source is compile_commands.json via the CDT CompileCommandsJsonParser) ---
    set(INCLUDE_PATHS_LIST
        "${NO_OS_DIR}/include"
        "${NO_OS_DIR}/drivers/platform/altera"
        "${NO_OS_DIR}/iio"
        "${NO_OS_DIR}/iio/iio_app"
        "${PROJECT_SOURCE_DIR}/src/common"
        "${PROJECT_SOURCE_DIR}/src/platform/altera"
    )
    # Nios V BSP include dirs (match config_altera_sdk()).
    if(DEFINED ALTERA_BSP_DIR)
        list(APPEND INCLUDE_PATHS_LIST
            "${ALTERA_BSP_DIR}"
            "${ALTERA_BSP_DIR}/HAL/inc"
            "${ALTERA_BSP_DIR}/drivers/inc"
        )
    endif()

    set(INCLUDE_PATHS "")
    foreach(inc_path ${INCLUDE_PATHS_LIST})
        if(EXISTS "${inc_path}")
            string(APPEND INCLUDE_PATHS "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${inc_path}\"/>\n")
        endif()
    endforeach()

    # --- Build compile definitions for .cproject (match config_altera_sdk()) ---
    set(COMPILE_DEFS_LIST
        "CONFIG_ALTERA_PLATFORM_NIOSV=1"
        "ALT_SINGLE_THREADED"
        "__hal__"
    )
    if(CONFIG_IIO)
        list(APPEND COMPILE_DEFS_LIST "IIO_SUPPORT")
    endif()

    set(COMPILE_DEFINITIONS "")
    foreach(def ${COMPILE_DEFS_LIST})
        string(APPEND COMPILE_DEFINITIONS "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${def}\"/>\n")
    endforeach()

    # --- Generate project files ---
    set(IDE_PROJECT_DIR "${CMAKE_SOURCE_DIR}/.riscfree")
    file(MAKE_DIRECTORY "${IDE_PROJECT_DIR}")

    configure_file(
        "${RISCFREE_TEMPLATES_DIR}/project.in"
        "${IDE_PROJECT_DIR}/.project"
        @ONLY
    )

    configure_file(
        "${RISCFREE_TEMPLATES_DIR}/cproject.in"
        "${IDE_PROJECT_DIR}/.cproject"
        @ONLY
    )

    # Language settings for indexing (CompileCommandsJsonParser reads
    # compile_commands.json from the CMake build dir).
    file(MAKE_DIRECTORY "${IDE_PROJECT_DIR}/.settings")
    configure_file(
        "${RISCFREE_TEMPLATES_DIR}/language_settings.xml.in"
        "${IDE_PROJECT_DIR}/.settings/language.settings.xml"
        @ONLY
    )

    message(STATUS "Generated RiscFree project in ${IDE_PROJECT_DIR} (device: ${DEVICE_NAME})")
    message(STATUS "  Import in RiscFree: File -> Import -> Existing Projects into Workspace")
    message(STATUS "  Browse to: ${IDE_PROJECT_DIR}")
    message(STATUS "  Build in the IDE invokes: ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_TARGET}")
    message(STATUS "  Or open it with: no_os_build.py build ... --open")
endfunction()

# No post-build steps needed for RiscFree (build-only backend).
function(ide_riscfree_post_build PROJECT_TARGET)
endfunction()
