add_executable(maxm86161)

# Common sources
target_sources(maxm86161 PRIVATE
	       $ {CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
	       $ {CMAKE_CURRENT_SOURCE_DIR}/src/platform/$ {PLATFORM}/main.c
	       $ {CMAKE_CURRENT_SOURCE_DIR}/src/platform/$ {PLATFORM}/parameters.c
	      )

# Basic example
if(CONFIG_MAXM86161_BASIC_EXAMPLE)
	target_sources(maxm86161 PRIVATE
		       $ {CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
		      )
	target_include_directories(maxm86161 PUBLIC
				   $ {CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
				  )
	endif()

# IIO example
	if(CONFIG_MAXM86161_IIO_EXAMPLE)
		target_sources(maxm86161 PRIVATE
			       $ {CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
			      )
		target_include_directories(maxm86161 PUBLIC
					   $ {CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
					  )
		endif()

# Attribute test example
		if(CONFIG_MAXM86161_ATTRIBUTE_TEST_EXAMPLE)
			target_sources(maxm86161 PRIVATE
				       $ {CMAKE_CURRENT_SOURCE_DIR}/src/examples/attribute_test/attribute_test_example.c
				      )
			target_include_directories(maxm86161 PUBLIC
						   $ {CMAKE_CURRENT_SOURCE_DIR}/src/examples/attribute_test
						  )
			endif()

			target_include_directories(maxm86161 PUBLIC
						   $ {CMAKE_CURRENT_SOURCE_DIR}/src/common
						   $ {CMAKE_CURRENT_SOURCE_DIR}/src/platform/$ {PLATFORM}
						   $ {CMAKE_CURRENT_SOURCE_DIR}/src/platform
						  )


			target_compile_definitions(maxm86161 PRIVATE
						   IIO_IGNORE_BUFF_OVERRUN_ERR
						  )
			target_compile_definitions(no-os PRIVATE IIO_IGNORE_BUFF_OVERRUN_ERR)

			target_link_libraries(maxm86161 no-os)

			config_platform_sdk(maxm86161)
			add_flash_target(maxm86161)
			post_build_config(maxm86161)
