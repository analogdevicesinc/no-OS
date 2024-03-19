/***************************************************************************//**
 *   @file   ad74416h.h
 *   @brief  Header file of AD74416h Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
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
*******************************************************************************/
#ifndef _AD74416H_H
#define _AD74416H_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "stdint.h"
#include "stdbool.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD74416H_N_CHANNELS             4

#define AD74416H_CH_A                   0
#define AD74416H_CH_B                   1
#define AD74416H_CH_C                   2
#define AD74416H_CH_D                   3

/** The value of the sense resistor in ohms */
#define AD74416H_RSENSE                 12
/** 16 bit ADC */
#define AD74416H_ADC_MAX_VALUE		16777215

/** Register map */
#define AD74416H_NOP				0x00
#define AD74416H_CH_FUNC_SETUP(x)               (0x01 + (x * 12))
#define AD74416H_ADC_CONFIG(x)                  (0x02 + (x * 12))
#define AD74416H_DIN_CONFIG0(x)			(0x03 + (x * 12))
#define AD74416H_DIN_CONFIG1(x)			(0x04 + (x * 12))
#define AD74416H_OUTPUT_CONFIG(x)               (0x05 + (x * 12))
#define AD74416H_RTD_CONFIG(x)			(0x06 + (x * 12))
#define AD74416H_FET_LKG_COMP(x)		(0x07 + (x * 12))
#define AD74416H_DO_EXT_CONFIG(x)		(0x08 + (x * 12))
#define AD74416H_I_BURNOUT_CONFIG(x)		(0x09 + (x * 12))
#define AD74416H_DAC_CODE(x)			(0x0A + (x * 12))
#define AD74416H_DAC_ACTIVE(x)			(0x0C + (x * 12))
#define AD74416H_GPIO_CONFIG(x)			(0x32 + x)
#define AD74416H_PWR_OPTIM_CONFIG		0x38
#define AD74416H_ADC_CONV_CTRL			0x39
#define AD74416H_DIAG_ASSIGN			0x3A
#define AD74416H_WTD_CONFIG			0x3B
#define AD74416H_DIN_COMP_OUT			0x3E
#define AD74416H_ALERT_STATUS			0x3F
#define AD74416H_LIVE_STATUS			0x40
#define AD74416H_ADC_RESULT_UPR(x)		(0x41 + (x * 2))
#define AD74416H_ADC_RESULT(x)			(0x42 + (x * 2))
#define AD74416H_ADC_DIAG_RESULT(x)		(0x49 + x)
#define AD74416H_LAST_ADC_RESULT_UPR		0x4D
#define AD74416H_LAST_ADC_RESULT		0x4E
#define AD74416H_DIN_COUNTER(x)			(0x50 + (x * 2))
#define AD74416H_SUPPLY_ALERT_STATUS		0x57
#define AD74416H_CHANNEL_ALERT_STATUS(x)	(0x58 + x)
#define AD74416H_ALERT_MASK			0x5C
#define AD74416H_SUPPLY_ALERT_MASK		0x5D
#define AD74416H_CHANNEL_ALERT_MASK(x)		(0x5E + x)
#define AD74416H_READ_SELECT			0x6E
#define AD74416H_BURST_READ_SEL			0x6F
#define AD74416H_THERM_RST			0x73
#define AD74416H_CMD_KEY			0x74
#define AD74416H_BORADCAST_CMD_KEY		0x75
#define AD74416H_SCRATCH(x)			(0x76 + x)
#define AD74416H_GENERIC_ID			0x7A
#define AD74416H_SILICON_REV			0x7B
#define AD74416H_SILICON_ID0			0x7D
#define AD74416H_SILICON_ID1			0x7E
#define AD74416H_HART_ALERT_STATUS(x)		(0x80 + (x * 16))
#define AD74416H_HART_RX(x)			(0x81 + (x * 16))
#define AD74416H_HART_TX(x)			(0x82 + (x * 16))
#define AD74416H_HART_FCR(x)			(0x83 + (x * 16))
#define AD74416H_HART_MCR(x)			(0x84 + (x * 16))
#define AD74416H_HART_RFC(x)			(0x85 + (x * 16))
#define AD74416H_HART_TFC(x)			(0x86 + (x * 16))
#define AD74416H_HART_ALERT_MASK(x)		(0x87 + (x * 16))
#define AD74416H_HART_CONFIG(x)			(0x88 + (x * 16))
#define AD74416H_HART_TX_PREM(x)		(0x89 + (x * 16))
#define AD74416H_HART_EVDET(x)			(0x8A + (x * 16))
#define AD74416H_HART_TX_GAIN(x)		(0x8B + (x * 16))
#define AD74416H_HART_GPIO_IF_CONFIG		0xC0
#define AD74416H_HART_GPIO_MON_CONFIG(x)	(0xC1 + x)

/** Software reset sequence */
#define AD74416H_CMD_KEY_RESET_1                0x15FA
#define AD74416H_CMD_KEY_RESET_2                0xAF51

#define AD74416H_SPI_RD_RET_INFO_MSK		NO_OS_BIT(8)
#define AD74416H_ERR_CLR_MSK			NO_OS_GENMASK(15, 0)
#define AD74416H_SPI_CRC_ERR_MSK		NO_OS_BIT(13)

/* AD74416H_CH_FUNC_SETUP */
#define AD74416H_CH_FUNC_SETUP_MSK		NO_OS_GENMASK(3, 0)

/* AD74416H_ADC_CONFIG */
#define AD74416H_ADC_CONV_RATE_MSK		NO_OS_GENMASK(10, 8)
#define AD74416H_ADC_CONV_RANGE_MSK		NO_OS_GENMASK(6, 4)
#define AD74416H_CONV_MUX_MSK			NO_OS_GENMASK(2, 0)

/* AD74416H_DIN_CONFIG0 */
#define AD74416H_COUNT_EN_MSK			NO_OS_BIT(15)
#define AD74416H_DIN_INV_COMP_OUT_MSK		NO_OS_BIT(14)
#define AD74416H_COMPARATOR_EN_MSK		NO_OS_BIT(13)
#define AD74416H_DIN_SINK_RANGE_MSK		NO_OS_BIT(12)
#define AD74416H_DIN_SINK_MSK			NO_OS_GENMASK(11, 7)
#define AD74416H_DEBOUNCE_MODE_MSK		NO_OS_BIT(6)
#define AD74416H_DEBOUNCE_TIME_MSK		NO_OS_GENMASK(4, 0)

/* AD74416H_DIN_CONFIG1 Register */
#define AD74416H_DIN_INPUT_SELECT_MSK		NO_OS_BIT(10)
#define AD74416H_DIN_SC_DET_EN_MSK		NO_OS_BIT(9)
#define AD74416H_DIN_OC_DET_EN_MSK		NO_OS_BIT(8)
#define AD74416H_DIN_THRESH_MODE_MSK		NO_OS_BIT(7)
#define AD74416H_COMP_THRESH_MSK		NO_OS_GENMASK(6, 0)

/** OUTPUT_CONFIGx Register */
#define AD74416H_AVDD_SELECT_MSK		NO_OS_GENMASK(15, 14)
#define AD74416H_ALARM_DEG_PERIOD_MSK		NO_OS_BIT(12)
#define AD74VOUT_4W_EN_MSK			NO_OS_BIT(11)
#define AD74416H_WAIT_LDAC_CMD_MSK		NO_OS_BIT(10)
#define AD74416H_VOUT_RANGE_MSK			NO_OS_BIT(7)
#define AD74416H_SLEW_EN_MSK			NO_OS_GENMASK(6, 5)
#define AD74416H_SLEW_LIN_STEP_MSK		NO_OS_GENMASK(4, 3)
#define AD74416H_SLEW_LIN_RATE_MSK		NO_OS_GENMASK(2, 1)
#define AD74416H_I_LIMIT_MSK			NO_OS_BIT(0)

/** RTD_CONFIG Register */
#define AD74416H_RTD_ADC_REF_MSK		NO_OS_BIT(3)
#define AD74416H_RTD_MODE_SEL_MSK		NO_OS_BIT(2)
#define AD74416H_RTD_EXC_SWAP_MSK		NO_OS_BIT(1)
#define AD74416H_RTD_CURRENT_MSK		NO_OS_BIT(0)

/** FET_LKG_COMP Register */
#define AD74416H_FET_SRC_LKG_COMP_EN_MSK	NO_OS_BIT(0)

/** DO_EXT_CONFIG Register */
#define AD74416H_DO_T2_MSK			NO_OS_GENMASK(12, 8)
#define AD74416H_DO_DATA_MSK			NO_OS_BIT(7)
#define AD74416H_DO_T1_MSK			NO_OS_GENMASK(6, 2)
#define AD74416H_DO_SRC_SEL_MSK			NO_OS_BIT(1)
#define AD74416H_DO_MODE_MSK			NO_OS_BIT(0)

/** I_BURNOUT_CONFIG Register */
#define AD74416H_BRN_SEN_VSENSEN_CURR_MSK	NO_OS_GENMASK(6, 5)
#define AD74416H_BRN_SEN_VSENSEN_POL_MSK	NO_OS_BIT(4)
#define AD74416H_BRN_VIOUT_CURR_MSK		NO_OS_GENAMSK(2, 1)
#define AD74416H_BRN_VIOUT_POL_MSK		NO_OS_BIT(0)

/** DAC_CODE Register*/
#define AD74416H_DAC_CODE_MSK			NO_OS_GENMASK(15, 0)

/** DAC_ACTIVE Register */
#define AD74416H_DAC_ACTIVE_MSK			NO_OS_GENMASK(15, 0)

/** ADC_CONV_CTRL Register */
#define AD74416H_ADC_RDY_CTRL_MSK		NO_OS_BIT(12)
#define AD74416H_CONV_RATE_DIAG_MSK		NO_OS_GENMASK(11, 10)
#define AD74416H_CONV_SEQ_MSK			NO_OS_GENMASK(9, 8)
#define AD74416H_DIAG_EN_MSK(x)			(NO_OS_BIT(x) << 4)
#define AD74416H_CH_EN_MSK(x)			NO_OS_BIT(x)

/** DIAG_ASSIGN register */
#define AD74416H_DIAG_ASSIGN_MSK(x)		(NO_OS_GENMASK(3, 0) << ((x) * 4))

/** GPIO_CONFIGx register */
#define AD74416H_DIN_DO_MSK			NO_OS_BIT(7, 6)
#define AD74416H_GPI_DATA_MSK			NO_OS_BIT(5)
#define AD74416H_GPO_DATA_MSK			NO_OS_BIT(4)
#define AD74416H_GP_WK_PD_EN_MSK		NO_OS_BIT(3)
#define AD74416H_GPIO_SELECT_MSK		NO_OS_GENMASK(2, 0)

/** PWR_OPTIM_CONFIG Register */
#define AD74416H_REF_EN_MSK			NO_OS_BIT(13)
#define AD74416H_SENSE_AGND_OPT_MSK		NO_OS_BIT(12)
#define AD74416H_SENSE_HF_OPT_D_MSK		NO_OS_BIT(11)
#define AD74416H_SENSE_HF_OPT_C_MSK		NO_OS_BIT(10)
#define AD74416H_SENSE_HF_OPT_B_MSK		NO_OS_BIT(9)
#define AD74416H_SENSE_HF_OPT_A_MSK		NO_OS_BIT(8)
#define AD74416H_SENSE_LF_OPT_D_MSK		NO_OS_BIT(7)
#define AD74416H_SENSE_LF_OPT_C_MSK		NO_OS_BIT(6)
#define AD74416H_SENSE_LF_OPT_B_MSK		NO_OS_BIT(5)
#define AD74416H_SENSE_LF_OPT_A_MSK		NO_OS_BIT(4)
#define AD74416H_VSENSEN_OPT_D_MSK		NO_OS_BIT(3)
#define AD74416H_VSENSEN_OPT_C_MSK		NO_OS_BIT(2)
#define AD74416H_VSENSEN_OPT_B_MSK		NO_OS_BIT(1)
#define AD74416H_VSENSEN_OPT_A_MSK		NO_OS_BIT(0)

/** WDT_CONFIG Register */
#define AD74416H_WDT_EN_MSK			NO_OS_BIT(4)
#define AD74416H_WDT_TIMEOUT_MSK		NO_OS_GENMASK(3, 0)

/** DIN_COMP_OUT Register */
#define AD74416H_DIN_COMP_OUT_D_MSK		NO_OS_BIT(3)
#define AD74416H_DIN_COMP_OUT_C_MSK		NO_OS_BIT(2)
#define AD74416H_DIN_COMP_OUT_B_MSK		NO_OS_BIT(1)
#define AD74416H_DIN_COMP_OUT_A_MSK		NO_OS_BIT(0)

/** ALERT_STATUS Register */
#define AD74416H_HART_ALERT_D_MSK		NO_OS_BIT(15)
#define AD74416H_HART_ALERT_C_MSK		NO_OS_BIT(14)
#define AD74416H_HART_ALERT_B_MSK		NO_OS_BIT(13)
#define AD74416H_HART_ALERT_A_MSK		NO_OS_BIT(12)
#define AD74416H_CHANNEL_ALERT_D_MSK		NO_OS_BIT(11)
#define AD74416H_CHANNEL_ALERT_C_MSK		NO_OS_BIT(10)
#define AD74416H_CHANNEL_ALERT_B_MSK		NO_OS_BIT(9)
#define AD74416H_CHANNEL_ALERT_A_MSK		NO_OS_BIT(8)
#define AD74416H_ADC_ERR_MSK			NO_OS_BIT(5)
#define AD74416H_TEMP_ALERT_MSK			NO_OS_BIT(4)
#define AD74416H_SPI_ERR_MSK			NO_OS_BIT(3)
#define AD74416H_SUPPLY_ERR_MSK			NO_OS_BIT(2)
#define AD74416H_WDT_RESET_OCCURRED_MSK		NO_OS_BIT(1)
#define AD74416H_RESET_OCCURRED_MSK		NO_OS_BIT(0)

/** LIVE_STATUS Register */
#define AD74416H_ANALOG_IO_STATUS_D_MSK		NO_OS_BIT(15)
#define AD74416H_ANALOG_IO_STATUS_C_MSK		NO_OS_BIT(14)
#define AD74416H_ANALOG_IO_STATUS_B_MSK		NO_OS_BIT(13)
#define AD74416H_ANALOG_IO_STATUS_A_MSK		NO_OS_BIT(12)
#define AD74416H_DO_STATUS_D_MSK		NO_OS_BIT(11)
#define AD74416H_DO_STATUS_C_MSK		NO_OS_BIT(10)
#define AD74416H_DO_STATUS_B_MSK		NO_OS_BIT(9)
#define AD74416H_DO_STATUS_A_MSK		NO_OS_BIT(8)
#define AD74416H_DIN_STATUS_D_MSK		NO_OS_BIT(7)
#define AD74416H_DIN_STATUS_C_MSK		NO_OS_BIT(6)
#define AD74416H_DIN_STATUS_B_MSK		NO_OS_BIT(5)
#define AD74416H_DIN_STATUS_A_MSK		NO_OS_BIT(4)
#define AD74416H_TEMP_ALERT_STATUS_MSK		NO_OS_BIT(3)
#define AD74416H_ADC_DATA_RDY_MSK		NO_OS_BIT(2)
#define AD74416H_ADC_BUSY_MSK			NO_OS_BIT(1)
#define AD74416H_SUPPLY_STATUS_MSK		NO_OS_BIT(0)

/* AD74416H_ADC_RESULT_UPRn Register */
#define AD74416H_CONV_RES_MUX_MSK		NO_OS_GENMASK(15, 13)
#define AD74416H_CONV_RES_RANGE_MSK		NO_OS_GENMASK(12, 10)
#define AD74416H_CONV_SEQ_COUNT_MSK		NO_OS_GENAMSK(9. 8)
#define AD74416H_CONV_RES_UPR_MSK		NO_OS_GENMASK(7, 0)

/* AD74416H_ADC_RESULTn Register */
#define AD74416H_CONV_RESULT_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H_ADC_DIAG_RESULTn */
#define AD74416H_DIAG_RESULT_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H_LAST_ADC_RESULT_UPRn Register */
#define AD74416H_LAST_CONV_CH_MSK		NO_OS_GENAMSK(10 8)
#define AD74416H_LAST_CONV_RES_UPR_MSK		NO_OS_GENMASK(7, 0)

/* AD74416H_ADC_RESULTn Register */
#define AD74416H_LAST_CONV_RES_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H_DIN_COUNTER_UPRn Register */
#define AD74416H_DIN_CNT_UPR_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H_DIN_COUNTERn Register */
#define AD74416H_DIN_CNT_MSK			NO_OS_GENMASK(15, 0)

/* AD74416H SUPPLY_ALERT_STATUS Register */
#define AD74416H_AVDD_HI_ERR_MSK		NO_OS_BIT(6)
#define AD74416H_AVDD_LO_ERR_MSK		NO_OS_BIT(5)
#define AD74416H_DO_VDD_ERR_MSK			NO_OS_BIT(4)
#define AD74416H_AVCC_ERR_MSK			NO_OS_BIT(3)
#define AD74416H_DVCC_ERR_MSK			NO_OS_BIT(2)
#define AD74416H_AVSS_ERR_MSK			NO_OS_BIT(1)
#define AD74416H_CAL_MEM_ERR_MSK		NO_OS_BIT(0)

/* AD74416H CHANNEL_ALERT_STATUS Register */
#define AD74416H_ANALOG_IO_OC_MSK		NO_OS_BIT(5)
#define AD74416H_ANALOG_IO_SC_MSK		NO_OS_BIT(4)
#define AD74416H_DO_TIMEOUT_MSK			NO_OS_BIT(3)
#define AD74416H_DO_SC_MSK			NO_OS_BIT(2)
#define AD74416H_DIN_OC_MSK			NO_OS_BIT(1)
#define AD74416H_DIN_SC_MSK			NO_OS_BIT(0)

/* AD74416H READ_SELECT Register */
#define AD74416H_READBACK_ADDR_MSK		NO_OS_GENMASK(8, 0)

/* AD74416H BURST_READ_SEL Register */
#define AD74416H_BURST_READ_SEL_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H THERM_RST Register */
#define AD74416H_THERM_RST_EN_MSK		NO_OS_BIT(0)

/* AD74416H CMD_KEY Register */
#define AD74416H_CMD_KEY_MSK			NO_OS_GENMASK(15, 0)

/* AD74416H BROADCAST_CMD_KEY Register */
#define AD74416H_BROADCAST_CMD_KEY_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H SCRATCHn Register */
#define AD74416H_SCRATCH_BITS_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H GENERIC_ID Register */
#define AD74416H_GENERIC_ID_MSK			NO_OS_GENMASK(2, 0)

/* AD74416H SILICON_REV Register */
#define AD74416H_SILICON_REV_ID_MSK		NO_OS_GENMASK(7, 0)

/* AD74416H SILICON_ID0 Register */
#define AD74416H_UID0_MSK			NO_OS_GENMASK(6, 0)

/* AD74416H SILICON_ID1 Register */
#define AD74416H_UID2_MSK			NO_OS_GENMASK(11, 6)
#define AD74416H_UID1_MSK			NO_OS_GENMASK(5, 0)

/* AD74416H HART_ALERT_STATUSn Register */
#define AD74416H_FRM_MON_STATE_MSK		NO_OS_GENMASK(15, 13)
#define AD74416H_EOM_MSK			NO_OS_BIT(12)
#define AD74416H_RX_BCNT_MSK			NO_OS_BIT(11)
#define AD74416H_RX_CMD_MSK			NO_OS_BIT(10)
#define AD74416H_SOM_MSK			NO_OS_BIT(9)
#define AD74416H_CD_MSK				NO_OS_BIT(8)
#define AD74416H_CD_EDGE_DET_MSK		NO_OS_BIT(7)
#define AD74416H_TX_COMPLETE_MSK		NO_OS_BIT(6)
#define AD74416H_TX_FIFO_ALERT_MSK		NO_OS_BIT(5)
#define AD74416H_RX_FIFO_ALERT_MSK		NO_OS_BIT(4)
#define AD74416H_RX_OVERFLOW_ERR_MSK		NO_OS_BIT(3)
#define AD74416H_FRAME_ERR_MSK			NO_OS_BIT(2)
#define AD74416H_PARITY_ERR_MSK			NO_OS_BIT(1)
#define AD74416H_GAP_ERR_MSK			NO_OS_BIT(0)

/* AD74416H HART_RXn Register */
#define AD74416H_RFGI_MSK			NO_OS_BIT(11)
#define AD74416H_RFFE_MSK			NO_OS_BIT(10)
#define AD74416H_RFPE_MSK			NO_OS_BIT(9)
#define AD74416H_RFBI_MSK			NO_OS_BIT(8)
#define AD74416H_RBR_MSK			NO_OS_GENMASK(7, 0)

/* AD74416H HART_TXn Register */
#define AD74416H_TDR_MSK			NO_OS_GENMASK(7, 0)

/* AD74416H HART_FCRn Register */
#define AD74416H_TFTRIG_MSK			NO_OS_GENMASK(11, 8)
#define AD74416H_RFTRIG_MSK			NO_OS_GENMASK(6, 3)
#define AD74416H_TFCLR_MSK			NO_OS_BIT(2)
#define AD74416H_RFCLR_MSK			NO_OS_BIT(1)
#define AD74416H_FIFOEN_MSK			NO_OS_BIT(0)

/* AD74416H HART_MCRn Register */
#define AD74416H_RTS_MSK			NO_OS_BIT(0)

/* AD74416H HART_RFCn Register */
#define AD74416H_RFC_MSK			NO_OS_GENMASK(4, 0)

/* AD74416H HART_TFCn Register */
#define AD74416H_TFC_MSK			NO_OS_GENMASK(4, 0)

/* AD74416H HART_CONFIGn Register */
#define AD74416H_CD_EXTD_QUAL_MSK		NO_OS_BIT(13)
#define AD74416H_FRM_MON_RX_PREMX2_MSK		NO_OS_BIT(12)
#define AD74416H_FRM_MON_RST_GAP_MSK		NO_OS_BIT(11)
#define AD74416H_FRM_MON_RST_CD_MSK		NO_OS_BIT(10)
#define AD74416H_RX_ALL_CHARS_MSK		NO_OS_BIT(9)
#define AD74416H_FRM_MON_EN_MSK			NO_OS_BIT(8)
#define AD74416H_EVENT_DET_SEL_MSK		NO_OS_GENMASK(7, 6)
#define AD74416H_TX_1B_AFTER_RST_MSK		NO_OS_BIT(5)
#define AD74416H_AUTO_CLR_RST_MSK		NO_OS_BIT(4)
#define AD74416H_CD_EDGE_SEL_MSK		NO_OS_GENAMSK(3, 2)
#define AD74416H_MODEM_DUPLEX_MSK		NO_OS_BIT(1)
#define AD74416H_MODEM_PWRUP_MSK		NO_OS_BIT(0)

/* AD74416H HART_TX_PREMn Register */
#define AD74416H_TX_PREM_CNT_MSK		NO_OS_GENMASK(4, 0)

/* AD74416H HART_EVDETn Register */
#define AD74416H_EVENT_DET_TIME_MSK		NO_OS_GENMASK(15, 0)

/* AD74416H HART_TX_GAINn Register */
#define AD74416H_TX_GAIN_MSK			NO_OS_GENMASK(3, 0)

/* AD74416H HART_GPIO_IF_CONFIG Register */
#define AD74416H_HART_GPIO_IF_CH_MSK		NO_OS_GENMASK(3, 2)
#define AD74416H_HART_GPIO_IF_SEL_MSK		NO_OS_GENMASK(1, 0)

/* AD74416H HART_GPIO_MON_CONFIG Register */
#define AD74416H_HART_GPIO_MON_CH_MSK		NO_OS_GENMASK(4, 3)
#define AD74416H_HART_GPIO_MON_SEL_MSK		NO_OS_GENMASK(2, 0)

#define AD74416H_TEMP_OFFSET			-2392
#define AD74416H_TEMP_SCALE			8950
#define AD74416H_TEMP_SCALE_DIV			1000

#define AD74416H_FRAME_SIZE 			5
#define AD74416H_THRESHOLD_DAC_RANGE		98
#define AD74416H_THRESHOLD_RANGE		30000
#define AD74416H_DAC_RANGE			12000
#define AD74416H_DAC_CURRENT_RANGE		25000
#define AD74416H_DAC_RESOLUTION			16
#define AD74414H_DAC_RESOLUTION			14
#define AD74116H_CONV_TIME_US			1000000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Supported device ids.
*/
enum ad74416h_dev_id {
	ID_AD74416H,
	ID_AD74414H,
};

/**
 * @brief Operation modes of the device.
 */
enum ad74416h_op_mode {
	AD74416H_HIGH_Z = 0x0,
	AD74416H_VOLTAGE_OUT = 0x1,
	AD74416H_CURRENT_OUT = 0x2,
	AD74416H_VOLTAGE_IN = 0x3,
	AD74416H_CURRENT_IN_EXT = 0x4,
	AD74416H_CURRENT_IN_LOOP = 0x5,
	AD74416H_RESISTANCE = 0x7,
	AD74416H_DIGITAL_INPUT = 0x8,
	AD74416H_DIGITAL_INPUT_LOOP = 0x9,
	AD74416H_CURRENT_OUT_HART = 0x10,
	AD74416H_CURRENT_IN_EXT_HART = 0x11,
	AD74416H_CURRENT_IN_LOOP_HART = 0x12,
};

/**
 * @brief GPO operation modes.
 */
enum ad74416h_gpio_select {
	AD74416H_GPIO_CONFIG_HIGH_Z,
	AD74416H_GPIO_CONFIG_DATA,
	AD74416H_GPIO_CONFIG_IN,
	AD74416H_GPIO_CONFIG_COMP,
	AD74416H_GPIO_CONFIG_DO
};

/**
 * @brief Conversion range configuration values
 */
enum ad74416h_adc_range {
	AD74416H_RNG_0_12_V,
	AD74416H_RNG_NEG12_12_V,
	AD74416H_RNG_NEG0P3125_0P3125V,
	AD74416H_RNG_NEG0P3125_0V,
	AD74416H_RNG_0_0P3125V,
	AD74416H_RNG_0_0P625V,
	AD74416H_RNG_NEG104_104MV,
	AD74416H_RNG_NEG2P5_2P5V,
};

/**
 * @brief Conversion configuration values.
 */
enum ad74416h_adc_rate {
	AD74416H_10SPS_50_60HZ_HART_REJECTION,
	AD74416H_20SPS_50_60HZ_REJECTION,
	AD74416H_1K2SPS,
	AD74416H_4K8SPS,
	AD74416H_9K6SPS,
	AD74416H_19K2SPS,
	AD74416H_200SPS_HART_REJECTION,
};

/**
 * @brief ADC input configuration values.
 */
enum ad74416h_adc_conv_mux {
	AD74416H_MUX_LF_TO_AGND,
	AD74416H_MUX_HF_TO_LF,
	AD74416H_MUX_VSENSEN_TO_AGND,
	AD74416H_MUX_LF_TO_VSENSEN,
	AD74416H_MUX_AGND_TO_AGND,
};

/**
 * @brief Debounce modes for the IOx inputs when using the digital input op mode.
 */
enum ad74416h_debounce_mode {
	AD74416H_DEBOUNCE_MODE_0,
	AD74416H_DEBOUNCE_MODE_1
};

/**
 * @brief ADC conversion sequence commands.
 */
enum ad74416h_conv_seq {
	AD74416H_STOP_PWR_UP,
	AD74416H_START_SINGLE,
	AD74416H_START_CONT,
	AD74416H_STOP_PWR_DOWN,
};

/**
 * @brief Possible values to be loaded in the DIAG_RESULT register
 */
enum ad74416h_diag_mode {
	AD74416H_DIAG_AGND,
	AD74416H_DIAG_TEMP,
	AD74416H_DIAG_DVCC,
	AD74416H_DIAG_AVCC,
	AD74416H_DIAG_LDO1V8,
	AD74416H_DIAG_AVDD_HI,
	AD74416H_DIAG_AVDD_LO,
	AD74416H_DIAG_AVSS,
	AD74416H_DIAG_LVIN,
	AD74416H_DIAG_DO_VDD,
	AD74416H_VSENSEP_C,
	AD74416H_VSENSEN_C,
	AD74416H_DO_C,
	AD74416H_AVDD_C,
};

/**
 * @brief The voltage step size of Full Scale DAC Voltage.
 */
enum ad74416h_slew_lin_step {
	AD74416H_STEP_0_8_PERCENT,
	AD74416H_STEP_1_5_PERCENT,
	AD74416H_STEP_6_1_PERCENT,
	AD74416H_STEP_22_2_PERCENT,
};

/**
 * @brief Possible update rates for a DAC when slew control is enabled
 */
enum ad74416h_lin_rate {
	AD74416H_LIN_RATE_4KHZ8,
	AD74416H_LIN_RATE_76KHZ8,
	AD74416H_LIN_RATE_153KHZ6,
	AD74416H_LIN_RATE_230KHZ4,
};

/**
 * @brief Possible voltage output ranges for the DAC
 */
enum ad74416h_vout_range {
	AD74416H_VOUT_RANGE_0_12V,
	AD74416H_VOUT_RANGE_NEG12_12V,
};

/**
 * @brief DAC Current limits in Vout mode
 */
enum ad74416h_i_limit {
	AD74416H_I_LIMIT0,
	AD74416H_I_LIMIT1,
};

/**
 * @brief Bitfield struct which maps on the LIVE_STATUS register
 */
struct _ad74416h_live_status {
	uint8_t SUPPLY_STATUS: 1;
	uint8_t ADC_BUSY: 1;
	uint8_t ADC_DATA_RDY: 1;
	uint8_t TEMP_ALERT_STATUS: 1;
	uint8_t DIN_STATUS_A: 1;
	uint8_t DIN_STATUS_B: 1;
	uint8_t DIN_STATUS_C: 1;
	uint8_t DIN_STATUD_D: 1;
	uint8_t DO_STATUS_A: 1;
	uint8_t DO_STATUS_B: 1;
	uint8_t DO_STATUS_C: 3;
	uint8_t DO_STATUS_D: 1;
	uint8_t ANALOG_IO_STATUS_A: 1;
	uint8_t ANALOG_IO_STATUS_B: 1;
	uint8_t ANALOG_IO_STATUS_C: 1;
	uint8_t ANALOG_IO_STATUS_D: 1;
};

/**
 * @brief Used to store the live status bit fields.
 */
union ad74416h_live_status {
	struct _ad74416h_live_status status_bits;
	uint16_t value;
};

struct ad74416h_init_param {
	enum ad74416h_dev_id id;
	uint8_t dev_addr;
	struct no_os_spi_init_param spi_ip;
	struct no_os_gpio_init_param *reset_gpio_param;
};

/**
 * @brief Device channel state.
 */

struct ad74416h_channel_config {
	bool enabled;
	enum ad74416h_op_mode function;
	enum ad74416h_vout_range vout_range;
	enum ad74416h_i_limit i_limit;
};

/**
 * @brief AD74416h device descriptor.
 */
struct ad74416h_desc {
	enum ad74416h_dev_id id;
	uint8_t dev_addr;
	struct no_os_spi_desc *spi_desc;
	uint8_t comm_buff[AD74416H_FRAME_SIZE];
	struct ad74416h_channel_config channel_configs[AD74416H_N_CHANNELS];
	struct no_os_gpio_desc *reset_gpio;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Converts a millivolt value in the corresponding DAC 13 bit code */
int ad74416h_dac_voltage_to_code(struct ad74416h_desc *, int32_t,
				 uint16_t *, uint32_t);

/** Converts a microamp value in the corresponding DAC 16 bit code */
int ad74416h_dac_current_to_code(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Write a register's value */
int ad74416h_reg_write(struct ad74416h_desc *, uint32_t, uint16_t);

/** Read a raw communication frame */
int ad74416h_reg_read_raw(struct ad74416h_desc *, uint32_t, uint8_t *);

/** Read a register's value */
int ad74416h_reg_read(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Update a register's field */
int ad74416h_reg_update(struct ad74416h_desc *, uint32_t, uint16_t,
			uint16_t);

/** Get the number of active channels */
int ad74416h_nb_active_channels(struct ad74416h_desc *, uint8_t *);

/**
 * Select which information the device will respond with (in the readback field)
 * when a read operation is performed
 */
int ad74416h_set_info(struct ad74416h_desc *desc, uint16_t mode);

/** Set the operation mode for a specific channel */
int ad74416h_set_channel_function(struct ad74416h_desc *,
				  uint32_t, enum ad74416h_op_mode);

/** Set the voltage range for a specific channel */
int ad74416h_set_channel_vout_range(struct ad74416h_desc *desc, uint32_t ch,
				    enum ad74416h_vout_range vout_range);

/** Set the current limit for a specific DAC channel in vout mode */
int ad74416h_set_channel_i_limit(struct ad74416h_desc *, uint32_t,
				 enum ad74416h_i_limit);

/** Read the raw ADC raw conversion value */
int ad74416h_get_raw_adc_result(struct ad74416h_desc *, uint32_t,
				uint32_t *);

/** Enable/disable a specific ADC channel */
int ad74416h_set_adc_channel_enable(struct ad74416h_desc *, uint32_t,
				    bool);

/** Enable conversions on a diagnostic register */
int ad74416h_set_diag_channel_enable(struct ad74416h_desc *, uint32_t, bool);

/** Get the ADC measurement range for a specific channel */
int ad74416h_get_adc_range(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Set the ADC measurement range for a specific channel */
int ad74416h_set_adc_range(struct ad74416h_desc *, uint32_t,
			   enum ad74416h_adc_range);

/** Get the ADC sample rate. */
int ad74416h_get_adc_rate(struct ad74416h_desc *, uint32_t,
			  enum ad74416h_adc_rate *);

/** Set the ADC sample rate. */
int ad74416h_set_adc_rate(struct ad74416h_desc *, uint32_t,
			  enum ad74416h_adc_rate);

/** Get the ADC input node */
int ad74416h_get_adc_conv_mux(struct ad74416h_desc *, uint32_t,
			      enum ad74416h_adc_conv_mux *);

/** Set the ADC input node */
int ad74416h_set_adc_conv_mux(struct ad74416h_desc *, uint32_t,
			      enum ad74416h_adc_conv_mux);

/** Start or stop ADC conversions */
int ad74416h_set_adc_conv_seq(struct ad74416h_desc *, enum ad74416h_conv_seq);

/** Get a single ADC raw value for a specific channel, then power down the ADC */
int ad74416h_get_adc_single(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Read the die's temperature from the diagnostic register */
int ad74416h_get_temp(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Set and load a code for the DAC on a specific channel */
int ad74416h_set_channel_dac_code(struct ad74416h_desc *, uint32_t, uint16_t);

/** Set which diagnostic value to be loaded in the DIAG_RESULT register */
int ad74416h_set_diag(struct ad74416h_desc *, uint32_t,
		      enum ad74416h_diag_mode);

/** Get the diagnostic value for a specific channel */
int ad74416h_get_diag(struct ad74416h_desc *, uint32_t, uint16_t *);

/**
 * Set the debounce mode for the IOx inputs when the ADC is running in digital
 * input mode.
 */
int ad74416h_set_debounce_mode(struct ad74416h_desc *, uint32_t,
			       enum ad74416h_debounce_mode);

/**
 * Set the debounce settle time for the IOx inputs when the ADC is
 * running in digital input mode
 */
int ad74416h_set_debounce_time(struct ad74416h_desc *, uint32_t, uint32_t);

/** Get the GPIO value for a specific channel */
int ad74416h_gpio_get(struct ad74416h_desc *, uint32_t, uint8_t *);

/** Set the GPIO operation mode */
int ad74416h_set_gpio_config(struct ad74416h_desc *, uint32_t,
			     enum ad74416h_gpio_select);

/**
 * Set the threshold, for which a signal would be considered high,
 * when the ADC is running in digital input mode
 */
int ad74416h_set_threshold(struct ad74416h_desc *, uint32_t, uint32_t);

/** Set the DO value of a channel */
int ad74416h_do_set(struct ad74416h_desc *, uint32_t, uint8_t);

/** Set the logic value of a GPO pin */
int ad74416h_gpio_set(struct ad74416h_desc *, uint32_t, uint8_t);

/** Read the live status bits */
int ad74416h_get_live(struct ad74416h_desc *,
		      union ad74416h_live_status *);

/** Configure and enable slew rate control for a DAC on a specific channel */
int ad74416h_dac_slew_enable(struct ad74416h_desc *, uint32_t,
			     enum ad74416h_slew_lin_step,
			     enum ad74416h_lin_rate);

/** Disable the slew rate control */
int ad74416h_dac_slew_disable(struct ad74416h_desc *, uint32_t);

/** Enable or disable the higher thermal reset */
int ad74416h_set_therm_rst(struct ad74416h_desc *, bool);

/** Perform a soft reset */
int ad74416h_reset(struct ad74416h_desc *);

/** Initialize the device structure */
int ad74416h_init(struct ad74416h_desc **, struct ad74416h_init_param *);

/** Free the device descriptor */
int ad74416h_remove(struct ad74416h_desc *desc);

#endif // _AD74416H_H
