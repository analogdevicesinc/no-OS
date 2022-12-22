/***************************************************************************//**
 *   @file   mbed_spi.cpp
 *   @brief  Implementation of SPI Mbed platform driver interfaces
********************************************************************************
 * Copyright (c) 2019-2022 Analog Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <mbed.h>

// Platform drivers needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif //  _cplusplus

#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "mbed_spi.h"

#define		SPI_8_BIT_FRAME			8		// SPI 8-bit frame size

#warning SPI delays are not supported on the mbed platform

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the Mbed SPI communication peripheral.
 * @param desc[in,out] - The SPI descriptor.
 * @param param[in] - The structure that contains the SPI parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_spi_init(struct no_os_spi_desc **desc,
		      const struct no_os_spi_init_param *param)
{
	mbed::SPI *spi;					// Pointer to new spi instance
	DigitalOut *csb;  				// pointer to new CSB gpio instance
	struct mbed_spi_desc *mbed_spi_desc;	// Pointer to mbed spi descriptor
	use_gpio_ssel_t use_gpio_ssel;	// For CSB hardware (implicit) control
	struct no_os_spi_desc *spi_desc;

	if (!desc || !param)
		return -EINVAL;

	/* Allocate memory for spi_desc structure */
	spi_desc = (struct no_os_spi_desc *)calloc(1, sizeof(*spi_desc));
	if (!spi_desc)
		return -ENOMEM;

	spi_desc->chip_select = param->chip_select;
	spi_desc->mode = param->mode;
	spi_desc->max_speed_hz = param->max_speed_hz;

	/* Allocate memory for mbed_spi_desc structure */
	mbed_spi_desc = (struct mbed_spi_desc *)calloc(1, sizeof(*mbed_spi_desc));
	if (!mbed_spi_desc)
		goto err_mbed_spi_desc;

	/* Configure and instantiate SPI peripheral */
	if (((struct mbed_spi_init_param *)param->extra)->use_sw_csb) {
		/* CSB pin toggled explicitly (s/w controlled) during SPI transaction */
		spi = new SPI(
			(PinName)(((struct mbed_spi_init_param *)param->extra)->spi_mosi_pin),
			(PinName)(((struct mbed_spi_init_param *)param->extra)->spi_miso_pin),
			(PinName)(((struct mbed_spi_init_param *)param->extra)->spi_clk_pin));

		if (spi) {
			/* Configure and instantiate chip select pin */
			csb = new DigitalOut((PinName)(spi_desc->chip_select));
			if (csb) {
				mbed_spi_desc->use_sw_csb = true;
				mbed_spi_desc->csb_gpio = (DigitalOut *)csb;
				csb->write(NO_OS_GPIO_HIGH);
			} else
				goto err_csb;
		}
	} else {
		/* CSB pin toggled implicitly (through HAL layer) during SPI transaction */
		spi = new SPI(
			(PinName)(((struct mbed_spi_init_param *)param->extra)->spi_mosi_pin),
			(PinName)(((struct mbed_spi_init_param *)param->extra)->spi_miso_pin),
			(PinName)(((struct mbed_spi_init_param *)param->extra)->spi_clk_pin),
			(PinName)(param->chip_select),
			use_gpio_ssel);

		mbed_spi_desc->use_sw_csb = false;
		mbed_spi_desc->csb_gpio = NULL;
	}

	if (!spi)
		goto err_spi;

	mbed_spi_desc->spi_port = spi;
	spi_desc->extra = mbed_spi_desc;
	*desc = spi_desc;

	/**
		NOTE: Actual frequency of SPI clk will be somewhat device
		dependent, relating to clock-settings, prescalars etc. If absolute
		SPI frequency is required, consult your device documentation.
		**/
	spi->frequency(param->max_speed_hz);
	spi->format(SPI_8_BIT_FRAME, param->mode);   // data write/read format
	spi->set_default_write_value(0x00);          // code to write when reading back

	return 0;

err_spi:
	if (((struct mbed_spi_init_param *)param->extra)->use_sw_csb) {
		if (csb)
			free(csb);
	}
err_csb:
	free(spi);
	free(mbed_spi_desc);
err_mbed_spi_desc:
	free(spi_desc);

	return -ENOMEM;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc[in, out] - The SPI descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	if (((struct mbed_spi_desc *)desc->extra)->use_sw_csb) {
		/* Free the CSB gpio object */
		if ((DigitalOut *)(((struct mbed_spi_desc *)(desc->extra))->csb_gpio))
			delete((DigitalOut *)(((struct mbed_spi_desc *)(desc->extra))->csb_gpio));
	}

	/* Free the SPI port object */
	if ((SPI *)(((struct mbed_spi_desc *)(desc->extra))->spi_port))
		delete((SPI *)(((struct mbed_spi_desc *)(desc->extra))->spi_port));

	/* Free the SPI descriptor objects */
	free(desc->extra);
	free(desc);

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc[in] - The SPI descriptor.
 * @param data[in, out] - The buffer with the transmitted/received data.
 * @param bytes_number[in] - Number of bytes to write/read.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_spi_write_and_read(struct no_os_spi_desc *desc,
				uint8_t *data,
				uint16_t bytes_number)
{
	mbed::SPI *spi; 		// pointer to new spi instance
	mbed::DigitalOut *csb;	// pointer to new CSB instance

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	spi = (SPI *)(((struct mbed_spi_desc *)(desc->extra))->spi_port);

	if (((struct mbed_spi_desc *)desc->extra)->use_sw_csb) {
		csb = (DigitalOut *)(((struct mbed_spi_desc *)(desc->extra))->csb_gpio);
		csb->write(NO_OS_GPIO_LOW);
	}

	/* Perform synchronous SPI write and read */
	spi->write((const char *)data, bytes_number, (char *)data, bytes_number);

	if (((struct mbed_spi_desc *)desc->extra)->use_sw_csb)
		csb->write(NO_OS_GPIO_HIGH);

	return 0;
}

/**
 * @brief Transfer (write/read) the number of SPI messages.
 * @param desc[in] - The SPI descriptor.
 * @param msgs[in, out] - Pointer to SPI messages.
 * @param num_of_msgs[in] - Number of SPI messages.
 * @return 0 in case of success, negative error code otherwise.
 * @note Use of this function requires CSB pin to be software controlled.
 */
int32_t mbed_spi_transfer(struct no_os_spi_desc *desc, struct no_os_spi_msg *msgs,
			  uint32_t num_of_msgs)
{
	mbed::SPI *spi; 			// pointer to new spi instance
	mbed::DigitalOut *csb;		// pointer to new CSB instance
	uint8_t msg_cnt;			// SPI message counter

	if (!desc || !desc->extra || !msgs)
		return -EINVAL;

	if (!((struct mbed_spi_desc *)desc->extra)->use_sw_csb)
		return -EINVAL;

	spi = (SPI *)(((struct mbed_spi_desc *)(desc->extra))->spi_port);
	csb = (DigitalOut *)(((struct mbed_spi_desc *)(desc->extra))->csb_gpio);

	for (msg_cnt = 0; msg_cnt < num_of_msgs; msg_cnt++) {
		csb->write(NO_OS_GPIO_LOW);

		/* Perform synchronous SPI write and read */
		if (!msgs[msg_cnt].tx_buff) {
			spi->write(NULL, 0,
				   (char *)msgs[msg_cnt].rx_buff, msgs[msg_cnt].bytes_number);
		} else {
			spi->write((const char *)msgs[msg_cnt].tx_buff, msgs[msg_cnt].bytes_number,
				   (char *)msgs[msg_cnt].rx_buff, msgs[msg_cnt].bytes_number);
		}

		if (msgs[msg_cnt].cs_change)
			csb->write(NO_OS_GPIO_HIGH);
	}

	csb->write(NO_OS_GPIO_HIGH);
	return 0;
}

/**
* @brief Mbed platform specific SPI platform ops structure
*/
const struct no_os_spi_platform_ops mbed_spi_ops = {
	.init = &mbed_spi_init,
	.write_and_read = &mbed_spi_write_and_read,
	.transfer = &mbed_spi_transfer,
	.remove = &mbed_spi_remove,
};

#ifdef __cplusplus  // Closing extern c
}
#endif //  _cplusplus
