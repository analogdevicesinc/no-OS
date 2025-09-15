# Board Compatibility Checking Module
# This module provides functions to check if a project supports a specific board

# Function to read supported boards from a project's boards/ directory
function(load_project_boards PROJECT_NAME OUTPUT_VAR)
    set(BOARDS_DIR "${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}/boards")
    set(SUPPORTED_BOARDS "")

    if(EXISTS "${BOARDS_DIR}" AND IS_DIRECTORY "${BOARDS_DIR}")
        file(GLOB BOARD_FILES "${BOARDS_DIR}/*.conf")
        foreach(BOARD_FILE ${BOARD_FILES})
            get_filename_component(BOARD_NAME "${BOARD_FILE}" NAME_WE)
            list(APPEND SUPPORTED_BOARDS "${BOARD_NAME}")
        endforeach()
    else()
        message(WARNING "Board compatibility directory not found: ${BOARDS_DIR}")
        message(WARNING "Project ${PROJECT_NAME} will accept any board (no restrictions)")
    endif()

    set(${OUTPUT_VAR} "${SUPPORTED_BOARDS}" PARENT_SCOPE)
endfunction()

# Function to load board-specific configuration
function(load_board_config PROJECT_NAME BOARD_NAME)
    set(BOARD_CONFIG_FILE "${CMAKE_SOURCE_DIR}/projects/${PROJECT_NAME}/boards/${BOARD_NAME}.conf")

    if(EXISTS "${BOARD_CONFIG_FILE}")
        # Read the board config file and set variables
        file(STRINGS "${BOARD_CONFIG_FILE}" CONFIG_LINES)
        foreach(LINE ${CONFIG_LINES})
            string(STRIP "${LINE}" LINE)
            if(NOT "${LINE}" STREQUAL "" AND NOT "${LINE}" MATCHES "^#.*" AND "${LINE}" MATCHES "^[A-Z_]+=.*")
                string(REGEX MATCH "^([A-Z_]+)=(.*)$" MATCH_RESULT "${LINE}")
                if(MATCH_RESULT)
                    set(VAR_NAME "${CMAKE_MATCH_1}")
                    set(VAR_VALUE "${CMAKE_MATCH_2}")
                    # Set the variable in parent scope with project+board prefix to avoid conflicts
                    set("${PROJECT_NAME}_${BOARD_NAME}_${VAR_NAME}" "${VAR_VALUE}" PARENT_SCOPE)
                    message(DEBUG "Loaded board config: ${VAR_NAME}=${VAR_VALUE}")
                endif()
            endif()
        endforeach()
        message(STATUS "📋 Loaded board configuration: ${PROJECT_NAME}/${BOARD_NAME}")
    endif()
endfunction()

# Function to check if a board is supported by a project
function(check_board_compatibility PROJECT_NAME BOARD_NAME)
    if(NOT DEFINED PROJECT_NAME OR "${PROJECT_NAME}" STREQUAL "")
        # No project specified, skip compatibility check
        return()
    endif()

    load_project_boards("${PROJECT_NAME}" SUPPORTED_BOARDS)

    # If no boards.conf found, allow any board (for legacy projects)
    if("${SUPPORTED_BOARDS}" STREQUAL "")
        message(STATUS "⚠️  No board restrictions for project '${PROJECT_NAME}'")
        return()
    endif()

    list(FIND SUPPORTED_BOARDS "${BOARD_NAME}" BOARD_INDEX)
    if(BOARD_INDEX EQUAL -1)
        # Board not supported - show helpful error
        string(REPLACE ";" ", " BOARDS_LIST "${SUPPORTED_BOARDS}")
        message(FATAL_ERROR
            "❌ Board compatibility error!\n"
            "   Project: ${PROJECT_NAME}\n"
            "   Board: ${BOARD_NAME}\n"
            "   Platform: ${PLATFORM}\n\n"
            "Project '${PROJECT_NAME}' does not support board '${BOARD_NAME}'.\n"
            "Supported boards: ${BOARDS_LIST}\n\n"
            "💡 Tips:\n"
            "   • Check projects/${PROJECT_NAME}/boards/ directory for supported board files\n"
            "   • Each supported board has a ${BOARD_NAME}.conf file in that directory\n"
            "   • Use 'cmake --list-presets=configure' to see available boards\n"
            "   • Use 'cmake --list-presets=build' to see compatible build targets\n\n"
            "Example valid combinations:\n"
            "   cmake --preset <supported_board> -DPROJECT_DEFCONFIG=${PROJECT_NAME}/project.conf\n"
        )
    else()
        message(STATUS "✅ Board compatibility: '${PROJECT_NAME}' supports board '${BOARD_NAME}'")
        # Load board-specific configuration
        load_board_config("${PROJECT_NAME}" "${BOARD_NAME}")
    endif()
endfunction()

# Function to get all supported boards for a project (for documentation/tooling)
function(get_project_boards PROJECT_NAME OUTPUT_VAR)
    load_project_boards("${PROJECT_NAME}" SUPPORTED_BOARDS)
    set(${OUTPUT_VAR} "${SUPPORTED_BOARDS}" PARENT_SCOPE)
endfunction()

# Function to validate all projects have boards/ directories (for CI/testing)
function(validate_all_board_configs)
    file(GLOB PROJECT_DIRS "${CMAKE_SOURCE_DIR}/projects/*")
    set(MISSING_BOARD_DIRS "")
    set(EMPTY_BOARD_DIRS "")

    foreach(PROJECT_DIR ${PROJECT_DIRS})
        if(IS_DIRECTORY "${PROJECT_DIR}")
            get_filename_component(PROJECT_NAME "${PROJECT_DIR}" NAME)
            set(BOARDS_DIR "${PROJECT_DIR}/boards")

            if(NOT EXISTS "${BOARDS_DIR}")
                list(APPEND MISSING_BOARD_DIRS "${PROJECT_NAME}")
            elseif(IS_DIRECTORY "${BOARDS_DIR}")
                file(GLOB BOARD_FILES "${BOARDS_DIR}/*.conf")
                if("${BOARD_FILES}" STREQUAL "")
                    list(APPEND EMPTY_BOARD_DIRS "${PROJECT_NAME}")
                endif()
            endif()
        endif()
    endforeach()

    if(NOT "${MISSING_BOARD_DIRS}" STREQUAL "")
        string(REPLACE ";" ", " MISSING_LIST "${MISSING_BOARD_DIRS}")
        message(WARNING "Projects missing boards/ directory: ${MISSING_LIST}")
        message(WARNING "These projects will accept any board. Consider adding boards/ directories.")
    endif()

    if(NOT "${EMPTY_BOARD_DIRS}" STREQUAL "")
        string(REPLACE ";" ", " EMPTY_LIST "${EMPTY_BOARD_DIRS}")
        message(WARNING "Projects with empty boards/ directory: ${EMPTY_LIST}")
        message(WARNING "These projects will reject all boards. Add board .conf files to boards/ directory.")
    endif()
endfunction()

# Auto-check compatibility if both PROJECT_NAME and BOARD are defined
if(DEFINED PROJECT_NAME AND DEFINED BOARD)
    check_board_compatibility("${PROJECT_NAME}" "${BOARD}")
endif()