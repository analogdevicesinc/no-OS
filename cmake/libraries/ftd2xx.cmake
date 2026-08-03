# FTD2XX library CMake configuration.
#
# Included from libraries/CMakeLists.txt when CONFIG_FTD2XX=y.  Performs three
# jobs in one pass:
#
#   1. Locate FTD2XX_LIBRARY — the release/ directory produced by
#      tools/scripts/config_ftd2xx.py.  Resolution order:
#        a. CMake cache variable FTD2XX_LIBRARY (e.g. -DFTD2XX_LIBRARY=...)
#        b. Environment variable FTD2XX_LIBRARY
#        c. Default: <no-OS root>/libraries/ftd2xx/release/
#
#   2. Add the MPSSE wrapper sources from drivers/platform/ftd2xx/mpsse/ to
#      the no-os OBJECT library, Kconfig-gated per peripheral.
#
#   3. Find the pre-built LibMPSSE and libftd2xx binaries and link them to
#      no-os PUBLIC so every project executable that links no-os inherits them.
#
# Install the libraries first:
#   python3 tools/scripts/config_ftd2xx.py \
#       --local-mpsse <LibMPSSE archive> \
#       --local-d2xx  <libftd2xx archive>
#
# Supported host platforms: Linux, macOS, Windows (MinGW / MSVC).

# ---------------------------------------------------------------------------
# Gather environment variables dependencies paths or local setup
# ---------------------------------------------------------------------------
if(NOT DEFINED "ENV{FTD2XX_D2XX_HEADERS_32}" OR NOT DEFINED "ENV{FTD2XX_D2XX_HEADERS_64}")    # Local setup via tools/scripts/config_ftd2xx.py configuration
    if(NOT DEFINED FTD2XX_LIBRARY)
        if(DEFINED ENV{FTD2XX_LIBRARY})
            set(FTD2XX_LIBRARY "$ENV{FTD2XX_LIBRARY}" CACHE PATH
                "Path to the LibMPSSE release/ directory")
        else()
            set(_default_ftd2xx "${NO_OS_DIR}/libraries/ftd2xx/release")
            if(EXISTS "${_default_ftd2xx}")
                set(FTD2XX_LIBRARY "${_default_ftd2xx}" CACHE PATH
                    "Path to the LibMPSSE release/ directory")
            else()
                message(FATAL_ERROR
                    "FTD2XX_LIBRARY is not set and the default location does not exist:\n"
                    "  ${_default_ftd2xx}\n"
                    "Run tools/scripts/config_ftd2xx.py to install LibMPSSE and libftd2xx,\n"
                    "or set -DFTD2XX_LIBRARY=<path-to-release-dir> on the cmake command line.")
            endif()
        endif()
    endif()

    if(NOT EXISTS "${FTD2XX_LIBRARY}")
        message(FATAL_ERROR
            "FTD2XX_LIBRARY directory does not exist: ${FTD2XX_LIBRARY}\n"
            "Run tools/scripts/config_ftd2xx.py to install the libraries.")
    endif()

    message(STATUS "FTD2XX_LIBRARY: ${FTD2XX_LIBRARY}")

    # ---------------------------------------------------------------------------
    # Add MPSSE wrapper sources to no-os
    # ---------------------------------------------------------------------------

    set(_ftd2xx_mpsse "${NO_OS_DIR}/drivers/platform/ftd2xx/mpsse")

    target_compile_definitions(no-os PUBLIC -DFTD2XX_PLATFORM)

    # Core sources — always compiled when FTD2XX is enabled.
    # Delay is intentionally omitted: each host platform (linux-userspace, win, mac)
    # already provides no_os_udelay / no_os_mdelay via its own delay driver.
    target_sources(no-os PRIVATE
        ${_ftd2xx_mpsse}/ftd2xx_platform.c
    )

    # Peripheral drivers — compiled only when the matching Kconfig symbol is set
    no_os_sources_ifdef(CONFIG_SPI_FTD2XX  ${_ftd2xx_mpsse}/ftd2xx_spi.c)
    no_os_sources_ifdef(CONFIG_I2C_FTD2XX  ${_ftd2xx_mpsse}/ftd2xx_i2c.c)
    no_os_sources_ifdef(CONFIG_GPIO_FTD2XX ${_ftd2xx_mpsse}/ftd2xx_gpio.c)
    no_os_sources_ifdef(CONFIG_UART_FTD2XX ${_ftd2xx_mpsse}/ftd2xx_uart.c)

    # Include paths needed by the wrapper sources and by project code:
    #   mpsse/              — ftd2xx_spi.h, ftd2xx_i2c.h, ftd2xx_gpio.h, ftd2xx_uart.h
    #   include/            — libmpsse_spi.h, libmpsse_i2c.h (from LibMPSSE package)
    #   libftd2xx/          — ftd2xx.h, WinTypes.h (from LibMPSSE package)
    target_include_directories(no-os PUBLIC ${_ftd2xx_mpsse})
    target_include_directories(no-os PUBLIC "${FTD2XX_LIBRARY}/include")
    target_include_directories(no-os PUBLIC "${FTD2XX_LIBRARY}/libftd2xx")

    # ---------------------------------------------------------------------------
    # Find and link the pre-built LibMPSSE and libftd2xx
    # ---------------------------------------------------------------------------

    if(WIN32)
        # Windows layout produced by config_ftd2xx.py:
        #   release/build/x64/DLL/  — libmpsse.lib + libmpsse.dll + ftd2xx.lib + FTD2XX64.dll
        #   release/build/x64/LIB/  — static FTD2XX.lib
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_mpsse_hints
                "${FTD2XX_LIBRARY}/build/x64/DLL"
                "${FTD2XX_LIBRARY}/build/x64/LIB")
        else()
            set(_mpsse_hints
                "${FTD2XX_LIBRARY}/build/Win32/DLL"
                "${FTD2XX_LIBRARY}/build/Win32/LIB")
        endif()
        set(_mpsse_names libmpsse)
    else()
        # Linux / macOS layout:
        #   release/build/  — libmpsse.so, libmpsse.a  (Linux)
        #                    — libmpsse.dylib, libmpsse.a  (macOS)
        set(_mpsse_hints "${FTD2XX_LIBRARY}/build")
        set(_mpsse_names mpsse libmpsse)
    endif()

    find_library(FTD2XX_MPSSE_LIB
        NAMES ${_mpsse_names}
        HINTS ${_mpsse_hints}
        NO_DEFAULT_PATH
        DOC "LibMPSSE shared/static library"
    )

    if(NOT FTD2XX_MPSSE_LIB)
        message(FATAL_ERROR
            "LibMPSSE library not found in:\n  ${_mpsse_hints}\n"
            "Run tools/scripts/config_ftd2xx.py to install the library.")
    endif()
    message(STATUS "Found LibMPSSE: ${FTD2XX_MPSSE_LIB}")

    find_library(FTD2XX_D2XX_LIB
        NAMES ftd2xx libftd2xx
        HINTS ${_mpsse_hints}
        NO_DEFAULT_PATH
        DOC "FTDI D2XX library"
    )

    if(NOT FTD2XX_D2XX_LIB)
        message(FATAL_ERROR
            "libftd2xx not found in:\n  ${_mpsse_hints}\n"
            "Run tools/scripts/config_ftd2xx.py --local-d2xx <archive> to install it.")
    endif()
    message(STATUS "Found libftd2xx: ${FTD2XX_D2XX_LIB}")

    # Link PUBLIC so that project executables linking no-os inherit both libraries.
    target_link_libraries(no-os PUBLIC "${FTD2XX_MPSSE_LIB}" "${FTD2XX_D2XX_LIB}")

    # ---------------------------------------------------------------------------
    # (Windows / macOS): register runtime libraries for post-build copy
    # ---------------------------------------------------------------------------
    # The shared libraries must sit next to the executable at runtime.
    # project_utils.cmake post_build_config() reads FTD2XX_RUNTIME_DLLS and adds
    # a cmake -E copy_if_different step for every file listed here.
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_ftd2xx_dll_dir "${FTD2XX_LIBRARY}/build/x64/DLL")
        else()
            set(_ftd2xx_dll_dir "${FTD2XX_LIBRARY}/build/Win32/DLL")
        endif()
        set(FTD2XX_RUNTIME_DLLS
            "${_ftd2xx_dll_dir}/libmpsse.dll"
            "${_ftd2xx_dll_dir}/FTD2XX.dll"
            CACHE INTERNAL "Runtime libraries to copy next to the project executable")
    elseif(APPLE)
        set(_ftd2xx_dylib_dir "${FTD2XX_LIBRARY}/build")
        set(FTD2XX_RUNTIME_DLLS
            "${_ftd2xx_dylib_dir}/libmpsse.dylib"
            "${_ftd2xx_dylib_dir}/libftd2xx.dylib"
            CACHE INTERNAL "Runtime libraries to copy next to the project executable")
    endif()
else()      # Static libraries installed setup
    if(CMAKE_C_FLAGS STREQUAL "-m32")
        set(FTD2XX_D2XX_HEADERS "$ENV{FTD2XX_D2XX_HEADERS_32}")
        set(FTD2XX_MPSSE_HEADERS "$ENV{FTD2XX_MPSSE_HEADERS_32}")
        set(FTD2XX_D2XX_LIB "$ENV{FTD2XX_D2XX_LIB_32}")
        set(FTD2XX_MPSSE_LIB "$ENV{FTD2XX_MPSSE_LIB_32}")
    else()
        set(FTD2XX_D2XX_HEADERS "$ENV{FTD2XX_D2XX_HEADERS_64}")
        set(FTD2XX_MPSSE_HEADERS "$ENV{FTD2XX_MPSSE_HEADERS_64}")
        set(FTD2XX_D2XX_LIB "$ENV{FTD2XX_D2XX_LIB_64}")
        set(FTD2XX_MPSSE_LIB "$ENV{FTD2XX_MPSSE_LIB_64}")
    endif()
    # ---------------------------------------------------------------------------
    # Add MPSSE wrapper sources to no-os
    # ---------------------------------------------------------------------------

    set(_ftd2xx_mpsse "${NO_OS_DIR}/drivers/platform/ftd2xx/mpsse")

    target_compile_definitions(no-os PUBLIC -DFTD2XX_PLATFORM)

    # Core sources — always compiled when FTD2XX is enabled.
    # Delay is intentionally omitted: each host platform (linux-userspace, win, mac)
    # already provides no_os_udelay / no_os_mdelay via its own delay driver.
    target_sources(no-os PRIVATE
        ${_ftd2xx_mpsse}/ftd2xx_platform.c
    )

    # Peripheral drivers — compiled only when the matching Kconfig symbol is set
    no_os_sources_ifdef(CONFIG_SPI_FTD2XX  ${_ftd2xx_mpsse}/ftd2xx_spi.c)
    no_os_sources_ifdef(CONFIG_I2C_FTD2XX  ${_ftd2xx_mpsse}/ftd2xx_i2c.c)
    no_os_sources_ifdef(CONFIG_GPIO_FTD2XX ${_ftd2xx_mpsse}/ftd2xx_gpio.c)
    no_os_sources_ifdef(CONFIG_UART_FTD2XX ${_ftd2xx_mpsse}/ftd2xx_uart.c)

    # Include paths needed by the wrapper sources and by project code:
    #   mpsse/              — ftd2xx_spi.h, ftd2xx_i2c.h, ftd2xx_gpio.h, ftd2xx_uart.h
    #   include/            — libmpsse_spi.h, libmpsse_i2c.h (from LibMPSSE package)
    #   libftd2xx/          — ftd2xx.h, WinTypes.h (from LibMPSSE package)
    target_include_directories(no-os PUBLIC ${_ftd2xx_mpsse})

    # ---------------------------------------------------------------------------
    # Find and link the pre-built LibMPSSE and libftd2xx
    # ---------------------------------------------------------------------------

    if(WIN32)
        # Windows layout produced by config_ftd2xx.py:
        #   release/build/x64/DLL/  — libmpsse.lib + libmpsse.dll + ftd2xx.lib + FTD2XX64.dll
        #   release/build/x64/LIB/  — static FTD2XX.lib
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_mpsse_hints
                "${FTD2XX_LIBRARY}/build/x64/DLL"
                "${FTD2XX_LIBRARY}/build/x64/LIB")
        else()
            set(_mpsse_hints
                "${FTD2XX_LIBRARY}/build/Win32/DLL"
                "${FTD2XX_LIBRARY}/build/Win32/LIB")
        endif()
        set(_mpsse_names libmpsse)
    endif()

    # Link PUBLIC so that project executables linking no-os inherit both libraries.
    target_include_directories(no-os PUBLIC "${FTD2XX_D2XX_HEADERS}")
    target_include_directories(no-os PUBLIC "${FTD2XX_MPSSE_HEADERS}")

    find_library(
            d2xx
            NAMES ftd2xx libftd2xx
            HINTS ${FTD2XX_D2XX_LIB}
    )
    find_library(
            mpsse
            NAMES mpsse libmpsse
            HINTS ${FTD2XX_MPSSE_LIB}
    )

    if(NOT d2xx)
            message(FATAL_ERROR "libftd2xx not found!")
    endif()
    if(NOT mpsse)
            message(FATAL_ERROR "LibMPSSE not found!")
    endif()

    message(STATUS "Found libftd2xx: ${d2xx}")
    message(STATUS "Found LibMPSSE: ${mpsse}")

    target_link_libraries(no-os PUBLIC ${d2xx} ${mpsse})

    # ---------------------------------------------------------------------------
    # (Windows / macOS): register runtime libraries for post-build copy
    # ---------------------------------------------------------------------------
    # The shared libraries must sit next to the executable at runtime.
    # project_utils.cmake post_build_config() reads FTD2XX_RUNTIME_DLLS and adds
    # a cmake -E copy_if_different step for every file listed here.
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_ftd2xx_dll_dir "${FTD2XX_LIBRARY}/build/x64/DLL")
        else()
            set(_ftd2xx_dll_dir "${FTD2XX_LIBRARY}/build/Win32/DLL")
        endif()
        set(FTD2XX_RUNTIME_DLLS
            "${_ftd2xx_dll_dir}/libmpsse.dll"
            "${_ftd2xx_dll_dir}/FTD2XX.dll"
            CACHE INTERNAL "Runtime libraries to copy next to the project executable")
    elseif(APPLE)
        set(_ftd2xx_dylib_dir "${FTD2XX_LIBRARY}/build")
        set(FTD2XX_RUNTIME_DLLS
            "${_ftd2xx_dylib_dir}/libmpsse.dylib"
            "${_ftd2xx_dylib_dir}/libftd2xx.dylib"
            CACHE INTERNAL "Runtime libraries to copy next to the project executable")
    endif()
endif()
