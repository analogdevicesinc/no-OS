/***************************************************************************//**
 *   @file   max17851.c
 *   @brief  Source file of the MAX17851 driver
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "max17851.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/**
 * @brief Read any UART related errors in case of failures communication wise.
 * @param desc - Instance of UART
 * @return Status byte that contains any UART errors that appeared.
 */
static uint32_t max17851_uart_get_errors(struct no_os_uart_desc *desc)
{
	struct max17851_desc *extra;
	uint8_t reg_val;
	int ret;

	extra = desc->extra;
	ret = max17851_reg_read(extra, MAX17851_STATUS_GEN_REG, &reg_val);
	if (ret)
		return ret;

	return reg_val;
}

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t max17851_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
				  uint32_t bytes_number)
{
	struct max17851_desc *extra;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	if (bytes_number > MAX17851_UART_MAX_BYTES_NB)
		return -EOPNOTSUPP;

	extra = desc->extra;
	return max17851_read_msg(extra, data, bytes_number);
}

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t max17851_uart_write(struct no_os_uart_desc *desc,
				   const uint8_t *data,
				   uint32_t bytes_number)
{
	struct max17851_desc *extra;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	if (bytes_number > MAX17851_UART_MAX_BYTES_NB)
		return -EOPNOTSUPP;

	extra = desc->extra;
	return max17851_write_msg(extra, data, bytes_number);
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max17851_uart_init(struct no_os_uart_desc **desc,
				  struct no_os_uart_init_param *param)
{
	struct max17851_desc *max17851_uart;
	struct max17851_init_param *eparam;
	struct no_os_uart_desc *descriptor;
	int ret;

	if (!param || !param->extra)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	eparam = param->extra;
	switch (param->baud_rate) {
	case 500000:
		eparam->baud_rate = MAX17851_UART_BAUD_500K;
		break;
	case 1000000:
		eparam->baud_rate = MAX17851_UART_BAUD_1M;
		break;
	case 2000000:
		eparam->baud_rate = MAX17851_UART_BAUD_2M;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}
	ret = max17851_init(&max17851_uart, eparam);
	if (ret)
		goto error;

	descriptor->extra = max17851_uart;
	descriptor->baud_rate = param->baud_rate;
	descriptor->device_id = 0;


	*desc = descriptor;

	return 0;

error:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by max_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max17851_uart_remove(struct no_os_uart_desc *desc)
{
	struct max17851_desc *extra;

	if (!desc)
		return -EINVAL;

	extra = desc->extra;

	max17851_remove(extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Send specific device command over SPI.
 * @param desc - MAX17851 device descriptor.
 * @param command - Specific command from the command list.
 * @param enable - Enable/Disable function in case of some commands.
 * 		   Appropiated value needed only for SWPORT or SLEEP_EN
 * 		   commands, ohterwise can be left either true or false.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_send_command(struct max17851_desc *desc,
			  enum max17851_command command, bool enable)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX17851_FRAME_SIZE - 1,
		.cs_change = 1,
	};

	switch (command) {
	case MAX17851_CLR_LSSM:
	case MAX17851_CLR_ALIVECOUNT_SEED:
	case MAX17851_VER_CONFIG:
	case MAX17851_LOAD_CONFIG:
	case MAX17851_WD_KEY:
		memset(desc->buff, 0, xfer.bytes_number);
		desc->buff[0] = command;

		return no_os_spi_transfer(desc->spi_desc, &xfer, 1);
	case MAX17851_SWPOR:
	case MAX17851_SLP_EN:
		xfer.bytes_number++;

		memset(desc->buff, 0, xfer.bytes_number);
		desc->buff[0] = command;
		desc->buff[1] = enable;

		return no_os_spi_transfer(desc->spi_desc, &xfer, 1);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read message over UART from the MAX17851.
 * @param desc - MAX17851 device descriptor.
 * @param data - Pointer to data containing buffer.
 * @param len - Length of the message in bytes.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_read_msg(struct max17851_desc *desc, uint8_t *data, uint8_t len)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = len + 2,
		.cs_change = 1,
	};
	int ret, i = 0, retries = 0;
	bool ready = false;
	bool stop = false;
	uint8_t reg_val;

	if (len > MAX17851_UART_MAX_BYTES_NB)
		return -EINVAL;

	while (!stop || !ready) {
		ret = max17851_reg_read(desc, MAX17851_STATUS_RX_REG, &reg_val);
		if (ret)
			return ret;

		stop = no_os_field_get(MAX17851_ALRT_MASK(MAX17851_RX_STOP_ALRT), reg_val);

		ret = max17851_reg_read(desc, MAX17851_STATUS_LSSM_BYTE_REG, &reg_val);
		if (ret)
			return ret;

		ready = no_os_field_get(NO_OS_BIT(7), reg_val);

		retries++;
		if (retries == 100)
			return -ETIMEDOUT;
	}

	memset(desc->buff, 0, len + 2);
	desc->buff[0] = MAX17851_RD_REG(MAX17851_RX_RD_NXT_MSG_REG);
	ret = no_os_spi_transfer(desc->spi_desc, &xfer, 1);
	if (ret)
		return ret;

	memcpy(data, &desc->buff[1], len);

	desc->lssm_byte = desc->buff[i + 1];

	return max17851_clear_buff(desc, false);
}

/**
 * @brief Write message over UART using MAX17851.
 * @param desc - MAX17851 device descriptor.
 * @param data - Pointer to data containing buffer.
 * @param len - Length of the message in bytes.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_write_msg(struct max17851_desc *desc, const uint8_t *data,
		       uint8_t len)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = len + 2,
		.cs_change = 1,
	};
	int ret, i;

	ret = max17851_config_gen2(desc, MAX17851_TX_QUEUE_SEL, true);
	if (ret)
		return ret;

	if (len > MAX17851_UART_MAX_BYTES_NB)
		return -EINVAL;

	memset(desc->buff, 0, len + 2);
	desc->buff[0] = MAX17851_LDQ_REG;
	desc->buff[1] = len;
	memcpy(&desc->buff[2], data, len);

	ret = no_os_spi_transfer(desc->spi_desc, &xfer, 1);
	if (ret)
		return ret;

	desc->buff[0] = MAX17851_NXT_LDQ_REG;
	xfer.bytes_number = 1;

	ret = no_os_spi_transfer(desc->spi_desc, &xfer, 1);
	if (ret)
		return ret;

	ret = max17851_config_gen2(desc, MAX17851_TX_QUEUE_SEL, false);
	if (ret)
		return ret;

	return max17851_clear_buff(desc, true);
}

/**
 * @brief Configure general settings for MAX17851.
 * @param desc - MAX17851 device descriptor.
 * @param sel - Specific setting to be changed.
 * @param enable - Disable/Enable specific setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_config_gen2(struct max17851_desc *desc,
			 enum max17851_config_gen2_sel sel, bool enable)
{
	return max17851_reg_update(desc, MAX17851_CONFIG_GEN2_REG,
				   MAX17851_CONFIG_GEN2_MASK(sel),
				   no_os_field_prep(MAX17851_CONFIG_GEN2_MASK(sel), enable));
}

/**
 * @brief Set timing for Alert Packets/Keep-Alive Packets.
 * @param desc - MAX17851 device descriptor.
 * @param time - Specific pre-defined time value to be set.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_sel_alrtpckt_time(struct max17851_desc *desc,
			       enum max17851_alrtpckt_time time)
{
	return max17851_reg_update(desc, MAX17851_CONFIG_GEN3_REG,
				   MAX17851_ALRTPCKT_TIME_MASK,
				   no_os_field_prep(MAX17851_ALRTPCKT_TIME_MASK,
						   time));
}

/**
 * @brief Select queue for transmitting/loading TX buffer for UART.
 * @param desc - MAX17851 device descriptor.
 * @param sel - Specific pre-defined value for either TX or LD queue selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_select_queue(struct max17851_desc *desc,
			  enum max17851_queue_sel sel)
{
	uint8_t mask;

	mask = (sel >= MAX17851_TXQ_0_SEL) ? MAX17851_TXQ_MASK : MAX17851_LOADQ_MASK;

	sel = sel % 4;
	return max17851_reg_update(desc, MAX17851_TX_QUEUE_SEL_REG, mask,
				   no_os_field_prep(mask, sel));
}

/**
 * @brief Clear RX/TX buffer inside the MAX17851 memory.
 * @param desc - MAX17851 device descriptor.
 * @param tx_buf - True - Clear TX buffer.
 * 		   False - Clear RX buffer.
 * @return 0 incase of success, negative error code otherwise.
 */
int max17851_clear_buff(struct max17851_desc *desc, bool tx_buf)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX17851_FRAME_SIZE - 1,
		.cs_change = 1,
	};

	memset(desc->buff, 0, xfer.bytes_number);
	desc->buff[0] = tx_buf ? MAX17851_CLR_TXBUF_REG : MAX17851_CLR_RXBUF_REG;

	return no_os_spi_transfer(desc->spi_desc, &xfer, 1);
}

/**
 * @brief Read specific alert state.
 * @param desc - MAX17851 device descriptor.
 * @param alert - Specific alert state to be read.
 * @param enable - Pointer to alert state value to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_get_alert(struct max17851_desc *desc, enum max17851_alert alert,
		       bool *enable)
{
	uint8_t reg, reg_val;
	int ret;

	reg = MAX17851_ALERT_RX_REG + (alert / 8) * 2;

	ret = max17851_reg_read(desc, reg, &reg_val);
	if (ret)
		return ret;

	*enable = no_os_field_get(MAX17851_ALRT_MASK(alert), reg_val);

	return 0;
}

/**
 * @brief Enable/Disable specific alert.
 * @param desc - MAX17851 device descriptor.
 * @param alert - Specific alert to be enabled/disabled.
 * @param enable - true - Enable specific alert.
 * 		   false - Disable specific alert.
 * @return 0 in case of success, negative erro code otherwise.
 */
int max17851_set_alert(struct max17851_desc *desc, enum max17851_alert alert,
		       bool enable)
{
	uint8_t reg;

	reg = MAX17851_ALERT_RX_REG + (alert / 8) * 2;

	return max17851_reg_update(desc, reg, MAX17851_ALRT_MASK(alert),
				   no_os_field_prep(MAX17851_ALRT_MASK(alert),
						   enable));
}

/**
 * @brief Select the number of unmasked consecutive Alert Packet status alerts,
 * 	  which must happen prior to taking action in either Safe Minotring or
 * 	  Sleep Mode.
 * @param desc - MAX17851 device descriptor.
 * @param dbnc - Specific pre-defined value for number of debounces.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_set_alert_debounce(struct max17851_desc *desc,
				enum max17851_status_debounce dbnc)
{
	return max17851_reg_update(desc, MAX17851_CONFIG_GEN5_REG,
				   MAX17851_ALRTPCKT_DBNC_MASK,
				   no_os_field_prep(MAX17851_ALRTPCKT_DBNC_MASK,
						   dbnc));
}

/**
 * @brief Select Safe Monitoring Mode Scan Delay.
 * @param desc - MAX17851 device descriptor.
 * @param delay - Specific pre-defined time delay.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_set_safemon_dly(struct max17851_desc *desc,
			     enum max17851_safemon_dly delay)
{
	return max17851_reg_write(desc, MAX17851_CONFIG_SAFEMON2_REG, delay);
}

/**
 * @brief Select which GPIO pins to be configured as SAFEMON output.
 * @param desc - MAX17851 device descriptor.
 * @param gpio - Selected GPIO.
 * @param enable - Enable/Disable the SAFEMON output function of the GPIO.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_set_safemon_gpio(struct max17851_desc *desc,
			      enum max17851_safemon_gpio gpio, bool enable)
{
	return max17851_reg_update(desc, MAX17851_CONFIG_SAFEMON3_REG,
				   MAX17851_SAFEMON_GPIO_MASK(gpio),
				   no_os_field_prep(MAX17851_SAFEMON_GPIO_MASK(gpio), enable));
}

/**
 * @brief Set communication retry count and timeout delay.
 * @param desc - MAX17851 device descriptor.
 * @param count - Specific pre-defined value for communication retry count.
 * @param delay - Specific pre-defined value for communication timeout delay.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_set_comm(struct max17851_desc *desc,
		      enum max17851_comm_try_count count,
		      enum max17851_comm_timeout_delay delay)
{
	uint8_t reg_val;
	int ret;

	reg_val = no_os_field_prep(MAX17851_COMM_RETRY_COUNT_MASK, count) |
		  no_os_field_prep(MAX17851_COMM_TIMEOUT_DELAY_MASK, delay);

	return max17851_reg_write(desc, MAX17851_CONFIG_COMM_REG, reg_val);
}

/**
 * @brief MAX17851 register read function.
 * @param desc - MAX17851 device descriptor.
 * @param reg - Register Address.
 * @param data - Data byte to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_reg_read(struct max17851_desc *desc, uint8_t reg, uint8_t *data)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX17851_FRAME_SIZE,
		.cs_change = 1,
	};
	int ret;

	memset(desc->buff, 0, xfer.bytes_number);
	desc->buff[0] = MAX17851_RD_REG(reg);

	ret = no_os_spi_transfer(desc->spi_desc, &xfer, 1);
	if (ret)
		return ret;

	*data = desc->buff[1];

	return 0;
}

/**
 * @brief MAX17851 register write function.
 * @param desc - MAX17851 device descriptor.
 * @param reg - Register Address.
 * @param data - Data byte to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_reg_write(struct max17851_desc *desc, uint8_t reg, uint8_t data)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX17851_FRAME_SIZE,
		.cs_change = 1,
	};

	desc->buff[0] = reg;
	desc->buff[1] = data;

	return no_os_spi_transfer(desc->spi_desc, &xfer, 1);
}

/**
 * @brief MAX17851 register update function.
 * @param desc - MAX17851 device descriptor.
 * @param reg - Register Address.
 * @param mask - Field mask to be udated inside the register.
 * @param data - Data byte to be updated.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_reg_update(struct max17851_desc *desc, uint8_t reg, uint8_t mask,
			uint8_t data)
{
	uint8_t reg_val = 0;
	int ret;

	ret = max17851_reg_read(desc, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & data;

	return max17851_reg_write(desc, reg, reg_val);
}

/**
 * @brief MAX17851 device initialization function
 * @param desc - MAX17851 device descriptor to be intialized.
 * @param init_param - MAX17851 initialization parameter.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_init(struct max17851_desc **desc,
		  struct max17851_init_param *init_param)
{
	struct max17851_desc *descriptor;
	uint8_t reg_val;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->spi_desc, init_param->spi_param);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get_optional(&descriptor->gpio1_desc,
				      init_param->gpio1_param);
	if (ret)
		goto free_spi;

	if (descriptor->gpio1_desc) {
		ret = no_os_gpio_direction_input(descriptor->gpio1_desc);
		if (ret)
			goto free_gpio1;
	}

	ret = no_os_gpio_get_optional(&descriptor->gpio2_desc,
				      init_param->gpio2_param);
	if (ret)
		goto free_gpio1;

	if (descriptor->gpio2_desc) {
		ret = no_os_gpio_direction_input(descriptor->gpio2_desc);
		if (ret)
			goto free_gpio2;
	}

	ret = max17851_send_command(descriptor, MAX17851_CLR_LSSM, true);
	if (ret)
		goto free_gpio2;

	/* Remaining SPI operation for MAX17851 initialization. */
	ret = max17851_reg_write(descriptor, MAX17851_CONFIG_GEN0_REG,
				 init_param->no_dev);
	if (ret)
		goto free_gpio2;

	descriptor->no_dev = init_param->no_dev;

	ret = max17851_reg_write(descriptor, MAX17851_CONFIG_GEN1_REG,
				 no_os_field_prep(MAX17851_SINGLE_MASK, init_param->single) |
				 no_os_field_prep(MAX17851_BAUD_MASK, init_param->baud_rate));
	if (ret)
		goto free_gpio2;

	descriptor->single = init_param->single;
	descriptor->baud_rate = init_param->baud_rate;

	ret = max17851_reg_update(descriptor, MAX17851_CONFIG_GEN4_REG,
				  MAX17851_MS_MASK,
				  no_os_field_prep(MAX17851_MS_MASK, init_param->op_mode));
	if (ret)
		goto free_gpio2;

	descriptor->op_mode = init_param->op_mode;

	ret = max17851_set_alert_debounce(descriptor, init_param->dbnc);
	if (ret)
		goto free_gpio2;

	descriptor->dbnc = init_param->dbnc;

	ret = max17851_reg_write(descriptor, MAX17851_CONFIG_SAFEMON0_REG,
				 init_param->gpio_rec_dly_csec);
	if (ret)
		goto free_gpio2;

	descriptor->gpio_rec_dly_csec = init_param->gpio_rec_dly_csec;

	ret = max17851_reg_write(descriptor, MAX17851_CONFIG_SAFEMON1_REG,
				 init_param->contact_tmr_dly_4xmin);
	if (ret)
		goto free_gpio2;

	descriptor->contact_tmr_dly_4xmin = init_param->contact_tmr_dly_4xmin;

	ret = max17851_set_safemon_dly(descriptor, init_param->safemon_dly);
	if (ret)
		goto free_gpio2;

	descriptor->safemon_dly = init_param->safemon_dly;

	ret = max17851_set_comm(descriptor, MAX17851_RETRY_COUNT_2,
				init_param->comm_to_delay);
	if (ret)
		goto free_gpio2;

	/* Wake up BMS chips in daisy-chain. */
	ret = max17851_sel_alrtpckt_time(descriptor, MAX17851_ALRTPCKT_INFINITE);
	if (ret)
		goto free_gpio2;

	ret = max17851_config_gen2(descriptor, MAX17851_TX_QUEUE_SEL, false);
	if (ret)
		goto free_gpio2;

	descriptor->comm_to_delay = init_param->comm_to_delay;

	/* Checking for errors on RX and clearing if necessary*/
	ret = max17851_reg_read(descriptor, MAX17851_ALERT_GEN_REG, &reg_val);
	if (ret)
		goto free_gpio2;

	if (reg_val) {
		ret = max17851_reg_write(descriptor, MAX17851_ALERT_GEN_REG,
					 0x00);
		if (ret)
			goto free_gpio2;
	}

	ret = max17851_clear_buff(descriptor, true);
	if (ret)
		goto free_gpio2;

	ret = max17851_clear_buff(descriptor, false);
	if (ret)
		goto free_gpio2;

	*desc = descriptor;

	return 0;

free_gpio2:
	no_os_gpio_remove(descriptor->gpio2_desc);
free_gpio1:
	no_os_gpio_remove(descriptor->gpio1_desc);
free_spi:
	no_os_spi_remove(descriptor->spi_desc);
free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Deallocate any resources used at the initialization function.
 * @param desc - MAX17851 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int max17851_remove(struct max17851_desc *desc)
{
	no_os_gpio_remove(desc->gpio2_desc);
	no_os_gpio_remove(desc->gpio1_desc);
	no_os_spi_remove(desc->spi_desc);
	no_os_free(desc);

	return 0;
}

/**
 * @brief MAX17851 platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops max17851_uart_ops = {
	.init = &max17851_uart_init,
	.read = &max17851_uart_read,
	.write = &max17851_uart_write,
	.get_errors = &max17851_uart_get_errors,
	.remove = &max17851_uart_remove
};
