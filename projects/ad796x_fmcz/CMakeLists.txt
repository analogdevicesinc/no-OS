add_executable(ad796x_fmcz)

target_sources(ad796x_fmcz PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_AD796X_FMCZ_IIO_EXAMPLE)
	target_sources(ad796x_fmcz PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
	)
	target_include_directories(ad796x_fmcz PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
	)
	# main.c selects the example via a bare #ifdef IIO_EXAMPLE
	target_compile_definitions(ad796x_fmcz PRIVATE IIO_EXAMPLE)
endif()

if(CONFIG_AD796X_FMCZ_BASIC_EXAMPLE)
	target_sources(ad796x_fmcz PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
	)
	target_include_directories(ad796x_fmcz PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
	)
	# main.c selects the example via a bare #elif BASIC_EXAMPLE
	target_compile_definitions(ad796x_fmcz PRIVATE BASIC_EXAMPLE)
endif()

target_include_directories(ad796x_fmcz PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/common
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad796x_fmcz no-os)

config_platform_sdk(ad796x_fmcz)
add_flash_target(ad796x_fmcz)
post_build_config(ad796x_fmcz)
