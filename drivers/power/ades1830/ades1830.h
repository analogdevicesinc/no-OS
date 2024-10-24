/***************************************************************************//**
 *   @file   ades1830.h
 *   @brief  Header file for the ADES1830 Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef __ADES1830_H__
#define __ADES1830_H__

#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_units.h"

/** Command Map */
#define ADES1830_WRCFGA_CMD			0x01
#define ADES1830_WRCFGB_CMD			0x24
#define ADES1830_RDCFGA_CMD			0x02
#define ADES1830_RDCFGB_CMD			0x26
#define ADES1830_RDCVA_CMD			0x04
#define ADES1830_RDCVB_CMD			0x06
#define ADES1830_RDCVC_CMD			0x08
#define ADES1830_RDCVD_CMD			0x0A
#define ADES1830_RDCVE_CMD			0x09
#define ADES1830_RDCVF_CMD			0x0B
#define ADES1830_RDCVALL_CMD			0x0C
#define ADES1830_RDACA_CMD			0x44
#define ADES1830_RDACB_CMD			0x46
#define ADES1830_RDACC_CMD			0x48
#define ADES1830_RDACD_CMD			0x4A
#define ADES1830_RDACE_CMD			0x49
#define ADES1830_RDACF_CMD			0x4B
#define ADES1830_RDACALL_CMD			0x4C
#define ADES1830_RDSVA_CMD			0x03
#define ADES1830_RDSVB_CMD			0x05
#define ADES1830_RDSVC_CMD			0x07
#define ADES1830_RDSVD_CMD			0x0D
#define ADES1830_RDSVE_CMD			0x0E
#define ADES1830_RDSVF_CMD			0x0F
#define ADES1830_RDSALL_CMD			0x10
#define ADES1830_RDCSALL_CMD			0x11
#define ADES1830_RDACSALL_CMD			0x51
#define ADES1830_RDFCA_CMD			0x12
#define ADES1830_RDFCB_CMD			0x13
#define ADES1830_RDFCC_CMD			0x14
#define ADES1830_RDFCD_CMD			0x15
#define ADES1830_RDFCE_CMD			0x16
#define ADES1830_RDFCF_CMD			0x17
#define ADES1830_RDFCALL_CMD			0x18
#define ADES1830_RDAUXA_CMD			0x19
#define ADES1830_RDAUXB_CMD			0x1A
#define ADES1830_RDAUXC_CMD			0x1B
#define ADES1830_RDAUXD_CMD			0x1F
#define ADES1830_RDRAXA_CMD			0x1C
#define ADES1830_RDRAXB_CMD			0x1D
#define ADES1830_RDRAXC_CMD			0x1E
#define ADES1830_RDRAXD_CMD			0x25
#define ADES1830_RDSTATA_CMD			0x30
#define ADES1830_RDSTATB_CMD			0x31
#define ADES1830_RDSTATC_CMD			0x32
#define ADES1830_RDSTATD_CMD			0x33
#define ADES1830_RDSTATE_CMD			0x34
#define ADES1830_RDASALL_CMD			0x35
#define ADES1830_WRPWMA_CMD			0x20
#define ADES1830_RDPWMA_CMD			0x22
#define ADES1830_WRPWMB_CMD			0x21
#define ADES1830_RDPWMB_CMD			0x23
#define ADES1830_CMDIS_CMD			0x40
#define ADES1830_CMEN_CMD			0x41
#define ADES1830_CMHB_CMD			0x43
#define ADES1830_WRCMCFG_CMD			0x58
#define ADES1830_RDCMCFG_CMD			0x59
#define ADES1830_WRCMCELLT_CMD			0x5A
#define ADES1830_RDCMCELLT_CMD			0x5B
#define ADES1830_WRCMGPIOT_CMD			0x5C
#define ADES1830_RDCMGPIOT_CMD			0x5D
#define ADES1830_CLRCMFLAG_CMD			0x5E
#define ADES1830_RDCMFLAG_CMD			0x5F

/** These will need further masks defined here. */
#define ADES1830_ADCV_BASE_CMD			0x260
#define ADES1830_ADSV_BASE_CMD			0x168
#define ADES1830_ADAX_BASE_CMD			0x410
#define ADES1830_ADAX2_BASE_CMD			0x100

#define ADES1830_RD_CMD_MASK			NO_OS_BIT(8)
#define ADES1830_CONT_CMD_MASK			NO_OS_BIT(7)
#define ADES1830_DCP_CMD_MASK			NO_OS_BIT(4)
#define ADES1830_RSTF_CMD_MASK			NO_OS_BIT(2)
#define ADES1830_OW_CMD_MASK			NO_OS_GENMASK(1, 0)
#define ADES1830_AUX_OW_CMD_MASK		NO_OS_BIT(8)
#define ADES1830_PUP_CMD_MASK			NO_OS_BIT(7)
#define ADES1830_CH_CMD_MASK			NO_OS_GENMASK(3, 0)
#define ADES1830_CH4_CMD_MASK			NO_OS_BIT(6)

#define ADES1830_CLRCELL_CMD			0x711
#define ADES1830_CLRFC_CMD			0x714
#define ADES1830_CLRAUX_CMD			0x712
#define ADES1830_CLRSPIN_CMD			0x716
#define ADES1830_CLRFLAG_CMD			0x717
#define ADES1830_CLOVUV_CMD			0x715
#define ADES1830_PLADC_CMD			0x718
#define ADES1830_PLCADC_CMD			0x71C
#define ADES1830_PLSADC_CMD			0x71D
#define ADES1830_PLAUX_CMD			0x71E
#define ADES1830_PLAUX2_CMD			0x71F
#define ADES1830_WRCOMM_CMD			0x721
#define ADES1830_RDCOMM_CMD			0x722
#define ADES1830_STCOMM_CMD			0x723
#define ADES1830_MUTE_CMD			0x28
#define ADES1830_UNMUTE_CMD			0x29
#define ADES1830_RDSID_CMD			0x2C
#define ADES1830_RSTCC_CCMDD			0x2E
#define ADES1830_SNAP_CMD			0x2D
#define ADES1830_UNSNAP_CMD			0x2F
#define ADES1830_SRST_CMD			0x27
#define ADES1830_ULRR_CMD			0x38
#define ADES1830_WRRR_CMD			0x39
#define ADES1830_RDRR_CMD			0x3A

/** UTIL MASKS */
#define ADES1830_MSB_MASK			NO_OS_GENMASK(15, 8)
#define ADES1830_LSB_MASK			NO_OS_GENMASK(7, 0)

#define ADES1830_CTH_MASK			NO_OS_GENMASK(2, 0)
#define ADES1830_SOAKON_MASK			NO_OS_BIT(7)
#define ADES1830_OWRNG_MASK			NO_OS_BIT(6)
#define ADES1830_OWA_MASK			NO_OS_GENMASK(5, 3) /* Will be used for soak time set. Later used in UTIL API. */
#define ADES1830_GPOX_MASK(offset)		NO_OS_BIT(offset - 1)
#define ADES1830_FC_MASK			NO_OS_GENMASK(2, 0)
#define ADES1830_COMM_BK_MASK			NO_OS_BIT(3)
#define ADES1830_MUTE_ST_MASK			NO_OS_BIT(4)
#define ADES1830_SNAP_ST_MASK			NO_OS_BIT(5)

#define ADES1830_DTMEN_MASK			NO_OS_BIT(7)
#define ADES1830_DTRNG_MASK			NO_OS_BIT(6)
#define ADES1830_DCTO_MASK			NO_OS_GENMASK(5, 0)

#define ADES1830_CSXFLT_MASK(offset)		NO_OS_BIT(offset)
#define ADES1830_CTMSB_MASK			NO_OS_GENMASK(4, 0)
#define ADES1830_CTLSB_MASK			ADES1830_LSB_MASK
#define ADES1830_VA_OV_MASK			NO_OS_BIT(7)
#define ADES1830_VA_UV_MASK			NO_OS_BIT(6)
#define ADES1830_VD_OV_MASK			NO_OS_BIT(5)
#define ADES1830_VD_UV_MASK			NO_OS_BIT(4)
#define ADES1830_CED_MASK			NO_OS_BIT(3)
#define ADES1830_CMED_MASK			NO_OS_BIT(2)
#define ADES1830_SED_MASK			NO_OS_BIT(1)
#define ADES1830_SMED_MASK			NO_OS_BIT(0)
#define ADES1830_VDEL_MASK			NO_OS_BIT(7)
#define ADES1830_VDE_MASK			NO_OS_BIT(6)
#define ADES1830_COMP_MASK			NO_OS_BIT(5)
#define ADES1830_SPIFLT_MASK			NO_OS_BIT(4)
#define ADES1830_SLEEP_MASK			NO_OS_BIT(3)
#define ADES1830_THSD_MASK			NO_OS_BIT(2)
#define ADES1830_TMODCHK_MASK			NO_OS_BIT(1)
#define ADES1830_OSCCHK_MASK			NO_OS_BIT(0)

#define ADES1830_CxUV_MASK(offset)		NO_OS_BIT(((offset - 1) % 4) * 2)
#define ADES1830_CxOV_MASK(offset)		NO_OS_BIT((((offset - 1) % 4) * 2) + 1)
#define ADES1830_GPX_MASK(offset)		NO_OS_BIT((offset - 1) % 8)
#define ADES1830_REV_MASK			NO_OS_GENMASK(7, 4)
#define ADES1830_PWMX_MASK(offset)		((offset % 2) ? NO_OS_GENMASK(7, 4) : NO_OS_GENMASK(3, 0))



#define ADES1830_ICOM_MASK			NO_OS_GENMASK(7, 4)
#define ADEs1830_FCOM_MASK			NO_OS_GENMASK(3, 0)

/** UTIL MACROS */
#define ADES1830_FLAGD_VAL(offset)		NO_OS_BIT(offset)

/** Enums */
enum ades1830_chan_adax {
	ADES1830_ALL_ADAX,
	ADES1830_GPIO1_ADAX,
	ADES1830_GPIO2_ADAX,
	ADES1830_GPIO3_ADAX,
	ADES1830_GPIO4_ADAX,
	ADES1830_GPIO5_ADAX,
	ADES1830_GPIO6_ADAX,
	ADES1830_GPIO7_ADAX,
	ADES1830_GPIO8_ADAX,
	ADES1830_GPIO9_ADAX,
	ADES1830_VREF2_ADAX = 0x10,
	ADES1830_VD_ADAX,
	ADES1830_ITEMP_ADAX,
	ADES1830_VPV_ADAX,
	ADES1830_VMV_ADAX,
	ADES1830_RES_ADAX,
};

enum ades1830_cont_sel {
	ADES1830_SINGLE_TO_STANDBY,
	ADES1830_CONT_MEAS,
};

enum ades1830_ow_sel {
	ADES1830_ALL_OFF_OW,
	ADES1830_EVEN_ON_OW,
	ADES1830_ODD_ON_OW,
	ADES1830_ALL_ON_OW,
};

enum ades1830_ow_aux_sel {
	ADES1830_OFF_AUXOW,
	ADES1830_ON_AUXOW,
};

enum ades1830_pup_sel {
	ADES1830_PD_AUX_CONV,
	ADES1830_PU_AUX_CONV,
};

enum ade1830_discharge_sel {
	ADES1830_NOPERMIT_DISCHARGE,
	ADES1830_PERMIT_DISCHARGE,
};

enum ades1830_reset_iir_sel {
	ADES1830_NO_RESET_IIR,
	ADES1830_RESET_IIR,
};

enum ades1830_err_sel {
	ADES1830_RD_STAT_REG_C_NO_ERROR,
	ADES1830_RD_STAT_REG_C_WITH_ERROR,
};

enum ades1830_cth_val {
	ADES1830_5100UV_CTH,
	ADES1830_8100UV_CTH,
	ADES1830_9000UV_CTH,
	ADES1830_10500UV_CTH,
	ADES1830_15000UV_CTH,
	ADES1830_19950UV_CTH,
	ADES1830_25050UV_CTH,
	ADES1830_40050UV_CTH,
};

enum ades1830_fc_iir_val {
	ADES1830_FILTER_DISABLED,
	ADES1830_110_FC_IIR,
	ADES1830_45_FC_IIR,
	ADES1830_21_FC_IIR,
	ADES1830_10_FC_IIR,
	ADES1830_5_FC_IIR,
	ADES1830_1_25_FC_IIR,
	ADES1830_0_625_FC_IIR,
};

enum ades1830_icom_sel {
	// ADES1830_I2C_START_ICOM,
	// ADES1830_I2C_STOP_ICOM,
	// ADES1830_I2C_BLANK_ICOM,
	// ADES1830_I2C_NOTRANSMIT_ICOM,
	ADES1830_SPI_CSB_LOW_ICOM = 0x08,
	ADES1830_SPI_CSB_HIGH_ICOM = 0x09,
	ADES1830_SPI_CSB_FALLING_ICOM = 0x0A,
	ADES1830_SPI_NO_TRANSMIT_ICOM = 0x0F,
};

// enum ades1830_fcom_write_sel {
// 	ADES1830_I2C_CTRL_ACK_WR_FCOM,
// 	ADES1830_I2C_CTRL_NACK_WR_FCOM,
// 	ADES1830_I2C_CTRL_NACK_STOP_WR_FCOM,
// };

// enum ades1830_fcom_i2c_read_sel {
// 	ADES1830_I2C_CTRL_ACK_RD_FCOM,
// 	ADES1830_I2C_TGT_ACK_CTRL_STOP_RD_FCOM = 0x01,
// 	ADES1830_I2C_TGT_ACK_RD_FCOM = 0x07,
// 	ADES1830_I2C_TGT_NACK_CTRL_STOP_RD_FCOM = 0x09,
// 	ADES1830_I2C_TGT_NACK_RD_FCOM = 0x0F,
// };

enum ades1830_fcom_spi_read_sel {
	ADES1830_SPI_CSB_LOW_RD_FCOM,
	ADES1830_SPI_CSB_HIGH_RD_FCOM = 0x09,
};

struct ades1830_init_param {
	struct no_os_spi_init_param *spi_param;
};

struct ades1830_desc {
	struct no_os_spi_descriptor *spi_desc;
};

int ades1830_cmd_send(struct ades1830_desc *desc, uint16_t cmd);

int ades1830_cmd_write(struct ades1830_desc *desc, uint16_t cmd, uint8_t *data);

int ades1830_cmd_read(struct ades1830_desc *desc, uint16_t cmd, uint8_t *data);

/** UTIL API. */

int ades1830_init(struct ades1830_desc **desc,
		  struct ades1830_init_param *init_param);

int ades1830_remove(struct ades1830_desc *desc);

#endif /* __ADES1830_H__ */