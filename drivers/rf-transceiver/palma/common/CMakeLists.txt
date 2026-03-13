project(AdiCommon)

# Get a list of files to build
file(GLOB SourceFiles *.c)
file(GLOB HeaderFiles *.h)
file(GLOB ErrSourceFiles ./adi_error/*.c)
file(GLOB ErrHeaderFiles ./adi_error/*.h)
file(GLOB HalSourceFiles ./adi_hal/*.c)
file(GLOB HalHeaderFiles ./adi_hal/*.h)
file(GLOB LogSourceFiles ./adi_logging/*.c)
file(GLOB LogHeaderFiles ./adi_logging/*.h)

add_library(AdiCommon STATIC ${SourceFiles} ${ErrSourceFiles} ${HalSourceFiles} ${LogSourceFiles} ${HeaderFiles} ${ErrHeaderFiles} ${HalHeaderFiles} ${LogHeaderFiles})

target_include_directories(AdiCommon PUBLIC
						   "${CommonAdi}"
						   "${CommonAdiError}"
						   "${CommonAdiHal}"
						   "${CommonAdiLogging}"
						   "${Platforms}"
                           )