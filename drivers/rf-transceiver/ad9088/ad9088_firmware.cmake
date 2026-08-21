# AD9088 firmware images.
#
# The device is brought up from binary images rather than C arrays: a use-case
# profile describing the datapath, and a set of signed core-firmware images the
# driver pushes to the on-chip cores at startup.  ad9088.c indexes them through
# fw_table[] using the _binary_* symbols declared in ad9088.h, which the build
# creates by linking each image in as an object (see cmake/FirmwareBlob.cmake).
#
# ad9088_add_firmware() registers all of them against one executable target:
#
#     include(${NO_OS_DIR}/drivers/rf-transceiver/ad9088/ad9088_firmware.cmake)
#     ad9088_add_firmware(my_project)
#
# PROFILE overrides the use-case profile; the eight signed images are fixed.
# A bare file name is resolved against firmware/ next to this module, and an
# absolute path is used as given.  The profile is always embedded under the
# name usecase.bin, so swapping profiles never changes the symbol the driver
# looks for:
#
#     ad9088_add_firmware(my_project PROFILE 204C_M4_L4_NP16_20p0_4x4.bin)
#
# This is a standalone module rather than part of
# drivers/rf-transceiver/CMakeLists.txt on purpose: the top-level CMakeLists
# adds projects/ before drivers/, so a function defined under drivers/ does not
# exist yet when a project's CMakeLists.txt runs.  A project include()s this
# file directly and the ordering stops mattering.

function(ad9088_add_firmware TARGET_NAME)
    cmake_parse_arguments(AD9088_FW "" "PROFILE" "" ${ARGN})

    if(AD9088_FW_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "ad9088_add_firmware(${TARGET_NAME}): unexpected argument(s) "
            "'${AD9088_FW_UNPARSED_ARGUMENTS}'.")
    endif()

    # Locate firmware/ relative to this file so the module works wherever the
    # driver tree is checked out.
    set(_fw_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/firmware")

    # Symbol root: this is the path objcopy is handed, so it decides the
    # _binary_* names and must match AD9088_FW_SYM() in ad9088.h.
    set(_sym_root "drivers/rf-transceiver/ad9088/firmware")

    # Empty counts as unset, so a project may pass an unset Kconfig string
    # straight through and still get the default.
    if(NOT AD9088_FW_PROFILE)
        set(AD9088_FW_PROFILE "204C_M4_L8_NP16_20p0_4x2.bin")
    endif()
    if(NOT IS_ABSOLUTE "${AD9088_FW_PROFILE}")
        set(AD9088_FW_PROFILE "${_fw_dir}/${AD9088_FW_PROFILE}")
    endif()

    no_os_add_firmware_blob(${TARGET_NAME}
        SOURCE      "${AD9088_FW_PROFILE}"
        SYMBOL_PATH "${_sym_root}/usecase.bin")

    # Signed core firmware: an engineering and a production set, each a boot
    # header plus the three images the driver loads after it.
    foreach(_set app_signed_encrypted_B app_signed_encrypted_prod_B)
        foreach(_img 0x01030000 0x02000000 0x20000000 0x21000000)
            no_os_add_firmware_blob(${TARGET_NAME}
                SOURCE      "${_fw_dir}/${_set}/flash_image_${_img}.bin"
                SYMBOL_PATH "${_sym_root}/${_set}/flash_image_${_img}.bin")
        endforeach()
    endforeach()
endfunction()
