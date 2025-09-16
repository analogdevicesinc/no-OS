# Global colored output configuration for no-OS builds
# This module is automatically included for all projects

# Enable colored output for CMake itself
if(NOT DEFINED CMAKE_COLOR_DIAGNOSTICS)
    set(CMAKE_COLOR_DIAGNOSTICS ON)
endif()

# Set colored output environment variables for build tools
set(ENV{CLICOLOR_FORCE} "1")
set(ENV{FORCE_COLOR} "1")

# Function to enable colored compiler diagnostics
function(enable_colored_compiler_output)
    # Enable colored compiler diagnostics for GCC and Clang
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fdiagnostics-color=always)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fdiagnostics-color=always)
    endif()

    # Also enable for ASM if needed
    if(CMAKE_ASM_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fdiagnostics-color=always)
    endif()
endfunction()

# Automatically enable colored output when this module is included
enable_colored_compiler_output()

# Set a variable to indicate colored output is enabled
set(NO_OS_COLORED_OUTPUT_ENABLED TRUE CACHE INTERNAL "Colored output is enabled")

message(STATUS "🎨 Colored build output enabled globally")