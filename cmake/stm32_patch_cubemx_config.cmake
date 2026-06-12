file(READ ${STM32_TEMPLATE_FILE} OLD_CONTENT)

# Resolve IOC path
if(STM32_IOC_PATH)
        if(IS_ABSOLUTE ${STM32_IOC_PATH})
                set(IOC_SOURCE "${STM32_IOC_PATH}")
        else()
                set(IOC_SOURCE "${SOURCE_DIR}/${STM32_IOC_PATH}")
        endif()
else()
        set(IOC_SOURCE "${SOURCE_DIR}/${BOARD}.ioc")
endif()

# Copy the IOC file into the expected CubeMX output directory so that
# CubeMX finds an existing project and does not pop up a "project
# doesn't exist" dialog.  The rest of the build expects the generated
# files at <BOARD>_build/<IOC_NAME>/, so we place the IOC there and
# load it in-place (no "project path" command needed).
get_filename_component(IOC_FILENAME "${IOC_SOURCE}" NAME)
get_filename_component(IOC_NAME "${IOC_SOURCE}" NAME_WE)
set(PROJECT_DIR "${STM32_TARGET_BUILD}/${BOARD}_build/${IOC_NAME}")
file(MAKE_DIRECTORY "${PROJECT_DIR}")
file(COPY_FILE "${IOC_SOURCE}" "${PROJECT_DIR}/${IOC_FILENAME}")

# Suppress the "migrate to newer version?" dialog by patching the copy
file(READ "${PROJECT_DIR}/${IOC_FILENAME}" IOC_CONTENT)
string(REPLACE "ProjectManager.AskForMigrate=true" "ProjectManager.AskForMigrate=false" IOC_CONTENT "${IOC_CONTENT}")
file(WRITE "${PROJECT_DIR}/${IOC_FILENAME}" "${IOC_CONTENT}")

set(CUBEMX_CONFIG_NAME "config load ${PROJECT_DIR}/${IOC_FILENAME}\n")
file(WRITE "${STM32_TARGET_BUILD}/stm32cubemx_config.cubemx" "${CUBEMX_CONFIG_NAME}${OLD_CONTENT}")
