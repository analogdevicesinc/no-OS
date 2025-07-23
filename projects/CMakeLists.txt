# include(ExternalProject)

# ExternalProject_Add(eval-adxl355-pmdz
#   PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/eval-adxl355-pmdz
#   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eval-adxl355-pmdz
#   INSTALL_COMMAND cmake -E echo "Skipping install step."
#   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DPLATFORM=${PLATFORM}
# )

include (project_utils)

if(${NATIVE_BUILD})
        if(NOT ${PROJECT_DEFCONFIG} STREQUAL "")
                get_filename_component(SELECTED_PROJECT ${PROJECT_DEFCONFIG} DIRECTORY)
                function(no_os_project_ifdef PROJECT_TARGET)
                        if(${PROJECT_TARGET} STREQUAL ${SELECTED_PROJECT})
                                add_subdirectory(${PROJECT_TARGET})
                        endif()
                endfunction()

                no_os_project_ifdef(eval-adxl355-pmdz)
                no_os_project_ifdef(template)
                no_os_project_ifdef(iio_demo)
        endif()
endif()