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

function(config_platform_sdk BUILD_TARGET)
        if(${PLATFORM} STREQUAL "stm32")
                if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build")
                        execute_process(
                                COMMAND ${CMAKE_COMMAND}
                                -D STM32_TEMPLATE_FILE=${NO_OS_DIR}/cmake/stm32_cubemx_template.cubemx
                                -D STM32_TARGET_BUILD=${CMAKE_CURRENT_BINARY_DIR}
                                -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                                -D BOARD=${BOARD}
                                -P "${CMAKE_SOURCE_DIR}/cmake/stm32_patch_cubemx_config.cmake"
                                COMMENT "Patching STM32CubeMX config file..."
                        )

                        execute_process(
                                COMMAND /home/xvr/STM32CubeMX/jre/bin/java -jar /home/xvr/STM32CubeMX/STM32CubeMX -q "${CMAKE_CURRENT_BINARY_DIR}/stm32cubemx_config.cubemx"
                                COMMENT "Generating STM32CubeMX project..."
                        )

                        execute_process(
                                COMMAND ${CMAKE_COMMAND}
                                -D CMAKE_FILE_TO_PATCH=${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}/cmake/stm32cubemx/CMakeLists.txt
                                -D STM32_PROJECT_BUILD=${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}
                                -P "${CMAKE_SOURCE_DIR}/cmake/stm32_patch_cubemx.cmake"
                                COMMENT "Generating STM32CubeMX project..."
                        )
                endif()

                if(NOT TARGET STM32_Drivers)
                        add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}/cmake/stm32cubemx ${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}/cmake/stm32cubemx/build)
                endif()

                target_link_options(${BUILD_TARGET} PRIVATE -T${CMAKE_CURRENT_BINARY_DIR}/${BOARD}_build/${BOARD}/STM32F469XX_FLASH.ld)
                target_link_libraries(${BUILD_TARGET} STM32_Drivers)
                target_compile_definitions(${BUILD_TARGET} PRIVATE -DSTM32_PLATFORM=1)
        endif()
endfunction()

# add_subdirectory(eval-adxl355-pmdz)
add_subdirectory(template)
add_subdirectory(iio_demo)