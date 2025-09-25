/***************************************************************************//**
 *   @file   test_max17616_support.h
 *   @brief  Support header for MAX17616 Driver unit tests
 *   @author Carlos Jones (carlosjr.jones@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef __TEST_MAX17616_SUPPORT_H__
#define __TEST_MAX17616_SUPPORT_H__

#include "max17616.h"

/*******************************************************************************
 *    FUNCTION DECLARATIONS
 ******************************************************************************/

/**
 * @brief Setup test environment for MAX17616 driver tests
 */
void max17616_test_setup(void);

/**
 * @brief Teardown test environment for MAX17616 driver tests
 */
void max17616_test_teardown(void);

/**
 * @brief Setup mock device for testing
 * @param device - Device structure to initialize
 * @param i2c_desc - I2C descriptor to use
 * @param chip_id - Chip ID to set
 */
void max17616_test_device_setup(struct max17616_dev *device,
				struct no_os_i2c_desc *i2c_desc,
				enum max17616_chip_id chip_id);

/**
 * @brief Validate telemetry structure
 * @param telemetry - Telemetry structure to validate
 * @return true if valid, false otherwise
 */
bool max17616_test_validate_telemetry(const struct max17616_telemetry
				      *telemetry);

/**
 * @brief Create test chip info structure
 * @return Pointer to test chip info
 */
const struct max17616_chip_info *max17616_test_get_chip_info(void);

/**
 * @brief Get test manufacturer ID string
 * @return Pointer to test manufacturer ID
 */
const char *max17616_test_get_mfr_id(void);

/**
 * @brief Get test device ID string
 * @param chip_id - Chip ID to get device string for
 * @return Pointer to test device ID string
 */
const char *max17616_test_get_device_id(enum max17616_chip_id chip_id);

/*******************************************************************************
 *    INTERNAL FUNCTION DECLARATIONS FOR TESTING
 ******************************************************************************/

/**
 * @brief Read and verify manufacturer ID (internal function exposed for testing)
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max17616_verify_manufacturer_id(struct max17616_dev *dev);

/**
 * @brief Read device ID and identify chip variant (internal function exposed for testing)
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max17616_identify_chip_variant(struct max17616_dev *dev);

/**
 * @brief Verify PMBus revision (internal function exposed for testing)
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max17616_verify_pmbus_revision(struct max17616_dev *dev);

#endif /* __TEST_MAX17616_SUPPORT_H__ */
