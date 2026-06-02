/*******************************************************************************
 *   @file   afe_config.c
 *   @brief  Analog frontend functionalities
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#include "afe_config.h"
#include "pqlib_example.h"

struct no_os_spi_desc *hSPI;
struct no_os_spi_msg spiMsg;
extern struct no_os_gpio_init_param reset_gpio_ip;

/**
 * @brief Initialize LCD display
 * @return int 0 in case of success, negative error code otherwise
 */
int init_lcd(void)
{
	int status;
	char buff[50];
	struct no_os_gpio_sec *spi_cs;
	struct nhd_c12832a1z_dev *nhd_c12832a1z_device;

	struct max_gpio_init_param gpio_extra_ip_cs = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct no_os_gpio_init_param gpio_dc_ip_cs = {
		.port = 2,
		.number = 26,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip_cs,
	};

	status = no_os_gpio_get(&spi_cs, &gpio_dc_ip_cs);
	if (status)
		return status;

	struct max_gpio_init_param gpio_extra_ip = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct no_os_gpio_init_param gpio_dc_ip = {
		.port = 3,
		.number = 5,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip,
	};

	struct no_os_gpio_init_param gpio_lcd_rst_ip = {
		.port = 3,
		.number = 4,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip,
	};

	struct max_spi_init_param spi_extra_ip = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	};

	struct no_os_spi_init_param spi_lcd_ip = {
		.device_id = 0,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &spi_extra_ip,
	};

	struct nhd_c12832a1z_init_param nhd_c12832a1z_ip = {
		.spi_ip = &spi_lcd_ip,
		.dc_pin_ip = &gpio_dc_ip,
		.reset_pin_ip = &gpio_lcd_rst_ip
	};

	status = nhd_c12832a1z_init(&nhd_c12832a1z_device, nhd_c12832a1z_ip);
	if (status)
		return status;
	sprintf(buff, "   AD-PQMON-SL     Eval Board");
	status = no_os_gpio_set_value(spi_cs, 1);
	if (status)
		return status;
	status = nhd_c12832a1z_print_string(nhd_c12832a1z_device, buff);
	if (status)
		return status;
	status = no_os_gpio_set_value(spi_cs, 0);
	if (status)
		return status;

}

/**
 * @brief Initialize AFE by configuring necessary registers and settings
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_init(void)
{
	int status = 0;
	uint32_t config;

	status = spi_device_init();
	if (status)
		return status;
	status = reset_afe();
	if (status)
		return status;
	if (status == 0) {
		config = 0;
		status = afe_write_16bit_reg(REG_RUN, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_RUN_FAILED;
		}
	}
	config = BITM_CONFIG0_RMS_SRC_SEL | BITM_CONFIG0_PERIOD_AVG_CFG_AV16;
	status = afe_write_32bit_reg(REG_CONFIG0, (uint32_t *)&config);
	if (status != 0) {
		status = SYS_STATUS_AFE_CONFIG0_FAILED;
	} else {
		config = BITM_CONFIG1_BURST_EN;
		config |= BITM_CONFIG1_CF3_CFG;
		status = afe_write_16bit_reg(REG_CONFIG1, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_CONFIG1_FAILED;
		}
	}

	if (status == 0) {
		config = 0xC00;
		status = afe_write_16bit_reg(REG_CONFIG2, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_CONFIG2_FAILED;
		}
	}

	if (status == 0) {
		config = LP_SEL_APERIOD;
		config |= ZX_SEL_ZXVA;
		status = afe_write_16bit_reg(REG_ZX_LP_SEL, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_ZX_LP_SEL_FAILED;
		}
	}

	if (status == 0) {
		status = config_wfb();
		if (status != 0) {
			status = SYS_STATUS_AFE_WFB_CFG_FAILED;
		}
	}

	if (status == 0) {
		status = config_afe_irq();
		if (status != 0) {
			status = SYS_STATUS_AFE_MASK0_FAILED;
		}
	}

	if (status == 0) {
		config = 1;
		status = afe_write_16bit_reg(REG_RUN, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_RUN_FAILED;
		}
	}

	return status;
}

/**
 * @brief Start AFE by enabling clearing status and setting RUN register
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_start(void)
{
	int status = SYS_STATUS_SUCCESS;
	uint32_t status0 = BITM_STATUS0_RMS1012RDY | BITM_STATUS0_RMSONERDY |
			   BITM_STATUS0_COH_PAGE_RDY | BITM_STATUS0_PAGE_FULL;
	uint32_t config;

	if (status == 0) {
		config = 0;
		status = afe_write_16bit_reg(REG_RUN, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_RUN_FAILED;
		}
	}

	status = afe_write_32bit_reg(REG_STATUS0, (uint32_t *)&status0);

	if (status == 0) {
		config = 1;
		status = afe_write_16bit_reg(REG_RUN, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_RUN_FAILED;
		}
	}

	return status;
}

/**
 * @brief Configure AFE interrupts by setting appropriate bits in MASK0 register
 * @return int 0 in case of success, negative error code otherwise
 */
int config_afe_irq(void)
{
	int status = SYS_STATUS_SUCCESS;
	uint32_t config = 0;
	status = afe_write_32bit_reg(REG_MASK0, (uint32_t *)&config);
	if (status == 0) {
		config = BITM_MASK0_PAGE_FULL;
		status = afe_write_32bit_reg(REG_MASK0, (uint32_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_MASK0_FAILED;
		}
	}
	return status;
}

/**
 * @brief Configure waveform capture settings in WFB_CFG register
 * @return int 0 in case of success, negative error code otherwise
 */
int config_wfb(void)
{
	int status = SYS_STATUS_SUCCESS;
	uint16_t config = 0;
	config = (config & ~(BITM_WFB_CFG_WF_CAP_EN));
	status = afe_write_16bit_reg(REG_WFB_CFG, (uint16_t *)&config);

	if (status == 0) {
		config = BITM_WFB_CFG_WF_MODE | BITM_WFB_CFG_WF_SRC |
			 BITM_WFB_CFG_WF_CAP_RESAMPLED_SEL |
			 BITM_WFB_CFG_BURST_ALL_CHAN;
		status = afe_write_16bit_reg(REG_WFB_CFG, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_WFB_CFG_FAILED;
		}
	}
	if (status == 0) {
		status = afe_read_16bit_buff(REG_WFB_CFG, 1, &config);
		if (status != 0) {
			status = SYS_STATUS_AFE_WFB_CFG_FAILED;
		}
	}
	if (status == 0) {
		config = config | BITM_WFB_CFG_WF_CAP_EN;
		status = afe_write_16bit_reg(REG_WFB_CFG, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_WFB_CFG_FAILED;
		}
	}

	return status;
}

/**
 * @brief Read waveform data from AFE based on current coherent page and store in provided buffer+
 * @param pData Pointer to buffer where waveform data will be stored
 * @param numSamples Number of samples to read
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_waveform(uint16_t *pData, uint16_t numSamples)
{
	int status = SYS_STATUS_SUCCESS;
	int32_t cohPage = 0;
	int32_t readWaveformStartAddr = 0;

	status = afe_read_32bit_buff(REG_COH_PAGE, 1, (uint32_t *)&cohPage);
	if (status == 0) {
		if (cohPage == 0) {
			readWaveformStartAddr = REG_WAVEFORM_FIRST_HALF_ADDRESS;
		} else {
			readWaveformStartAddr = REG_WAVEFORM_SECOND_HALF_ADDRESS;
		}
		status = afe_read_16bit_buff(readWaveformStartAddr, numSamples,
					     (uint16_t *)&pData[0]);
		if (status != 0) {
			status = SYS_STATUS_AFE_READWAVEFORM_FAILED;
		}
	} else {
		status = SYS_STATUS_AFE_READWAVEFORM_FAILED;
	}
	return status;
}

/**
 * @brief Configure AFE for SINC4 waveform capture mode by setting appropriate bits in WFB_CFG register
 * @return int 0 in case of success, negative error code otherwise
 */
int config_wfb_sinc4(void)
{
	int status = SYS_STATUS_SUCCESS;
	uint16_t config = 0;
	uint16_t run_reg;
	uint32_t clr;

	/* Stop DSP — WFB_CFG changes only take effect when RUN=0 */
	run_reg = 0;
	status = afe_write_16bit_reg(REG_RUN, &run_reg);
	if (status != 0)
		return SYS_STATUS_AFE_WFB_CFG_FAILED;

	/* Disable capture first */
	status = afe_write_16bit_reg(REG_WFB_CFG, (uint16_t *)&config);
	if (status != 0)
		goto restart;

	/* WF_SRC=00 (SINC4), WF_MODE=11 (continuous fill + event addr),
	 * WF_CAP_SEL=1 (continuous), WF_IN_EN=0 (6 ch, no neutral),
	 * BURST_CHAN=0000 (all channels in burst) */
	config = BITM_WFB_CFG_WF_SRC_SINC4 | BITM_WFB_CFG_WF_MODE_CONT |
		 BITM_WFB_CFG_WF_CAP_SEL | BITM_WFB_CFG_BURST_ALL_CHAN;
	status = afe_write_16bit_reg(REG_WFB_CFG, (uint16_t *)&config);
	if (status != 0) {
		status = SYS_STATUS_AFE_WFB_CFG_FAILED;
		goto restart;
	}

	/* Read back and enable capture */
	status = afe_read_16bit_buff(REG_WFB_CFG, 1, &config);
	if (status != 0) {
		status = SYS_STATUS_AFE_WFB_CFG_FAILED;
		goto restart;
	}

	config |= BITM_WFB_CFG_WF_CAP_EN;
	status = afe_write_16bit_reg(REG_WFB_CFG, (uint16_t *)&config);
	if (status != 0) {
		status = SYS_STATUS_AFE_WFB_CFG_FAILED;
		goto restart;
	}

	/* Enable page interrupts at half-buffer boundaries (pages 7 and 15) */
	config = WFB_PG_IRQEN_HALF_BUF;
	status = afe_write_16bit_reg(REG_WFB_PG_IRQEN, (uint16_t *)&config);
	if (status != 0)
		status = SYS_STATUS_AFE_WFB_CFG_FAILED;

restart:
	/* Restart DSP and clear stale status */
	{
		clr = BITM_STATUS0_PAGE_FULL;
		afe_write_32bit_reg(REG_STATUS0, &clr);
	}
	run_reg = 1;
	afe_write_16bit_reg(REG_RUN, &run_reg);

	return status;
}

/**
 * @brief Read period data from AFE for specified number of periods and store in provided buffer
 * @param pPeriod Pointer to buffer where period data will be stored
 * @param numPeriods Number of periods to read
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_period(uint32_t *pPeriod, uint16_t numPeriods)
{
	int status = SYS_STATUS_SUCCESS;
	for (uint16_t i = 0; i < numPeriods; i++) {
		status =
			afe_read_32bit_buff(REG_PERIOD_FORRMS + i, 1, (uint32_t *)&pPeriod[i]);
	}
	if (status != 0) {
		status = SYS_STATUS_AFE_READPERIOD_FAILED;
	}
	return status;
}

/**
 * @brief Read RMS data from AFE for specified number of RMS values and store in provided buffer
 * @param pRMS Pointer to buffer where RMS data will be stored
 * @param numRMS Number of RMS values to read
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_rms_one(uint32_t *pRMS, uint16_t numRMS)
{
	int status = SYS_STATUS_SUCCESS;
	status = afe_read_32bit_buff(REG_AVRMSONE_1, numRMS, (uint32_t *)&pRMS[0]);
	if (status != 0) {
		status = SYS_STATUS_AFE_READRMSONE_FAILED;
	}
	return status;
}

/**
 * @brief Read RMS data from AFE for specified number of RMS values and store in provided buffer
 * @param pRMS Pointer to buffer where RMS data will be stored
 * @param numRMS Number of RMS values to read
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_rms_1012(uint32_t *pRMS, uint16_t numRMS)
{
	int status = SYS_STATUS_SUCCESS;
	status = afe_read_32bit_buff(REG_AIRMS1012_1, numRMS, (uint32_t *)&pRMS[0]);
	if (status != 0) {
		status = SYS_STATUS_AFE_READRMS1012_FAILED;
	}
	return status;
}

/**
 * @brief Read angle data from AFE for specified number of angles and store in provided buffer
 * @param pAngle Pointer to buffer where angle data will be stored
 * @param numAngles Number of angles to read
 * @param angleVolCur Flag indicating whether to read voltage angles (0) or current angles (1)
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_angle(uint16_t *pAngle, uint16_t numAngles,
		   uint16_t angleVolCur)
{
	int status = SYS_STATUS_SUCCESS;
	uint16_t angleAddress = 0;
	uint16_t i;

	if (angleVolCur == 0) {
		angleAddress = REG_ANGL_VA_VB;
	} else {
		angleAddress = REG_ANGL_IA_IB;
	}
	for (i = 0; i < numAngles; i++) {
		status = afe_read_16bit_buff(angleAddress + i, 1, (uint16_t *)&pAngle[i]);
	}
	if (status != 0) {
		status = SYS_STATUS_AFE_READANGLE_FAILED;
	}
	return status;
}

/**
 * @brief Read status from AFE STATUS1 register and clear ZXVA bit if set
 * @param pSTATUS1 Pointer to variable where STATUS1 value will be stored
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_status_1(uint32_t *pSTATUS1)
{
	int status = SYS_STATUS_SUCCESS;
	int32_t status1 = 0;

	status = afe_read_32bit_buff(REG_STATUS1, 1, pSTATUS1);
	status1 = *pSTATUS1;
	if (status1 & BITM_STATUS1_ZXVA) {
		status = afe_write_32bit_reg(REG_STATUS1, (uint32_t *)&status1);
	}
	if (status != 0) {
		status = SYS_STATUS_AFE_STATUS1_FAILED;
	}
	return status;
}

/**
 * @brief Read status from AFE STATUS1 register and clear ZX bit for specified phase if set
 * @param pSTATUS1 Pointer to variable where STATUS1 value will be stored
 * @param phase Phase for which ZX bit should be cleared (0 for A, 1 for B, 2 for C)
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_status_1_clear_zx(uint32_t *pSTATUS1, uint8_t phase)
{
	int status = SYS_STATUS_SUCCESS;
	uint32_t status1 = 0;
	uint32_t zx_mask;
	uint32_t clear_mask;

	/* Select ZX mask based on phase */
	switch (phase) {
	case 0:
		zx_mask = BITM_STATUS1_ZXVA;
		break;
	case 1:
		zx_mask = BITM_STATUS1_ZXVB;
		break;
	case 2:
		zx_mask = BITM_STATUS1_ZXVC;
		break;
	default:
		zx_mask = BITM_STATUS1_ZXVA;
		break;
	}

	status = afe_read_32bit_buff(REG_STATUS1, 1, pSTATUS1);
	if (status != 0) {
		return SYS_STATUS_AFE_STATUS1_FAILED;
	}

	status1 = *pSTATUS1;

	/* Clear ZX bit for the specified phase by writing 1 to it */
	if (status1 & zx_mask) {
		clear_mask = zx_mask;
		status = afe_write_32bit_reg(REG_STATUS1, &clear_mask);
		if (status != 0) {
			return SYS_STATUS_AFE_STATUS1_FAILED;
		}
	}

	return status;
}

/**
 * @brief Read AFE version information from REG_VERSION2 register and store in provided variable
 * @param pVersion Pointer to variable where version information will be stored
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_version(uint32_t *pVersion)
{
	int status = SYS_STATUS_SUCCESS;

	status = afe_read_32bit_buff(REG_VERSION2, 1, pVersion);
	if (status != 0) {
		status = SYS_STATUS_AFE_READVERSION_FAILED;
	}
	return status;
}

/**
 * @brief Write a 32-bit value to a specified AFE register address
 * @param addr Register address to write to
 * @param pData Pointer to 32-bit data to be written
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_write_32bit_reg(uint16_t addr, uint32_t *pData)
{
	int status = SYS_STATUS_SUCCESS;
	uint8_t cmdBuffer[2 * ADI_PQLIB_WAVEFORM_BLOCK_SIZE *
			    ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS +
			    2];

	cmdBuffer[0] = (addr >> 4);
	cmdBuffer[1] = ((addr & 0x0F) << 4);

	no_os_memswap64((void *)(&pData[0]), 4, 4);
	memcpy(&cmdBuffer[2], &pData[0], SPI_32BIT_DATA_SIZE);

	status = no_os_spi_write_and_read(hSPI, cmdBuffer, 6);

	return status;
}

/**
 * @brief Write a 16-bit value to a specified AFE register address
 * @param addr Register address to write to
 * @param pData Pointer to 16-bit data to be written
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_write_16bit_reg(uint16_t addr, uint16_t *pData)
{
	int status = SYS_STATUS_SUCCESS;
	uint8_t cmdBuffer[2 * ADI_PQLIB_WAVEFORM_BLOCK_SIZE *
			    ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS +
			    2];

	cmdBuffer[0] = (addr >> 4);
	cmdBuffer[1] = ((addr & 0x0F) << 4);

	no_os_memswap64((void *)(&pData[0]), 2, 2);
	memcpy(&cmdBuffer[2], &pData[0], SPI_16BIT_DATA_SIZE);

	status = no_os_spi_write_and_read(hSPI, cmdBuffer, 4);

	return status;
}

/**
 * @brief Read a block of 32-bit data from AFE starting at a specified register address
 * @param addr Starting register address to read from
 * @param numSamples Number of 32-bit samples to read
 * @param pData Pointer to buffer where read data will be stored
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_32bit_buff(uint16_t addr, uint16_t numSamples,
			uint32_t *pData)
{
	int status = SYS_STATUS_SUCCESS;
	uint8_t cmdBuffer[2 * ADI_PQLIB_WAVEFORM_BLOCK_SIZE *
			    ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS +
			    2];

	cmdBuffer[0] = (addr >> 4);
	cmdBuffer[1] = ((addr & 0x0F) << 4) + 8;
	memset((uint8_t *)&cmdBuffer[2], 0, sizeof(uint8_t) * numSamples * 4);
	status =
		no_os_spi_write_and_read(hSPI, (uint8_t *)cmdBuffer, 2 + numSamples * 4);

	memcpy((uint8_t *)pData, &cmdBuffer[2], numSamples * 4);
	no_os_memswap64((void *)pData, numSamples * 4, 4);

	return status;
}

/**
 * @brief Read a block of 16-bit data from AFE starting at a specified register address
 * @param addr Starting register address to read from
 * @param numSamples Number of 16-bit samples to read
 * @param pData Pointer to buffer where read data will be stored
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_16bit_buff(uint16_t addr, uint16_t numSamples,
			uint16_t *pData)
{
	int status = SYS_STATUS_SUCCESS;
	uint8_t cmdBuffer[2 * ADI_PQLIB_WAVEFORM_BLOCK_SIZE *
			    ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS +
			    2];

	cmdBuffer[0] = (addr >> 4);
	cmdBuffer[1] = ((addr & 0x0F) << 4) + 8;
	memset((uint8_t *)&cmdBuffer[2], 0, sizeof(uint8_t) * numSamples * 2);
	status =
		no_os_spi_write_and_read(hSPI, (uint8_t *)cmdBuffer, 2 + numSamples * 2);

	memcpy((uint8_t *)pData, &cmdBuffer[2], numSamples * 2);
	no_os_memswap64((void *)pData, numSamples * 2, 2);

	return status;
}

/**
 * @brief Reset AFE by toggling the reset GPIO pin
 * @return int 0 in case of success, negative error code otherwise
 */
int reset_afe(void)
{
	int status = 0;
	struct no_os_gpio_desc *sw_reset_gpio_desc;

	status = no_os_gpio_get(&sw_reset_gpio_desc, &reset_gpio_ip);
	if (status)
		goto exit;
	status = no_os_gpio_direction_output(sw_reset_gpio_desc, 1);
	if (status)
		goto exit;
	status = no_os_gpio_set_value(sw_reset_gpio_desc, 0);
	if (status)
		goto exit;
	no_os_mdelay(100);
	status = no_os_gpio_set_value(sw_reset_gpio_desc, 1);
	if (status)
		goto exit;
	no_os_mdelay(100);
	return status;

exit:
	no_os_gpio_remove(sw_reset_gpio_desc);
	return status;
}

/**
 * @brief Initialize SPI communication with AFE
 * @return int 0 in case of success, negative error code otherwise
 */
int spi_device_init(void)
{
	return no_os_spi_init(&hSPI, &spi_egy_ip);
}

/**
 * @brief Read status from AFE STATUS0 register and store in provided variable
 * @param pSTATUS0 Pointer to variable where STATUS0 value will be stored
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_status0(uint32_t *pSTATUS0)
{
	int status = SYS_STATUS_SUCCESS;

	*pSTATUS0 = 0;

	status = afe_read_32bit_buff(REG_STATUS0, 1, pSTATUS0);
	if (status != 0) {
		status = SYS_STATUS_AFE_STATUS0_FAILED;
	}

	return status;
}

/**
 * @brief Wait for AFE to settle by continuously reading STATUS0 register for specified number of cycles
 * @param cycles Number of cycles to wait for settling
 */
void afe_wait_settling(uint32_t cycles)
{
	int status = 0;
	uint32_t status0;

	while (cycles > 0) {
		status = afe_read_status0((uint32_t *)&status0);
		if ((status == SYS_STATUS_AFE_STATUS0_FAILED) ||
		    (status == SYS_STATUS_SUCCESS)) {
			cycles--;
		}
	}
}

/**
 * @brief Configure AFE acc mode
 * @param frequency Desired frequency for acc mode (50 or 60 Hz)
 * @param vconsel Voltage/current selection for acc mode
 * @param iconsel Iconsel selection for acc mode
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_set_acc_mode(uint16_t frequency, uint16_t vconsel,
		     uint16_t iconsel)
{
	int status = SYS_STATUS_SUCCESS;
	uint16_t config = 0;

	vconsel <<= BITP_ACCMODE_VCONSEL;
	iconsel <<= BITP_ACCMODE_ICONSEL;

	if (frequency == 50) {
		frequency = BITM_ACCMODE_SELFREQ_50HZ;
	} else if (frequency == 60) {
		frequency = BITM_ACCMODE_SELFREQ_60HZ;
	} else {
		return SYS_STATUS_PQLIB_CONFIG_ERROR;
	}
	status = afe_read_16bit_buff(REG_ACCMODE, 1, &config);
	if (status != 0) {
		status = SYS_STATUS_AFE_ACCMODE_FAILED;
	}
	if (status == 0) {
		config = config & BITM_FREQ_VCONSEL_ICONSEL;
		config = config | frequency | vconsel | iconsel;
		status = afe_write_16bit_reg(REG_ACCMODE, (uint16_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_ACCMODE_FAILED;
		}
	}
	return status;
}

/**
 * @brief Read power and energy data from AFE for all phases and store in provided structure
 * @param pData Pointer to structure where power and energy data will be stored
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_read_power_energy(POWER_ENERGY_DATA *pData)
{
	int status = SYS_STATUS_SUCCESS;
	int i;
	static const uint16_t watt_regs[] = {REG_AWATT, REG_BWATT, REG_CWATT};
	static const uint16_t watthr_hi_regs[] = {REG_AWATTHR_HI, REG_BWATTHR_HI, REG_CWATTHR_HI};
	static const uint16_t varhr_hi_regs[] = {REG_AVARHR_HI, REG_BVARHR_HI, REG_CVARHR_HI};
	static const uint16_t fwatt_regs[] = {REG_AFWATT, REG_BFWATT, REG_CFWATT};
	static const uint16_t fwatthr_hi_regs[] = {REG_AFWATTHR_HI, REG_BFWATTHR_HI, REG_CFWATTHR_HI};
	static const uint16_t fvarhr_hi_regs[] = {REG_AFVARHR_HI, REG_BFVARHR_HI, REG_CFVARHR_HI};

	for (i = 0; i < POWER_ENERGY_NUM_PHASES && status == 0; i++) {
		status = afe_read_32bit_buff(watt_regs[i], 1,
					     (uint32_t *)&pData->activePower[i]);
	}
	for (i = 0; i < POWER_ENERGY_NUM_PHASES && status == 0; i++) {
		status = afe_read_32bit_buff(watthr_hi_regs[i], 1,
					     (uint32_t *)&pData->activeEnergyHi[i]);
	}
	for (i = 0; i < POWER_ENERGY_NUM_PHASES && status == 0; i++) {
		status = afe_read_32bit_buff(varhr_hi_regs[i], 1,
					     (uint32_t *)&pData->reactiveEnergyHi[i]);
	}
	for (i = 0; i < POWER_ENERGY_NUM_PHASES && status == 0; i++) {
		status = afe_read_32bit_buff(fwatt_regs[i], 1,
					     (uint32_t *)&pData->fundActivePower[i]);
	}
	for (i = 0; i < POWER_ENERGY_NUM_PHASES && status == 0; i++) {
		status = afe_read_32bit_buff(fwatthr_hi_regs[i], 1,
					     (uint32_t *)&pData->fundActiveEnergyHi[i]);
	}
	for (i = 0; i < POWER_ENERGY_NUM_PHASES && status == 0; i++) {
		status = afe_read_32bit_buff(fvarhr_hi_regs[i], 1,
					     (uint32_t *)&pData->fundReactiveEnergyHi[i]);
	}

	return status;
}

/**
 * @brief Configure AFE reference channel by setting appropriate bits in ZX_LP_SEL register
 * @param refChannel Reference channel selection
 * @return int 0 in case of success, negative error code otherwise
 */
int afe_set_ref_channel(uint16_t refChannel)
{
	int status = SYS_STATUS_SUCCESS;
	uint16_t config = 0;
	if (refChannel == ADI_PQLIB_REF_CHANNEL_VA) {
		refChannel = LP_SEL_APERIOD | ZX_SEL_ZXVA;
	} else if (refChannel == ADI_PQLIB_REF_CHANNEL_VB) {
		refChannel = LP_SEL_BPERIOD | ZX_SEL_ZXVB;
	} else if (refChannel == ADI_PQLIB_REF_CHANNEL_VC) {
		refChannel = LP_SEL_CPERIOD | ZX_SEL_ZXVC;
	} else {
		refChannel = BITM_ZX_LP_SEL_ZX_SEL | BITM_ZX_LP_SEL_LP_SEL;
	}
	config = refChannel;
	status = afe_write_16bit_reg(REG_ZX_LP_SEL, (uint16_t *)&config);
	if (status != 0) {
		status = SYS_STATUS_AFE_ZX_LP_SEL_FAILED;
	}

	return status;
}
