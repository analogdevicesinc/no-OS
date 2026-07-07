/***************************************************************************//**
 *   @file   iio_k_tx.h
 *   @brief  IIO OUTPUT device for large bitstream transmit (K project).
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
#ifndef IIO_K_TX_H_
#define IIO_K_TX_H_

#include <stdint.h>
#include "k_data.h"

struct iio_device;

/**
 * @struct iio_k_tx_desc
 * @brief  State for the k-tx IIO OUTPUT device.
 */
struct iio_k_tx_desc {
	struct k_data		*kd;
	enum k_stream_id	stream_id;
	enum k_tx_mode		tx_mode;
	enum k_payload_mode	payload_mode;
	uint32_t		timeout_ms;
	uint32_t		active_mask;
};

struct iio_k_tx_init_param {
	struct k_data		*kd;
	enum k_stream_id	stream_id;	/* default stream id */
	enum k_tx_mode		tx_mode;	/* default oneshot/cyclic */
	enum k_payload_mode	payload_mode;	/* default samples/raw */
	uint32_t		timeout_ms;
};

int iio_k_tx_init(struct iio_k_tx_desc **desc,
		  const struct iio_k_tx_init_param *init);
void iio_k_tx_remove(struct iio_k_tx_desc *desc);
void iio_k_tx_get_dev_descriptor(struct iio_k_tx_desc *desc,
				 struct iio_device **dev_descriptor);

#endif /* IIO_K_TX_H_ */
