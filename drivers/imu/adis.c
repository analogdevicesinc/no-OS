/***************************************************************************//**
 *   @file   adis.c
 *   @brief  Implementation of adis.c
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "adis.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include <string.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS_PAGE_SIZE			0x80
#define ADIS_REG_PAGE_ID		0x00
#define ADIS_WRITE_REG(reg)		((NO_OS_BIT(7) | (reg)))
#define ADIS_READ_REG(reg)		((reg) & NO_OS_GENMASK(6,0))
#define ADIS_4_BYTES_SIZE		4
#define ADIS_2_BYTES_SIZE		2
#define ADIS_1_BYTE_SIZE		1
#define ADIS_16_BIT_BURST_SIZE		0
#define ADIS_32_BIT_BURST_SIZE		1
#define ADIS_MSG_SIZE_16_BIT_BURST 	20 /* in bytes */
#define ADIS_MSG_SIZE_32_BIT_BURST 	32 /* in bytes */
#define ADIS_CHECKSUM_SIZE		2  /* in bytes */
#define ADIS_READ_BURST_DATA_CMD_SIZE	2  /* in bytes */
#define ADIS_READ_BURST_DATA_CMD_MSB	0x68
#define ADIS_READ_BURST_DATA_CMD_LSB	0x00
#define ADIS_SIGN_BIT_POS		15
#define ADIS_DIAG_IDX_16_BIT_BURST	0
#define ADIS_XGYRO_IDX_16_BIT_BURST	2
#define ADIS_YGYRO_IDX_16_BIT_BURST	4
#define ADIS_ZGYRO_IDX_16_BIT_BURST	6
#define ADIS_XACCL_IDX_16_BIT_BURST	8
#define ADIS_YACCL_IDX_16_BIT_BURST	10
#define ADIS_ZACCL_IDX_16_BIT_BURST	12
#define ADIS_TEMP_IDX_16_BIT_BURST	14
#define ADIS_CNT_IDX_16_BIT_BURST	16
#define ADIS_DIAG_IDX_32_BIT_BURST	0
#define ADIS_XGYRO_IDX_32_BIT_BURST	2
#define ADIS_YGYRO_IDX_32_BIT_BURST	6
#define ADIS_ZGYRO_IDX_32_BIT_BURST	10
#define ADIS_XACCL_IDX_32_BIT_BURST	14
#define ADIS_YACCL_IDX_32_BIT_BURST	18
#define ADIS_ZACCL_IDX_32_BIT_BURST	22
#define ADIS_TEMP_IDX_32_BIT_BURST	26
#define ADIS_CNT_IDX_32_BIT_BURST	28

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const uint8_t burst_size_bytes[] = {
	[ADIS_16_BIT_BURST_SIZE] = ADIS_MSG_SIZE_16_BIT_BURST,
	[ADIS_32_BIT_BURST_SIZE] = ADIS_MSG_SIZE_32_BIT_BURST,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize adis device.
 * @param adis - The adis device.
 * @param info - Initialization data.
 * @return 0 in case of success, error code otherwise.
 */
int adis_init(struct adis_dev **adis, const struct adis_chip_info *info)
{
	struct adis_dev *dev;
	int ret;

	dev = (struct adis_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, info->ip->spi_init);
	if (ret)
		goto error_spi;

	if(info->has_paging)
		dev->current_page = -1;
	else
		dev->current_page = 0;

	dev->dev_id = info->ip->dev_id;
	ret = no_os_gpio_get_optional(&dev->gpio_reset, info->ip->gpio_reset);
	if (ret) {
		pr_warning("No reset pin found \n");
	}

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto error;
	}

	dev->info = info;
	dev->int_clk = info->int_clk;

	ret = adis_initial_startup(dev);
	if (ret)
		goto error;

	ret = adis_write_sync_mode(dev, info->ip->sync_mode,
				   info->ip->ext_clk);
	if (ret)
		goto error;

	*adis = dev;

	return ret;

error:
	no_os_gpio_remove(dev->gpio_reset);
	no_os_spi_remove(dev->spi_desc);
error_spi:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Remove adis device.
 * @param adis - The adis device.
 */
void adis_remove(struct adis_dev *adis)
{
	if (adis->gpio_reset)
		no_os_gpio_remove(adis->gpio_reset);
	if (adis->spi_desc)
		no_os_spi_remove(adis->spi_desc);

	no_os_free(adis);
	adis = NULL;
}

/**
 * @brief Device initial setup.
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_initial_startup(struct adis_dev *adis)
{
	const struct adis_timeout *timeouts = adis->info->timeouts;
	struct adis_diag_flags diag_flags;
	int ret;

	if (adis->gpio_reset) {
		ret = no_os_gpio_set_value(adis->gpio_reset, NO_OS_GPIO_HIGH);
		if(ret)
			return ret;
		no_os_mdelay(timeouts->reset_ms);
	} else {
		ret = adis_cmd_sw_res(adis);
		if (ret)
			return ret;
	}

	ret = adis_cmd_snsr_self_test(adis);
	if (ret)
		return ret;

	ret = adis_cmd_fls_mem_test(adis);
	if (ret)
		return ret;

	return adis_read_diag_stat(adis, &diag_flags);
}

/**
 * @brief Read N bytes from register.
 * @param adis - The adis device.
 * @param reg  - The address of the lower of the two registers.
 * @param val  - The value read back from the device.
 * @param size - The size of the val buffer.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_reg(struct adis_dev *adis,  uint32_t reg, uint32_t *val,
		  uint32_t size)
{
	int ret;
	uint32_t page = reg / ADIS_PAGE_SIZE;
	struct no_os_spi_msg msgs[] = {
		{
			.tx_buff = adis->tx,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->read_delay,
		},
		{
			.tx_buff = adis->tx + 2,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->read_delay,
		},
		{
			.tx_buff = adis->tx + 4,
			.rx_buff = adis->rx,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->read_delay,
		},
		{
			.rx_buff = adis->rx + 2,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_delay_last = adis->info->read_delay,
		},
	};

	struct no_os_spi_msg msgs_to_send[NO_OS_ARRAY_SIZE(msgs)];
	uint32_t send_idx = 0;

	if (adis->current_page != page) {
		adis->tx[0] = ADIS_WRITE_REG(ADIS_REG_PAGE_ID);
		adis->tx[1] = page;
		msgs_to_send[send_idx] = msgs[0];
		send_idx++;
	}
	switch (size) {
	case ADIS_4_BYTES_SIZE:
		adis->tx[2] = ADIS_READ_REG(reg + 2);
		adis->tx[3] = 0;
		msgs_to_send[send_idx] = msgs[1];
		send_idx++;
	/* fallthrough */
	case ADIS_2_BYTES_SIZE:
		adis->tx[4] = ADIS_READ_REG(reg);
		adis->tx[5] = 0;
		msgs_to_send[send_idx] = msgs[2];
		msgs_to_send[send_idx + 1] = msgs[3];
		send_idx += 2;
		break;
	default:
		return -EINVAL;
	}

	ret = no_os_spi_transfer(adis->spi_desc, msgs_to_send, send_idx);
	if (ret)
		return ret;

	adis->current_page = page;

	switch (size) {
	case ADIS_4_BYTES_SIZE:
		*val = no_os_get_unaligned_be32(adis->rx);
		break;
	case ADIS_2_BYTES_SIZE:
		*val = no_os_get_unaligned_be16(adis->rx + 2);
		break;
	}

	return 0;
}

/**
 * @brief Write N bytes to register.
 * @param adis - The adis device.
 * @param reg  - The address of the lower of the two registers.
 * @param val  - The value to write to device (up to 4 bytes).
 * @param size - The size of the val buffer.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_reg(struct adis_dev *adis, uint32_t reg, uint32_t val,
		   uint32_t size)
{
	int ret;
	uint32_t page = reg / ADIS_PAGE_SIZE, i;
	struct no_os_spi_msg msgs[] = {
		{
			.tx_buff = adis->tx,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->write_delay,
		},
		{
			.tx_buff = adis->tx + 2,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->write_delay,
		},
		{
			.tx_buff = adis->tx + 4,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->write_delay,
		},
		{
			.tx_buff = adis->tx + 6,
			.bytes_number = 2,
			.cs_delay_last = adis->info->write_delay,
		},
		{
			.tx_buff = adis->tx + 8,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_delay_last = adis->info->write_delay,
		},
	};

	struct no_os_spi_msg msgs_to_send[NO_OS_ARRAY_SIZE(msgs)];
	uint32_t send_idx = 0;

	if (adis->current_page != page) {
		adis->tx[0] = ADIS_WRITE_REG(ADIS_REG_PAGE_ID);
		adis->tx[1] = page;
		msgs_to_send[send_idx] = msgs[0];
		send_idx++;
	}

	switch (size) {
	case ADIS_4_BYTES_SIZE:
		adis->tx[8] = ADIS_WRITE_REG(reg + 3);
		adis->tx[9] = (val >> 24) & 0xff;
		adis->tx[6] = ADIS_WRITE_REG(reg + 2);
		adis->tx[7] = (val >> 16) & 0xff;
	/* fallthrough */
	case ADIS_2_BYTES_SIZE:
		adis->tx[4] = ADIS_WRITE_REG(reg + 1);
		adis->tx[5] = (val >> 8) & 0xff;
	/* fallthrough */
	case ADIS_1_BYTE_SIZE:
		adis->tx[2] = ADIS_WRITE_REG(reg);
		adis->tx[3] = val & 0xff;
		break;
	default:
		return -EINVAL;
	}

	for (i = 1; i <= size; i++, send_idx ++)
		msgs_to_send[send_idx] = msgs[i];

	ret = no_os_spi_transfer(adis->spi_desc, msgs_to_send, send_idx);
	if (ret)
		return ret;

	adis->current_page = page;

	return 0;
}

/**
 * @brief Read field to uint32 value.
 * @param adis      - The adis device.
 * @param field     - The field structure to be read
 * @param field_val - The read field value.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_read_field_u32(struct adis_dev *adis, struct adis_field field,
			       uint32_t *field_val)
{
	int ret;
	uint32_t reg_val;
	ret = adis_read_reg(adis, field.reg_addr, &reg_val, field.reg_size);
	if(ret)
		return ret;

	*field_val = no_os_field_get(field.field_mask, reg_val);

	return 0;
}

/**
 * @brief Read field to int32 value.
 * @param adis      - The adis device.
 * @param field     - The field structure to be read
 * @param field_val - The read field value.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_read_field_s32(struct adis_dev *adis, struct adis_field field,
			       int32_t *field_val)
{
	int ret;
	uint32_t reg_val;

	ret = adis_read_reg(adis, field.reg_addr, &reg_val, field.reg_size);
	if(ret)
		return ret;

	*field_val = no_os_field_get(field.field_mask, reg_val);
	*field_val = no_os_sign_extend32(*field_val,
					 no_os_find_last_set_bit(field.field_mask));

	return 0;
}

/**
 * @brief Write field from uint32 value.
 * @param adis      - The adis device.
 * @param field     - The field structure to be written.
 * @param field_val - The field value to be written.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_write_field_u32(struct adis_dev *adis, struct adis_field field,
				uint32_t field_val)
{
	if (field_val > no_os_field_get(field.field_mask, field.field_mask))
		return -EINVAL;

	return adis_update_bits_base(adis, field.reg_addr, field.field_mask,
				     field_val, field.reg_size);
}

/**
 * @brief Update the desired bits of reg in accordance with mask and val.
 * @param adis - The adis device.
 * @param reg  - The address of the lower of the two registers.
 * @param mask - Bitmask to change.
 * @param val  - The value to be written.
 * @param size - Size of the register to update.
 * @return 0 in case of success, error code otherwise.
 */
int adis_update_bits_base(struct adis_dev *adis, uint32_t reg,
			  const uint32_t mask, const uint32_t val, uint8_t size)
{
	int ret;
	uint32_t __val;

	ret = adis_read_reg(adis, reg, &__val, size);
	if (ret)
		return ret;

	__val = (__val & ~mask) | no_os_field_prep(mask, val);

	return adis_write_reg(adis, reg, __val, size);
}

/**
 * @brief Check if the checksum for burst data is correct.
 * @param buffer - The received burst data buffer.
 * @param size   - The size of the buffer.
 * @return 0 in case of success, error code otherwise.
 */
static bool adis_validate_checksum(uint8_t *buffer, uint8_t size)
{
	uint8_t i;
	uint16_t checksum = no_os_get_unaligned_be16(&buffer[size-ADIS_CHECKSUM_SIZE]);

	for (i = 0; i < size - ADIS_CHECKSUM_SIZE; i++)
		checksum -= buffer[i];

	return checksum == 0;
}

/**
 * @brief Update device diagnosis flags according to the received parameter.
 * @param adis      - The adis device.
 * @param diag_stat - Diagnosis flags.
 */
static void adis_update_diag_flags(struct adis_dev *adis, uint16_t diag_stat)
{
	const struct adis_data_field_map_def *field_map = adis->info->field_map;

	adis->diag_flags.snsr_init_failure = diag_stat &
					     field_map->diag_snsr_init_failure_mask ? 1 : 0;
	adis->diag_flags.data_path_overrun = diag_stat &
					     field_map->diag_data_path_overrun_mask ? 1 : 0;
	adis->diag_flags.spi_comm_err = diag_stat &
					field_map->diag_spi_comm_err_mask ? 1 : 0;
	adis->diag_flags.standby_mode = diag_stat &
					field_map->diag_standby_mode_mask ? 1 : 0;
	adis->diag_flags.clk_err = diag_stat &
				   field_map->diag_clk_err_mask ? 1 : 0;
	adis->diag_flags.fls_mem_update_failure = diag_stat &
			field_map->diag_fls_mem_update_failure_mask ? 1 : 0;
	adis->diag_flags.mem_failure = diag_stat &
				       field_map->diag_mem_failure_mask ? 1 : 0;
	adis->diag_flags.snsr_failure = diag_stat &
					field_map->diag_snsr_failure_mask ? 1 : 0;
	adis->diag_flags.gyro1_failure = diag_stat &
					 field_map->diag_gyro1_failure_mask ? 1 : 0;
	adis->diag_flags.gyro2_failure = diag_stat &
					 field_map->diag_gyro2_failure_mask ? 1 : 0;
	adis->diag_flags.accl_failure = diag_stat &
					field_map->diag_accl_failure_mask ? 1 : 0;
	adis->diag_flags.x_axis_gyro_failure = diag_stat &
					       field_map->diag_x_axis_gyro_failure_mask ? 1 : 0;
	adis->diag_flags.y_axis_gyro_failure = diag_stat &
					       field_map->diag_y_axis_gyro_failure_mask ? 1 : 0;
	adis->diag_flags.z_axis_gyro_failure = diag_stat &
					       field_map->diag_z_axis_gyro_failure_mask ? 1 : 0;
	adis->diag_flags.x_axis_accl_failure = diag_stat &
					       field_map->diag_x_axis_accl_failure_mask ? 1 : 0;
	adis->diag_flags.y_axis_accl_failure = diag_stat &
					       field_map->diag_y_axis_accl_failure_mask ? 1 : 0;
	adis->diag_flags.z_axis_accl_failure = diag_stat &
					       field_map->diag_z_axis_accl_failure_mask ? 1 : 0;
	adis->diag_flags.aduc_mcu_fault = diag_stat &
					  field_map->diag_aduc_mcu_fault_mask ? 1 : 0;
}

/**
 * @brief Read diag status register and update device diag flags.
 * @param adis       - The adis device.
 * @param diag_flags - The read diag flags.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_stat(struct adis_dev *adis,
			struct adis_diag_flags *diag_flags)
{
	struct adis_field field = adis->info->field_map->diag_stat;
	uint32_t field_val;
	int ret;

	ret = adis_read_reg(adis, field.reg_addr, &field_val, field.reg_size);
	if (ret)
		return ret;

	adis_update_diag_flags(adis, field_val);
	*diag_flags = adis->diag_flags;

	return 0;
}

/**
 * @brief Diagnosis: read sensor initialization failure flag value.
 * @param adis              - The adis device.
 * @param snsr_init_failure - Sensor initialization failure flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_snsr_init_failure(struct adis_dev *adis,
				     uint32_t *snsr_init_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*snsr_init_failure = diag_flags.snsr_init_failure;

	return 0;
}

/**
 * @brief Diagnosis: read data path overrun flag value.
 * @param adis                  - The adis device.
 * @param data_path_overrun_err - Data path overrun flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_data_path_overrun(struct adis_dev *adis,
				     uint32_t *data_path_overrun_err)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*data_path_overrun_err = diag_flags.data_path_overrun;

	return 0;
}

/**
 * @brief Diagnosis: read flash memory update error flag value.
 * @param adis                   - The adis device.
 * @param fls_mem_update_failure - Flash memory update error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_fls_mem_update_failure(struct adis_dev *adis,
		uint32_t *fls_mem_update_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*fls_mem_update_failure = diag_flags.fls_mem_update_failure;

	return 0;
}

/**
 * @brief Diagnosis: read spi communication error flag value.
 * @param adis         - The adis device.
 * @param spi_comm_err - Spi communication error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_spi_comm_err(struct adis_dev *adis, uint32_t *spi_comm_err)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*spi_comm_err = diag_flags.spi_comm_err;

	return 0;
}

/**
 * @brief Diagnosis: read standby mode flag value.
 * @param adis              - The adis device.
 * @param standby_mode - Standby mode flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_standby_mode(struct adis_dev *adis,
				uint32_t *standby_mode)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*standby_mode = diag_flags.standby_mode;

	return 0;
}

/**
 * @brief Diagnosis: read sensor self test error flag value.
 * @param adis         - The adis device.
 * @param snsr_failure - Sensor self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_snsr_failure(struct adis_dev *adis, uint32_t *snsr_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*snsr_failure = diag_flags.snsr_failure;

	return 0;
}

/**
 * @brief Diagnosis: read flash memory test error flag value.
 * @param adis        - The adis device.
 * @param mem_failure - Flash memory test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_mem_failure(struct adis_dev *adis, uint32_t *mem_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*mem_failure = diag_flags.mem_failure;

	return 0;
}

/**
 * @brief Diagnosis: read clock error flag value.
 * @param adis    - The adis device.
 * @param clk_err - Clock error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_clk_err(struct adis_dev *adis, uint32_t *clk_err)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*clk_err = diag_flags.clk_err;

	return 0;
}

/**
 * @brief Diagnosis: read gyroscope1 self test error flag value.
 * @param adis          - The adis device.
 * @param gyro1_failure - Gyroscope1 self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_gyro1_failure(struct adis_dev *adis, uint32_t *gyro1_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*gyro1_failure = diag_flags.gyro1_failure;

	return 0;
}

/**
 * @brief Diagnosis: read gyroscope2 self test error flag value.
 * @param adis          - The adis device.
 * @param gyro2_failure - Gyroscope2 self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_gyro2_failure(struct adis_dev *adis, uint32_t *gyro2_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*gyro2_failure = diag_flags.gyro2_failure;

	return 0;
}

/**
 * @brief Diagnosis: read accelerometer self test error flag value.
 * @param adis         - The adis device.
 * @param accl_failure - Accelerometer self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_accl_failure(struct adis_dev *adis, uint32_t *accl_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*accl_failure = diag_flags.accl_failure;

	return 0;
}

/**
 * @brief Diagnosis: read X-Axis Gyroscope failure flag value.
 * @param adis                - The adis device.
 * @param x_axis_gyro_failure - X-Axis Gyroscope failure flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_x_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *x_axis_gyro_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*x_axis_gyro_failure = diag_flags.x_axis_gyro_failure;

	return 0;
}

/**
 * @brief Diagnosis: read Y-Axis Gyroscope failure flag value.
 * @param adis                - The adis device.
 * @param y_axis_gyro_failure - Y-Axis Gyroscope failure flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_y_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *y_axis_gyro_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*y_axis_gyro_failure = diag_flags.y_axis_gyro_failure;

	return 0;
}

/**
 * @brief Diagnosis: read Z-Axis Gyroscope failure flag value.
 * @param adis                - The adis device.
 * @param z_axis_gyro_failure - Z-Axis Gyroscope failure flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_z_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *z_axis_gyro_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*z_axis_gyro_failure = diag_flags.z_axis_gyro_failure;

	return 0;
}

/**
 * @brief Diagnosis: read X-Axis Accelerometer failure flag value.
 * @param adis                - The adis device.
 * @param x_axis_accl_failure - X-Axis Accelerometer failure flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_x_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *x_axis_accl_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*x_axis_accl_failure = diag_flags.x_axis_accl_failure;

	return 0;
}

/**
 * @brief Diagnosis: read Y-Axis Accelerometer failure flag value.
 * @param adis                - The adis device.
 * @param y_axis_accl_failure - Y-Axis Accelerometer failure flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_y_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *y_axis_accl_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*y_axis_accl_failure = diag_flags.y_axis_accl_failure;

	return 0;
}

/**
 * @brief Diagnosis: read Z-Axis Accelerometer failure flag value.
 * @param adis                - The adis device.
 * @param z_axis_accl_failure - Z-Axis Accelerometer failure flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_z_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *z_axis_accl_failure)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*z_axis_accl_failure = diag_flags.z_axis_accl_failure;

	return 0;
}

/**
 * @brief Diagnosis: read ADuC microcontroller fault flag value.
 * @param adis           - The adis device.
 * @param aduc_mcu_fault - ADuC microcontroller fault flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_aduc_mcu_fault(struct adis_dev *adis,
				  uint32_t *aduc_mcu_fault)
{
	struct adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*aduc_mcu_fault = diag_flags.aduc_mcu_fault;

	return 0;
}

/**
 * @brief Diagnosis: read checksum error flag value.
 * @param adis         - The adis device.
 * @param checksum_err - Checksum error flag value.
 */
void adis_read_diag_checksum_err(struct adis_dev *adis, uint32_t *checksum_err)
{
	*checksum_err = adis->diag_flags.checksum_err;
}

/**
 * @brief Diagnosis: read flash memory write counts exceeded flag value.
 * @param adis                  - The adis device.
 * @param fls_mem_wr_cnt_exceed - Flash memory write counts exceeded flag value.
 */
void adis_read_diag_fls_mem_wr_cnt_exceed(struct adis_dev *adis,
		uint32_t *fls_mem_wr_cnt_exceed)
{
	*fls_mem_wr_cnt_exceed =
		adis->diag_flags.fls_mem_wr_cnt_exceed;
}

/**
 * @brief Read raw gyroscope data on x axis.
 * @param adis   - The adis device.
 * @param x_gyro - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_gyro(struct adis_dev *adis, int32_t *x_gyro)
{
	return adis_read_field_s32(adis, adis->info->field_map->x_gyro, x_gyro);
}

/**
 * @brief Read raw gyroscope data on y axis.
 * @param adis   - The adis device.
 * @param y_gyro - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_gyro(struct adis_dev *adis, int32_t *y_gyro)
{
	return adis_read_field_s32(adis, adis->info->field_map->y_gyro, y_gyro);
}

/**
 * @brief Read raw gyroscope data on z axis.
 * @param adis   - The adis device.
 * @param z_gyro - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_gyro(struct adis_dev *adis, int32_t *z_gyro)
{
	return adis_read_field_s32(adis, adis->info->field_map->z_gyro, z_gyro);
}

/**
 * @brief Read raw acceleration data on x axis.
 * @param adis   - The adis device.
 * @param x_accl - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_accl(struct adis_dev *adis, int32_t *x_accl)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->x_accl, x_accl);
}

/**
 * @brief Read raw acceleration data on y axis.
 * @param adis   - The adis device.
 * @param y_accl - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_accl(struct adis_dev *adis, int32_t *y_accl)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->y_accl, y_accl);
}

/**
 * @brief Read raw acceleration data on z axis.
 * @param adis   - The adis device.
 * @param z_accl - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_accl(struct adis_dev *adis, int32_t *z_accl)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->z_accl, z_accl);
}

/**
 * @brief Read raw temperature data.
 * @param adis     - The adis device.
 * @param temp_out - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_temp_out(struct adis_dev *adis, int32_t *temp_out)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->temp_out, temp_out);
}

/**
 * @brief Read raw time stamp data.
 * @param adis       - The adis device.
 * @param time_stamp - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_time_stamp(struct adis_dev *adis, uint32_t *time_stamp)
{
	return adis_read_field_u32(adis, adis->info->field_map->time_stamp, time_stamp);
}

/**
 * @brief Read data counter value.
 * @param adis      - The adis device.
 * @param data_cntr - The read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_data_cntr(struct adis_dev *adis, uint32_t *data_cntr)
{
	return adis_read_field_u32(adis, adis->info->field_map->data_cntr, data_cntr);
}

/**
 * @brief Read raw delta angle data on x axis.
 * @param adis      - The adis device.
 * @param x_deltang - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_deltang(struct adis_dev *adis, int32_t *x_deltang)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->x_deltang, x_deltang);
}

/**
 * @brief Read raw delta angle data on y axis.
 * @param adis      - The adis device.
 * @param y_deltang - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_deltang(struct adis_dev *adis, int32_t *y_deltang)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->y_deltang, y_deltang);
}

/**
 * @brief Read raw delta angle data on z axis.
 * @param adis      - The adis device.
 * @param z_deltang - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_deltang(struct adis_dev *adis, int32_t *z_deltang)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->z_deltang, z_deltang);
}

/**
 * @brief Read raw delta velocity data on x axis.
 * @param adis      - The adis device.
 * @param x_deltvel - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_deltvel(struct adis_dev *adis, int32_t *x_deltvel)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->x_deltvel, x_deltvel);
}

/**
 * @brief Read raw delta velocity data on y axis.
 * @param adis      - The adis device.
 * @param y_deltvel - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_deltvel(struct adis_dev *adis, int32_t *y_deltvel)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->y_deltvel, y_deltvel);
}

/**
 * @brief Read raw delta velocity data on z axis.
 * @param adis      - The adis device.
 * @param z_deltvel - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_deltvel(struct adis_dev *adis, int32_t *z_deltvel)
{
	int ret;

	if (adis->fifo_enabled) {
		ret = adis_write_fifo_en(adis, 0);
		if (ret)
			return ret;
	}

	return adis_read_field_s32(adis, adis->info->field_map->z_deltvel, z_deltvel);
}

/**
 * @brief Read output FIFO sample count.
 * @param adis     - The adis device.
 * @param fifo_cnt - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_fifo_cnt(struct adis_dev *adis, uint32_t *fifo_cnt)
{
	return adis_read_field_u32(adis, adis->info->field_map->fifo_cnt, fifo_cnt);
}

/**
 * @brief Read current sample SPI transaction checksum.
 * @param adis     - The adis device.
 * @param checksum - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_spi_chksum(struct adis_dev *adis, uint32_t *checksum)
{
	return adis_read_field_u32(adis, adis->info->field_map->spi_chksum, checksum);
}

/**
 * @brief Read raw gyroscope offset correction on x axis.
 * @param adis    - The adis device.
 * @param xg_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_xg_bias(struct adis_dev *adis, int32_t *xg_bias)
{
	return adis_read_field_s32(adis, adis->info->field_map->xg_bias, xg_bias);
}

/**
 * @brief Write raw gyroscope offset correction on x axis.
 * @param adis    - The adis device.
 * @param xg_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_xg_bias(struct adis_dev *adis, int32_t xg_bias)
{
	return adis_write_field_u32(adis, adis->info->field_map->xg_bias, xg_bias);
}

/**
 * @brief Read raw gyroscope offset correction on y axis.
 * @param adis    - The adis device.
 * @param yg_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_yg_bias(struct adis_dev *adis, int32_t *yg_bias)
{
	return adis_read_field_s32(adis, adis->info->field_map->yg_bias, yg_bias);
}

/**
 * @brief Write raw gyroscope offset correction on y axis.
 * @param adis    - The adis device.
 * @param yg_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_yg_bias(struct adis_dev *adis, int32_t yg_bias)
{
	return adis_write_field_u32(adis, adis->info->field_map->yg_bias, yg_bias);
}

/**
 * @brief Read raw gyroscope offset correction on z axis.
 * @param adis    - The adis device.
 * @param zg_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_zg_bias(struct adis_dev *adis, int32_t *zg_bias)
{
	return adis_read_field_s32(adis, adis->info->field_map->zg_bias, zg_bias);
}

/**
 * @brief Write raw gyroscope offset correction on z axis.
 * @param adis    - The adis device.
 * @param zg_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_zg_bias(struct adis_dev *adis, int32_t zg_bias)
{
	return adis_write_field_u32(adis, adis->info->field_map->zg_bias, zg_bias);
}

/**
 * @brief Read raw acceleration offset correction on x axis.
 * @param adis    - The adis device.
 * @param xa_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_xa_bias(struct adis_dev *adis, int32_t *xa_bias)
{
	return adis_read_field_s32(adis, adis->info->field_map->xa_bias, xa_bias);
}

/**
 * @brief Write raw acceleration offset correction on x axis.
 * @param adis    - The adis device.
 * @param xa_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_xa_bias(struct adis_dev *adis, int32_t xa_bias)
{
	return adis_write_field_u32(adis, adis->info->field_map->xa_bias, xa_bias);
}

/**
 * @brief Read raw acceleration offset correction on y axis.
 * @param adis    - The adis device.
 * @param ya_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_ya_bias(struct adis_dev *adis, int32_t *ya_bias)
{
	return adis_read_field_s32(adis, adis->info->field_map->ya_bias, ya_bias);
}

/**
 * @brief Write raw acceleration offset correction on y axis.
 * @param adis    - The adis device.
 * @param ya_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_ya_bias(struct adis_dev *adis, int32_t ya_bias)
{
	return adis_write_field_u32(adis, adis->info->field_map->ya_bias, ya_bias);
}

/**
 * @brief Read raw acceleration offset correction on z axis.
 * @param adis    - The adis device.
 * @param za_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_za_bias(struct adis_dev *adis, int32_t *za_bias)
{
	return adis_read_field_s32(adis, adis->info->field_map->za_bias, za_bias);
}

/**
 * @brief Write raw acceleration offset correction on z axis.
 * @param adis    - The adis device.
 * @param za_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_za_bias(struct adis_dev *adis, int32_t za_bias)
{
	return adis_write_field_u32(adis, adis->info->field_map->za_bias, za_bias);
}

/*
 * @brief Read FIFO enable bit value.
 * @param adis    - The adis device.
 * @param fifo_en - The FIFO enable bit read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_fifo_en(struct adis_dev *adis, uint32_t *fifo_en)
{
	return adis_read_field_u32(adis, adis->info->field_map->fifo_en, fifo_en);
}

/**
 * @brief Write FIFO enable bit value.
 * @param adis    - The adis device.
 * @param fifo_en - The FIFO enable bit value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_fifo_en(struct adis_dev *adis, uint32_t fifo_en)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->fifo_en, fifo_en);
	if (ret)
		return ret;

	adis->fifo_enabled = fifo_en == 1;

	return 0;
}

/**
 * @brief Read FIFO overflow bit value.
 * @param adis          - The adis device.
 * @param fifo_overflow - The FIFO overflow bit read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_fifo_overflow(struct adis_dev *adis, uint32_t *fifo_overflow)
{
	return adis_read_field_u32(adis, adis->info->field_map->fifo_overflow,
				   fifo_overflow);
}

/**
 * @brief Write FIFO overflow bit value.
 * @param adis          - The adis device.
 * @param fifo_overflow - The FIFO overflow bit value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_fifo_overflow(struct adis_dev *adis, uint32_t fifo_overflow)
{
	return adis_write_field_u32(adis, adis->info->field_map->fifo_overflow,
				    fifo_overflow);
}

/**
 * @brief Read FIFO watermark interrupt enable bit value.
 * @param adis           - The adis device.
 * @param fifo_wm_int_en - The FIFO watermark interrupt enable bit read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_fifo_wm_int_en(struct adis_dev *adis, uint32_t *fifo_wm_int_en)
{
	return adis_read_field_u32(adis, adis->info->field_map->fifo_wm_int_en,
				   fifo_wm_int_en);
}

/**
 * @brief Write FIFO watermark interrupt enable bit value.
 * @param adis           - The adis device.
 * @param fifo_wm_int_en - The FIFO watermark interrupt enable bit value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_fifo_wm_int_en(struct adis_dev *adis, uint32_t fifo_wm_int_en)
{
	return adis_write_field_u32(adis, adis->info->field_map->fifo_wm_int_en,
				    fifo_wm_int_en);
}

/**
 * @brief Read FIFO watermark interrupt polarity bit value.
 * @param adis            - The adis device.
 * @param fifo_wm_int_pol - The FIFO watermark interrupt polarity bit read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_fifo_wm_int_pol(struct adis_dev *adis, uint32_t *fifo_wm_int_pol)
{
	return adis_read_field_u32(adis, adis->info->field_map->fifo_wm_int_pol,
				   fifo_wm_int_pol);
}

/**
 * @brief Write FIFO watermark interrupt polarity bit value.
 * @param adis            - The adis device.
 * @param fifo_wm_int_pol - The FIFO watermark interrupt polarity bit value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_fifo_wm_int_pol(struct adis_dev *adis, uint32_t fifo_wm_int_pol)
{
	return adis_write_field_u32(adis, adis->info->field_map->fifo_wm_int_pol,
				    fifo_wm_int_pol);
}

/**
 * @brief Read FIFO watermark threshold level value.
 * @param adis        - The adis device.
 * @param fifo_wm_lvl - The FIFO watermark threshold level read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_fifo_wm_lvl(struct adis_dev *adis, uint32_t *fifo_wm_lvl)
{
	return adis_read_field_u32(adis, adis->info->field_map->fifo_wm_lvl,
				   fifo_wm_lvl);
}

/**
 * @brief Write FIFO watermark threshold level value.
 * @param adis        - The adis device.
 * @param fifo_wm_lvl - The FIFO watermark threshold level value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_fifo_wm_lvl(struct adis_dev *adis, uint32_t fifo_wm_lvl)
{
	return adis_write_field_u32(adis, adis->info->field_map->fifo_wm_lvl,
				    fifo_wm_lvl);
}

/**
 * @brief Read filter size variable B value.
 * @param adis            - The adis device.
 * @param filt_size_var_b - The filter size variable B read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_filt_size_var_b(struct adis_dev *adis, uint32_t *filt_size_var_b)
{
	return adis_read_field_u32(adis, adis->info->field_map->filt_size_var_b,
				   filt_size_var_b);
}

/**
 * @brief Write filter size variable B value.
 * @param adis            - The adis device.
 * @param filt_size_var_b - The filter size variable B value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_filt_size_var_b(struct adis_dev *adis, uint32_t filt_size_var_b)
{
	int ret;

	if(filt_size_var_b > adis->info->filt_size_var_b_max)
		return -EINVAL;

	ret = adis_write_field_u32(adis, adis->info->field_map->filt_size_var_b,
				   filt_size_var_b);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->filt_size_var_b_update_us);

	return 0;
}

/**
 * @brief Read gyroscope measurement range value.
 * @param adis            - The adis device.
 * @param gyro_meas_range - The gyroscope measurement range value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_gyro_meas_range(struct adis_dev *adis, uint32_t *gyro_meas_range)
{
	return adis_read_field_u32(adis, adis->info->field_map->gyro_meas_range,
				   gyro_meas_range);
}

/**
 * @brief Read data ready polarity encoded value.
 * @param adis        - The adis device.
 * @param dr_polarity - The data ready polarity encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_dr_polarity(struct adis_dev *adis, uint32_t *dr_polarity)
{
	return adis_read_field_u32(adis, adis->info->field_map->dr_polarity,
				   dr_polarity);
}

/**
 * @brief Write data ready polarity encoded value.
 * @param adis        - The adis device.
 * @param dr_polarity - The data ready polarity encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_dr_polarity(struct adis_dev *adis, uint32_t dr_polarity)
{
	int ret;

	if (dr_polarity > 1)
		return -EINVAL;

	ret = adis_write_field_u32(adis, adis->info->field_map->dr_polarity,
				   dr_polarity);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read sync polarity encoded value.
 * @param adis          - The adis device.
 * @param sync_polarity - The sync polarity encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_sync_polarity(struct adis_dev *adis, uint32_t *sync_polarity)
{
	return adis_read_field_u32(adis, adis->info->field_map->sync_polarity,
				   sync_polarity);
}

/**
 * @brief Write sync polarity encoded value.
 * @param adis          - The adis device.
 * @param sync_polarity - The sync polarity encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_sync_polarity(struct adis_dev *adis, uint32_t sync_polarity)
{
	int ret;

	if (sync_polarity > 1)
		return -EINVAL;

	ret = adis_write_field_u32(adis, adis->info->field_map->sync_polarity,
				   sync_polarity);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read synchronization mode encoded value.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_sync_mode(struct adis_dev *adis, uint32_t *sync_mode)
{
	return adis_read_field_u32(adis, adis->info->field_map->sync_mode, sync_mode);
}

/**
 * @brief Update synchronization mode.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value to update.
 * @param ext_clk   - The external clock frequency to update, will be ignored
 * if sync_mode is different from ADIS_SYNC_SCALED and ADIS_SYNC_DIRECT.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_sync_mode(struct adis_dev *adis, uint32_t sync_mode,
			 uint32_t ext_clk)
{
	int ret;

	if(sync_mode > adis->info->sync_mode_max)
		return -EINVAL;

	if (sync_mode != ADIS_SYNC_DEFAULT && sync_mode != ADIS_SYNC_OUTPUT) {
		/* Sync pulse is external */
		if (ext_clk < adis->info->sync_clk_freq_limits[sync_mode].min_freq
		    || ext_clk > adis->info->sync_clk_freq_limits[sync_mode].max_freq)
			return -EINVAL;

		adis->ext_clk = ext_clk;

		if (sync_mode == ADIS_SYNC_SCALED) {
			/*
			 * In sync scaled mode, the IMU sample rate is the
			 * clk_freq * sync_scale.
			 * Hence, default the IMU sample rate to the highest
			 * multiple of the input clock lower than the IMU max
			 * sample rate.
			 */
			ret = adis_write_up_scale(adis,
						  adis->info->sampling_clk_limits.max_freq / ext_clk);
			if (ret)
				return ret;
		}
	}

	return adis_write_field_u32(adis, adis->info->field_map->sync_mode, sync_mode);
}

/**
 * @brief Read internal sensor bandwidth encoded value.
 * @param adis    - The adis device.
 * @param sens_bw - The internal sensor bandwidth encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_sens_bw(struct adis_dev *adis, uint32_t *sens_bw)
{
	return adis_read_field_u32(adis, adis->info->field_map->sens_bw, sens_bw);
}

/**
 * @brief Write internal sensor bandwidth encoded value.
 * @param adis    - The adis device.
 * @param sens_bw - The internal sensor bandwidth encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_sens_bw(struct adis_dev *adis, uint32_t sens_bw)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->sens_bw, sens_bw);
	if (ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->sens_bw_update_ms);

	return 0;
}

/**
 * @brief Read point of percussion alignment enable bit value.
 * @param adis              - The adis device.
 * @param pt_of_perc_algnmt - The point of percussion alignment enable bit
 *                            value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_pt_of_perc_algnmt(struct adis_dev *adis,
				uint32_t *pt_of_perc_algnmt)
{
	return adis_read_field_u32(adis, adis->info->field_map->pt_of_perc_algnmt,
				   pt_of_perc_algnmt);
}

/**
 * @brief Write point of percussion alignment enable bit value.
 * @param adis              - The adis device.
 * @param pt_of_perc_algnmt - The point of percussion alignment enable bit
 * 			      value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_pt_of_perc_algnmt(struct adis_dev *adis,
				 uint32_t pt_of_perc_algnmt)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->pt_of_perc_algnmt,
				   pt_of_perc_algnmt);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read linear acceleration compensation enable bit value.
 * @param adis             - The adis device.
 * @param linear_accl_comp - The linear acceleration compensation enable bit
 * 			     value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_linear_accl_comp(struct adis_dev *adis,
			       uint32_t *linear_accl_comp)
{
	return adis_read_field_u32(adis, adis->info->field_map->linear_accl_comp,
				   linear_accl_comp);
}

/**
 * @brief Write linear acceleration compensation enable bit value.
 * @param adis             - The adis device.
 * @param linear_accl_comp - The linear acceleration compensation enable bit
 * 			     value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_linear_accl_comp(struct adis_dev *adis,
				uint32_t linear_accl_comp)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->linear_accl_comp,
				   linear_accl_comp);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read burst selection encoded value.
 * @param adis      - The adis device.
 * @param burst_sel - The burst selection encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_burst_sel(struct adis_dev *adis, uint32_t *burst_sel)
{
	return adis_read_field_u32(adis, adis->info->field_map->burst_sel, burst_sel);
}

/**
 * @brief Write burst selection encoded value.
 * @param adis      - The adis device.
 * @param burst_sel - The burst selection encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_burst_sel(struct adis_dev *adis, uint32_t burst_sel)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->burst_sel, burst_sel);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read burst32 enable bit value.
 * @param adis    - The adis device.
 * @param burst32 - The burst32 enable bit value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_burst32(struct adis_dev *adis, uint32_t *burst32)
{
	return adis_read_field_u32(adis, adis->info->field_map->burst32, burst32);
}

/**
 * @brief Write burst32 enable bit value.
 * @param adis    - The adis device.
 * @param burst32 - The burst32 enable bit value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_burst32(struct adis_dev *adis, uint32_t burst32)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->burst32, burst32);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read timestamp32 enable bit value.
 * @param adis        - The adis device.
 * @param timestamp32 - The timestamp32 enable bit value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_timestamp32(struct adis_dev *adis, uint32_t *timestamp32)
{
	return adis_read_field_u32(adis, adis->info->field_map->timestamp32,
				   timestamp32);
}

/**
 * @brief Write timestamp32 enable bit value.
 * @param adis        - The adis device.
 * @param timestamp32 - The timestamp32 enable bit value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_timestamp32(struct adis_dev *adis, uint32_t timestamp32)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->timestamp32,
				   timestamp32);
	if (ret)
		return ret;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read 4khz internal sync enable bit value.
 * @param adis      - The adis device.
 * @param sync_4khz - The 4khz internal sync enable bit value (1/0 - enabled/disabled).
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_sync_4khz(struct adis_dev *adis, uint32_t *sync_4khz)
{
	return adis_read_field_u32(adis, adis->info->field_map->sync_4khz, sync_4khz);
}

/**
 * @brief Write 4khz internal sync enable bit value.
 * @param adis      - The adis device.
 * @param sync_4khz - The 4khz internal sync enable bit value (1/0 - enabled/disabled).
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_sync_4khz(struct adis_dev *adis, uint32_t sync_4khz)
{
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->sync_4khz, sync_4khz);
	if (ret)
		return ret;

	adis->int_clk = sync_4khz ? 4000 : 2000;

	no_os_udelay(adis->info->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read external clock scale factor value.
 * @param adis     - The adis device.
 * @param up_scale - The external clock scale factor read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_up_scale(struct adis_dev *adis, uint32_t *up_scale)
{
	return adis_read_field_u32(adis, adis->info->field_map->up_scale, up_scale);
}

/**
 * @brief Write external clock scale factor value.
 * @param adis     - The adis device.
 * @param up_scale - The external clock scale factor value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_up_scale(struct adis_dev *adis, uint32_t up_scale)
{
	uint32_t sync_mode;
	int ret;

	ret = adis_read_sync_mode(adis, &sync_mode);
	if (ret)
		return ret;

	/*
	 * Allow for any value to be written unless the device is in SYNC_SCALED
	 * synchronization mode.
	 * If the device is in SYNC_SCALED syncronization mode, make sure the
	 * result for clk_freq * up_scale is between sampling clock limits,
	 * otherwise return -EINVAL.
	 */
	if (sync_mode == ADIS_SYNC_SCALED
	    && (adis->ext_clk*up_scale > adis->info->sampling_clk_limits.max_freq
		|| adis->ext_clk*up_scale < adis->info->sampling_clk_limits.min_freq))
		return -EINVAL;

	return adis_write_field_u32(adis, adis->info->field_map->up_scale, up_scale);
}

/**
 * @brief Read decimation rate value.
 * @param adis     - The adis device.
 * @param dec_rate - The decimation rate read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_dec_rate(struct adis_dev *adis, uint32_t *dec_rate)
{
	return adis_read_field_u32(adis, adis->info->field_map->dec_rate, dec_rate);
}

/**
 * @brief Write decimation rate value.
 * @param adis     - The adis device.
 * @param dec_rate - The decimation rate value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_dec_rate(struct adis_dev *adis, uint32_t dec_rate)
{
	int ret;

	if(dec_rate > adis->info->dec_rate_max)
		return -EINVAL;

	ret = adis_write_field_u32(adis, adis->info->field_map->dec_rate, dec_rate);
	if(ret)
		return ret;

	no_os_udelay(adis->info->timeouts->dec_rate_update_us);

	return 0;
}

/**
 * @brief Read time base control value.
 * @param adis           - The adis device.
 * @param bias_corr_tbc - The time base control read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_bias_corr_tbc(struct adis_dev *adis, uint32_t *bias_corr_tbc)
{
	return adis_read_field_u32(adis, adis->info->field_map->bias_corr_tbc,
				   bias_corr_tbc);
}

/**
 * @brief Write time base control value.
 * @param adis           - The adis device.
 * @param bias_corr_tbc - The time base control value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_bias_corr_tbc(struct adis_dev *adis, uint32_t bias_corr_tbc)
{
	if (bias_corr_tbc > adis->info->bias_corr_tbc_max)
		return -EINVAL;

	return adis_write_field_u32(adis, adis->info->field_map->bias_corr_tbc,
				    bias_corr_tbc);
}

/**
 * @brief Read x axis gyroscope bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_xg - The x axis gyroscope bias correction enable bit read
 *                          value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_bias_corr_en_xg(struct adis_dev *adis, uint32_t *bias_corr_en_xg)
{
	return adis_read_field_u32(adis, adis->info->field_map->bias_corr_en_xg,
				   bias_corr_en_xg);
}

/**
 * @brief Write x axis gyroscope bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_xg - The x axis gyroscope bias correction enable bit
 *                          value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_bias_corr_en_xg(struct adis_dev *adis, uint32_t bias_corr_en_xg)
{
	return adis_write_field_u32(adis, adis->info->field_map->bias_corr_en_xg,
				    bias_corr_en_xg);
}

/**
 * @brief Read y axis gyroscope bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_yg - The y axis gyroscope bias correction enable bit read
 *                          value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_bias_corr_en_yg(struct adis_dev *adis, uint32_t *bias_corr_en_yg)
{
	return adis_read_field_u32(adis, adis->info->field_map->bias_corr_en_yg,
				   bias_corr_en_yg);
}

/**
 * @brief Write y axis gyroscope bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_yg - The y axis gyroscope bias correction enable bit
 *                          value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_bias_corr_en_yg(struct adis_dev *adis, uint32_t bias_corr_en_yg)
{
	return adis_write_field_u32(adis, adis->info->field_map->bias_corr_en_yg,
				    bias_corr_en_yg);
}

/**
 * @brief Read z axis gyroscope bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_zg - The z axis gyroscope bias correction enable bit read
 *                          value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_bias_corr_en_zg(struct adis_dev *adis, uint32_t *bias_corr_en_zg)
{
	return adis_read_field_u32(adis, adis->info->field_map->bias_corr_en_zg,
				   bias_corr_en_zg);
}

/**
 * @brief Write z axis gyroscope bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_zg - The z axis gyroscope bias correction enable bit
 *                          value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_bias_corr_en_zg(struct adis_dev *adis, uint32_t bias_corr_en_zg)
{
	return adis_write_field_u32(adis, adis->info->field_map->bias_corr_en_zg,
				    bias_corr_en_zg);
}

/**
 * @brief Read x axis accelerometer bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_xa - The x axis accelerometer bias correction enable bit read
 *                          value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_bias_corr_en_xa(struct adis_dev *adis, uint32_t *bias_corr_en_xa)
{
	return adis_read_field_u32(adis, adis->info->field_map->bias_corr_en_xa,
				   bias_corr_en_xa);
}

/**
 * @brief Write x axis accelerometer bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_xa - The x axis accelerometer bias correction enable bit
 *                          value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_bias_corr_en_xa(struct adis_dev *adis, uint32_t bias_corr_en_xa)
{
	return adis_write_field_u32(adis, adis->info->field_map->bias_corr_en_xa,
				    bias_corr_en_xa);
}

/**
 * @brief Read y axis accelerometer bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_ya - The y axis accelerometer bias correction enable bit read
 *                          value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_bias_corr_en_ya(struct adis_dev *adis, uint32_t *bias_corr_en_ya)
{
	return adis_read_field_u32(adis, adis->info->field_map->bias_corr_en_ya,
				   bias_corr_en_ya);
}

/**
 * @brief Write y axis accelerometer bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_ya - The y axis accelerometer bias correction enable bit
 *                          value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_bias_corr_en_ya(struct adis_dev *adis, uint32_t bias_corr_en_ya)
{
	return adis_write_field_u32(adis, adis->info->field_map->bias_corr_en_ya,
				    bias_corr_en_ya);
}

/**
 * @brief Read z axis accelerometer bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_za - The z axis accelerometer bias correction enable bit read
 *                          value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_bias_corr_en_za(struct adis_dev *adis, uint32_t *bias_corr_en_za)
{
	return adis_read_field_u32(adis, adis->info->field_map->bias_corr_en_za,
				   bias_corr_en_za);
}

/**
 * @brief Write z axis accelerometer bias correction enable bit value.
 * @param adis            - The adis device.
 * @param bias_corr_en_za - The z axis accelerometer bias correction enable bit
 *                          value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_bias_corr_en_za(struct adis_dev *adis, uint32_t bias_corr_en_za)
{
	return adis_write_field_u32(adis, adis->info->field_map->bias_corr_en_za,
				    bias_corr_en_za);
}

/**
 * @brief Command: bias correction update
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_bias_corr_update(struct adis_dev *adis)
{
	struct adis_field field = adis->info->field_map->bias_corr_update;
	return adis_write_reg(adis, field.reg_addr, field.field_mask, field.reg_size);
}

/**
 * @brief Command: factory calibration restore
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_fact_calib_restore(struct adis_dev *adis)
{
	struct adis_field field = adis->info->field_map->fact_calib_restore;
	int ret;

	ret = adis_write_reg(adis, field.reg_addr, field.field_mask, field.reg_size);
	if (ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->fact_calib_restore_ms);

	return 0;
}

/**
 * @brief Command: sensor self test
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_snsr_self_test(struct adis_dev *adis)
{
	int ret;
	struct adis_field field = adis->info->field_map->snsr_self_test;

	ret = adis_write_reg(adis, field.reg_addr, field.field_mask, field.reg_size);
	if(ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->self_test_ms);

	return 0;
}

/**
 * @brief Command: flash memory update
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_fls_mem_update(struct adis_dev *adis)
{
	int ret;
	uint32_t flash_cnt;
	struct adis_field field = adis->info->field_map->fls_mem_update;

	ret =  adis_write_reg(adis, field.reg_addr, field.field_mask, field.reg_size);
	if (ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->fls_mem_update_ms);

	/* Make sure flash counter is read after each flash update */
	return adis_read_fls_mem_wr_cntr(adis, &flash_cnt);
}

/**
 * @brief Command: flash memory test
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_fls_mem_test(struct adis_dev *adis)
{
	struct adis_field field = adis->info->field_map->fls_mem_test;
	int ret;

	ret = adis_write_reg(adis, field.reg_addr, field.field_mask, field.reg_size);
	if (ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->fls_mem_test_ms);

	return 0;
}

/**
 * @brief Command: fifo flush
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_fifo_flush(struct adis_dev *adis)
{
	struct adis_field field = adis->info->field_map->fifo_flush;
	return adis_write_reg(adis, field.reg_addr, field.field_mask, field.reg_size);
}

/**
 * @brief Command: software reset.
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_sw_res(struct adis_dev *adis)
{
	int ret;
	struct adis_field field = adis->info->field_map->sw_res;

	ret = adis_write_reg(adis, field.reg_addr, field.field_mask, field.reg_size);
	if(ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->sw_reset_ms);

	return 0;
}

/**
 * @brief Read firmware revision value.
 * @param adis     - The adis device.
 * @param firm_rev - The firmware revision value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_firm_rev(struct adis_dev *adis, uint32_t *firm_rev)
{
	return adis_read_field_u32(adis, adis->info->field_map->firm_rev, firm_rev);
}

/**
 * @brief Read firmware factory configuration day value.
 * @param adis   - The adis device.
 * @param firm_d - The factory configuration day value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_firm_d(struct adis_dev *adis, uint32_t *firm_d)
{
	return adis_read_field_u32(adis, adis->info->field_map->firm_d, firm_d);
}

/**
 * @brief Read firmware factory configuration month value.
 * @param adis   - The adis device.
 * @param firm_m - The factory configuration month value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_firm_m(struct adis_dev *adis, uint32_t *firm_m)
{
	return adis_read_field_u32(adis, adis->info->field_map->firm_m, firm_m);
}

/**
 * @brief Read firmware factory configuration year value.
 * @param adis   - The adis device.
 * @param firm_y - The factory configuration year value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_firm_y(struct adis_dev *adis, uint32_t *firm_y)
{
	return adis_read_field_u32(adis, adis->info->field_map->firm_y, firm_y);
}

/**
 * @brief Read product id value.
 * @param adis    - The adis device.
 * @param prod_id - The product id value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_prod_id(struct adis_dev *adis, uint32_t *prod_id)
{
	return adis_read_field_u32(adis, adis->info->field_map->prod_id, prod_id);
}

/**
 * @brief Read serial number value.
 * @param adis       - The adis device.
 * @param serial_num - The serial number value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_serial_num(struct adis_dev *adis, uint32_t *serial_num)
{
	return adis_read_field_u32(adis, adis->info->field_map->serial_num, serial_num);
}

/**
 * @brief Read user scratch register 1 value.
 * @param adis      - The adis device.
 * @param usr_scr_1 - The user scratch register 1 value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_usr_scr_1(struct adis_dev *adis, uint32_t *usr_scr_1)
{
	return adis_read_field_u32(adis, adis->info->field_map->usr_scr_1, usr_scr_1);
}

/**
 * @brief Write user scratch register 1 value.
 * @param adis      - The adis device.
 * @param usr_scr_1 - The user scratch register 1 value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_usr_scr_1(struct adis_dev *adis, uint32_t usr_scr_1)
{
	return adis_write_field_u32(adis, adis->info->field_map->usr_scr_1, usr_scr_1);
}

/**
 * @brief Read user scratch register 2 value.
 * @param adis      - The adis device.
 * @param usr_scr_2 - The user scratch register 2 value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_usr_scr_2(struct adis_dev *adis, uint32_t *usr_scr_2)
{
	return adis_read_field_u32(adis, adis->info->field_map->usr_scr_2, usr_scr_2);
}

/**
 * @brief Write user scratch register 2 value.
 * @param adis      - The adis device.
 * @param usr_scr_2 - The user scratch register 2 value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_usr_scr_2(struct adis_dev *adis, uint32_t usr_scr_2)
{
	return adis_write_field_u32(adis, adis->info->field_map->usr_scr_2, usr_scr_2);
}

/**
 * @brief Read user scratch register 3 value.
 * @param adis      - The adis device.
 * @param usr_scr_3 - The user scratch register 3 value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_usr_scr_3(struct adis_dev *adis, uint32_t *usr_scr_3)
{
	return adis_read_field_u32(adis, adis->info->field_map->usr_scr_3, usr_scr_3);
}

/**
 * @brief Write user scratch register 3 value.
 * @param adis      - The adis device.
 * @param usr_scr_3 - The user scratch register 3 value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_usr_scr_3(struct adis_dev *adis, uint32_t usr_scr_3)
{
	return adis_write_field_u32(adis, adis->info->field_map->usr_scr_3, usr_scr_3);
}

/**
 * @brief Read flash memory write cycle counter value.
 * @param adis    - The adis device.
 * @param fls_mem_wr_cntr - The flash memory write cycle counter value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_fls_mem_wr_cntr(struct adis_dev *adis, uint32_t *fls_mem_wr_cntr)
{
	int ret;

	ret = adis_read_field_u32(adis, adis->info->field_map->fls_mem_wr_cntr,
				  fls_mem_wr_cntr);
	if (ret)
		return ret;

	if(*fls_mem_wr_cntr > adis->info->fls_mem_wr_cntr_max)
		adis->diag_flags.fls_mem_wr_cnt_exceed = true;

	return 0;
}

/**
 * @brief Read burst data.
 * @param adis                 - The adis device.
 * @param burst_data           - Array filled with read data.
 * @param burst_data_size      - Size of burst_data.
 * @param burst_size_selection - Burst size selection encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_burst_data(struct adis_dev *adis, uint8_t burst_data_size,
			 uint16_t *burst_data, uint8_t burst_size_selection)
{
	int ret;
	uint8_t msg_size;

	msg_size = burst_size_bytes[burst_size_selection];

	if (burst_data_size > (msg_size - ADIS_CHECKSUM_SIZE))
		burst_data_size = msg_size - ADIS_CHECKSUM_SIZE;

	uint8_t buffer[msg_size + ADIS_READ_BURST_DATA_CMD_SIZE];
	buffer[0] = ADIS_READ_BURST_DATA_CMD_MSB;
	buffer[1] = ADIS_READ_BURST_DATA_CMD_LSB;

	ret = no_os_spi_write_and_read(adis->spi_desc, buffer,
				       msg_size + ADIS_READ_BURST_DATA_CMD_SIZE);
	if (ret)
		return ret;

	if (adis->info->burst_request) {
		/* Delay between consecutive reads */
		no_os_udelay(adis->info->read_delay + adis->info->cs_change_delay);

		ret = no_os_spi_write_and_read(adis->spi_desc, buffer,
					       msg_size + ADIS_READ_BURST_DATA_CMD_SIZE);
		if (ret)
			return ret;
	}

	if(!adis_validate_checksum(&buffer[ADIS_READ_BURST_DATA_CMD_SIZE], msg_size)) {
		adis->diag_flags.checksum_err = true;
		return -EINVAL;
	}

	adis->diag_flags.checksum_err = false;

	/* Copy read data to buffer, based on burst_data_size value */
	memcpy(burst_data, &buffer[ADIS_READ_BURST_DATA_CMD_SIZE], burst_data_size);

	/* Update diagnosis flags at each reading */
	adis_update_diag_flags(adis, buffer[ADIS_READ_BURST_DATA_CMD_SIZE]);

	return 0;
}

/**
 * @brief Update external clock frequency.
 * @param adis     - The adis device.
 * @param clk_freq - New external clock frequency in Hz.
 * @return 0 in case of success, error code otherwise.
 */
int adis_update_ext_clk_freq(struct adis_dev *adis, uint32_t clk_freq)
{
	uint32_t sync_mode;
	int ret;
	ret = adis_read_sync_mode(adis, &sync_mode);
	if (ret)
		return ret;

	if (sync_mode != ADIS_SYNC_DEFAULT && sync_mode != ADIS_SYNC_OUTPUT)
		if (clk_freq < adis->info->sync_clk_freq_limits[sync_mode].min_freq
		    || clk_freq > adis->info->sync_clk_freq_limits[sync_mode].max_freq)
			return -EINVAL;

	/*
	 * Allow setting of clock frequency in other modes because it will not
	 * be used.
	 */
	adis->ext_clk = clk_freq;

	return 0;
}

/**
 * @brief Read adis synchronization clock frequency value in Hertz.
 * @param adis     - The adis device.
 * @param clk_freq - The current adis synchronization clock frequency.
 * @return 0 in case of success, error code otherwise.
 */
int adis_get_sync_clk_freq(struct adis_dev *adis, uint32_t *clk_freq)
{
	int ret;
	uint32_t sync_mode;
	ret = adis_read_sync_mode(adis, &sync_mode);
	if (ret)
		return ret;

	if (sync_mode == ADIS_SYNC_DEFAULT || sync_mode == ADIS_SYNC_OUTPUT)
		*clk_freq = adis->int_clk;
	else
		*clk_freq = adis->ext_clk;

	return 0;
}
