project(AdrvGen6Public)

# Get a list of files to build
file(GLOB SourceFiles *.c)
file(GLOB HeaderFiles ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../private/src ${CMAKE_CURRENT_BINARY_DIR}/private)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../private/bf ${CMAKE_CURRENT_BINARY_DIR}/bf)

add_library(AdrvGen6Public STATIC ${SourceFiles} ${HeaderFiles})

target_link_libraries(AdrvGen6Public PUBLIC Adrvgen6Private)
target_link_libraries(AdrvGen6Public PUBLIC Adrvgen6Bf)

target_include_directories(AdrvGen6Public PUBLIC
                           "${Adrvgen6PublicDeviceHeaders}"
						   "${CommonAdi}"
						   "${CommonAdiError}"
						   "${CommonAdiHal}"
						   "${CommonAdiLogging}"
                           )