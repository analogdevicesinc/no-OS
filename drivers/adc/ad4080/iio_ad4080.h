/***************************************************************************//**
 *   @file   iio_ad4080.h
 *   @brief  Implementation of iio_ad4080.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *           Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#ifndef __IIO_AD4080_H__
#define __IIO_AD4080_H__
#include <stddef.h>
#include <errno.h>
#include <ad4080.h>
#include <no_os_irq.h>
#include <iio.h>
#include <iio_types.h>
#include <iio_app.h>

#define IIO_AD4080_FIFO_SIGNATURE_LEN 	16
#define IIO_AD4080_FIFO_SIGNATURE 	"IIO_AD4080_FIFO"
#define AD4080_IIO_APP_DEVICE_NAME_LEN 	16
#define AD4080_ADC_DATA_BUFFER_LEN 	65536 /* 16K_watermark * 4_byte_storage per sample */
#define IIO_AD4080_COMPLETION_SIGNATURE 	0xdeadc0de

/**
 * @brief Interrupt Service Routine (ISR) for AD4080 FIFO full event.
 */
typedef void (*iio_ad4080_fifo_isr)(void *isr_data);

/**
 * @struct iio_ad4080_fifo_struct
 */
struct iio_ad4080_fifo_struct {
	char signature[IIO_AD4080_FIFO_SIGNATURE_LEN];
	struct ad4080_dev *ad4080;
	struct no_os_gpio_desc *ff_full;
	struct no_os_irq_ctrl_desc *irq_desc;
	size_t i_gp;
	iio_ad4080_fifo_isr isr;
	void *isr_data;

	size_t watermark;
	uint8_t *raw_fifo;
	size_t bufsize;
	uint32_t *formatted_fifo;
	size_t formatted_bufsize;
};

/**
 * @struct iio_ad4080_completion - forward declaration for the ad4080
 * completion mechanism.
 */
struct iio_ad4080_completion;

/**
 * @struct Initializes the AD4080 IIO descriptor.
 */
struct iio_ad4080_desc {
	struct ad4080_dev *ad4080;
	struct iio_ad4080_fifo_struct fifo;
	struct iio_ad4080_completion *ff_full_completion;
	struct no_os_gpio_desc *afe_ctrl;

	uint32_t app_device_count;
	char app_device_name[AD4080_IIO_APP_DEVICE_NAME_LEN];
	struct iio_app_device app_device;
	struct iio_data_buffer adc_buffer;
	uint32_t adc_data_buffer[0];
};

/**
 * @brief Structure holding the AD4080 IIO initialization parameter.
 */
struct iio_ad4080_init_param {
	struct ad4080_init_param *ad4080_init_param;
	struct no_os_gpio_init_param *ff_full_init_param;
	struct no_os_gpio_init_param *afe_ctrl_init_param;
	struct no_os_irq_platform_ops *gpio_irq_platform_ops;
	size_t i_gp;
	size_t watermark;
};

/**
 * @brief Initializes the IIO AD4080 descriptor.
 */
int iio_ad4080_init(struct iio_ad4080_desc **iio_ad4080,
		    struct iio_ad4080_init_param *iio_ad4080_init_param);

/**
 * @brief teardown the ad4080 IIO driver
 */
void iio_ad4080_fini(struct iio_ad4080_desc *iio_ad4080);

/**
 * @brief Checks if the given pointer is a valid AD4080 IIO descriptor.
 */
int ad4080_device(struct iio_ad4080_desc *iio_ad4080,
		  struct ad4080_dev **ad4080);

/**
 * @brief Initializes the IIO device structure for the AD4080 device.
 */
int ad4080_iio_device(struct iio_ad4080_desc *iio_ad4080,
		      struct iio_device *iio_device);
/**
 * @brief Checks if the given pointer is a valid AD4080 IIO FIFO descriptor. 
 */		      
int iio_ad4080_fifo_init(struct iio_ad4080_fifo_struct *fifo,
			 struct ad4080_dev *ad4080);
void iio_ad4080_fifo_fini(struct iio_ad4080_fifo_struct *fifo);

/**
 * @brief Registers an IRQ for the AD4080 FIFO full event.
 */
int iio_ad4080_fifo_register_irq(struct iio_ad4080_fifo_struct *fifo, 
		struct no_os_gpio_init_param *gpio_init_param,
		struct no_os_irq_platform_ops *gpio_irq_platform_ops,
		const size_t i_gp,
		iio_ad4080_fifo_isr isr,
		void *isr_data);
/**
 * @brief Unregisters an IRQ for the AD4080 FIFO full event.
 */
void iio_ad4080_fifo_unregister_irq(struct iio_ad4080_fifo_struct *fifo,
		const size_t i_gp);

/**
 * @brief Sets the FIFO watermark level.
 */
int iio_ad4080_fifo_set_watermark(struct iio_ad4080_fifo_struct *fifo,
		const size_t watermark);

/**
 * @brief Unsets the FIFO watermark level.
 */
void iio_ad4080_fifo_unset_watermark(struct iio_ad4080_fifo_struct *fifo);

/**
 * @brief Reads the FIFO data from the AD4080 device.
 */
int iio_ad4080_read_fifo(struct iio_ad4080_fifo_struct *fifo,
		uint8_t *buf, const size_t buflen);

#endif /* _IIO_AD4080_H__ */

