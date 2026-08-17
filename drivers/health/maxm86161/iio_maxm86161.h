/***************************************************************************//**
 *   @file   iio_maxm86161.h
 *   @brief  Header file for MAXM86161 IIO driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
*******************************************************************************/

#ifndef __IIO_MAXM86161_H__
#define __IIO_MAXM86161_H__

#include "iio.h"
#include "maxm86161.h"
#include "no_os_circular_buffer.h"

#define MAXM86161_ATTR(_name, _priv) { \
	.name = _name, \
	.show = maxm86161_iio_get_attr, \
	.store = maxm86161_iio_set_attr, \
	.priv = _priv, \
}

#define MAXM86161_ATTR_RO(_name, _priv) { \
	.name = _name, \
	.show = maxm86161_iio_get_attr, \
	.store = NULL, \
	.priv = _priv, \
}

struct maxm86161_iio_desc {
	/** Scratch buffer for one FIFO drain in the INTB handler. */
	uint8_t fifo_read_buf[MAXM86161_FIFO_DEPTH * MAXM86161_FIFO_DATA_SIZE];
	/** Circular buffer bridging the INTB handler and the submit callback. */
	struct no_os_circular_buffer *fifo_buf;
	/** Underlying MAXM86161 driver device. */
	struct maxm86161_dev *drv_dev;
	/** IIO device structure registered with the IIO framework. */
	struct iio_device *iio_dev;
	/** IRQ controller — used to gate the INTB line during buffer resets. */
	struct no_os_irq_ctrl_desc *irq_ctrl;
	/** IRQ id for the INTB line. */
	uint32_t irq_id;
	/** Number of decoded samples currently queued in @ref fifo_buf. */
	uint32_t samples_available;
	/** Samples dropped from buffer due to software overflow */
	uint32_t sample_ovf_count;
	/** Mutex guarding @ref samples_available across IRQ/submit contexts. */
	void *samples_lock;
	/** True while a libiio client is actively streaming. */
	bool data_capture;
};

struct maxm86161_iio_init_param {
	struct maxm86161_init_param *drv_init_param;
	struct no_os_irq_ctrl_desc *irq_ctrl;  /* INTB GPIO IRQ controller */
	uint32_t irq_id;                        /* INTB IRQ id */
	uint16_t fifo_buf_size;
	uint8_t fifo_watermark;
};

/**
 * @brief Initialize the MAXM86161 IIO device descriptor.
 *
 * Allocates the IIO descriptor, initializes the underlying MAXM86161 driver
 * device, sets up the FIFO circular buffer, configures the FIFO watermark and
 * registers the FIFO almost-full interrupt handler. The IRQ must be enabled in
 * the driver init parameters for IIO operation. On failure, all previously
 * acquired resources are released before returning.
 *
 * @param iio_desc   - Pointer that receives the address of the allocated IIO
 *                     descriptor on success.
 * @param init_param - Initialization parameters, including the driver init
 *                     parameters and the FIFO watermark level.
 * @return 0 on success, negative error code otherwise:
 *         -EINVAL if arguments are invalid or IRQ is not enabled,
 *         -ENOMEM if descriptor allocation fails, or a propagated error
 *         from the driver/FIFO/IRQ setup.
 */
int maxm86161_iio_init(struct maxm86161_iio_desc **iio_desc,
		       struct maxm86161_iio_init_param *init_param);

/**
 * @brief Free resources allocated by maxm86161_iio_init().
 *
 * Detaches the FIFO almost-full callback from the driver-level IRQ dispatcher,
 * removes the FIFO circular buffer, removes the underlying MAXM86161 driver
 * device and frees the IIO descriptor.
 *
 * @param iio_desc - IIO descriptor to free.
 * @return 0 on success, -EINVAL if @p iio_desc is NULL.
 */
int maxm86161_iio_remove(struct maxm86161_iio_desc *iio_desc);

/**
 * @brief Retrieve the IIO device structure from the IIO descriptor.
 *
 * Provides access to the underlying struct iio_device so it can be registered
 * with the IIO framework.
 *
 * @param iio_desc - IIO descriptor.
 * @param iio_dev  - Pointer that receives the address of the IIO device
 *                   structure.
 * @return 0 on success, -EINVAL if @p iio_desc or @p iio_dev is NULL.
 */
int maxm86161_iio_get_dev_descriptor(struct maxm86161_iio_desc *iio_desc,
				     struct iio_device **iio_dev);

/**
 * @brief FIFO almost-full interrupt handler for the MAXM86161 IIO device.
 *
 * Invoked by the driver-level IRQ dispatcher when the FIFO almost-full
 * (A_FULL) condition is asserted. It queries the current FIFO sample count,
 * clamps it to the physical FIFO depth, reads the pending samples from the
 * device, decodes each sample (tag + 24-bit data) and writes the raw data
 * words into the IIO circular buffer for later consumption by the submit
 * callback. Samples that fail to decode are skipped. The function returns
 * silently on any error or when there are no samples to read.
 *
 * @param ctx - Opaque context pointer to the MAXM86161 IIO descriptor
 *              (struct maxm86161_iio_desc *).
 */
void maxm86161_iio_fifo_irq_handler(void *ctx);

#endif /* __IIO_MAXM86161_H__ */
