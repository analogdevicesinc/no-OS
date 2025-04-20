# include(ExternalProject)

# ExternalProject_Add(eval-adxl355-pmdz
#   PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/eval-adxl355-pmdz
#   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eval-adxl355-pmdz
#   INSTALL_COMMAND cmake -E echo "Skipping install step."
#   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DPLATFORM=${PLATFORM}
# )

add_subdirectory(eval-adxl355-pmdz)
add_subdirectory(template)
add_subdirectory(iio_demo)