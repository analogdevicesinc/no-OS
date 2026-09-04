/***************************************************************************//**
 *   @file   iio_maxm86161.h
 *   @brief  Header file for MAXM86161 IIO driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

int maxm86161_iio_init(struct maxm86161_iio_desc **iio_desc,
		       struct maxm86161_iio_init_param *init_param);

int maxm86161_iio_remove(struct maxm86161_iio_desc *iio_desc);

int maxm86161_iio_get_dev_descriptor(struct maxm86161_iio_desc *iio_desc,
				     struct iio_device **iio_dev);

void maxm86161_iio_fifo_irq_handler(void *ctx);

#endif /* __IIO_MAXM86161_H__ */
