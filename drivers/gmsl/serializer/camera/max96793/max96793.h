/*******************************************************************************
 *   @file   max96793.h
 *   @brief  Header file of MAX96793 Driver.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
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
#ifndef MAX96793_H
#define MAX96793_H

#include <stdbool.h>
#include <stdint.h>
#include "gmsl_common.h"

#define MAX96793_LMN_COUNT      (2u)     /* Number of LMN */
#define MAX96793_MIPI_PHY_COUNT (2u)     /* Number of PHY */

#define MAX96793_DEV_STATE_MEM_SIZE     (sizeof(struct max96793_state))

/**
 * @enum max96793_phy
 * @brief Enumeration for MAX96793 PHY.
 */
enum max96793_phy {
	MAX96793_PHY_1 = 0u,    /*!< Phy 1 */
	MAX96793_PHY_2 = 1u,    /*!< Phy 2 */
};

/**
 * @struct max96793_dev_capabilities
 * @brief Structure representing the capabilities of the MAX96793 device.
 */
struct max96793_dev_capabilities {
	uint8_t max_links_count;     /*!< Maximum number of Links in the device */
	uint8_t max_lmn_count;       /*!< Maximum number of Line Fault Monitors in the device */
	uint8_t max_phy_ctrlr;       /*!< Maximum number of Phy controllers in the device */
};

/**
 * @struct max96793_state
 * @brief Structure representing MAX96793 device state.
 */
struct max96793_state {
	/* To_do: state variable or members to be added */
	uint8_t dummy;     /*!< MAX96793 state */
};

int32_t max96793_init(struct gmsl_dev **device, struct no_os_i2c_desc *i2c_desc,
		      void *init_param);
int32_t max96793_remove(struct gmsl_dev *dev);

#endif
