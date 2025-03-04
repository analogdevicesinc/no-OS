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


add_subdirectory_ifdef(CONFIG_NO_OS NO_OS)

# No_os
# Hardcoded path, TODO: fixit
set(NOOS "${CMAKE_CURRENT_SOURCE_DIR}")
zephyr_library_include_directories(${NOOS}/include)

# No_os platform api
zephyr_library_sources(${NOOS}/drivers/api/no_os_i2c.c)

zephyr_library_sources(${NOOS}/drivers/platform/zephyr/zephyr_i2c.c)
zephyr_library_include_directories(${NOOS}/drivers/platform/zephyr)

zephyr_library_sources(${NOOS}/drivers/temperature/max31875/max31875.c)
zephyr_library_include_directories(${NOOS}/drivers/temperature/max31875)

zephyr_library_sources(${NOOS}/util/no_os_alloc.c)
zephyr_library_sources(${NOOS}/util/no_os_mutex.c)
