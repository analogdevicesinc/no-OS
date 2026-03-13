project(Adrvgen6Private)

# Get a list of files to build
file(GLOB SourceFiles *.c)
file(GLOB HeaderFiles ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h)

add_library(Adrvgen6Private STATIC ${SourceFiles} ${HeaderFiles})

target_include_directories(Adrvgen6Private PUBLIC
                           "${Adrvgen6PublicDeviceHeaders}"
						   "${CommonAdi}"
						   "${CommonAdiError}"
						   "${CommonAdiHal}"
						   "${CommonAdiLogging}"
						   "${Platforms}"
						   )