/***************************************************************************//**
 *   @file   iio_k_regmap.h
 *   @brief  IIO device exposing the chip register API over the JESD CMD path.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright (C) 2026 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef IIO_K_REGMAP_H_
#define IIO_K_REGMAP_H_

#include <stdint.h>
#include "k_data.h"

struct iio_device;

#define IIO_K_REGMAP_MAX_STREAMWORDS	32

/**
 * @struct iio_k_regmap_desc
 * @brief  State for the k-regmap IIO device (chip register access).
 */
struct iio_k_regmap_desc {
	struct k_data	*kd;
	/* control-channel scratch for the streaming-read attribute */
	uint16_t	cmd_addr;
	uint8_t		cmd_numwords;
	uint32_t	last_response;
	uint32_t	stream_words[IIO_K_REGMAP_MAX_STREAMWORDS];
};

struct iio_k_regmap_init_param {
	struct k_data	*kd;
};

int iio_k_regmap_init(struct iio_k_regmap_desc **desc,
		      const struct iio_k_regmap_init_param *init);
void iio_k_regmap_remove(struct iio_k_regmap_desc *desc);
void iio_k_regmap_get_dev_descriptor(struct iio_k_regmap_desc *desc,
				     struct iio_device **dev_descriptor);

#endif /* IIO_K_REGMAP_H_ */
