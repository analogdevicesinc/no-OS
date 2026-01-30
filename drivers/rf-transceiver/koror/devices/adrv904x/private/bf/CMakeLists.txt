project(Adrvgen6Bf)

# Get a list of files to build
file(GLOB SourceFiles *.c)
file(GLOB HeaderFiles *.h)

# Currently for 'ISO C restricts enumerator values to range of ‘int’' warning for BF API
add_compile_options(-Wno-error=pedantic)

if(BUILD_BF_FILES EQUAL 1)
add_library(Adrvgen6Bf STATIC ${SourceFiles} ${HeaderFiles})

target_include_directories(Adrvgen6Bf PUBLIC
                           "${Adrvgen6PublicDeviceHeaders}"
						   "${CommonAdi}"
						   "${CommonAdiError}"
						   "${CommonAdiHal}"
						   "${CommonAdiLogging}"
						   "${Platforms}"
						   )

add_custom_command(
   TARGET Adrvgen6Bf
   POST_BUILD
   COMMAND echo "Copying bf lib to resources directory"
   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../resources/bf/
   COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:Adrvgen6Bf>" "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../resources/bf/"
   )
endif()