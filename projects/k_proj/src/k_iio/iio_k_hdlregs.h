/***************************************************************************//**
 *   @file   iio_k_hdlregs.h
 *   @brief  IIO device for windowed local AXI/PL IP register access.
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
#ifndef IIO_K_HDLREGS_H_
#define IIO_K_HDLREGS_H_

#include <stdint.h>

struct iio_device;

/**
 * @enum k_hdl_window
 * @brief Selectable local AXI IP register windows.
 */
enum k_hdl_window {
	K_HDL_WIN_JESD_TX,
	K_HDL_WIN_JESD_RX,
	K_HDL_WIN_ADXCVR_TX,
	K_HDL_WIN_ADXCVR_RX,
	K_HDL_WIN_TPL_TX,
	K_HDL_WIN_TPL_RX,
	K_HDL_WIN_OFFLOAD_TX,
	K_HDL_WIN_OFFLOAD_RX,
	K_HDL_WIN_COUNT,
};

/**
 * @struct iio_k_hdlregs_desc
 * @brief  State for the k-hdl-regs IIO device.
 */
struct iio_k_hdlregs_desc {
	uint32_t		base[K_HDL_WIN_COUNT];
	enum k_hdl_window	window;
};

struct iio_k_hdlregs_init_param {
	/* base address per window; index with enum k_hdl_window */
	uint32_t	base[K_HDL_WIN_COUNT];
};

int iio_k_hdlregs_init(struct iio_k_hdlregs_desc **desc,
		       const struct iio_k_hdlregs_init_param *init);
void iio_k_hdlregs_remove(struct iio_k_hdlregs_desc *desc);
void iio_k_hdlregs_get_dev_descriptor(struct iio_k_hdlregs_desc *desc,
				      struct iio_device **dev_descriptor);

#endif /* IIO_K_HDLREGS_H_ */
