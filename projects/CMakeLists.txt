include(project_utils)

if(NATIVE_BUILD AND NOT "${PROJECT_DEFCONFIG}" STREQUAL "")
    get_filename_component(SELECTED_PROJECT "${PROJECT_DEFCONFIG}" DIRECTORY)

    if(NOT "${SELECTED_PROJECT}" STREQUAL "" AND
       EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SELECTED_PROJECT}/CMakeLists.txt")
        add_subdirectory("${SELECTED_PROJECT}")
    endif()
endif()
