/***************************************************************************//**
 *   @file   adis1655x.c
 *   @brief  Implementation of adis1655x.c
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "adis_internals.h"
#include "adis1655x.h"
#include "no_os_units.h"
#include "no_os_delay.h"
#include <string.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS1655X_ID_NO_OFFSET(x) 	((x) - ADIS16550)
#define ADIS1655X_WRITE_REQ		NO_OS_BIT(7)
#define ADIS1655X_READ_REQ		0x00
#define ADIS1655X_CRC32_SEED		0xFFFFFFFF
#define ADIS1655X_STALL_PERIOD_BURST_US	8

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const struct adis_data_field_map_def adis1655x_def = {
	.x_gyro 		= {.reg_addr = 0x12, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_gyro 		= {.reg_addr = 0x14, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_gyro 		= {.reg_addr = 0x16, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_accl 		= {.reg_addr = 0x18, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_accl 		= {.reg_addr = 0x1A, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_accl 		= {.reg_addr = 0x1C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.temp_out 		= {.reg_addr = 0x10, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.temp_flags		= {.reg_addr = 0x11, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.time_stamp 		= {.reg_addr = 0x2A, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.data_cntr 		= {.reg_addr = 0x0C, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.x_deltang 		= {.reg_addr = 0x1E, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltang 		= {.reg_addr = 0x20, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltang 		= {.reg_addr = 0x22, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_deltvel 		= {.reg_addr = 0x24, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltvel 		= {.reg_addr = 0x26, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltvel 		= {.reg_addr = 0x28, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.xg_bias 		= {.reg_addr = 0x40, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.yg_bias 		= {.reg_addr = 0x42, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.zg_bias 		= {.reg_addr = 0x44, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.xa_bias 		= {.reg_addr = 0x46, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.ya_bias 		= {.reg_addr = 0x48, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.za_bias 		= {.reg_addr = 0x4A, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.xg_scale		= {.reg_addr = 0x30, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.yg_scale		= {.reg_addr = 0x32, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.zg_scale		= {.reg_addr = 0x34, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.xa_scale		= {.reg_addr = 0x36, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.ya_scale		= {.reg_addr = 0x38, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.za_scale		= {.reg_addr = 0x3A, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.up_scale 		= {.reg_addr = 0x56, .reg_size = 0x04, .field_mask = 0x0000FFFF},
	.dec_rate 		= {.reg_addr = 0x54, .reg_size = 0x04, .field_mask = 0x00000FFF},
	.proc_rev 		= {.reg_addr = 0x78, .reg_size = 0x02, .field_mask = 0x00000007},
	.firm_rev 		= {.reg_addr = 0x7A, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.firm_d 		= {.reg_addr = 0x7C, .reg_size = 0x02, .field_mask = 0x000000FF},
	.firm_m 		= {.reg_addr = 0x7C, .reg_size = 0x02, .field_mask = 0x0000FF00},
	.firm_y 		= {.reg_addr = 0x7D, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.prod_id 		= {.reg_addr = 0x7E, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.serial_num 		= {.reg_addr = 0x76, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.lot_num 		= {.reg_addr = 0x74, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.usr_scr_1 		= {.reg_addr = 0x60, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.usr_scr_2 		= {.reg_addr = 0x62, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.usr_scr_3 		= {.reg_addr = 0x64, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.usr_scr_4 		= {.reg_addr = 0x66, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.fls_mem_wr_cntr	= {.reg_addr = 0x72, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.pt_of_perc_algnmt 	= {.reg_addr = 0x52, .reg_size = 0x04, .field_mask = NO_OS_BIT(4)},
	.gyro_fir_enable 	= {.reg_addr = 0x52, .reg_size = 0x04, .field_mask = NO_OS_BIT(3)},
	.accl_fir_enable 	= {.reg_addr = 0x52, .reg_size = 0x04, .field_mask = NO_OS_BIT(2)},
	.sync_mode 		= {.reg_addr = 0x52, .reg_size = 0x04, .field_mask = 0x00000003},
	.sw_res 		= {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = NO_OS_BIT(15)},
	.fls_mem_update 	= {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = NO_OS_BIT(3)},
	.fact_calib_restore 	= {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = NO_OS_BIT(2)},
	.snsr_self_test 	= {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = NO_OS_BIT(1)},
	.write_lock	 	= {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = NO_OS_BIT(0)},
	.diag_stat 		= {.reg_addr = 0x0E, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.diag_mem_failure_mask			= NO_OS_BIT(0),
	.diag_config_calib_crc_error_mask	= NO_OS_BIT(1),
	.diag_fls_mem_update_failure_mask 	= NO_OS_BIT(2),
	.diag_overrange_mask			= NO_OS_BIT(3),
	.diag_snsr_failure_mask 		= NO_OS_BIT(4),
	.diag_temp_err_mask			= NO_OS_BIT(5),
	.diag_spi_comm_err_mask 		= NO_OS_BIT(6),
	.diag_data_path_overrun_mask 		= NO_OS_BIT(7),
	.diag_power_supply_failure_mask		= NO_OS_BIT(8),
	.diag_boot_memory_failure_mask		= NO_OS_BIT(9),
	.diag_clk_err_mask			= NO_OS_BIT(10),
	.diag_reg_nvm_err_mask			= NO_OS_BIT(11),
	.diag_wdg_timer_flag_mask		= NO_OS_BIT(15),
	.diag_x_axis_gyro_failure_mask  	= NO_OS_BIT(17) | NO_OS_BIT(16),
	.diag_y_axis_gyro_failure_mask  	= NO_OS_BIT(19) | NO_OS_BIT(18),
	.diag_z_axis_gyro_failure_mask  	= NO_OS_BIT(21) | NO_OS_BIT(20),
	.diag_x_axis_accl_failure_mask  	= NO_OS_BIT(23) | NO_OS_BIT(22),
	.diag_y_axis_accl_failure_mask  	= NO_OS_BIT(25) | NO_OS_BIT(24),
	.diag_z_axis_accl_failure_mask  	= NO_OS_BIT(27) | NO_OS_BIT(26),
	.diag_int_proc_supply_err_mask		= NO_OS_BIT(28),
	.diag_ext_5v_supply_err_mask		= NO_OS_BIT(29),
	.diag_int_snsr_supply_err_mask		= NO_OS_BIT(30),
	.diag_int_reg_err_mask			= NO_OS_BIT(31),
	.coeff_c0		= {.reg_addr = 0x88, .reg_size = 0x02, .field_mask = 0x0000FFFF},
};

static const struct adis_timeout adis1655x_timeouts = {
	.reset_ms 			= 2500,
	.fact_calib_restore_ms		= 1,
	.self_test_ms 			= 21,
	.fls_mem_update_ms		= 110,
	.sw_reset_ms 			= 330,
};

static const struct adis_clk_freq_limit adis1655x_sync_clk_freq_limits[] = {
	[ADIS_SYNC_DEFAULT] = { },
	[ADIS_SYNC_DIRECT] = { 3000, 4500 },
	[ADIS_SYNC_SCALED] = { 1, 128 },
};

static const struct adis_clk_freq_limit adis1655x_sampling_clk_limits = {
	.min_freq = 3000,
	.max_freq = 4500,
};

/* Values from datasheet for 32-bit data */
static const struct adis_scale_members adis1655x_anglvel_scale[] = {
	[ADIS1655X_ID_NO_OFFSET(ADIS16550)] = {1, RAD_TO_DEGREE(80 << 16)},
};

static const struct adis_scale_members adis1655x_accl_scale[] = {
	[ADIS1655X_ID_NO_OFFSET(ADIS16550)] = {1, M_S_2_TO_G(102400000)},
};

static const struct adis_scale_members adis1655x_deltaangl_scale[] = {
	[ADIS1655X_ID_NO_OFFSET(ADIS16550)] = {DEGREE_TO_RAD(720), 31},
};

static const struct adis_scale_members adis1655x_deltavelocity_scale[]
	= {
	[ADIS1655X_ID_NO_OFFSET(ADIS16550)]   = {125, 31},
};

/* Milli-degrees Celsius for temperature */
static const struct adis_scale_members adis1655x_temp_scale[] = {
	[ADIS1655X_ID_NO_OFFSET(ADIS16550)]   = {4, 1},
};

/* Milli-degrees Celsius for temperature */
static const int adis1655x_temp_offset[] = {
	[ADIS1655X_ID_NO_OFFSET(ADIS16550)]   = 6250,
};

/**
 * @brief Read adis device channel scale.
 * @param adis      - The adis device.
 * @param scale_m1  - Scale member 1.
 * @param scale_m2  - Scale member 2.
 * @param chan_type - The channel type to read scale for.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_get_scale(struct adis_dev *adis,
			       uint32_t *scale_m1, uint32_t *scale_m2,
			       enum adis_chan_type chan_type)
{
	switch(chan_type) {
	case ADIS_ACCL_CHAN:
		*scale_m1 = adis1655x_accl_scale[ADIS1655X_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis1655x_accl_scale[ADIS1655X_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_GYRO_CHAN:
		*scale_m1 = adis1655x_anglvel_scale[ADIS1655X_ID_NO_OFFSET(
				adis->dev_id)].scale_m1;
		*scale_m2 = adis1655x_anglvel_scale[ADIS1655X_ID_NO_OFFSET(
				adis->dev_id)].scale_m2;
		return 0;
	case ADIS_TEMP_CHAN:
		*scale_m1 = adis1655x_temp_scale[ADIS1655X_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis1655x_temp_scale[ADIS1655X_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_DELTAANGL_CHAN:
		*scale_m1 = adis1655x_deltaangl_scale[ADIS1655X_ID_NO_OFFSET(
				adis->dev_id)].scale_m1;
		*scale_m2 = adis1655x_deltaangl_scale[ADIS1655X_ID_NO_OFFSET(
				adis->dev_id)].scale_m2;
		return 0;
	case ADIS_DELTAVEL_CHAN:
		*scale_m1 = adis1655x_deltavelocity_scale[ADIS1655X_ID_NO_OFFSET(
					adis->dev_id)].scale_m1;
		*scale_m2 = adis1655x_deltavelocity_scale[ADIS1655X_ID_NO_OFFSET(
					adis->dev_id)].scale_m2;
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read adis device channel offset in integer.
 * @param adis      - The adis device.
 * @param offset    - The read channel offset.
 * @param chan_type - The channel type to read offset for.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_get_offset(struct adis_dev *adis, int *offset,
				enum adis_chan_type chan_type)
{
	switch(chan_type) {
	case ADIS_TEMP_CHAN:
		*offset = adis1655x_temp_offset[ADIS1655X_ID_NO_OFFSET(adis->dev_id)];
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief CRC32 computation algorithm for adis1655x burst access.
 * @param len  - size of buff in bytes.
 * @param buff - data buffer to check.
 * @return computed crc.
 */
static uint32_t adis1655x_crc32_computation(uint32_t len, const uint8_t *buff)
{
	uint32_t crc = ADIS1655X_CRC32_SEED;
	unsigned int j;
	uint32_t poly = 0xEDB88320;

	while (len > 0) {
		crc ^= *buff;

		for (j = 8; j > 0; j--) {
			if (crc & 1)
				crc = crc >> 1 ^ poly;
			else
				crc = crc >> 1;
		}
		buff++;
		len--;
	}

	crc = crc ^ 0xFFFFFFFFU;

	return crc;
}

/**
 * @brief CRC4 computation algorithm for adis1655x register access.
 * @param data - data buffer to check (always 32 bits).
 * @return computed crc.
 */
static uint8_t adis1655x_crc4_computation(uint8_t *data)
{
	uint8_t crc[4] = {0, 1, 0, 1};  // initialize with seed value
	uint8_t invert, result, i;
	uint32_t val = no_os_get_unaligned_be32(data);

	/* Always first 28 MSB bits are checked, MSB to LSB */
	for (i = 31; i > 3; i--) {
		invert = !!(val & NO_OS_BIT(i)) ^ crc[3];
		crc[3] = crc[2];
		crc[2] = crc[1];
		crc[1] = crc[0];
		crc[0] = invert;
	}

	result = crc[0] | crc[1] << 1 | crc[2] << 2 | crc[3] << 3;
	return result;
}

/**
 * @brief Fill read request.
 * @param reg_addr  - Register address to be read.
 * @param tx_buffer - TX buffer containing the read request.
 * @return 0 in case of success, error code otherwise.
 */
static void adis1655x_fill_read_req(uint32_t reg_addr, uint8_t *tx_buff)
{
	uint8_t crc;

	/* bits 0 - 15: 0
	 * bit 16: 0
	 * bits 17 - 23: register address
	 * bit 24: read (0)
	 * bits 25 - 27: 0
	 * bits 28 - 31: crc
	 */
	tx_buff[0] = 0;
	tx_buff[1] = 0;
	tx_buff[2] = reg_addr;
	tx_buff[3] = ADIS1655X_READ_REQ;
	crc = adis1655x_crc4_computation(tx_buff);
	tx_buff[3] |= crc;
}

/**
 * @brief Fill write request.
 * @param reg_addr  - Register address to be written to.
*  @param reg_val   - Register value to be written.
 * @param tx_buffer - TX buffer containing the write request.
 * @return 0 in case of success, error code otherwise.
 */
static void adis1655x_fill_write_req(uint32_t reg_addr, uint16_t reg_val,
				     uint8_t *tx_buff)
{
	uint8_t crc;

	/* bits 0 - 15: register value
	 * bit 16: 0
	 * bits 17 - 23: register address
	 * bit 24: write (1)
	 * bits 25 - 27: 0
	 * bits 28 - 31: crc
	 */
	tx_buff[0] = reg_val >> 8;
	tx_buff[1] = reg_val & 0xFF;
	tx_buff[2] = reg_addr;
	tx_buff[3] = ADIS1655X_WRITE_REQ;
	crc = adis1655x_crc4_computation(tx_buff);
	tx_buff[3] |= crc;
}

/**
 * @brief Process read response in rx buffer.
 * @param reg_addr  - Register address to check.
 * @param rx_buffer - RX buffer to check the data in.
*  @param reg_val   - Read register value.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_process_read_resp(uint32_t reg_addr, uint8_t *rx_buff,
				       uint16_t *reg_val)
{
	uint8_t crc;

	/* Verify CRC */
	crc = adis1655x_crc4_computation(rx_buff);
	if (crc != (rx_buff[3] & 0x0F))
		return -EIO;

	/* Verify the condition of the device */
	if ((rx_buff[3] & NO_OS_BIT(7)) != 0)
		return -EIO;

	/* Verify if data corresponds to the desired register */
	if (reg_addr != rx_buff[2])
		return -EIO;

	*reg_val = no_os_get_unaligned_be16(rx_buff);

	return 0;
}

/**
 * @brief Process write response in rx buffer.
 * @param reg_val  - Register value to check.
 * @param reg_addr - Register address to check.
 * @param rx_buffer - RX buffer to check the data in.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_process_write_resp(uint16_t reg_val, uint8_t reg_addr,
					uint8_t *rx_buff)
{
	if (rx_buff[0] == (reg_val >> 8) && rx_buff[1] == (reg_val & 0xFF)
	    && rx_buff[2] == reg_addr)
		return 0;

	return -EIO;
}

/**
 * @brief Update page number
 * @param adis    - The adis device.
 * @param page_nb - New page number to be set.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_update_page(struct adis_dev *adis, uint8_t page_nb)
{
	uint8_t data[4];
	uint8_t crc;
	int ret;

	/* bits 0 - 15: register value
	 * bit 16: 0
	 * bits 17 - 23: register address
	 * bit 24: read (0) / write (1)
	 * bits 25 - 27: 0
	 * bits 28 - 31: crc
	 */
	data[0] = 0;
	data[1] = page_nb;
	data[2] = ADIS_REG_PAGE_ID;
	data[3] = ADIS1655X_WRITE_REQ;

	crc = adis1655x_crc4_computation(data);
	data[3] |= crc;

	ret = no_os_spi_write_and_read(adis->spi_desc, data, 4);
	if (ret)
		return ret;


	/* bits 0 - 15: register value
	 * bit 16: 0
	 * bits 17 - 23: register address
	 * bit 24: read (0) / write (1)
	 * bits 25 - 27: 0
	 * bits 28 - 31: crc
	 */
	data[0] = 0;
	data[1] = 0;
	data[2] = ADIS_REG_PAGE_ID + 1;
	data[3] = ADIS1655X_WRITE_REQ;

	crc = adis1655x_crc4_computation(data);
	data[3] |= crc;

	ret = no_os_spi_write_and_read(adis->spi_desc, data, 4);
	if (ret)
		return ret;

	adis->current_page = page_nb;

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
static int adis1655x_read_reg(struct adis_dev *adis,  uint32_t reg,
			      uint32_t *val, uint32_t size)
{
	int ret;
	uint32_t page = reg / ADIS_PAGE_SIZE;
	uint16_t reg_val;

	if (adis->current_page != page) {
		/* Call update page API */
		ret = adis1655x_update_page(adis, page);
		if (ret)
			return ret;
	}

	struct no_os_spi_msg msgs[] = {
		{
			.tx_buff = adis->tx,
			.bytes_number = 4,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->read_delay,
		},
		{
			.tx_buff = adis->tx + 4,
			.rx_buff = adis->rx,
			.bytes_number = 4,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->read_delay,
		},
		{
			.tx_buff = adis->tx + 8,
			.rx_buff = adis->rx + 4,
			.bytes_number = 4,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->read_delay,
		},
	};

	struct no_os_spi_msg msgs_to_send[NO_OS_ARRAY_SIZE(msgs)];
	uint32_t send_idx = 0;

	switch (size) {
	case ADIS_4_BYTES_SIZE:
		adis1655x_fill_read_req(reg + 1, &adis->tx[0]);
		msgs_to_send[send_idx] = msgs[0];
		send_idx++;
	/* fallthrough */
	case ADIS_2_BYTES_SIZE:
		adis1655x_fill_read_req(reg, &adis->tx[4]);
		adis1655x_fill_read_req(reg, &adis->tx[8]);
		msgs_to_send[send_idx] = msgs[1];
		msgs_to_send[send_idx + 1] = msgs[2];
		send_idx += 2;
		break;
	default:
		return -EINVAL;
	}

	ret = no_os_spi_transfer(adis->spi_desc, msgs_to_send, send_idx);
	if (ret)
		return ret;

	switch (size) {
	case ADIS_4_BYTES_SIZE:
		ret = adis1655x_process_read_resp(reg + 1, adis->rx, &reg_val);
		if (ret)
			return ret;
		*val = reg_val << 16;

		ret = adis1655x_process_read_resp(reg, adis->rx + 4, &reg_val);
		if (ret)
			return ret;
		*val |= reg_val & 0xFFFF;
		break;
	case ADIS_2_BYTES_SIZE:
		ret = adis1655x_process_read_resp(reg, adis->rx + 4, &reg_val);
		if (ret)
			return ret;
		*val = reg_val;
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
static int adis1655x_write_reg(struct adis_dev *adis, uint32_t reg,
			       uint32_t val, uint32_t size)
{
	int ret;
	uint16_t reg_val;
	uint32_t page = reg / ADIS_PAGE_SIZE;

	/* If device is locked, no writes are allowed, except for software reset. */
	if (adis->is_locked && reg != adis->info->field_map->sw_res.reg_addr)
		return -EPERM;

	if (size != ADIS_4_BYTES_SIZE)
		return -EINVAL;

	if (adis->current_page != page) {
		/* Call update page API */
		ret = adis1655x_update_page(adis, page);
		if (ret)
			return ret;
	}

	struct no_os_spi_msg msgs[] = {
		{
			.tx_buff = adis->tx,
			.bytes_number = 4,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->write_delay,
		},
		{
			.tx_buff = adis->tx + 4,
			.rx_buff = adis->rx,
			.bytes_number = 4,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->write_delay,
		},
		{
			.tx_buff = adis->tx + 8,
			.rx_buff = adis->rx + 4,
			.bytes_number = 4,
			.cs_change = 1,
			.cs_change_delay = adis->info->cs_change_delay,
			.cs_delay_last = adis->info->write_delay,
		},
	};

	reg_val = val >> 16;
	adis1655x_fill_write_req(reg + 1, reg_val, &adis->tx[0]);
	reg_val = val & 0xFFFF;
	adis1655x_fill_write_req(reg, reg_val, &adis->tx[4]);
	adis1655x_fill_read_req(reg, &adis->tx[8]);
	ret = no_os_spi_transfer(adis->spi_desc, msgs, 3);
	if (ret)
		return ret;

	reg_val = val >> 16;
	ret = adis1655x_process_write_resp(reg_val, reg + 1, &adis->rx[0]);
	if (ret)
		return ret;

	reg_val = val & 0xFFFF;
	ret = adis1655x_process_write_resp(reg_val, reg, &adis->rx[4]);
	return 0;
}

/**
 * @brief Read burst command based on burst selection.
 * @param adis                 - The adis device.
 * @param burst_sel	       - 0 if accel and gyro data is requested, 1
 *                               if delta angle and delta velocity is requested.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_send_burst_cmd(struct adis_dev *adis, uint8_t burst_sel)
{
	uint8_t data[4], crc;

	/* bits 0 - 15: 0
	 * bit 16: 0
	 * bits 0xA for burst_sel = 0, 0xB for burst_sel = 1
	 * bit 24: read (0)
	 * bits 25 - 27: 0
	 * bits 28 - 31: crc
	 */
	data[0] = 0;
	data[1] = 0;
	data[2] = burst_sel ? 0xB: 0xA;
	data[3] = ADIS1655X_READ_REQ;
	crc = adis1655x_crc4_computation(data);
	data[3] |= crc;

	return no_os_spi_write_and_read(adis->spi_desc, data, 4);
}

/**
 * @brief Read burst data.
 * @param adis      - The adis device.
 * @param data      - The burst read data structure to be populated.
 * @param burst32   - True if 32-bit data is requested for accel
 *		      and gyro (or delta angle and delta velocity)
 *		      measurements, false if 16-bit data is requested.
 * @param burst_sel - 0 if accel and gyro data is requested, 1
 *		      if delta angle and delta velocity is requested.
 * @param fifo_pop  - In case FIFO is present, will pop the fifo if
 * 		      true. Unused if FIFO is not present.
 * @param crc_check - If true CRC will be checked, if false check will be skipped.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_read_burst_data(struct adis_dev *adis,
				     struct adis_burst_data *data,
				     bool burst32, uint8_t burst_sel, bool fifo_pop, bool crc_check)
{
	int ret;
	uint8_t buffer[48], crc_buffer[40], crc4;
	uint32_t computed_crc32, recv_crc32;

	/* FIFO not present: fifo_pop not used. */

	/* Only 32-bit reads available for this device */
	if (!burst32)
		return -EINVAL;

	ret = adis1655x_send_burst_cmd(adis, burst_sel);
	if (ret)
		return ret;

	no_os_udelay(ADIS1655X_STALL_PERIOD_BURST_US);

	/* First 4 bytes are the header, any valid command can be sent: sending a burst read command. */
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0x0B;
	buffer[3] = 0;
	crc4 = adis1655x_crc4_computation(&buffer[0]);
	buffer[3] |= crc4;


	ret = no_os_spi_write_and_read(adis->spi_desc, buffer, sizeof(buffer));
	if (ret)
		return ret;

	if (crc_check) {
		recv_crc32 = no_os_get_unaligned_be32(&buffer[44]);
		memcpy(crc_buffer, &buffer[4], 40);
		/* The burst CRC value is calculated from burst word 1 through burst word 10. */
		/* The CRC algorithm expects data to be LSB format, swap memory */
		no_os_memswap64(crc_buffer, 40, 4);
		computed_crc32 = adis1655x_crc32_computation(40, crc_buffer);
		if (recv_crc32 != computed_crc32)
			return -EIO;
	}

	/*
	 * The burst data returned in buffer for burst_sel = 0 is:
	 * DATA_CNT, STATUS, TEMP, X_GYRO, Y_GYRO, Z_GYRO, X_ACCL, Y_ACCL, Z_ACCL, TIMESTAMP
	 * The burst data returned in buffer for burst_sel = 1 is:
	 * DATA_CNT, STATUS, TEMP, X_DELTANG, Y_DELTANG, Z_DELTANG, X_DELTVEL, Y_DELTVEL, Z_DELTVEL, TIMESTAMP
	 * The read_burst_data API expects the following format:
	 * burst_sel = 0: STATUS, X_GYRO, Y_GYRO, Z_GYRO, X_ACCL, Y_ACCL, Z_ACCL, TEMP, DATA_CNT
	 * burst_sel = 1: STATUS, X_DELTANG, Y_DELTANG, Z_DELTANG, X_DELTVEL, Y_DELTVEL, Z_DELTVEL, TEMP, DATA_CNT
	 */
	uint8_t axis_data_offset = 16;
	uint8_t temp_offset = 14;
	uint8_t data_cntr_offset = 4;

	memcpy(&data->x_gyro_msb, &buffer[axis_data_offset], 2);
	memcpy(&data->x_gyro_lsb, &buffer[axis_data_offset + 2], 2);
	memcpy(&data->y_gyro_msb, &buffer[axis_data_offset + 4], 2);
	memcpy(&data->y_gyro_lsb, &buffer[axis_data_offset + 6], 2);
	memcpy(&data->z_gyro_msb, &buffer[axis_data_offset + 8], 2);
	memcpy(&data->z_gyro_lsb, &buffer[axis_data_offset + 10], 2);
	memcpy(&data->x_accel_msb, &buffer[axis_data_offset + 12], 2);
	memcpy(&data->x_accel_lsb, &buffer[axis_data_offset + 14], 2);
	memcpy(&data->y_accel_msb, &buffer[axis_data_offset + 16], 2);
	memcpy(&data->y_accel_lsb, &buffer[axis_data_offset + 18], 2);
	memcpy(&data->z_accel_msb, &buffer[axis_data_offset + 20], 2);
	memcpy(&data->z_accel_lsb, &buffer[axis_data_offset + 22], 2);

	data->temp_msb = 0;
	/* Temp data */
	memcpy(&data->temp_lsb, &buffer[temp_offset], 2);
	/* Counter data - aligned */
	data->data_cntr_lsb = no_os_get_unaligned_be16(&buffer[data_cntr_offset + 2]);
	data->data_cntr_msb = no_os_get_unaligned_be16(&buffer[data_cntr_offset]);

	/* Update diag flags */
	uint32_t diag_flags = no_os_get_unaligned_be32(&buffer[8]);
	adis_update_diag_flags(adis, diag_flags);

	/* Update temp flags */
	uint16_t temp_flags = no_os_get_unaligned_be16(&buffer[12]);
	adis_update_temp_flags(adis, temp_flags);

	return 0;
}

/**
 * @brief Read synchronization mode encoded value.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_read_sync_mode(struct adis_dev *adis, uint32_t *sync_mode)
{
	int ret;
	ret = adis_read_field_u32(adis, adis->info->field_map->sync_mode, sync_mode);
	if(ret)
		return ret;

	/*
	 * ADIS16550 does not follow the same logic for sync mode:
	 * SYNC_DIRECT = 1
	 * SYNC_SCALED = 3
	 */
	if(*sync_mode == 3)
		*sync_mode = ADIS_SYNC_SCALED;

	return 0;
}

/**
 * @brief Update synchronization mode.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value to update.
 * @param ext_clk   - The external clock frequency to update, will be ignored
 * if sync_mode is different from ADIS_SYNC_SCALED and ADIS_SYNC_DIRECT.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_write_sync_mode(struct adis_dev *adis, uint32_t sync_mode,
				     uint32_t ext_clk)
{
	int ret;

	if(sync_mode > adis->info->sync_mode_max)
		return -EINVAL;

	if (sync_mode != ADIS_SYNC_DEFAULT) {
		/* Sync pulse is external */
		if (ext_clk < adis->info->sync_clk_freq_limits[sync_mode].min_freq
		    || ext_clk > adis->info->sync_clk_freq_limits[sync_mode].max_freq)
			return -EINVAL;

		adis->ext_clk = ext_clk;

		if (sync_mode == ADIS_SYNC_SCALED) {
			sync_mode = 3;
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
 * @brief Read configured filter frequency.
 * @param adis - The adis device.
 * @param chan - The adis channel.
 * @param axis - Tha adis channel axis.
 * @param freq - The filter frequency, in Hz.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_read_lpf(struct adis_dev *adis, enum adis_chan_type chan,
			      enum adis_axis_type axis, uint32_t *freq)
{
	int ret;
	uint32_t fir_en;

	*freq = 0;

	/* Axis is ignored, because filter is only available per accel/gyro device. */
	switch(chan) {
	case ADIS_ACCL_CHAN:
		ret = adis_read_accl_fir_enable(adis, &fir_en);
		break;
	case ADIS_GYRO_CHAN:
		ret = adis_read_gyro_fir_enable(adis, &fir_en);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	if (fir_en)
		*freq = 100;

	return 0;
}

/**
 * @brief Configure filter for the given filter frequency.
 * @param adis - The adis device.
 * @param chan - The adis channel.
 * @param axis - Tha adis channel axis.
 * @param freq - The filter frequency, in Hz.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1655x_write_lpf(struct adis_dev *adis, enum adis_chan_type chan,
			       enum adis_axis_type axis, uint32_t freq)
{
	uint32_t fir_en = 0;

	if (freq > 0)
		fir_en = 1;

	/* Axis is ignored, because filter is only available per accel/gyro device. */
	switch(chan) {
	case ADIS_ACCL_CHAN:
		return adis_write_accl_fir_enable(adis, fir_en);
	case ADIS_GYRO_CHAN:
		return adis_write_gyro_fir_enable(adis, fir_en);
	default:
		return -EINVAL;
	}
}

const struct adis_chip_info adis1655x_chip_info = {
	.cs_change_delay 	= 5, /* stall period between data */
	.read_delay 		= 0,
	.write_delay 		= 0,
	.timeouts 		= &adis1655x_timeouts,
	.field_map 		= &adis1655x_def,
	.dec_rate_max 		= 4095,
	.sync_mode_max 		= ADIS_SYNC_SCALED,
	.fls_mem_wr_cntr_max 	= 100000,
	.fir_coef_idx_max	= 119,
	.int_clk		= 4000,
	.sync_clk_freq_limits	= adis1655x_sync_clk_freq_limits,
	.sampling_clk_limits	= adis1655x_sampling_clk_limits,
	.flags			= ADIS_HAS_BURST32 | ADIS_HAS_BURST_DELTA_DATA,
	.read_reg		= &adis1655x_read_reg,
	.write_reg		= &adis1655x_write_reg,
	.has_paging		= true,
	.has_lock		= true,
	.read_burst_data	= &adis1655x_read_burst_data,
	.get_scale		= &adis1655x_get_scale,
	.get_offset		= &adis1655x_get_offset,
	.read_sync_mode		= &adis1655x_read_sync_mode,
	.write_sync_mode	= &adis1655x_write_sync_mode,
	.read_lpf		= &adis1655x_read_lpf,
	.write_lpf		= &adis1655x_write_lpf,
};
