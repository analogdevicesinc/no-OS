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
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize adis device.
 * @param adis - The adis device.
 * @param data - Initialization data.
 * @return 0 in case of success, error code otherwise.
 */
int adis_init(struct adis_dev **adis, const struct adis_data *data)
{
	struct adis_dev *dev;
	int ret;

	dev = (struct adis_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, data->adis_ip->spi_init);
	if (ret)
		goto error_spi;

	if(data->has_paging)
		dev->current_page = -1;
	else
		dev->current_page = 0;

	ret = no_os_gpio_get_optional(&dev->gpio_reset, data->adis_ip->gpio_reset);
	if (ret) {
		pr_warning("No reset pin found \n");
	}

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_LOW);
		if (ret)
			goto error;
	}

	dev->data = data;

	if(data->adis_ip->sync_mode > data->sync_mode_max) {
		ret = -EINVAL;
		goto error;
	}

	dev->sync_mode = data->adis_ip->sync_mode;

	if (dev->sync_mode != ADIS_SYNC_DEFAULT && dev->sync_mode != ADIS_SYNC_OUTPUT) {
		/* Sync pulse is external */
		if (data->adis_ip->ext_clk < data->sync_rate_limit[dev->sync_mode].sync_min_rate
		    || data->adis_ip->ext_clk >
		    data->sync_rate_limit[dev->sync_mode].sync_max_rate) {
			ret = -EINVAL;
			goto error;
		}

		dev->ext_clk = data->adis_ip->ext_clk;
		dev->clk_freq = dev->ext_clk;

		if (dev->sync_mode == ADIS_SYNC_SCALED) {
			/*
			 * In sync scaled mode, the IMU sample rate is the clk_freq * sync_scale.
			 * Hence, default the IMU sample rate to the highest multiple of the input
			 * clock lower than the IMU max sample rate. The optimal range is
			 * 1900-2100 sps...
			 */
			ret = adis_write_up_scale(dev, 2100 / dev->clk_freq);
			if (ret)
				goto error;
		}
	} else {
		dev->clk_freq = data->int_clk;
	}

	ret = adis_initial_startup(dev);
	if (ret)
		goto error;

	ret = adis_write_sync_mode(dev, dev->sync_mode);
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
	const struct adis_timeout *timeouts = adis->data->timeouts;
	union adis_diag_flags diag_flags;
	unsigned int prod_id;
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

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	ret = adis_read_prod_id(adis, &prod_id);
	if (ret)
		return ret;

	if (prod_id != adis->data->prod_id)
		pr_warning("Device ID(%u) and product ID(%u) do not match.\n",
			   adis->data->prod_id, prod_id);

	return 0;
}

/**
 * @brief Read N bytes from register.
 * @param adis - The adis device.
 * @param reg  - The address of the lower of the two registers.
 * @param val  - The value read back from the device.
 * @param size - The size of the val buffer.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_reg(struct adis_dev *adis,  unsigned int reg,
		  unsigned int *val, unsigned int size)
{
	int ret;
	unsigned int page = reg / ADIS_PAGE_SIZE;
	struct no_os_spi_msg msgs[] = {
		{
			.tx_buff = adis->tx,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->data->cs_change_delay,
			.cs_delay_last = adis->data->read_delay,
		},
		{
			.tx_buff = adis->tx + 2,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->data->cs_change_delay,
			.cs_delay_last = adis->data->read_delay,
		},
		{
			.tx_buff = adis->tx + 4,
			.rx_buff = adis->rx,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->data->cs_change_delay,
			.cs_delay_last = adis->data->read_delay,
		},
		{
			.rx_buff = adis->rx + 2,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_delay_last = adis->data->read_delay,
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
int adis_write_reg(struct adis_dev *adis, unsigned int reg,
		   unsigned int val, unsigned int size)
{
	int ret;
	unsigned int page = reg / ADIS_PAGE_SIZE, i;
	struct no_os_spi_msg msgs[] = {
		{
			.tx_buff = adis->tx,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->data->cs_change_delay,
			.cs_delay_last = adis->data->write_delay,
		},
		{
			.tx_buff = adis->tx + 2,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->data->cs_change_delay,
			.cs_delay_last = adis->data->write_delay,
		},
		{
			.tx_buff = adis->tx + 4,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_change_delay = adis->data->cs_change_delay,
			.cs_delay_last = adis->data->write_delay,
		},
		{
			.tx_buff = adis->tx + 6,
			.bytes_number = 2,
			.cs_delay_last = adis->data->write_delay,
		},
		{
			.tx_buff = adis->tx + 8,
			.bytes_number = 2,
			.cs_change = 1,
			.cs_delay_last = adis->data->write_delay,
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
 * @brief Update the desired bits of reg in accordance with mask and val.
 * @param adis - The adis device.
 * @param reg  - The address of the lower of the two registers.
 * @param mask - Bitmask to change.
 * @param val  - The value to be written.
 * @param size - Size of the register to update.
 * @return 0 in case of success, error code otherwise.
 */
int adis_update_bits_base(struct adis_dev *adis, unsigned int reg,
			  const unsigned int mask,
			  const unsigned int val, uint8_t size)
{
	int ret;
	unsigned int __val;

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
	const struct adis_data_def *reg_map = adis->data->reg_map;

	adis->diag_flags.adis_diag_flags_bits.DATA_PATH_OVERRUN = diag_stat &
			reg_map->data_path_overrun_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.SPI_COMM_ERR = diag_stat &
			reg_map->spi_comm_err_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.STANDBY_MODE = diag_stat &
			reg_map->standby_mode_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.CLOCK_ERR = diag_stat &
			reg_map->clock_err_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.FLS_MEM_UPDATE_ERR = diag_stat &
			reg_map->fls_mem_update_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.FLS_MEM_TEST_ERR = diag_stat &
			reg_map->fls_mem_test_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.SNSR_SELF_TEST_ERR = diag_stat &
			reg_map->snsr_self_test_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.GYRO1_SELF_TEST_ERR = diag_stat &
			reg_map->gyro1_self_test_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.GYRO2_SELF_TEST_ERR = diag_stat &
			reg_map->gyro2_self_test_mask ? 1 : 0;
	adis->diag_flags.adis_diag_flags_bits.ACCL_SELF_TEST_ERR = diag_stat &
			reg_map->accl_self_test_mask ? 1 : 0;
}

/**
 * @brief Read burst data.
 * @param adis       - The adis device.
 * @param burst_data - Array filled with read data.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_burst_data(struct adis_dev *adis,
			 struct adis_burst_data *burst_data)
{
	uint8_t msg_size;
	int ret;

	if (adis->burst_size == ADIS_16_BIT_BURST_SIZE)
		msg_size = ADIS_MSG_SIZE_16_BIT_BURST;
	else
		msg_size = ADIS_MSG_SIZE_32_BIT_BURST;

	uint8_t buffer[msg_size + ADIS_READ_BURST_DATA_CMD_SIZE];
	buffer[0] = ADIS_READ_BURST_DATA_CMD_MSB;
	buffer[1] = ADIS_READ_BURST_DATA_CMD_LSB;

	ret = no_os_spi_write_and_read(adis->spi_desc, buffer,
				       msg_size + ADIS_READ_BURST_DATA_CMD_SIZE);
	if (ret)
		return ret;

	if(!adis_validate_checksum(&buffer[ADIS_READ_BURST_DATA_CMD_SIZE], msg_size)) {
		adis->diag_flags.adis_diag_flags_bits.CHECKSUM_ERR = true;
		return -EINVAL;
	}

	adis->diag_flags.adis_diag_flags_bits.CHECKSUM_ERR = false;

	if (adis->burst_size == ADIS_16_BIT_BURST_SIZE) {
		burst_data->diag_stat = no_os_get_unaligned_be16(
						&buffer[ADIS_DIAG_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->x_gyro = no_os_sign_extend16(no_os_get_unaligned_be16(
					     &buffer[ADIS_XGYRO_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]),
				     ADIS_SIGN_BIT_POS);
		burst_data->y_gyro = no_os_sign_extend16(no_os_get_unaligned_be16(
					     &buffer[ADIS_YGYRO_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]),
				     ADIS_SIGN_BIT_POS);
		burst_data->z_gyro = no_os_sign_extend16(no_os_get_unaligned_be16(
					     &buffer[ADIS_ZGYRO_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]),
				     ADIS_SIGN_BIT_POS);
		burst_data->x_accl = no_os_sign_extend16(no_os_get_unaligned_be16(
					     &buffer[ADIS_XACCL_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]),
				     ADIS_SIGN_BIT_POS);
		burst_data->y_accl = no_os_sign_extend16(no_os_get_unaligned_be16(
					     &buffer[ADIS_YACCL_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]),
				     ADIS_SIGN_BIT_POS);
		burst_data->z_accl = no_os_sign_extend16(no_os_get_unaligned_be16(
					     &buffer[ADIS_ZACCL_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]),
				     ADIS_SIGN_BIT_POS);
		burst_data->temp_out = no_os_get_unaligned_be16(
					       &buffer[ADIS_TEMP_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->data_cntr = no_os_get_unaligned_be16(
						&buffer[ADIS_CNT_IDX_16_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
	} else {
		burst_data->diag_stat = no_os_get_unaligned_be16(
						&buffer[ADIS_DIAG_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->x_gyro = no_os_get_unaligned_be32(
					     &buffer[ADIS_XGYRO_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->y_gyro = no_os_get_unaligned_be32(
					     &buffer[ADIS_YGYRO_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->z_gyro =no_os_get_unaligned_be32(&buffer[ADIS_ZGYRO_IDX_32_BIT_BURST
				    + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->x_accl = no_os_get_unaligned_be32(
					     &buffer[ADIS_XACCL_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->y_accl = no_os_get_unaligned_be32(
					     &buffer[ADIS_YACCL_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->z_accl = no_os_get_unaligned_be32(
					     &buffer[ADIS_ZACCL_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->temp_out = no_os_get_unaligned_be16(
					       &buffer[ADIS_TEMP_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
		burst_data->data_cntr = no_os_get_unaligned_be16(
						&buffer[ADIS_CNT_IDX_32_BIT_BURST + ADIS_READ_BURST_DATA_CMD_SIZE]);
	}

	adis_update_diag_flags(adis, burst_data->diag_stat);

	return 0;
}

/**
 * @brief Read diag status register and update device diag flags.
 * @param adis       - The adis device.
 * @param diag_flags - The read diag flags.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_stat(struct adis_dev *adis,
			union adis_diag_flags *diag_flags)
{
	struct adis_reg reg = adis->data->reg_map->diag_stat;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if (ret)
		return ret;

	adis_update_diag_flags(adis, reg_val);
	*diag_flags = adis->diag_flags;

	return 0;
}

/**
 * @brief Diagnosis: read checksum error flag value.
 * @param adis         - The adis device.
 * @param checksum_err - Checksum error flag value.
 */
void adis_read_diag_checksum_err(struct adis_dev *adis,
				 unsigned int *checksum_err)
{
	*checksum_err = adis->diag_flags.adis_diag_flags_bits.CHECKSUM_ERR;
}

/**
 * @brief Diagnosis: read flash memory write counts exceeded flag value.
 * @param adis                  - The adis device.
 * @param fls_mem_wr_cnt_exceed - Flash memory write counts exceeded flag value.
 */
void adis_read_diag_fls_mem_wr_cnt_exceed(struct adis_dev *adis,
		unsigned int *fls_mem_wr_cnt_exceed)
{
	*fls_mem_wr_cnt_exceed =
		adis->diag_flags.adis_diag_flags_bits.FLS_MEM_WR_CNT_EXCEED;
}

/**
 * @brief Diagnosis: read accelerometer self test error flag value.
 * @param adis               - The adis device.
 * @param accl_self_test_err - Accelerometer self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_accl_self_test_err(struct adis_dev *adis,
				      unsigned int *accl_self_test_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*accl_self_test_err = diag_flags.adis_diag_flags_bits.ACCL_SELF_TEST_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read gyroscope2 self test error flag value.
 * @param adis                - The adis device.
 * @param gyro2_self_test_err - Gyroscope2 self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_gyro2_self_test_err(struct adis_dev *adis,
				       unsigned int *gyro2_self_test_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*gyro2_self_test_err = diag_flags.adis_diag_flags_bits.GYRO2_SELF_TEST_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read gyroscope1 self test error flag value.
 * @param adis                - The adis device.
 * @param gyro1_self_test_err - Gyroscope1 self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_gyro1_self_test_err(struct adis_dev *adis,
				       unsigned int *gyro1_self_test_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*gyro1_self_test_err = diag_flags.adis_diag_flags_bits.GYRO1_SELF_TEST_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read clock error flag value.
 * @param adis      - The adis device.
 * @param clock_err - Clock error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_clock_err(struct adis_dev *adis, unsigned int *clock_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*clock_err = diag_flags.adis_diag_flags_bits.CLOCK_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read flash memory test error flag value.
 * @param adis             - The adis device.
 * @param fls_mem_test_err - Flash memory test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_fls_mem_test_err(struct adis_dev *adis,
				    unsigned int *fls_mem_test_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*fls_mem_test_err = diag_flags.adis_diag_flags_bits.FLS_MEM_TEST_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read sensor self test error flag value.
 * @param adis               - The adis device.
 * @param snsr_self_test_err - Sensor self test error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_snsr_self_test_err(struct adis_dev *adis,
				      unsigned int *snsr_self_test_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*snsr_self_test_err = diag_flags.adis_diag_flags_bits.SNSR_SELF_TEST_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read standby mode flag value.
 * @param adis         - The adis device.
 * @param standby_mode - Standby mode flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_standby_mode(struct adis_dev *adis,
				unsigned int *standby_mode)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*standby_mode = diag_flags.adis_diag_flags_bits.STANDBY_MODE;

	return 0;
}

/**
 * @brief Diagnosis: read spi communication error flag value.
 * @param adis         - The adis device.
 * @param spi_comm_err - Spi communication error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_spi_comm_err(struct adis_dev *adis,
				unsigned int *spi_comm_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*spi_comm_err = diag_flags.adis_diag_flags_bits.SPI_COMM_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read flash memory update error flag value.
 * @param adis               - The adis device.
 * @param fls_mem_update_err - Flash memory update error flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_fls_mem_update_err(struct adis_dev *adis,
				      unsigned int *fls_mem_update_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*fls_mem_update_err = diag_flags.adis_diag_flags_bits.FLS_MEM_UPDATE_ERR;

	return 0;
}

/**
 * @brief Diagnosis: read data path overrun flag value.
 * @param adis                  - The adis device.
 * @param data_path_overrun_err - Data path overrun flag value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_diag_data_path_overrun(struct adis_dev *adis,
				     unsigned int *data_path_overrun_err)
{
	union adis_diag_flags diag_flags;
	int ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	*data_path_overrun_err = diag_flags.adis_diag_flags_bits.DATA_PATH_OVERRUN;

	return 0;
}

/**
 * @brief Read raw gyroscope data on x axis.
 * @param adis   - The adis device.
 * @param x_gyro - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_gyro(struct adis_dev *adis, int *x_gyro)
{
	struct adis_reg reg = adis->data->reg_map->x_gyro;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*x_gyro = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw gyroscope data on y axis.
 * @param adis   - The adis device.
 * @param y_gyro - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_gyro(struct adis_dev *adis, int *y_gyro)
{
	struct adis_reg reg = adis->data->reg_map->y_gyro;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*y_gyro = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw gyroscope data on z axis.
 * @param adis   - The adis device.
 * @param z_gyro - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_gyro(struct adis_dev *adis, int *z_gyro)
{
	struct adis_reg reg = adis->data->reg_map->z_gyro;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*z_gyro = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw acceleration data on x axis.
 * @param adis   - The adis device.
 * @param x_accl - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_accl(struct adis_dev *adis, int *x_accl)
{
	struct adis_reg reg = adis->data->reg_map->x_accl;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*x_accl = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw acceleration data on y axis.
 * @param adis   - The adis device.
 * @param y_accl - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_accl(struct adis_dev *adis, int *y_accl)
{
	struct adis_reg reg = adis->data->reg_map->y_accl;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*y_accl = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw acceleration data on z axis.
 * @param adis   - The adis device.
 * @param z_accl - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_accl(struct adis_dev *adis, int *z_accl)
{
	struct adis_reg reg = adis->data->reg_map->z_accl;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*z_accl = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw temperature data.
 * @param adis     - The adis device.
 * @param temp_out - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_temp_out(struct adis_dev *adis, int *temp_out)
{
	struct adis_reg reg = adis->data->reg_map->temp_out;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*temp_out = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw time stamp data.
 * @param adis       - The adis device.
 * @param time_stamp - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_time_stamp(struct adis_dev *adis, unsigned int *time_stamp)
{
	struct adis_reg reg = adis->data->reg_map->time_stamp;
	return adis_read_reg(adis, reg.addr, time_stamp, reg.size);
}

/**
 * @brief Read data counter value.
 * @param adis      - The adis device.
 * @param data_cntr - The read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_data_cntr(struct adis_dev *adis, unsigned int *data_cntr)
{
	struct adis_reg reg = adis->data->reg_map->data_cntr;
	return adis_read_reg(adis, reg.addr, data_cntr, reg.size);
}

/**
 * @brief Read raw delta angle data on x axis.
 * @param adis      - The adis device.
 * @param x_deltang - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_deltang(struct adis_dev *adis, int *x_deltang)
{
	struct adis_reg reg = adis->data->reg_map->x_deltang;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*x_deltang = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw delta angle data on y axis.
 * @param adis      - The adis device.
 * @param y_deltang - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_deltang(struct adis_dev *adis, int *y_deltang)
{
	struct adis_reg reg = adis->data->reg_map->y_deltang;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*y_deltang = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw delta angle data on z axis.
 * @param adis      - The adis device.
 * @param z_deltang - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_deltang(struct adis_dev *adis, int *z_deltang)
{
	struct adis_reg reg = adis->data->reg_map->z_deltang;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*z_deltang = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw delta velocity data on x axis.
 * @param adis      - The adis device.
 * @param x_deltvel - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_x_deltvel(struct adis_dev *adis, int *x_deltvel)
{
	struct adis_reg reg = adis->data->reg_map->x_deltvel;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*x_deltvel = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw delta velocity data on y axis.
 * @param adis      - The adis device.
 * @param y_deltvel - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_y_deltvel(struct adis_dev *adis, int *y_deltvel)
{
	struct adis_reg reg = adis->data->reg_map->y_deltvel;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*y_deltvel = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw delta velocity data on z axis.
 * @param adis      - The adis device.
 * @param z_deltvel - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_z_deltvel(struct adis_dev *adis, int *z_deltvel)
{
	struct adis_reg reg = adis->data->reg_map->z_deltvel;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*z_deltvel = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Read raw gyroscope offset correction on x axis.
 * @param adis    - The adis device.
 * @param xg_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_xg_bias(struct adis_dev *adis, int *xg_bias)
{
	struct adis_reg reg = adis->data->reg_map->xg_bias;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*xg_bias = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Write raw gyroscope offset correction on x axis.
 * @param adis    - The adis device.
 * @param xg_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_xg_bias(struct adis_dev *adis, int xg_bias)
{
	struct adis_reg reg = adis->data->reg_map->xg_bias;
	return adis_write_reg(adis, reg.addr, xg_bias, reg.size);
}

/**
 * @brief Read raw gyroscope offset correction on y axis.
 * @param adis    - The adis device.
 * @param yg_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_yg_bias(struct adis_dev *adis, int *yg_bias)
{
	struct adis_reg reg = adis->data->reg_map->yg_bias;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*yg_bias = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Write raw gyroscope offset correction on y axis.
 * @param adis    - The adis device.
 * @param yg_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_yg_bias(struct adis_dev *adis, int yg_bias)
{
	struct adis_reg reg = adis->data->reg_map->yg_bias;
	return adis_write_reg(adis, reg.addr, yg_bias, reg.size);
}

/**
 * @brief Read raw gyroscope offset correction on z axis.
 * @param adis    - The adis device.
 * @param zg_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_zg_bias(struct adis_dev *adis, int *zg_bias)
{
	struct adis_reg reg = adis->data->reg_map->zg_bias;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*zg_bias = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Write raw gyroscope offset correction on z axis.
 * @param adis    - The adis device.
 * @param zg_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_zg_bias(struct adis_dev *adis, int zg_bias)
{
	struct adis_reg reg = adis->data->reg_map->zg_bias;
	return adis_write_reg(adis, reg.addr, zg_bias, reg.size);
}

/**
 * @brief Read raw acceleration offset correction on x axis.
 * @param adis    - The adis device.
 * @param xa_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_xa_bias(struct adis_dev *adis, int *xa_bias)
{
	struct adis_reg reg = adis->data->reg_map->xa_bias;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*xa_bias = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Write raw acceleration offset correction on x axis.
 * @param adis    - The adis device.
 * @param xa_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_xa_bias(struct adis_dev *adis, int xa_bias)
{
	struct adis_reg reg = adis->data->reg_map->xa_bias;
	return adis_write_reg(adis, reg.addr, xa_bias, reg.size);
}

/**
 * @brief Read raw acceleration offset correction on y axis.
 * @param adis    - The adis device.
 * @param ya_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_ya_bias(struct adis_dev *adis, int *ya_bias)
{
	struct adis_reg reg = adis->data->reg_map->ya_bias;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*ya_bias = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Write raw acceleration offset correction on y axis.
 * @param adis    - The adis device.
 * @param ya_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_ya_bias(struct adis_dev *adis, int ya_bias)
{
	struct adis_reg reg = adis->data->reg_map->ya_bias;
	return adis_write_reg(adis, reg.addr, ya_bias, reg.size);
}

/**
 * @brief Read raw acceleration offset correction on z axis.
 * @param adis    - The adis device.
 * @param za_bias - The raw read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_za_bias(struct adis_dev *adis, int *za_bias)
{
	struct adis_reg reg = adis->data->reg_map->za_bias;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*za_bias = no_os_sign_extend32(reg_val, reg.size * 8 - 1);

	return 0;
}

/**
 * @brief Write raw acceleration offset correction on z axis.
 * @param adis    - The adis device.
 * @param za_bias - The raw value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_za_bias(struct adis_dev *adis, int za_bias)
{
	struct adis_reg reg = adis->data->reg_map->za_bias;
	return adis_write_reg(adis, reg.addr, za_bias, reg.size);
}

/**
 * @brief Read filter size variable B value.
 * @param adis      - The adis device.
 * @param filt_ctrl - The filter size variable B read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_filt_ctrl(struct adis_dev *adis, unsigned int *filt_ctrl)
{
	struct adis_reg reg = adis->data->reg_map->filt_ctrl;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*filt_ctrl = reg_val & reg.mask;

	return 0;
}

/**
 * @brief Write filter size variable B value.
 * @param adis      - The adis device.
 * @param filt_ctrl - The filter size variable B value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_filt_ctrl(struct adis_dev *adis, unsigned int filt_ctrl)
{
	struct adis_reg reg = adis->data->reg_map->filt_ctrl;
	int ret;

	if(filt_ctrl > adis->data->filt_ctrl_max)
		return -EINVAL;

	ret = adis_write_reg(adis, reg.addr, filt_ctrl, reg.size);
	if (ret)
		return ret;

	no_os_udelay(adis->data->timeouts->filter_update_us);

	return 0;
}

/**
 * @brief Read gyroscope measurement range value.
 * @param adis     - The adis device.
 * @param rang_mdl - The gyroscope measurement range value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_rang_mdl(struct adis_dev *adis, unsigned int *rang_mdl)
{
	struct adis_reg reg = adis->data->reg_map->rang_mdl;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if (ret)
		return ret;

	*rang_mdl = no_os_field_get(reg.mask, reg_val);

	return 0;
}

/**
 * @brief Read burst size encoded value.
 * @param adis       - The adis device.
 * @param burst_size - The burst size encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_burst_size(struct adis_dev *adis, unsigned int *burst_size)
{
	struct adis_reg reg = adis->data->reg_map->burst_size;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*burst_size = reg_val & reg.mask ? 1 : 0;
	adis->burst_size = *burst_size;

	return 0;
}

/**
 * @brief Write burst size value.
 * @param adis       - The adis device.
 * @param burst_size - The burst size encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_burst_size(struct adis_dev *adis, unsigned int burst_size)
{
	struct adis_reg reg = adis->data->reg_map->burst_size;
	int ret;

	if (burst_size > adis->data->burst_size_max)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, burst_size, reg.size);
	if (ret)
		return ret;
	adis->burst_size = burst_size;

	no_os_udelay(adis->data->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read burst selection encoded value.
 * @param adis      - The adis device.
 * @param burst_sel - The burst selection encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_burst_sel(struct adis_dev *adis, unsigned int *burst_sel)
{
	struct adis_reg reg = adis->data->reg_map->burst_sel;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*burst_sel = reg_val & reg.mask ? 1 : 0;
	adis->burst_sel = *burst_sel;

	return 0;
}

/**
 * @brief Write burst selection encoded value.
 * @param adis      - The adis device.
 * @param burst_sel - The burst selection encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_burst_sel(struct adis_dev *adis, unsigned int burst_sel)
{
	struct adis_reg reg = adis->data->reg_map->burst_sel;
	int ret;

	if (burst_sel > adis->data->burst_sel_max)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, burst_sel, reg.size);
	if (ret)
		return ret;

	adis->burst_sel = burst_sel;

	no_os_udelay(adis->data->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read linear acceleration compensation encoded value.
 * @param adis             - The adis device.
 * @param linear_accl_comp - The linear acceleration compensation encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_linear_accl_comp(struct adis_dev *adis,
			       unsigned int *linear_accl_comp)
{
	struct adis_reg reg = adis->data->reg_map->linear_accl_comp;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*linear_accl_comp = reg_val & reg.mask ? 1 : 0;

	return 0;
}

/**
 * @brief Write linear acceleration compensation encoded value.
 * @param adis             - The adis device.
 * @param linear_accl_comp - The linear acceleration compensation encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_linear_accl_comp(struct adis_dev *adis,
				unsigned int linear_accl_comp)
{
	struct adis_reg reg = adis->data->reg_map->linear_accl_comp;
	int ret;

	if (linear_accl_comp > adis->data->linear_accl_comp_max)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, linear_accl_comp,
				    reg.size);
	if (ret)
		return ret;

	no_os_udelay(adis->data->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read point of percussion alignment encoded value.
 * @param adis              - The adis device.
 * @param pt_of_perc_algnmt - The point of percussion alignment encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_pt_of_perc_algnmt(struct adis_dev *adis,
				unsigned int *pt_of_perc_algnmt)
{
	struct adis_reg reg = adis->data->reg_map->pt_of_perc_algnmt;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*pt_of_perc_algnmt = reg_val & reg.mask ? 1 : 0;

	return 0;
}

/**
 * @brief Write point of percussion alignment encoded value.
 * @param adis              - The adis device.
 * @param pt_of_perc_algnmt - The point of percussion alignment encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_pt_of_perc_algnmt(struct adis_dev *adis,
				 unsigned int pt_of_perc_algnmt)
{
	struct adis_reg reg = adis->data->reg_map->pt_of_perc_algnmt;
	int ret;

	if (pt_of_perc_algnmt > adis->data->pt_of_perc_algnmt_max)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, pt_of_perc_algnmt,
				    reg.size);
	if (ret)
		return ret;

	no_os_udelay(adis->data->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read internal sensor bandwidth encoded value.
 * @param adis    - The adis device.
 * @param sens_bw - The internal sensor bandwidth encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_sens_bw(struct adis_dev *adis, unsigned int *sens_bw)
{
	struct adis_reg reg = adis->data->reg_map->sens_bw;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*sens_bw = no_os_field_get(reg.mask, reg_val);

	return 0;
}

/**
 * @brief Write internal sensor bandwidth encoded value.
 * @param adis    - The adis device.
 * @param sens_bw - The internal sensor bandwidth encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_sens_bw(struct adis_dev *adis, unsigned int sens_bw)
{
	struct adis_reg reg = adis->data->reg_map->sens_bw;
	int ret;

	if (sens_bw > adis->data->sens_bw_max)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, sens_bw, reg.size);
	if (ret)
		return ret;

	no_os_mdelay(adis->data->timeouts->sens_bw_update_ms);

	return 0;
}

/**
 * @brief Read synchronization mode encoded value.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_sync_mode(struct adis_dev *adis, unsigned int *sync_mode)
{
	struct adis_reg reg = adis->data->reg_map->sync_mode;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*sync_mode = no_os_field_get(reg.mask, reg_val);

	return 0;
}

/**
 * @brief Write synchronization mode encoded value.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_sync_mode(struct adis_dev *adis, unsigned int sync_mode)
{
	struct adis_reg reg = adis->data->reg_map->sync_mode;
	int ret;

	if (sync_mode > adis->data->sync_mode_max)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, sync_mode, reg.size);
	if (ret)
		return ret;

	adis->sync_mode = sync_mode;

	/* Update clock frequency is external clock is used */
	if(sync_mode == ADIS_SYNC_DIRECT || sync_mode == ADIS_SYNC_SCALED)
		adis->clk_freq = adis->ext_clk;

	return 0;
}

/**
 * @brief Read sync polarity encoded value.
 * @param adis          - The adis device.
 * @param sync_polarity - The sync polarity encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_sync_polarity(struct adis_dev *adis,
			    unsigned int *sync_polarity)
{
	struct adis_reg reg = adis->data->reg_map->sync_polarity;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*sync_polarity = reg_val & reg.mask ? 1 : 0;

	return 0;
}

/**
 * @brief Write sync polarity encoded value.
 * @param adis          - The adis device.
 * @param sync_polarity - The sync polarity encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_sync_polarity(struct adis_dev *adis,
			     unsigned int sync_polarity)
{
	struct adis_reg reg = adis->data->reg_map->sync_polarity;
	int ret;

	if (sync_polarity > 1)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, sync_polarity, reg.size);
	if (ret)
		return ret;

	no_os_udelay(adis->data->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read data ready polarity encoded value.
 * @param adis        - The adis device.
 * @param dr_polarity - The data ready polarity encoded value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_dr_polarity(struct adis_dev *adis,
			  unsigned int *dr_polarity)
{
	struct adis_reg reg = adis->data->reg_map->dr_polarity;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*dr_polarity = reg_val & reg.mask ? 1 : 0;

	return 0;
}

/**
 * @brief Write data ready polarity encoded value.
 * @param adis        - The adis device.
 * @param dr_polarity - The data ready polarity encoded value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_dr_polarity(struct adis_dev *adis,
			   unsigned int dr_polarity)
{
	struct adis_reg reg = adis->data->reg_map->dr_polarity;
	int ret;

	if (dr_polarity > 1)
		return -EINVAL;

	ret = adis_update_bits_base(adis, reg.addr, reg.mask, dr_polarity, reg.size);
	if (ret)
		return ret;

	no_os_udelay(adis->data->timeouts->msc_reg_update_us);

	return 0;
}

/**
 * @brief Read external clock scale factor value.
 * @param adis     - The adis device.
 * @param up_scale - The external clock scale factor read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_up_scale(struct adis_dev *adis, unsigned int *up_scale)
{
	struct adis_reg reg = adis->data->reg_map->up_scale;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*up_scale = reg_val & reg.mask;

	return 0;
}

/**
 * @brief Write external clock scale factor value.
 * @param adis     - The adis device.
 * @param up_scale - The external clock scale factor value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_up_scale(struct adis_dev *adis, unsigned int up_scale)
{
	struct adis_reg reg = adis->data->reg_map->up_scale;
	return adis_write_reg(adis, reg.addr, up_scale, reg.size);
}

/**
 * @brief Read decimation rate value.
 * @param adis     - The adis device.
 * @param dec_rate - The decimation rate read value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_dec_rate(struct adis_dev *adis, unsigned int *dec_rate)
{
	struct adis_reg reg = adis->data->reg_map->dec_rate;
	unsigned int reg_val;
	int ret;

	ret = adis_read_reg(adis, reg.addr, &reg_val, reg.size);
	if(ret)
		return ret;

	*dec_rate = reg_val & reg.mask;

	return 0;
}

/**
 * @brief Write decimation rate value.
 * @param adis     - The adis device.
 * @param dec_rate - The decimation rate value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_dec_rate(struct adis_dev *adis, unsigned int dec_rate)
{
	struct adis_reg reg = adis->data->reg_map->dec_rate;
	int ret;

	if(dec_rate > adis->data->dec_rate_max)
		return -EINVAL;

	ret = adis_write_reg(adis, reg.addr, dec_rate, reg.size);
	if(ret)
		return ret;

	no_os_udelay(adis->data->timeouts->filter_update_us);

	return 0;
}

/**
 * @brief Command: software reset
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_sw_res(struct adis_dev *adis)
{
	int ret;
	struct adis_reg reg = adis->data->reg_map->sw_res;

	ret = adis_write_reg(adis, reg.addr, reg.mask, reg.size);
	if(ret)
		return ret;

	no_os_mdelay(adis->data->timeouts->sw_reset_ms);

	return 0;
}

/**
 * @brief Command: flash memory test
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_fls_mem_test(struct adis_dev *adis)
{
	struct adis_reg reg = adis->data->reg_map->fls_mem_test;
	return adis_write_reg(adis, reg.addr, reg.mask, reg.size);
}

/**
 * @brief Command: flash memory update
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_fls_mem_update(struct adis_dev *adis)
{
	int ret;
	unsigned int flash_cnt;
	struct adis_reg reg = adis->data->reg_map->fls_mem_update;

	ret =  adis_write_reg(adis, reg.addr, reg.mask, reg.size);
	if (ret)
		return ret;

	/* Make sure flash counter is read after each flash update */
	return adis_read_flshcnt(adis, &flash_cnt);
}

/**
 * @brief Command: sensor self test
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_snsr_self_test(struct adis_dev *adis)
{
	int ret;
	struct adis_reg reg = adis->data->reg_map->snsr_self_test;

	ret = adis_write_reg(adis, reg.addr, reg.mask, reg.size);
	if(ret)
		return ret;

	no_os_mdelay(adis->data->timeouts->self_test_ms);

	return 0;
}

/**
 * @brief Command: factory calibration restore
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
int adis_cmd_fact_calib_restore(struct adis_dev *adis)
{
	struct adis_reg reg = adis->data->reg_map->fact_calib_restore;
	return adis_write_reg(adis, reg.addr, reg.mask, reg.size);
}

/**
 * @brief Read firmware revision value.
 * @param adis     - The adis device.
 * @param firm_rev - The firmware revision value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_firm_rev(struct adis_dev *adis, unsigned int *firm_rev)
{
	struct adis_reg reg = adis->data->reg_map->firm_rev;
	return adis_read_reg(adis, reg.addr, firm_rev, reg.size);
}

/**
 * @brief Read firmware factory configuration month and day value.
 * @param adis    - The adis device.
 * @param firm_dm - The factory configuration month and day value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_firm_dm(struct adis_dev *adis, unsigned int *firm_dm)
{
	struct adis_reg reg = adis->data->reg_map->firm_dm;
	return adis_read_reg(adis, reg.addr, firm_dm, reg.size);
}

/**
 * @brief Read firmware factory configuration year value.
 * @param adis   - The adis device.
 * @param firm_y - The factory configuration year value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_firm_y(struct adis_dev *adis, unsigned int *firm_y)
{
	struct adis_reg reg = adis->data->reg_map->firm_y;
	return adis_read_reg(adis, reg.addr, firm_y, reg.size);
}

/**
 * @brief Read product id value.
 * @param adis   - The adis device.
 * @param prod_id - The product id value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_prod_id(struct adis_dev *adis, unsigned int *prod_id)
{
	struct adis_reg reg = adis->data->reg_map->prod_id;
	return adis_read_reg(adis, reg.addr, prod_id, reg.size);
}

/**
 * @brief Read serial number value.
 * @param adis   - The adis device.
 * @param serial_num - The serial number value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_serial_num(struct adis_dev *adis, unsigned int *serial_num)
{
	struct adis_reg reg = adis->data->reg_map->serial_num;
	return adis_read_reg(adis, reg.addr, serial_num, reg.size);
}

/**
 * @brief Read user scratch register 1 value.
 * @param adis      - The adis device.
 * @param usr_scr_1 - The user scratch register 1 value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_usr_scr_1(struct adis_dev *adis, unsigned int *usr_scr_1)
{
	struct adis_reg reg = adis->data->reg_map->usr_scr_1;
	return adis_read_reg(adis, reg.addr, usr_scr_1, reg.size);
}

/**
 * @brief Write user scratch register 1 value.
 * @param adis      - The adis device.
 * @param usr_scr_1 - The user scratch register 1 value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_usr_scr_1(struct adis_dev *adis, unsigned int usr_scr_1)
{
	struct adis_reg reg = adis->data->reg_map->usr_scr_1;
	return adis_write_reg(adis, reg.addr, usr_scr_1, reg.size);
}

/**
 * @brief Read user scratch register 2 value.
 * @param adis      - The adis device.
 * @param usr_scr_2 - The user scratch register 2 value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_usr_scr_2(struct adis_dev *adis, unsigned int *usr_scr_2)
{
	struct adis_reg reg = adis->data->reg_map->usr_scr_2;
	return adis_read_reg(adis, reg.addr, usr_scr_2, reg.size);
}

/**
 * @brief Write user scratch register 2 value.
 * @param adis      - The adis device.
 * @param usr_scr_2 - The user scratch register 2 value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_usr_scr_2(struct adis_dev *adis, unsigned int usr_scr_2)
{
	struct adis_reg reg = adis->data->reg_map->usr_scr_2;
	return adis_write_reg(adis, reg.addr, usr_scr_2, reg.size);
}

/**
 * @brief Read user scratch register 3 value.
 * @param adis      - The adis device.
 * @param usr_scr_3 - The user scratch register 3 value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_usr_scr_3(struct adis_dev *adis, unsigned int *usr_scr_3)
{
	struct adis_reg reg = adis->data->reg_map->usr_scr_3;
	return adis_read_reg(adis, reg.addr, usr_scr_3, reg.size);
}

/**
 * @brief Write user scratch register 3 value.
 * @param adis      - The adis device.
 * @param usr_scr_3 - The user scratch register 3 value to write.
 * @return 0 in case of success, error code otherwise.
 */
int adis_write_usr_scr_3(struct adis_dev *adis, unsigned int usr_scr_3)
{
	struct adis_reg reg = adis->data->reg_map->usr_scr_3;
	return adis_write_reg(adis, reg.addr, usr_scr_3, reg.size);
}

/**
 * @brief Read flash memory write cycle counter value.
 * @param adis    - The adis device.
 * @param flshcnt - The flash memory write cycle counter value.
 * @return 0 in case of success, error code otherwise.
 */
int adis_read_flshcnt(struct adis_dev *adis, unsigned int *flshcnt)
{
	int ret;
	struct adis_reg reg = adis->data->reg_map->flshcnt;

	ret = adis_read_reg(adis, reg.addr, flshcnt, reg.size);
	if (ret)
		return ret;

	if(*flshcnt > adis->data->flshcnt_max)
		adis->diag_flags.adis_diag_flags_bits.FLS_MEM_WR_CNT_EXCEED = true;

	return 0;
}
