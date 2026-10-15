/*******************************************************************************
 *   @file   afe_config.h
 *   @brief  Analog frontend header file
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef __AFE_CONFIG_H__
#define __AFE_CONFIG_H__

#include "ade9430.h"
#include "adi_pqlib.h"
#include "no_os_shim.h"
#include "status.h"
#include <stdint.h>
#include <string.h>

/*! WFB_CFG burst all channels */
#define BITM_WFB_CFG_BURST_ALL_CHAN 0x00000000
/*! WFB_CFG resampled data for waveform */
#define BITM_WFB_CFG_WF_CAP_RESAMPLED_SEL 0x00000000
/*! Waveform buffer first half address */
#define REG_WAVEFORM_FIRST_HALF_ADDRESS 0x801
/*! Waveform buffer second half address */
#define REG_WAVEFORM_SECOND_HALF_ADDRESS 0xC01
/*! 50Hz System nominal frequency */
#define BITM_ACCMODE_SELFREQ_50HZ 0x0
/*! 60Hz System nominal frequency */
#define BITM_ACCMODE_SELFREQ_60HZ 0x0100
/*! frequency, VCON and ICON selection */
#define BITM_FREQ_VCONSEL_ICONSEL 0xFE0F
/*! Average by 8 to reduce jitter*/
#define BITM_CONFIG0_PERIOD_AVG_CFG_AV8 0x00010000
/*! Average by 16 to reduce jitter*/
#define BITM_CONFIG0_PERIOD_AVG_CFG_AV16 0x00020000
/*! Line period measurement on APERIOD*/
#define LP_SEL_APERIOD 0x0000
/*! Line period measurement on BPERIOD*/
#define LP_SEL_BPERIOD 0x0008
/*! Line period measurement on CPERIOD*/
#define LP_SEL_CPERIOD 0x0010
/*! zero-crossing signal routed to the CF3/ZX on ZXVA*/
#define ZX_SEL_ZXVA 0x0000
/*! zero-crossing signal routed to the CF3/ZX on ZXVB*/
#define ZX_SEL_ZXVB 0x0002
/*! zero-crossing signal routed to the CF3/ZX on ZXVC*/
#define ZX_SEL_ZXVC 0x0004
/*! Spi timeout */
#define SPI_TIMEOUT 100
/*! status of afe functions */
typedef uint32_t AFE_OUTPUT_STATUS;
/** SPI header size in bytes*/
#define SPI_HEADER_SIZE 2
/** SPI max data size in bytes*/
#define MAX_SPI_DATA_SIZE 4
/** SPI 16bit data size in bytes*/
#define SPI_16BIT_DATA_SIZE 2
/** SPI 32bit data size in bytes*/
#define SPI_32BIT_DATA_SIZE 4
/** maximum size of SPI buffer*/
#define MAX_SPI_BUFFER_SIZE SPI_HEADER_SIZE + MAX_SPI_DATA_SIZE

/**
 * Enumeration to capture status of SPI
 */
typedef enum {
	AFE_SPI_STATUS_DMA_IDLE,
	AFE_SPI_STATUS_DMA_RUNNING,
	AFE_SPI_STATUS_DMA_COMPLETED
} AFE_SPI_STATUS;

int spi_device_init(void);
int config_wfb(void);
int reset_afe(void);
int afe_start(void);
int afe_init(void);
int afe_read_waveform(uint16_t *pData, uint16_t numSamples);
int afe_read_period(uint32_t *pPeriod, uint16_t numPeriods);
int afe_read_rms_one(uint32_t *pRMS, uint16_t numRMS);
int afe_read_rms_1012(uint32_t *pRMS, uint16_t numRMS);
int afe_read_angle(uint16_t *pAngle, uint16_t numAngles,
		   uint16_t angleVolCur);
int afe_read_status_1(uint32_t *pSTATUS1);
int afe_read_status_1_clear_zx(uint32_t *pSTATUS1, uint8_t phase);
int afe_read_status0(uint32_t *pSTATUS0);
void afe_wait_settling(uint32_t cycles);
int afe_read_version(uint32_t *pVersion);
int config_afe_irq(void);
int afe_set_acc_mode(uint16_t frequency, uint16_t vconsel,
		     uint16_t iconsel);
int afe_set_ref_channel(uint16_t refChannel);
int afe_read_32bit_buff(uint16_t addr, uint16_t numSamples,
			uint32_t *pData);
int afe_read_16bit_buff(uint16_t addr, uint16_t numSamples,
			uint16_t *pData);
int afe_write_32bit_reg(uint16_t addr, uint32_t *pData);
int afe_write_16bit_reg(uint16_t addr, uint16_t *pData);

struct power_energy_data;
int afe_read_power_energy(struct power_energy_data *pData);

#endif /* __AFE_CONFIG_H__ */
