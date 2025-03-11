##############################################################################
#
# Copyright (C) 2025 Analog Devices, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

cmake_minimum_required(VERSION 3.20.0)

set(NOOS_DIR ${ZEPHYR_CURRENT_MODULE_DIR})

zephyr_library()

zephyr_include_directories(
    ${NOOS_DIR}/include
    ${NOOS_DIR}/drivers/platform/zephyr
    ${NOOS_DIR}/drivers/temperature/max31875
    ${NOOS_DIR}/drivers/accel/adxl355
)

zephyr_library_sources(
    ${NOOS_DIR}/util/no_os_alloc.c
    ${NOOS_DIR}/util/no_os_mutex.c
    ${NOOS_DIR}/util/no_os_util.c
    ${NOOS_DIR}/drivers/api/no_os_i2c.c
    ${NOOS_DIR}/drivers/api/no_os_spi.c
    ${NOOS_DIR}/drivers/platform/zephyr/zephyr_i2c.c
    ${NOOS_DIR}/drivers/platform/zephyr/zephyr_spi.c
    ${NOOS_DIR}/drivers/platform/zephyr/zephyr_delay.c
    ${NOOS_DIR}/drivers/temperature/max31875/max31875.c
    ${NOOS_DIR}/drivers/accel/adxl355/adxl355.c
)
