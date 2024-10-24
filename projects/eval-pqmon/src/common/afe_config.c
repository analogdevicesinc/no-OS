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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "afe_config.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

struct no_os_spi_desc *hSPI;
struct no_os_spi_msg spiMsg;
extern struct no_os_gpio_init_param reset_gpio_ip;

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

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

	// To remove
	uint16_t pTest[10];
	afe_read_16bit_buff(0x4A4, 1, pTest);
	printf("ID register: %d\n\r", pTest[0]);

	return status;
}

int afe_start(void)
{
	int status = SYS_STATUS_SUCCESS;
	uint32_t status0 = BITM_STATUS0_RMS1012RDY | BITM_STATUS0_RMSONERDY |
			   BITM_STATUS0_COH_PAGE_RDY;
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

int config_afe_irq(void)
{
	int status = SYS_STATUS_SUCCESS;
	uint32_t config = 0;
	status = afe_write_32bit_reg(REG_MASK0, (uint32_t *)&config);
	if (status == 0) {
		config = BITM_MASK0_RMSONERDY | BITM_MASK0_COH_PAGE_RDY;
		status = afe_write_32bit_reg(REG_MASK0, (uint32_t *)&config);
		if (status != 0) {
			status = SYS_STATUS_AFE_MASK0_FAILED;
		}
	}
	return status;
}

int config_wfb(void)
{
	int status = SYS_STATUS_SUCCESS;
	uint16_t config = 0;
	config = (config & ~(BITM_WFB_CFG_WF_CAP_EN));
	status = afe_write_16bit_reg(REG_WFB_CFG, (uint16_t *)&config);

	if (status == 0) {
		config = BITM_WFB_CFG_WF_MODE | BITM_WFB_CFG_WF_SRC |
			 BITM_WFB_CFG_WF_IN_EN | BITM_WFB_CFG_WF_CAP_RESAMPLED_SEL |
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

int afe_read_rms_one(uint32_t *pRMS, uint16_t numRMS)
{
	int status = SYS_STATUS_SUCCESS;
	status = afe_read_32bit_buff(REG_AVRMSONE_1, numRMS, (uint32_t *)&pRMS[0]);
	if (status != 0) {
		status = SYS_STATUS_AFE_READRMSONE_FAILED;
	}
	return status;
}

int afe_read_rms_1012(uint32_t *pRMS, uint16_t numRMS)
{
	int status = SYS_STATUS_SUCCESS;
	status = afe_read_32bit_buff(REG_AIRMS1012_1, numRMS, (uint32_t *)&pRMS[0]);
	if (status != 0) {
		status = SYS_STATUS_AFE_READRMS1012_FAILED;
	}
	return status;
}

int afe_read_angle(uint16_t *pAngle, uint16_t numAngles,
		   uint16_t angleVolCur)
{
	int status = SYS_STATUS_SUCCESS;
	uint16_t angleAddress = 0;
	if (angleVolCur == 0) {
		angleAddress = REG_ANGL_VA_VB;
	} else {
		angleAddress = REG_ANGL_IA_IB;
	}
	for (uint16_t i = 0; i < numAngles; i++) {
		status = afe_read_16bit_buff(angleAddress + i, 1, (uint16_t *)&pAngle[i]);
	}
	if (status != 0) {
		status = SYS_STATUS_AFE_READANGLE_FAILED;
	}
	return status;
}

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

int afe_read_version(uint32_t *pVersion)
{
	int status = SYS_STATUS_SUCCESS;

	status = afe_read_32bit_buff(REG_VERSION2, 1, pVersion);
	if (status != 0) {
		status = SYS_STATUS_AFE_READVERSION_FAILED;
	}
	return status;
}
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

int spi_device_init(void)
{
	return no_os_spi_init(&hSPI, &spi_egy_ip);
}

int afe_read_status0(uint32_t *pSTATUS0)
{
	int status = SYS_STATUS_SUCCESS;

	uint32_t status0 = 0;
	*pSTATUS0 = 0;

	status = afe_read_32bit_buff(REG_STATUS0, 1, pSTATUS0);
	status0 = *pSTATUS0;
	if (status != 0) {
		status = SYS_STATUS_AFE_STATUS0_FAILED;
	}

	if (status == 0) {
		status0 |= BITM_STATUS0_RMSONERDY;
		status = afe_write_32bit_reg(REG_STATUS0, (uint32_t *)&status0);
		if (status != 0) {
			status = SYS_STATUS_AFE_STATUS0_FAILED;
		}
	}

	return status;
}

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
