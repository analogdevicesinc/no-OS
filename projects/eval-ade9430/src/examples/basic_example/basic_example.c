/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for the EVAL-ADE9430 project.
 *   @author RaduE (radu.etz@analog.com)
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

#include "common_data.h"
#include "ade9430.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/***************************************************************************//**
 * @brief Read the ADE9430 temperature using the conversion ready status.
 *
 * Start a conversion, poll the TEMP_RDY bit in
 * STATUS0 until the result is ready (with a timeout), then compute the
 * temperature in degrees Celsius from the raw result and trim coefficients.
 *
 * @param dev - The ADE9430 device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int basic_read_temp(struct ade9430_dev *dev)
{
	int ret;
	/* raw temperature value */
	uint32_t temp_raw;
	/* register value */
	uint32_t temp;
	/* temperature gain */
	uint32_t gain;
	/* temperature offset */
	uint32_t offset;
	/* status0 register value */
	uint32_t status0;
	/* status of temperature ready */
	uint8_t status = 0;
	/* timeout for temperature read */
	uint16_t timeout = 0;

	if (!dev)
		return -ENODEV;

	/* start a new temperature conversion */
	ret = ade9430_update_bits(dev, ADE9430_REG_TEMP_CFG, ADE9430_TEMP_START,
				  no_os_field_prep(ADE9430_TEMP_START, 1));
	if (ret)
		return ret;

	/* wait for the conversion result */
	while (!status) {
		ret = ade9430_read(dev, ADE9430_REG_STATUS0, &status0);
		if (ret)
			return ret;

		status = no_os_field_get(ADE9430_STATUS0_TEMP_RDY, status0);

		no_os_mdelay(2);
		timeout++;
		if (timeout == 2000)
			return -ENODATA;
	}

	/* if conversion succeeded clear the staus bit */
	ret = ade9430_update_bits(dev, ADE9430_REG_STATUS0, ADE9430_STATUS0_TEMP_RDY,
				  no_os_field_prep(ADE9430_STATUS0_TEMP_RDY, 1));
	if (ret)
		return ret;

	/* if conversion succeeded compute the temperature */
	ret = ade9430_read(dev, ADE9430_REG_TEMP_RSLT, &temp_raw);
	if (ret)
		return ret;

	temp_raw = no_os_field_get(ADE9430_TEMP_RESULT, temp_raw);

	ret = ade9430_read(dev, ADE9430_REG_TEMP_TRIM, &temp);
	if (ret)
		return ret;

	gain = no_os_field_get(ADE9430_TEMP_GAIN, temp);
	offset = no_os_field_get(ADE9430_TEMP_OFFSET, temp);

	/* Temperature (°C) = TEMP_RSLT × (−TEMP_GAIN/65536) + (TEMP_OFFSET/32) */
	dev->temp_deg = (offset / (1 << 5)) - (temp_raw * (1000
					       * (int64_t)gain / 65536) / 1000);

	return 0;
}

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main(void)
{
	int ret;

	struct ade9430_init_param ade9430_ip = {
		.spi_init = &ade9430_spi_ip,
		.temp_en = true,
	};
	struct ade9430_dev *ade9430_device;

	struct no_os_uart_desc *uart_desc;
#ifdef HW_RESET_ENABLED
	struct no_os_gpio_desc *gpio_reset_desc;
#endif

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("ADE9430 SPI basic example\n");

#ifdef HW_RESET_ENABLED
	/* Hardware reset the ADE9430 before any SPI access (EVAL-PQMON board)
		If EVAL-PQMON is used the user needs to at least set the reset pin high
		if the reset procedure is not used */
	ret = no_os_gpio_get(&gpio_reset_desc, &gpio_reset_ip);
	if (ret)
		goto remove_uart;

	ret = no_os_gpio_direction_output(gpio_reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_reset;

	no_os_mdelay(100);

	ret = no_os_gpio_set_value(gpio_reset_desc, NO_OS_GPIO_LOW);
	if (ret)
		goto remove_reset;

	no_os_mdelay(100);

	ret = no_os_gpio_set_value(gpio_reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_reset;

	no_os_mdelay(100);
#endif

	ret = ade9430_init(&ade9430_device, ade9430_ip);
	if (ret) {
		pr_info("\nADE9430 init ret=%d\n", ret);
		goto remove_reset;
	}

	ret = ade9430_set_egy_model(ade9430_device, ADE9430_EGY_NR_SAMPLES,
				    ADE9430_SAMPLES_NR);
	if (ret) {
		pr_err("Error setting the energy use model!\n");
		goto remove_ade9430;
	}

	while (1) {
		no_os_mdelay(READ_INTERVAL);

		/* Read and print the RMS measured values for PHASE A */
		ret = ade9430_read_data_ph(ade9430_device, ADE9430_PHASE_A);
		if (ret)
			goto remove_ade9430;

		pr_info("Vrms: %u\n", ade9430_device->vrms_val);
		pr_info("Irms: %u\n", ade9430_device->irms_val);

		/* Read and print the temperature value, waiting for the result
		 * conversion. If the user wants to use the temperature read
		 * implemented in the driver, just replace basic_read_temp with
		 * ade9430_read_temp. */
		ret = basic_read_temp(ade9430_device);
		if (ret)
			goto remove_ade9430;

		pr_info("Temperature: %d deg C\n", ade9430_device->temp_deg);
	}

remove_ade9430:
	ade9430_remove(ade9430_device);
remove_reset:
#ifdef HW_RESET_ENABLED
	no_os_gpio_remove(gpio_reset_desc);
#endif
remove_uart:
	no_os_uart_remove(uart_desc);

	if (ret)
		pr_err("Error!\n");

	return ret;
}
