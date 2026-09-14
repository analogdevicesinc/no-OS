# Embedding binary firmware blobs in the application ELF.
#
# Some devices are configured from a vendor-supplied binary image rather than a
# C array.  Rather than converting the image to a header, the blob is handed to
# `objcopy -I binary`, which wraps it in a relocatable object exporting three
# symbols the firmware can reference directly:
#
#     _binary_<mangled path>_start
#     _binary_<mangled path>_end
#     _binary_<mangled path>_size
#
# objcopy derives <mangled path> from the path *as written on its command line*,
# with '/' and '.' replaced by '_'.  That makes the symbol names a function of
# the caller's working directory, so this module stages the blob under a fixed
# root in the build tree and runs objcopy from there: SYMBOL_PATH is exactly the
# path objcopy sees, and therefore exactly what the symbol names encode.
#
# `_size` is an address-valued symbol, not an integer holding the length -- take
# the address, do not dereference it:
#
#     extern const uint8_t _binary_drivers_foo_bar_bin_start[];
#     extern const uint8_t _binary_drivers_foo_bar_bin_size[];
#
#     const uint8_t *blob = _binary_drivers_foo_bar_bin_start;
#     size_t len = (size_t)_binary_drivers_foo_bar_bin_size;
#
# Usage, from a project CMakeLists.txt:
#
#     set(FOO_FIRMWARE "${NO_OS_DIR}/drivers/foo/firmware/variant_a.bin"
#         CACHE FILEPATH "Firmware image to embed")
#
#     no_os_add_firmware_blob(foo
#         SOURCE      "${FOO_FIRMWARE}"
#         SYMBOL_PATH drivers/foo/bar.bin)
#
# which yields _binary_drivers_foo_bar_bin_{start,end,size}.  SOURCE and
# SYMBOL_PATH are separate arguments precisely so the image can be swapped
# without disturbing the symbol names: point SOURCE at a different .bin and the
# build restages and relinks, while the driver keeps referring to the same three
# symbols.
#
# Unlike the Make build this replaces, which globbed every *.bin under SRC_DIRS,
# blobs are registered explicitly -- consistent with how the rest of the CMake
# port handled Make's recursive globbing (see the FLAG comments in
# drivers/rf-transceiver/CMakeLists.txt).
#
# Adding a platform: define NO_OS_OBJCOPY_BIN_FORMAT and NO_OS_OBJCOPY_BIN_ARCH
# in its drivers/platform/<name>/toolchain.cmake, next to CMAKE_OBJCOPY.  See
# drivers/platform/xilinx/toolchain.cmake for the pattern.

function(no_os_add_firmware_blob TARGET_NAME)
    cmake_parse_arguments(FW "" "SOURCE;SYMBOL_PATH" "" ${ARGN})

    if(NOT FW_SOURCE OR NOT FW_SYMBOL_PATH)
        message(FATAL_ERROR
            "no_os_add_firmware_blob(${TARGET_NAME}): both SOURCE and "
            "SYMBOL_PATH are required.")
    endif()
    if(FW_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "no_os_add_firmware_blob(${TARGET_NAME}): unexpected argument(s) "
            "'${FW_UNPARSED_ARGUMENTS}'.")
    endif()
    if(NOT EXISTS "${FW_SOURCE}")
        message(FATAL_ERROR
            "no_os_add_firmware_blob(${TARGET_NAME}): SOURCE does not exist: "
            "${FW_SOURCE}")
    endif()

    # SYMBOL_PATH becomes both a path under the staging root and the symbol
    # names.  An absolute path or a '..' component would escape that root and
    # mangle into something other than what the caller wrote, so reject both
    # here rather than let it surface as an undefined reference at link time.
    if(IS_ABSOLUTE "${FW_SYMBOL_PATH}" OR FW_SYMBOL_PATH MATCHES "(^|/)\\.\\.(/|$)")
        message(FATAL_ERROR
            "no_os_add_firmware_blob(${TARGET_NAME}): SYMBOL_PATH must be a "
            "relative path without '..' components, got '${FW_SYMBOL_PATH}'.")
    endif()

    if(NOT CMAKE_OBJCOPY)
        message(FATAL_ERROR
            "no_os_add_firmware_blob(${TARGET_NAME}): CMAKE_OBJCOPY is not set "
            "for PLATFORM=${PLATFORM}.")
    endif()
    if(NOT NO_OS_OBJCOPY_BIN_FORMAT OR NOT NO_OS_OBJCOPY_BIN_ARCH)
        message(FATAL_ERROR
            "no_os_add_firmware_blob(${TARGET_NAME}): embedding binary blobs is "
            "not configured for PLATFORM=${PLATFORM}.  Set "
            "NO_OS_OBJCOPY_BIN_FORMAT (the objcopy -O BFD target, e.g. "
            "elf32-microblazeel) and NO_OS_OBJCOPY_BIN_ARCH (the -B "
            "architecture, e.g. microblaze) in "
            "drivers/platform/${PLATFORM}/toolchain.cmake.")
    endif()

    # Staging root shared by every blob in this build.  objcopy runs from here,
    # so the relative path it is handed is SYMBOL_PATH verbatim.
    set(_stage_root "${CMAKE_BINARY_DIR}/fw_blobs")
    set(_staged "${_stage_root}/${FW_SYMBOL_PATH}")

    # Two blobs sharing a SYMBOL_PATH would collide on these OUTPUTs and, more
    # to the point, on their symbol names.
    get_property(_seen GLOBAL PROPERTY NO_OS_FIRMWARE_BLOBS)
    if("${FW_SYMBOL_PATH}" IN_LIST _seen)
        message(FATAL_ERROR
            "no_os_add_firmware_blob(${TARGET_NAME}): SYMBOL_PATH "
            "'${FW_SYMBOL_PATH}' is already registered; two blobs cannot share "
            "one set of _binary_* symbols.")
    endif()
    set_property(GLOBAL APPEND PROPERTY NO_OS_FIRMWARE_BLOBS "${FW_SYMBOL_PATH}")

    # Object file name mirrors the symbol path so distinct blobs never collide.
    string(REPLACE "/" "_" _obj_name "${FW_SYMBOL_PATH}")
    set(_obj "${CMAKE_CURRENT_BINARY_DIR}/${_obj_name}.o")

    add_custom_command(
        OUTPUT "${_staged}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FW_SOURCE}" "${_staged}"
        DEPENDS "${FW_SOURCE}"
        COMMENT "Staging firmware blob ${FW_SYMBOL_PATH}"
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${_obj}"
        COMMAND ${CMAKE_OBJCOPY}
                -I binary
                -O ${NO_OS_OBJCOPY_BIN_FORMAT}
                -B ${NO_OS_OBJCOPY_BIN_ARCH}
                "${FW_SYMBOL_PATH}"
                "${_obj}"
        WORKING_DIRECTORY "${_stage_root}"
        DEPENDS "${_staged}"
        COMMENT "Embedding firmware blob ${FW_SYMBOL_PATH}"
        VERBATIM
    )

    # EXTERNAL_OBJECT puts the pre-built object straight on the link line
    # instead of trying to compile it, while keeping the dependency edges above.
    set_source_files_properties("${_obj}" PROPERTIES
        EXTERNAL_OBJECT TRUE
        GENERATED TRUE
    )
    target_sources(${TARGET_NAME} PRIVATE "${_obj}")
endfunction()
