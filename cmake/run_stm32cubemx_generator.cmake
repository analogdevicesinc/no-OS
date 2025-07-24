# This script is executed by `cmake -P`.
# The path to our new animation script
set(ANIMATION_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32cubemx_generator.py")

# The full java command we want to run
set(CUBEMX_COMMAND
        COMMAND /home/xvr/STM32CubeMX/jre/bin/java -jar /home/xvr/STM32CubeMX/STM32CubeMX -q "/mnt/drive/testing/no-OS/projects/iio_demo/iio_demo.cubemx"
)

# Use execute_process to run the Python wrapper, which in turn runs the java command.
# We don't need OUTPUT_QUIET here because the python script handles the output.
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${ANIMATION_SCRIPT} ${CUBEMX_COMMAND}
    WORKING_DIRECTORY /mnt/drive/testing/no-OS/build/projects/iio_demo
    RESULT_VARIABLE SCRIPT_RESULT
)

# if(NOT SCRIPT_RESULT EQUAL 0)
#     message(FATAL_ERROR "Generator script failed with exit code: ${SCRIPT_RESULT}")
# endif()