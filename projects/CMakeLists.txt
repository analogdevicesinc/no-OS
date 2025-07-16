# include(ExternalProject)

# ExternalProject_Add(eval-adxl355-pmdz
#   PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/eval-adxl355-pmdz
#   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eval-adxl355-pmdz
#   INSTALL_COMMAND cmake -E echo "Skipping install step."
#   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DPLATFORM=${PLATFORM}
# )

include(FlashTools)

set(GENERATED_SOURCES_CMAKE "${CMAKE_CURRENT_BINARY_DIR}/generated_sources.cmake")
include(${GENERATED_SOURCES_CMAKE} OPTIONAL)

function(stm32_platform_sdk BUILD_TARGET)
        # message(FATAL_ERROR "Trigger")
        add_custom_target(generate_${BUILD_TARGET}
                COMMAND ${STM32CUBEMX}/jre/bin/java -jar ${STM32CUBEMX}/STM32CubeMX
                        -q iio_demo.cubemx
                BYPRODUCTS "${GENERATED_SOURCES_CMAKE}"
                VERBATIM
        )
endfunction()

if(${PLATFORM} STREQUAL "stm32")
        if (NOT EXISTS "${CMAKE_BINARY_DIR}/test_build")
                execute_process(
                        COMMAND /home/xvr/STM32CubeMX/jre/bin/java -jar /home/xvr/STM32CubeMX/STM32CubeMX -q "/mnt/drive/testing/no-OS/projects/iio_demo/iio_demo.cubemx"
                        COMMENT "Generating STM32CubeMX project..."
                )

                execute_process(
                        COMMAND ${CMAKE_COMMAND}
                        -D CMAKE_FILE_TO_PATCH=${CMAKE_BINARY_DIR}/test_build/app/cmake/stm32cubemx/CMakeLists.txt
                        -P "${CMAKE_SOURCE_DIR}/cmake/stm32_patch_cubemx.cmake"
                        COMMENT "Generating STM32CubeMX project..."
                )
        endif()

        add_subdirectory(${CMAKE_BINARY_DIR}/test_build/app/cmake/stm32cubemx ${CMAKE_BINARY_DIR}/build/test_build/app/cmake/stm32cubemx/build)
endif()

# add_custom_target(generate_cubemx_project
#     # Ensure the output directory is clean before generating
#     # COMMAND ${CMAKE_COMMAND} -E rm -rf "${GENERATED_PROJECT_DIR}"
#     # Run CubeMX, telling it to output into our specific directory
#     COMMAND /home/xvr/STM32CubeMX/jre/bin/java -jar /home/xvr/STM32CubeMX/STM32CubeMX -q "/mnt/drive/testing/no-OS/projects/iio_demo/iio_demo.cubemx"
#     COMMAND ${CMAKE_COMMAND}
#         -D CMAKE_FILE_TO_PATCH=${CMAKE_BINARY_DIR}/test_build/app/cmake/stm32cubemx/CMakeLists.txt
#         -P "${CMAKE_SOURCE_DIR}/cmake/stm32_patch_cubemx.cmake"
#     COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}
# )

# function(add_platform_sdk BUILD_TARGET)
#         # if(PLATFORM STREQUAL "stm32")
#         stm32_platform_sdk(${BUILD_TARGET})
#         # endif()

#         add_library(platformsdk STATIC "")
#         # target_link_libraries(${BUILD_TARGET} platformsdk)

#         if(GENERATED_SOURCES)
#                 target_sources(platformsdk PRIVATE "test_build/app/Core/Src/main.c")
#                 target_link_libraries(${TARGET} platformsdk)
#         endif()
                
#         add_dependencies(platformsdk generate_${BUILD_TARGET})
# endfunction()

function(add_platform_sdk TARGET_NAME)
    # 1. Define the keywords your function will accept
    set(options) # No boolean options in this case
    set(oneValueArgs) # No single-value arguments
    set(multiValueArgs SOURCES INCLUDE_DIRS) # Keywords for lists of values

    # 2. This command processes all arguments passed to the function
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 3. Check if any sources were actually passed with the SOURCES keyword
    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "No SOURCES provided to add_platform_sdk for target '${TARGET_NAME}'.")
    endif()

    # 4. Create the library using the PARSED arguments (ARG_SOURCES)
    add_library(${TARGET_NAME} STATIC ${ARG_SOURCES})

    # 5. Add include directories from the PARSED arguments (ARG_INCLUDE_DIRS)
    target_include_directories(${TARGET_NAME} INTERFACE ${ARG_INCLUDE_DIRS})
endfunction()

add_subdirectory(eval-adxl355-pmdz)
add_subdirectory(template)
add_subdirectory(iio_demo)