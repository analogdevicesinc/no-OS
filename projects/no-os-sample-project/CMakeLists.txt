# Check if this is being built as part of the main no-OS tree
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Standalone/out-of-tree build
    cmake_minimum_required(VERSION 3.27)

    # Handle PROJECT_DEFCONFIG for out-of-tree build
    if(NOT DEFINED PROJECT_DEFCONFIG)
        if(EXISTS "${CMAKE_SOURCE_DIR}/project.conf")
            set(PROJECT_DEFCONFIG "project.conf")
            message(STATUS "Using default project.conf for out-of-tree build")
        endif()
    else()
        message(STATUS "Using custom PROJECT_DEFCONFIG: ${PROJECT_DEFCONFIG}")
    endif()

    # Set project name for out-of-tree builds (for board compatibility)
    set(PROJECT_NAME "no-os-sample-project")

    include(FetchContent)

    if (NO_OS_PATH AND EXISTS ${NO_OS_PATH})
            message(STATUS "Using local no-OS from: ${NO_OS_PATH}")
            set(CMAKE_TOOLCHAIN_FILE ${NO_OS_PATH}/drivers/platform/${PLATFORM}/toolchain.cmake)
            set(NO_OS_DIR ${NO_OS_PATH})
            add_subdirectory(${NO_OS_PATH} ${CMAKE_BINARY_DIR}/no-os-core-build)
    else()
            # set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
            FetchContent_Declare(
                    no-os-core
                    GIT_REPOSITORY /mnt/drive/testing/no-OS
                    GIT_TAG feature/unified-presets
                    GIT_SHALLOW TRUE
                    GIT_SUBMODULES ""
            )

            set(FETCHCONTENT_QUIET OFF)
            message(STATUS "Cloning No-OS... 📥")
            FetchContent_MakeAvailable(no-os-core)

            # Set toolchain for FetchContent case
            set(CMAKE_TOOLCHAIN_FILE ${no-os-core_SOURCE_DIR}/drivers/platform/${PLATFORM}/toolchain.cmake)
            set(NO_OS_DIR ${no-os-core_SOURCE_DIR})
    endif()

    # Perform board compatibility check for out-of-tree projects
    if(DEFINED PROJECT_NAME AND DEFINED BOARD)
        # Load board compatibility function
        include(${NO_OS_DIR}/cmake/BoardCompatibility.cmake)
    endif()

    project(no-os-sample VERSION 1.0 LANGUAGES C CXX ASM)
else()
    # In-tree build - no-OS is already configured
    message(STATUS "Building no-os-sample-project as part of no-OS tree")
endif()

add_executable(sample main.c)

config_platform_sdk(sample)
add_flash_target(sample)
post_build_config(sample)

target_link_libraries(sample no-os)