# ad9361
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_AD9361
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361_conv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361_util.c)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_IIO_AD9361 ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/iio_ad9361.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9361)

# adf7023
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_ADF7023 ${CMAKE_CURRENT_SOURCE_DIR}/adf7023/adf7023.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf7023)

# hmc630x
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_HMC630X ${CMAKE_CURRENT_SOURCE_DIR}/hmc630x/hmc630x.c)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_IIO_HMC630X ${CMAKE_CURRENT_SOURCE_DIR}/hmc630x/iio_hmc630x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hmc630x)

# koror (adrv904x) — top-level wrapper + full vendor API tree
# FLAG (shared driver change): Added recurse helpers to compile the full vendor
# API tree under koror/ (common/adi_{error,hal,logging}/, devices/adrv904x/
# private/src/ and public/src/).  The legacy Make used SRC_DIRS+=koror which
# compiled all .c files recursively; CMake needs explicit registration.
no_os_sources_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV904X
    ${CMAKE_CURRENT_SOURCE_DIR}/koror)
no_os_include_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV904X
    ${CMAKE_CURRENT_SOURCE_DIR}/koror)

# palma (adrv903x) — top-level wrapper + full vendor API tree
# FLAG (shared driver change): Added recurse helpers to compile the full vendor
# API tree under palma/ (common/adi_{error,hal,logging}/, devices/adrv903x/
# private/src/ and public/src/).  The legacy Make used SRC_DIRS+=palma which
# compiled all .c files recursively; CMake needs explicit registration.
no_os_sources_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV903X
    ${CMAKE_CURRENT_SOURCE_DIR}/palma)
no_os_include_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV903X
    ${CMAKE_CURRENT_SOURCE_DIR}/palma)

# madura (adrv9025) — top-level wrapper + full vendor API tree
# FLAG (shared driver change): Added recurse helpers to compile the full vendor
# API tree under madura/ (common/adi_{error,hal,logging}/, devices/adrv9025/
# private/src/ and public/src/).  The legacy Make used SRC_DIRS+=madura which
# compiled all .c files recursively; CMake needs explicit registration.
no_os_sources_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9025
    ${CMAKE_CURRENT_SOURCE_DIR}/madura)
no_os_include_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9025
    ${CMAKE_CURRENT_SOURCE_DIR}/madura)

# navassa (adrv9002) — top-level wrapper + full vendor API tree
# FLAG (shared driver change): Switched from two explicit sources to recurse
# helpers to compile the full vendor API tree under navassa/ (common/, third_party/,
# devices/adrv9001/private/src/ and public/src/). The legacy Make used
# SRC_DIRS+=navassa which compiled all .c files recursively; CMake needs explicit
# registration via these helpers to do the same.
no_os_sources_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9002
    ${CMAKE_CURRENT_SOURCE_DIR}/navassa)
no_os_include_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9002
    ${CMAKE_CURRENT_SOURCE_DIR}/navassa)

# talise (adrv9009) — top-level wrapper + full vendor API tree
# FLAG (shared driver change): Switched from two explicit sources to recurse
# helpers to compile the full vendor API tree under talise/ (top-level
# adrv9009.c/adrv9009_conv.c wrappers + api/*.c with all talise vendor
# functions). The legacy Make arm_src.mk listed all api/*.c explicitly and
# mb_src.mk used SRC_DIRS+=talise recursively; CMake needs recurse registration
# to do the same. Also adds recurse include so talise/api/ headers are found.
no_os_sources_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9009
    ${CMAKE_CURRENT_SOURCE_DIR}/talise)
no_os_include_dir_recurse_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9009
    ${CMAKE_CURRENT_SOURCE_DIR}/talise)
