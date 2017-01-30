/***************************************************************************//**
 * @file ad9144.h
 * @brief Header file of AD9144 Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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
#ifndef AD9144_H_
#define AD9144_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define REG_SPI_INTFCONFA			0x000 /* Interface configuration A */
#define REG_SPI_INTFCONFB			0x001 /* Interface configuration B */
#define REG_SPI_DEVCONF				0x002 /* Device Configuration */
#define REG_SPI_PRODIDL				0x004 /* Product Identification Low Byte */
#define REG_SPI_PRODIDH				0x005 /* Product Identification High Byte */
#define REG_SPI_CHIPGRADE			0x006 /* Chip Grade */
#define REG_SPI_PAGEINDX			0x008 /* Page Pointer or Device Index */
#define REG_SPI_DEVINDX2			0x009 /* Secondary Device Index */
#define REG_SPI_SCRATCHPAD			0x00A /* Scratch Pad */
#define REG_SPI_MS_UPDATE			0x00F /* Master/Slave Update Bit */
#define REG_PWRCNTRL0				0x011 /* Power Control Reg 1 */
#define REG_TXENMASK1				0x012 /* TXenable masks */
#define REG_PWRCNTRL3				0x013 /* Power control register 3 */
#define REG_COARSE_GROUP_DLY			0x014 /* Coarse Group Delay Adjustment */
#define REG_IRQ_ENABLE0				0x01F /* Interrupt Enable */
#define REG_IRQ_ENABLE1				0x020 /* Interrupt Enable */
#define REG_IRQ_ENABLE2				0x021 /* Interrupt Enable */
#define REG_IRQ_ENABLE3				0x022 /* Interrupt Enable */
#define REG_IRQ_STATUS0				0x023 /* Interrupt Status */
#define REG_IRQ_STATUS1				0x024 /* Interrupt Status */
#define REG_IRQ_STATUS2				0x025 /* Interrupt Status */
#define REG_IRQ_STATUS3				0x026 /* Interrupt Status */
#define REG_JESD_CHECKS				0x030 /* JESD Parameter Checking */
#define REG_SYNC_TESTCTRL			0x031 /* Sync Control Reg0 */
#define REG_SYNC_DACDELAY_L			0x032 /* Sync Logic DacDelay [7:0] */
#define REG_SYNC_DACDELAY_H			0x033 /* Sync Logic DacDelay [8] */
#define REG_SYNC_ERRWINDOW			0x034 /* Sync Error Window */
#define REG_SYNC_DLYCOUNT			0x035 /* Sync Control Ref Delay Count */
#define REG_SYNC_REFCOUNT			0x036 /* Sync SysRef InActive Interval */
#define REG_SYNC_LASTERR_L			0x038 /* SyncLASTerror_L */
#define REG_SYNC_LASTERR_H			0x039 /* SyncLASTerror_H */
#define REG_SYNC_CTRL				0x03A /* Sync Mode Control */
#define REG_SYNC_STATUS				0x03B /* Sync Alignment Flags */
#define REG_SYNC_CURRERR_L			0x03C /* Sync Alignment Error[7:0] */
#define REG_SYNC_CURRERR_H			0x03D /* Sync Alignment Error[8] */
#define REG_ERROR_THERM				0x03E /* Sync Error Thermometer */
#define REG_DACGAIN0_1				0x040 /* MSBs of Full Scale Adjust DAC */
#define REG_DACGAIN0_0				0x041 /* LSBs of Full Scale Adjust DAC */
#define REG_DACGAIN1_1				0x042 /* MSBs of Full Scale Adjust DAC */
#define REG_DACGAIN1_0				0x043 /* LSBs of Full Scale Adjust DAC */
#define REG_DACGAIN2_1				0x044 /* MSBs of Full Scale Adjust DAC */
#define REG_DACGAIN2_0				0x045 /* LSBs of Full Scale Adjust DAC */
#define REG_DACGAIN3_1				0x046 /* MSBs of Full Scale Adjust DAC */
#define REG_DACGAIN3_0				0x047 /* LSBs of Full Scale Adjust DAC */
#define REG_PD_DACLDO				0x048 /* Powerdown DAC LDOs */
#define REG_STAT_DACLDO				0x049 /* DAC LDO Status */
#define REG_DECODE_CTRL0			0x04B /* Decoder Control */
#define REG_DECODE_CTRL1			0x04C /* Decoder Control */
#define REG_DECODE_CTRL2			0x04D /* Decoder Control */
#define REG_DECODE_CTRL3			0x04E /* Decoder Control */
#define REG_NCO_CLRMODE				0x050 /* NCO CLR Mode */
#define REG_NCOKEY_ILSB				0x051 /* NCO Clear on Data Key I lsb */
#define REG_NCOKEY_IMSB				0x052 /* NCO Clear on Data Key I msb */
#define REG_NCOKEY_QLSB				0x053 /* NCO Clear on Data Key Q lsb */
#define REG_NCOKEY_QMSB				0x054 /* NCO Clear on Data Key Q msb */
#define REG_PA_THRES0				0x060 /* PDP Threshold */
#define REG_PA_THRES1				0x061 /* PDP Threshold */
#define REG_PA_AVG_TIME				0x062 /* PDP Control */
#define REG_PA_POWER0				0x063 /* PDP Power */
#define REG_PA_POWER1				0x064 /* PDP Power */
#define REG_CLKCFG0				0x080 /* Clock Configuration */
#define REG_SYSREF_ACTRL0			0x081 /* SYSREF Analog Control 0 */
#define REG_SYSREF_ACTRL1			0x082 /* SYSREF Analog Control 1 */
#define REG_DACPLLCNTRL				0x083 /* Top Level Control DAC Clock PLL */
#define REG_DACPLLSTATUS			0x084 /* DAC PLL Status Bits */
#define REG_DACINTEGERWORD0			0x085 /* Feedback divider tuning word */
#define REG_DACLOOPFILT1			0x087 /* C1 and C2 control */
#define REG_DACLOOPFILT2			0x088 /* R1 and C3 control */
#define REG_DACLOOPFILT3			0x089 /* Bypass and R2 control */
#define REG_DACCPCNTRL				0x08A /* Charge Pump/Cntrl Voltage */
#define REG_DACLOGENCNTRL			0x08B /* Logen Control */
#define REG_DACLDOCNTRL1			0x08C /* LDO Control1 + Reference Divider */
#define REG_CAL_DAC_ERR				0x0E0 /* Report DAC Cal errors */
#define REG_CAL_MSB_THRES			0x0E1 /* MSB sweep Threshold definition */
#define REG_CAL_CTRL_GLOBAL			0x0E2 /* Global Calibration DAC Control */
#define REG_CAL_MSBHILVL			0x0E3 /* High Level for MSB level compare */
#define REG_CAL_MSBLOLVL			0x0E4 /* Low Level for MSB level compare */
#define REG_CAL_THRESH				0x0E5 /* TAC Threshold definition */
#define REG_CAL_AVG_CNT				0x0E6 /* CAL DAC Number of averages */
#define REG_CAL_CLKDIV				0x0E7 /* Calibration DAC clock divide */
#define REG_CAL_INDX				0x0E8 /* Calibration DAC Select */
#define REG_CAL_CTRL				0x0E9 /* Calibration DAC Control */
#define REG_CAL_ADDR				0x0EA /* Calibration DAC Address */
#define REG_CAL_DATA				0x0EB /* Calibration DAC Data */
#define REG_CAL_UPDATE				0x0EC /* Calibration DAC Write Update */
#define REG_DATA_FORMAT				0x110 /* Data format */
#define REG_DATAPATH_CTRL			0x111 /* Datapath Control */
#define REG_INTERP_MODE				0x112 /* Interpolation Mode */
#define REG_NCO_FTW_UPDATE			0x113 /* NCO Frequency Tuning Word Update */
#define REG_FTW0				0x114 /* NCO Frequency Tuning Word LSB */
#define REG_FTW1				0x115 /* NCO Frequency Tuning Word */
#define REG_FTW2				0x116 /* NCO Frequency Tuning Word */
#define REG_FTW3				0x117 /* NCO Frequency Tuning Word */
#define REG_FTW4				0x118 /* NCO Frequency Tuning Word */
#define REG_FTW5				0x119 /* NCO Frequency Tuning Word MSB */
#define REG_NCO_PHASE_OFFSET0			0x11A /* NCO Phase Offset LSB */
#define REG_NCO_PHASE_OFFSET1			0x11B /* NCO Phase Offset MSB */
#define REG_NCO_PHASE_ADJ0			0x11C /* I/Q Phase Adjust LSB */
#define REG_NCO_PHASE_ADJ1			0x11D /* I/Q Phase Adjust MSB */
#define REG_TXEN_FUNC				0x11E /* Transmit Enable function */
#define REG_TXEN_SM_0				0x11F /* Transmit enable power control state machine */
#define REG_TXEN_SM_1				0x120 /* Rise and fall */
#define REG_TXEN_SM_2				0x121 /* Transmit enable maximum A */
#define REG_TXEN_SM_3				0x122 /* Transmit enable maximum B */
#define REG_TXEN_SM_4				0x123 /* Transmit enable maximum C */
#define REG_TXEN_SM_5				0x124 /* Transmit enable maximum D */
#define REG_DACOUT_ON_DOWN			0x125 /* DAC out down control and on trigger */
#define REG_DACOFF				0x12C /* DAC Shutdown Source */
#define REG_DATA_PATH_FLUSH_COUNT0		0x12D /* Data path flush counter LSB */
#define REG_DATA_PATH_FLUSH_COUNT1		0x12E /* Data path flush counter MSB */
#define REG_DIE_TEMP_CTRL0			0x12F /* Die Temp Range Control */
#define REG_DIE_TEMP_CTRL1			0x130 /* Die temperature control register */
#define REG_DIE_TEMP_CTRL2			0x131 /* Die temperature control register */
#define REG_DIE_TEMP0				0x132 /* Die temp LSB */
#define REG_DIE_TEMP1				0x133 /* Die Temp MSB */
#define REG_DIE_TEMP_UPDATE			0x134 /* Die temperature update */
#define REG_DC_OFFSET_CTRL			0x135 /* DC Offset Control */
#define REG_IPATH_DC_OFFSET_1PART0		0x136 /* LSB of first part of DC Offset value for I path */
#define REG_IPATH_DC_OFFSET_1PART1		0x137 /* MSB of first part of DC Offset value for I path */
#define REG_QPATH_DC_OFFSET_1PART0		0x138 /* LSB of first part of DC Offset value for Q path */
#define REG_QPATH_DC_OFFSET_1PART1		0x139 /* MSB of first part of DC Offset value for Q path */
#define REG_IPATH_DC_OFFSET_2PART		0x13A /* Second part of DC Offset value for I path */
#define REG_QPATH_DC_OFFSET_2PART		0x13B /* Second part of DC Offset value for Q path */
#define REG_IDAC_DIG_GAIN0			0x13C /* I DAC Gain LSB */
#define REG_IDAC_DIG_GAIN1			0x13D /* I DAC Gain MSB */
#define REG_QDAC_DIG_GAIN0			0x13E /* Q DAC Gain LSB */
#define REG_QDAC_DIG_GAIN1			0x13F /* Q DAC Gain MSB */
#define REG_GAIN_RAMP_UP_STP0			0x140 /* LSB of digital gain rises */
#define REG_GAIN_RAMP_UP_STP1			0x141 /* MSB of digital gain rises */
#define REG_GAIN_RAMP_DOWN_STP0			0x142 /* LSB of digital gain drops */
#define REG_GAIN_RAMP_DOWN_STP1			0x143 /* MSB of digital gain drops */
#define REG_BLSM_CTRL				0x146 /* Blanking SM control and func */
#define REG_BLSM_STAT				0x147 /* Blanking SM control and func */
#define REG_PRBS				0x14B /* PRBS Input Data Checker */
#define REG_PRBS_ERROR_I			0x14C /* PRBS Error Counter Real */
#define REG_PRBS_ERROR_Q			0x14D /* PRBS Error Counter Imaginary */
#define REG_DACPLLT5				0x1B5 /* ALC/Varactor control */
#define REG_DACPLLTB				0x1BB /* VCO Bias Control */
#define REG_DACPLLTD				0x1BD /* VCO Cal control */
#define REG_DACPLLT17				0x1C4 /* Varactor ControlV */
#define REG_ASPI_SPARE0				0x1C6 /* Spare Register 0 */
#define REG_ASPI_SPARE1				0x1C7 /* Spare Register 1 */
#define REG_SPISTRENGTH				0x1DF /* Reg 70 Description */
#define REG_CLK_TEST				0x1EB /* Clock related control signaling */
#define REG_ATEST_VOLTS				0x1EC /* Analog Test Voltage Extraction */
#define REG_ASPI_CLKSRC				0x1ED /* Analog Spi clock source for PD machines */
#define REG_MASTER_PD				0x200 /* Master power down for Receiver PHYx */
#define REG_PHY_PD				0x201 /* Power down for individual Receiver PHYx */
#define REG_GENERIC_PD				0x203 /* Miscellaneous power down controls */
#define REG_CDR_OPERATING_MODE_REG_0		0x230 /* Clock and data recovery operating modes */
#define REG_EQ_CONFIG_PHY_0_1			0x250 /* Equalizer configuration for PHY 0 and PHY 1 */
#define REG_EQ_CONFIG_PHY_2_3			0x251 /* Equalizer configuration for PHY 2 and PHY 3 */
#define REG_EQ_CONFIG_PHY_4_5			0x252 /* Equalizer configuration for PHY 4 and PHY 5 */
#define REG_EQ_CONFIG_PHY_6_7			0x253 /* Equalizer configuration for PHY 6 and PHY 7 */
#define REG_EQ_BIAS_REG				0x268 /* Equalizer bias control */
#define REG_SYNTH_ENABLE_CNTRL			0x280 /* Rx PLL enable controls */
#define REG_PLL_STATUS				0x281 /* Rx PLL status readbacks */
#define REG_REF_CLK_DIVIDER_LDO			0x289 /* Rx PLL LDO control */
#define REG_TERM_BLK1_CTRLREG0			0x2A7 /* Termination controls for PHYs 0, 1, 6, and 7 */
#define REG_TERM_BLK1_CTRLREG1			0x2A8 /* Termination controls for PHYs 0, 1, 6, and 7 */
#define REG_TERM_BLK2_CTRLREG0			0x2AE /* Termination controls for PHYs 2, 3, 4, and 5 */
#define REG_TERM_BLK2_CTRLREG1			0x2AF /* Termination controls for PHYs 2, 3, 4, and 5 */
#define REG_GENERAL_JRX_CTRL_0			0x300 /* General JRX Control Register 0 */
#define REG_GENERAL_JRX_CTRL_1			0x301 /* General JRX Control Register 1 */
#define REG_DYN_LINK_LATENCY_0			0x302 /* Register 1 description */
#define REG_DYN_LINK_LATENCY_1			0x303 /* Register 2 description */
#define REG_LMFC_DELAY_0			0x304 /* Register 3 description */
#define REG_LMFC_DELAY_1			0x305 /* Register 4 description */
#define REG_LMFC_VAR_0				0x306 /* Register 5 description */
#define REG_LMFC_VAR_1				0x307 /* Register 6 description */
#define REG_XBAR_LN_0_1				0x308 /* Register 7 description */
#define REG_XBAR_LN_2_3				0x309 /* Register 8 description */
#define REG_XBAR_LN_4_5				0x30A /* Register 9 description */
#define REG_XBAR_LN_6_7				0x30B /* Register 10 description */
#define REG_FIFO_STATUS_REG_0			0x30C /* Register 11 description */
#define REG_FIFO_STATUS_REG_1			0x30D /* Register 12 description */
#define REG_FIFO_STATUS_REG_2			0x30E /* Register 13 description */
#define REG_SYNCB_GEN_0				0x311 /* Register 16 description */
#define REG_SYNCB_GEN_1				0x312 /* Register 17 description */
#define REG_SYNCB_GEN_3				0x313 /* Register 18 description */
#define REG_PHY_PRBS_TEST_EN			0x315 /* PHY PRBS TEST ENABLE FOR INDIVIDUAL LANES */
#define REG_PHY_PRBS_TEST_CTRL			0x316 /* Reg 20 Description */
#define REG_PHY_PRBS_TEST_THRESH_LOBITS		0x317 /* Reg 21 Description */
#define REG_PHY_PRBS_TEST_THRESH_MIDBITS	0x318 /* Reg 22 Description */
#define REG_PHY_PRBS_TEST_THRESH_HIBITS		0x319 /* Reg 23 Description */
#define REG_PHY_PRBS_TEST_ERRCNT_LOBITS		0x31A /* Reg 24 Description */
#define REG_PHY_PRBS_TEST_ERRCNT_MIDBITS	0x31B /* Reg 25 Description */
#define REG_PHY_PRBS_TEST_ERRCNT_HIBITS		0x31C /* Reg 26 Description */
#define REG_PHY_PRBS_TEST_STATUS		0x31D /* Reg 27 Description */
#define REG_SHORT_TPL_TEST_0			0x32C /* Reg 46 Description */
#define REG_SHORT_TPL_TEST_1			0x32D /* Reg 47 Description */
#define REG_SHORT_TPL_TEST_2			0x32E /* Reg 48 Description */
#define REG_SHORT_TPL_TEST_3			0x32F /* Reg 49 Description */
#define REG_JESD_BIT_INVERSE_CTRL		0x334 /* Reg 42 Description */
#define REG_DID_REG				0x400 /* Reg 0 Description */
#define REG_BID_REG				0x401 /* Reg 1 Description */
#define REG_LID0_REG				0x402 /* Reg 2 Description */
#define REG_SCR_L_REG				0x403 /* Reg 3 Description */
#define REG_F_REG				0x404 /* Reg 4 Description */
#define REG_K_REG				0x405 /* Reg 5 Description */
#define REG_M_REG				0x406 /* Reg 6 Description */
#define REG_CS_N_REG				0x407 /* Reg 7 Description */
#define REG_NP_REG				0x408 /* Reg 8 Description */
#define REG_S_REG				0x409 /* Reg 9 Description */
#define REG_HD_CF_REG				0x40A /* Reg 10 Description */
#define REG_RES1_REG				0x40B /* Reg 11 Description */
#define REG_RES2_REG				0x40C /* Reg 12 Description */
#define REG_CHECKSUM_REG			0x40D /* Reg 13 Description */
#define REG_COMPSUM0_REG			0x40E /* Reg 14 Description */
#define REG_LID1_REG				0x412 /* Reg 18 Description */
#define REG_CHECKSUM1_REG			0x415 /* Reg 19 Description */
#define REG_COMPSUM1_REG			0x416 /* Reg 22 Description */
#define REG_LID2_REG				0x41A /* Reg 26 Description */
#define REG_CHECKSUM2_REG			0x41D /* Reg 29 Description */
#define REG_COMPSUM2_REG			0x41E /* Reg 30 Description */
#define REG_LID3_REG				0x422 /* Reg 34 Description */
#define REG_CHECKSUM3_REG			0x425 /* Reg 37 Description */
#define REG_COMPSUM3_REG			0x426 /* Reg 38 Description */
#define REG_LID4_REG				0x42A /* Reg 34 Description */
#define REG_CHECKSUM4_REG			0x42D /* Reg 37 Description */
#define REG_COMPSUM4_REG			0x42E /* Reg 38 Description */
#define REG_LID5_REG				0x432 /* Reg 34 Description */
#define REG_CHECKSUM5_REG			0x435 /* Reg 37 Description */
#define REG_COMPSUM5_REG			0x436 /* Reg 38 Description */
#define REG_LID6_REG				0x43A /* Reg 34 Description */
#define REG_CHECKSUM6_REG			0x43D /* Reg 37 Description */
#define REG_COMPSUM6_REG			0x43E /* Reg 38 Description */
#define REG_LID7_REG				0x442 /* Reg 34 Description */
#define REG_CHECKSUM7_REG			0x445 /* Reg 37 Description */
#define REG_COMPSUM7_REG			0x446 /* Reg 38 Description */
#define REG_ILS_DID				0x450 /* Reg 80 Description */
#define REG_ILS_BID				0x451 /* Reg 81 Description */
#define REG_ILS_LID0				0x452 /* Reg 82 Description */
#define REG_ILS_SCR_L				0x453 /* Reg 83 Description */
#define REG_ILS_K				0x455 /* Reg 85 Description */
#define REG_ILS_M				0x456 /* Reg 86 Description */
#define REG_ILS_CS_N				0x457 /* Reg 87 Description */
#define REG_ILS_NP				0x458 /* Reg 88 Description */
#define REG_ILS_S				0x459 /* Reg 89 Description */
#define REG_ILS_HD_CF				0x45A /* Reg 90 Description */
#define REG_ILS_RES1				0x45B /* Reg 91 Description */
#define REG_ILS_RES2				0x45C /* Reg 92 Description */
#define REG_ILS_CHECKSUM			0x45D /* Reg 93 Description */
#define REG_ERRCNTRMON				0x46B /* Reg 107 Description */
#define REG_LANEDESKEW				0x46C /* Reg 108 Description */
#define REG_BADDISPARITY			0x46D /* Reg 109 Description */
#define REG_NITDISPARITY			0x46E /* Reg 110 Description */
#define REG_UNEXPECTEDKCHAR			0x46F /* Reg 111 Description */
#define REG_CODEGRPSYNCFLG			0x470 /* Reg 112 Description */
#define REG_FRAMESYNCFLG			0x471 /* Reg 113 Description */
#define REG_GOODCHKSUMFLG			0x472 /* Reg 114 Description */
#define REG_INITLANESYNCFLG			0x473 /* Reg 115 Description */
#define REG_CTRLREG1				0x476 /* Reg 118 Description */
#define REG_CTRLREG2				0x477 /* Reg 119 Description */
#define REG_KVAL				0x478 /* Reg 120 Description */
#define REG_IRQVECTOR				0x47A /* Reg 122 Description */
#define REG_SYNCASSERTIONMASK			0x47B /* Reg 123 Description */
#define REG_ERRORTHRES				0x47C /* Reg 124 Description */
#define REG_LANEENABLE				0x47D /* Reg 125 Description */

/*
 *	REG_SPI_INTFCONFA
 */
#define SOFTRESET_M				(1 << 7) /* Soft Reset (Mirror) */
#define LSBFIRST_M				(1 << 6) /* LSB First (Mirror) */
#define ADDRINC_M				(1 << 5) /* Address Increment (Mirror) */
#define SDOACTIVE_M				(1 << 4) /* SDO Active (Mirror) */
#define SDOACTIVE				(1 << 3) /* SDO Active */
#define ADDRINC					(1 << 2) /* Address Increment */
#define LSBFIRST				(1 << 1) /* LSB First */
#define SOFTRESET				(1 << 0) /* Soft Reset */

/*
 *	REG_SPI_INTFCONFB
 */
#define SINGLEINS				(1 << 7) /* Single Instruction */
#define CSBSTALL				(1 << 6) /* CSb Stalling */

/*
 *	REG_SPI_DEVCONF
 */
#define DEVSTATUS(x)				(((x) & 0xF) << 4) /* Device Status */
#define CUSTOPMODE(x)				(((x) & 0x3) << 2) /* Customer Operating Mode */
#define SYSOPMODE(x)				(((x) & 0x3) << 0) /* System Operating Mode */

/*
 *	REG_SPI_CHIPGRADE
 */
#define PROD_GRADE(x)				(((x) & 0xF) << 4) /* Product Grade */
#define DEV_REVISION(x)				(((x) & 0xF) << 0) /* Device Revision */

/*
 *	REG_SPI_PAGEINDX
 */
#define PAGEINDX(x)				(((x) & 0x3) << 0) /* Page or Index Pointer */

/*
 *	REG_SPI_MS_UPDATE
 */
#define SLAVEUPDATE				(1 << 0) /* M/S Update Bit */

/*
 *	REG_PWRCNTRL0
 */
#define PD_BG					(1 << 7) /* Reference PowerDown */
#define PD_DAC_0				(1 << 6) /* PD Ichannel DAC 0 */
#define PD_DAC_1				(1 << 5) /* PD Qchannel DAC 1 */
#define PD_DAC_2				(1 << 4) /* PD Ichannel DAC 2 */
#define PD_DAC_3				(1 << 3) /* PD Qchannel DAC 3 */
#define PD_DACM					(1 << 2) /* PD Dac master Bias */

/*
 *	REG_TXENMASK1
 */
#define SYS_MASK				(1 << 2) /* SYSREF Receiver TXen mask */
#define DACB_MASK				(1 << 1) /* Dual B Dac TXen1 mask */
#define DACA_MASK				(1 << 0) /* Dual A Dac TXen0 mask */

/*
 *	REG_PWRCNTRL3
 */
#define ENA_PA_CTRL_FROM_PAPROT_ERR		(1 << 6) /* Control PDP enable from PAProt block */
#define ENA_PA_CTRL_FROM_TXENSM			(1 << 5) /* Control PDP enable from Txen State machine */
#define ENA_PA_CTRL_FROM_BLSM			(1 << 4) /* Control PDP enable from Blanking state machine */
#define ENA_PA_CTRL_FROM_SPI			(1 << 3) /* Control PDP enable via SPI */
#define SPI_PA_CTRL				(1 << 2) /* PDP on/off via SPI */
#define ENA_SPI_TXEN				(1 << 1) /* TXEN from SPI control */
#define SPI_TXEN				(1 << 0) /* Spi TXEN */

/*
 *	REG_COARSE_GROUP_DLY
 */
#define COARSE_GROUP_DLY(x)			(((x) & 0xF) << 0) /* Coarse group delay */

/*
 *	REG_IRQ_ENABLE0
 */
#define EN_CALPASS				(1 << 7) /* Enable Calib PASS detection */
#define EN_CALFAIL				(1 << 6) /* Enable Calib FAIL detection */
#define EN_DACPLLLOST				(1 << 5) /* Enable DAC Pll Lost detection */
#define EN_DACPLLLOCK				(1 << 4) /* Enable DAC Pll Lock detection */
#define EN_SERPLLLOST				(1 << 3) /* Enable Serdes PLL Lost detection */
#define EN_SERPLLLOCK				(1 << 2) /* Enable  Serdes PLL Lock detection */
#define EN_LANEFIFOERR				(1 << 1) /* Enable Lane FIFO Error detection */
#define EN_DRDLFIFOERR				(1 << 0) /* Enable DRDL FIFO Error detection */

/*
 *	REG_IRQ_ENABLE1
 */
#define EN_PARMBAD				(1 << 7) /* enable BAD Parameter interrupt */
#define EN_PRBSQ1				(1 << 3) /* enable PRBS imag DAC B interrupt */
#define EN_PRBSI1				(1 << 2) /* enable PRBS real DAC B interrupt */
#define EN_PRBSQ0				(1 << 1) /* enable PRBS imag DAC A interrupt */
#define EN_PRBSI0				(1 << 0) /* enable PRBS real DAC A interrupt */

/*
 *	REG_IRQ_ENABLE2
 */
#define EN_PAERR0				(1 << 7) /* Link A PA Error */
#define EN_BIST_DONE0				(1 << 6) /* Link A BIST done */
#define EN_BLNKDONE0				(1 << 5) /* Link A Blanking done */
#define EN_REFNCOCLR0				(1 << 4) /* Link A Nco Clear Tripped */
#define EN_REFLOCK0				(1 << 3) /* Link A Alignment Locked */
#define EN_REFROTA0				(1 << 2) /* Link A Alignment Rotate */
#define EN_REFWLIM0				(1 << 1) /* Link A Over/Under Threshold */
#define EN_REFTRIP0				(1 << 0) /* Link A Alignment Trip */

/*
 *	REG_IRQ_ENABLE3
 */
#define EN_PAERR1				(1 << 7) /* Link B PA Error */
#define EN_BIST_DONE1				(1 << 6) /* Link B BIST done */
#define EN_BLNKDONE1				(1 << 5) /* Link B Blanking done */
#define EN_REFNCOCLR1				(1 << 4) /* Link B Nco Clear Tripped */
#define EN_REFLOCK1				(1 << 3) /* Link B Alignment Locked */
#define EN_REFROTA1				(1 << 2) /* Link B Alignment Rotate */
#define EN_REFWLIM1				(1 << 1) /* Link B Over/Under Threshold */
#define EN_REFTRIP1				(1 << 0) /* Link B Alignment Trip */

/*
 *	REG_IRQ_STATUS0
 */
#define IRQ_CALPASS				(1 << 7) /* Calib PASS detection */
#define IRQ_CALFAIL				(1 << 6) /* Calib FAIL detection */
#define IRQ_DACPLLLOST				(1 << 5) /* DAC PLL Lost */
#define IRQ_DACPLLLOCK				(1 << 4) /* DAC PLL Lock */
#define IRQ_SERPLLLOST				(1 << 3) /* Serdes PLL Lost */
#define IRQ_SERPLLLOCK				(1 << 2) /* Serdes PLL Lock */
#define IRQ_LANEFIFOERR				(1 << 1) /* Lane Fifo Error */
#define IRQ_DRDLFIFOERR				(1 << 0) /* DRDL Fifo Error */

/*
 *	REG_IRQ_STATUS1
 */
#define IRQ_PARMBAD				(1 << 7) /* BAD Parameter interrupt */
#define IRQ_PRBSQ1				(1 << 3) /* PRBS data check error DAC 1 imag */
#define IRQ_PRBSI1				(1 << 2) /* PRBS data check error DAC 1 real */
#define IRQ_PRBSQ0				(1 << 1) /* PRBS data check error DAC 0 imag */
#define IRQ_PRBSI0				(1 << 0) /* PRBS data check error DAC 0 real */

/*
 *	REG_IRQ_STATUS2
 */
#define IRQ_PAERR0				(1 << 7) /* Link A PA Error */
#define IRQ_BIST_DONE0				(1 << 6) /* Link A BIST done */
#define IRQ_BLNKDONE0				(1 << 5) /* Link A Blanking Done */
#define IRQ_REFNCOCLR0				(1 << 4) /* Link A Alignment UnderRange */
#define IRQ_REFLOCK0				(1 << 3) /* Link A BIST done */
#define IRQ_REFROTA0				(1 << 2) /* Link A Alignment Trip */
#define IRQ_REFWLIM0				(1 << 1) /* Link A Alignment Lock */
#define IRQ_REFTRIP0				(1 << 0) /* Link A Alignment Rotate */

/*
 *	REG_IRQ_STATUS3
 */
#define IRQ_PAERR1				(1 << 7) /* Link B PA Error */
#define IRQ_BIST_DONE1				(1 << 6) /* Link B BIST done */
#define IRQ_BLNKDONE1				(1 << 5) /* Link A Blanking Done */
#define IRQ_REFNCOCLR1				(1 << 4) /* Link B Alignment UnderRange */
#define IRQ_REFLOCK1				(1 << 3) /* Link B BIST done */
#define IRQ_REFROTA1				(1 << 2) /* Link B Alignment Trip */
#define IRQ_REFWLIM1				(1 << 1) /* Link B Alignment Lock */
#define IRQ_REFTRIP1				(1 << 0) /* Link B Alignment Rotate */

/*
 *	REG_JESD_CHECKS
 */
#define ERR_DLYOVER				(1 << 5) /* LMFC_Delay > JESD_K parameter */
#define ERR_WINLIMIT				(1 << 4) /* Unsupported Window Limit */
#define ERR_JESDBAD				(1 << 3) /* Unsupported M/L/S/F selection */
#define ERR_KUNSUPP				(1 << 2) /* Unsupported K values */
#define ERR_SUBCLASS				(1 << 1) /* Unsupported SubClassv value */
#define ERR_INTSUPP				(1 << 0) /* Unsupported Interpolation rate factor */

/*
 *	REG_SYNC_TESTCTRL
 */
#define TARRFAPHAZ				(1 << 0) /* Target Polarity of Rf Divider */
#define SYNCBYPASS(x)				(((x) & 0x3) << 6) /* Sync Bypass handshaking */

/*
 *	REG_SYNC_DACDELAY_H
 */
#define DAC_DELAY_H				(1 << 0) /* Dac Delay[8] */

/*
 *	REG_SYNC_ERRWINDOW
 */
#define ERRWINDOW(x)				(((x) & 0x7) << 0) /* Sync Error Window */

/*
 *	REG_SYNC_LASTERR_H
 */
#define LASTUNDER				(1 << 7) /* Sync Last Error Under Flag */
#define LASTOVER				(1 << 6) /* Sync Last Error Over Flag */
#define LASTERROR_H				(1 << 0) /* Sync Last Error[8] and Flags */

/*
 *	REG_SYNC_CTRL
 */
#define SYNCENABLE				(1 << 7) /* SyncLogic Enable */
#define SYNCARM					(1 << 6) /* Sync Arming Strobe */
#define SYNCCLRSTKY				(1 << 5) /* Sync Sticky Bit Clear */
#define SYNCCLRLAST				(1 << 4) /* Sync Clear LAST_ */
#define SYNCMODE(x)				(((x) & 0xF) << 0) /* Sync Mode */

/*
 *	REG_SYNC_STATUS
 */
#define REFBUSY					(1 << 7) /* Sync Machine Busy */
#define REFLOCK					(1 << 3) /* Sync Alignment Locked */
#define REFROTA					(1 << 2) /* Sync Rotated */
#define REFWLIM					(1 << 1) /* Sync Alignment Limit Range */
#define REFTRIP					(1 << 0) /* Sync Tripped after Arming */

/*
 *	REG_SYNC_CURRERR_H
 */
#define CURRUNDER				(1 << 7) /* Sync Current Error Under Flag */
#define CURROVER				(1 << 6) /* Sync Current Error Over Flag */
#define CURRERROR_H				(1 << 0) /* SyncCurrent Error[8] */

/*
 *	REG_ERROR_THERM
 */
#define THRMOLD					(1 << 7) /* Error is from a prior sample */
#define THRMOVER				(1 << 4) /* Error > +WinLimit */
#define THRMPOS					(1 << 3) /* Sync Current Error Under Flag */
#define THRMZERO				(1 << 2) /* Error = 0 */
#define THRMNEG					(1 << 1) /* Error < 0 */
#define THRMUNDER				(1 << 0) /* Error < -WinLimit */

/*
 *	REG_DACGAIN0_1
 */
#define DACGAIN_IM0(x)				(((x) & 0x3) << 0) /* I Channel DAC gain <9:8> Dual A */

/*
 *	REG_DACGAIN1_1
 */
#define DACGAIN_IM1(x)				(((x) & 0x3) << 0) /* Q Channel DAC gain <9:8> Dual A */

/*
 *	REG_DACGAIN2_1
 */
#define DACGAIN_IM2(x)				(((x) & 0x3) << 0) /* I Channel DAC gain <9:8> Dual B */

/*
 *	REG_DACGAIN3_1
 */
#define DACGAIN_IM3(x)				(((x) & 0x3) << 0) /* Q Channel DAC gain <9:8> Dual B */

/*
 *	REG_PD_DACLDO
 */
#define ENB_DACLDO3				(1 << 7) /* Disable DAC3 ldo */
#define ENB_DACLDO2				(1 << 6) /* Disable DAC2 ldo */
#define ENB_DACLDO1				(1 << 5) /* Disable DAC1 ldo */
#define ENB_DACLDO0				(1 << 4) /* Disable DAC0 ldo */

/*
 *	REG_STAT_DACLDO
 */
#define STAT_LDO3				(1 << 3) /* DAC3 LDO status */
#define STAT_LDO2				(1 << 2) /* DAC2 LDO status */
#define STAT_LDO1				(1 << 1) /* DAC1 LDO status */
#define STAT_LDO0				(1 << 0) /* DAC0 LDO status */

/*
 *	REG_DECODE_CTRL0
 */
#define SHUFFLE_MSB0				(1 << 2) /* MSB shuffling mode */
#define SHUFFLE_ISB0				(1 << 1) /* ISB shuffling mode */

/*
 *	REG_DECODE_CTRL1
 */
#define SHUFFLE_MSB1				(1 << 2) /* MSB shuffling mode */
#define SHUFFLE_ISB1				(1 << 1) /* ISB shuffling mode */

/*
 *	REG_DECODE_CTRL2
 */
#define SHUFFLE_MSB2				(1 << 2) /* MSB shuffling mod */
#define SHUFFLE_ISB2				(1 << 1) /* ISB shuffling mode */

/*
 *	REG_DECODE_CTRL3
 */
#define SHUFFLE_MSB3				(1 << 2) /* MSB shuffling mode */
#define SHUFFLE_ISB3				(1 << 1) /* ISB shuffling mode */

/*
 *	REG_NCO_CLRMODE
 */
#define NCOCLRARM				(1 << 7) /* Arm NCO Clear */
#define NCOCLRMTCH				(1 << 5) /* NCO Clear Data Match */
#define NCOCLRPASS				(1 << 4) /* NCO Clear PASSed */
#define NCOCLRFAIL				(1 << 3) /* NCO Clear FAILed */
#define NCOCLRMODE(x)				(((x) & 0x3) << 0) /* NCO Clear Mode */

/*
 *	REG_PA_THRES1
 */
#define PA_THRESH_MSB(x)			(((x) & 0x1F) << 0) /* Average power threshold for comparison. */

/*
 *	REG_PA_AVG_TIME
 */
#define PA_ENABLE				(1 << 7) /* 1 = Enable average power calculation and error detection */
#define PA_BUS_SWAP				(1 << 6) /* Swap channelA or channelB databus for power calculation */
#define PA_AVG_TIME(x)				(((x) & 0xF) << 0) /* Set power average time */

/*
 *	REG_PA_POWER1
 */
#define PA_POWER_MSB(x)				(((x) & 0x1F) << 0) /* average power bus = I^2+Q^2 (I/Q use 6MSB of databus) */

/*
 *	REG_CLKCFG0
 */
#define PD_CLK01				(1 << 7) /* Powerdown clock for Dual A */
#define PD_CLK23				(1 << 6) /* Powerdown clock for Dual B */
#define PD_CLK_DIG				(1 << 5) /* Powerdown clocks to all DACs */
#define PD_PCLK					(1 << 4) /* Cal reference/Serdes PLL clock powerdown */
#define PD_CLK_REC				(1 << 3) /* Clock reciever powerdown */

/*
 *	REG_SYSREF_ACTRL0
 */
#define PD_SYSREF				(1 << 4) /* Powerdown SYSREF buffer */
#define HYS_ON					(1 << 3) /* Hysteresis enabled */
#define SYSREF_RISE				(1 << 2) /* Use SYSREF rising edge */
#define HYS_CNTRL1(x)				(((x) & 0x3) << 0) /* Hysteresis control bits <9:8> */

/*
 *	REG_DACPLLCNTRL
 */
#define SYNTH_RECAL				(1 << 7) /* Recalibrate VCO Band */
#define ENABLE_SYNTH				(1 << 4) /* Synthesizer Enable */

/*
 *	REG_DACPLLSTATUS
 */
#define CP_CAL_VALID				(1 << 5) /* Charge Pump Cal Valid */
#define RFPLL_LOCK				(1 << 1) /* PLL Lock bit */

/*
 *	REG_DACLOOPFILT1
 */
#define LF_C2_WORD(x)				(((x) & 0xF) << 4) /* C2 control word */
#define LF_C1_WORD(x)				(((x) & 0xF) << 0) /* C1 control word */

/*
 *	REG_DACLOOPFILT2
 */
#define LF_R1_WORD(x)				(((x) & 0xF) << 4) /* R1 control word */
#define LF_C3_WORD(x)				(((x) & 0xF) << 0) /* C3 control word */

/*
 *	REG_DACLOOPFILT3
 */
#define LF_BYPASS_R3				(1 << 7) /* Bypass R3 res */
#define LF_BYPASS_R1				(1 << 6) /* Bypass R1 res */
#define LF_BYPASS_C2				(1 << 5) /* Bypass C2 cap */
#define LF_BYPASS_C1				(1 << 4) /* Bypass C1 cap */
#define LF_R3_WORD(x)				(((x) & 0xF) << 0) /* R3 Control Word */

/*
 *	REG_DACCPCNTRL
 */
#define CP_CURRENT(x)				(((x) & 0x3F) << 0) /* Charge Pump Current Control */

/*
 *	REG_DACLOGENCNTRL
 */
#define LO_DIV_MODE(x)				(((x) & 0x3) << 0) /* Logen_Division */

/*
 *	REG_DACLDOCNTRL1
 */
#define REF_DIVRATE(x)				(((x) & 0x7) << 0) /* Reference Clock Division Ratio */

/*
 *	REG_CAL_DAC_ERR
 */
#define INIT_SWEEP_ERR_DAC			(1 << 1) /* Initial setup sweep failed */
#define MSB_SWEEP_ERR_DAC			(1 << 0) /* MSB sweep failed */

/*
 *	REG_CAL_MSB_THRES
 */
#define CAL_MSB_TAC(x)				(((x) & 0x7) << 0) /* MSB sweep TAC */

/*
 *	REG_CAL_CTRL_GLOBAL
 */
#define CAL_START_GL				(1 << 1) /* Global Calibration start */
#define CAL_EN_GL				(1 << 0) /* Global Calibration enable */

/*
 *	REG_CAL_MSBHILVL
 */
#define CAL_MSBLVLHI(x)				(((x) & 0x3F) << 0) /* High level limit for msb sweep average */

/*
 *	REG_CAL_MSBLOLVL
 */
#define CAL_MSBLVLLO(x)				(((x) & 0x3F) << 0) /* Low level limit for msb sweep average */

/*
 *	REG_CAL_THRESH
 */
#define CAL_LTAC_THRES(x)			(((x) & 0x7) << 3) /* Long TAC threshold */
#define CAL_TAC_THRES(x)			(((x) & 0x7) << 0) /* TAC threshold */

/*
 *	REG_CAL_AVG_CNT
 */
#define MSB_GLOBAL_SUBAVG(x)			(((x) & 0x3) << 6) /* Local Averages for MSB in Global Calibration */
#define GLOBAL_AVG_CNT(x)			(((x) & 0x7) << 3) /* Global avg Terminal count */
#define LOCAL_AVRG_CNT(x)			(((x) & 0x7) << 0) /* Local avg terminal count */

/*
 *	REG_CAL_CLKDIV
 */
#define CAL_CLKDIV(x)				(((x) & 0xF) << 0) /* Calibration clock divider */

/*
 *	REG_CAL_INDX
 */
#define CAL_INDX(x)				(((x) & 0xF) << 0) /* DAC Calibration Index paging bits */

/*
 *	REG_CAL_CTRL
 */
#define CAL_FIN					(1 << 7) /* Calibration finished */
#define CAL_ACTIVE				(1 << 6) /* Calibration active */
#define CAL_ERRHI				(1 << 5) /* SAR data error: too hi */
#define CAL_ERRLO				(1 << 4) /* SAR data error: too lo */
#define CAL_TXDACBYDAC				(1 << 3) /* Calibration of TXDAC by TXDAC */
#define CAL_START				(1 << 1) /* Calibration start */
#define CAL_EN					(1 << 0) /* Calibration enable */

/*
 *	REG_CAL_ADDR
 */
#define CAL_ADDR(x)				(((x) & 0x3F) << 0) /* Calibration DAC address */

/*
 *	REG_CAL_DATA
 */
#define CAL_DATA(x)				(((x) & 0x3F) << 0) /* Calibration DAC Coefficient Data */

/*
 *	REG_CAL_UPDATE
 */
#define CAL_UPDATE				(1 << 7) /* Calibration DAC Coefficient Update */

/*
 *	REG_DATA_FORMAT
 */
#define BINARY_FORMAT				(1 << 7) /* Binary or 2's complementary format on DATA bus */

/*
 *	REG_DATAPATH_CTRL
 */
#define INVSINC_ENABLE				(1 << 7) /* 1 = Enable inver sinc filter */
#define DIG_GAIN_ENABLE				(1 << 5) /* 1 = Enable digital gain */
#define PHASE_ADJ_ENABLE			(1 << 4) /* 1 = Enable phase compensation */
#define SEL_SIDEBAND				(1 << 1) /* 1 = Select upper or lower sideband from modulation result */
#define I_TO_Q					(1 << 0) /* 1 = send I datapath into Q DAC */
#define MODULATION_TYPE(x)			(((x) & 0x3) << 2) /* selects type of modulation operation */

/*
 *	REG_INTERP_MODE
 */
#define INTERP_MODE(x)				(((x) & 0x7) << 0) /* Interpolation Mode */

/*
 *	REG_NCO_FTW_UPDATE
 */
#define FTW_UPDATE_ACK				(1 << 1) /* Frequency Tuning Word Update Acknowledge */
#define FTW_UPDATE_REQ				(1 << 0) /* Frequency Tuning Word Update Request from SPI */

/*
 *	REG_TXEN_FUNC
 */
#define TX_DIG_CLK_PD				(1 << 0) /* 1 = Digital clocks will be shut down when Tx_enable pin is low. */

/*
 *	REG_TXEN_SM_0
 */
#define GP_PA_ON_INVERT				(1 << 2) /* External Modulator polarity invert */
#define GP_PA_CTRL				(1 << 1) /* External PA control */
#define TXEN_SM_EN				(1 << 0) /* Enable TXEN state machine */
#define PA_FALL(x)				(((x) & 0x3) << 6) /* PA fall control */
#define PA_RISE(x)				(((x) & 0x3) << 4) /* PA rises control */

/*
 *	REG_TXEN_SM_1
 */
#define DIG_FALL(x)				(((x) & 0x3) << 6) /* DIG_FALL */
#define DIG_RISE(x)				(((x) & 0x3) << 4) /* DIG_RISE */
#define DAC_FALL(x)				(((x) & 0x3) << 2) /* DAC_FALL */
#define DAC_RISE(x)				(((x) & 0x3) << 0) /* DAC_RISE */

/*
 *	REG_DACOUT_ON_DOWN
 */
#define DACOUT_SHUTDOWN				(1 << 1) /* Shut down DAC output. 1 means DAC get shut down manually. */
#define DACOUT_ON_TRIGGER			(1 << 0) /* Turn on DAC output manually. Self clear signal. */

/*
 *	REG_DACOFF
 */
#define PROTECT_MODE				(1 << 7) /* PROTECT_MODE */
#define DACOFF_AVG_PW				(1 << 0) /* DACOFF_AVG_PW */

/*
 *	REG_DIE_TEMP_CTRL0
 */
#define ADC_TESTMODE				(1 << 7) /* ADC_TESTMODE */
#define AUXADC_ENABLE				(1 << 0) /* AUXADC_ENABLE */
#define FS_CURRENT(x)				(((x) & 0x7) << 4) /* FS_CURRENT */
#define REF_CURRENT(x)				(((x) & 0x7) << 1) /* REF_CURRENT */

/*
 *	REG_DIE_TEMP_CTRL1
 */
#define SELECT_CLKDIG				(1 << 3) /* SELECT_CLKDIG */
#define EN_DIV2					(1 << 2) /* EN_DIV2 */
#define INCAP_CTRL(x)				(((x) & 0x3) << 0) /* INCAP_CTRL */

/*
 *	REG_DIE_TEMP_UPDATE
 */
#define DIE_TEMP_UPDATE				(1 << 0) /* Die temperature update */

/*
 *	REG_DC_OFFSET_CTRL
 */
#define DISABLE_NOISE				(1 << 1) /* DISABLE_NOISE */
#define DC_OFFSET_ON				(1 << 0) /* DC_OFFSET_ON */

/*
 *	REG_IPATH_DC_OFFSET_2PART
 */
#define IPATH_DC_OFFSET_2PART(x)		(((x) & 0x1F) << 0) /* second part of DC Offset value for I path */

/*
 *	REG_QPATH_DC_OFFSET_2PART
 */
#define QPATH_DC_OFFSET_2PART(x)		(((x) & 0x1F) << 0) /* second part of DC Offset value for Q path */

/*
 *	REG_IDAC_DIG_GAIN1
 */
#define IDAC_DIG_GAIN1(x)			(((x) & 0xF) << 0) /* MSB of I DAC digital gain */

/*
 *	REG_QDAC_DIG_GAIN1
 */
#define QDAC_DIG_GAIN1(x)			(((x) & 0xF) << 0) /* MSB of Q DAC digital gain */

/*
 *	REG_GAIN_RAMP_UP_STP1
 */
#define GAIN_RAMP_UP_STP1(x)			(((x) & 0xF) << 0) /* MSB of digital gain rises */

/*
 *	REG_GAIN_RAMP_DOWN_STP1
 */
#define GAIN_RAMP_DOWN_STP1(x)			(((x) & 0xF) << 0) /* MSB of digital gain drops */

/*
 *	REG_BLSM_CTRL
 */
#define RESET_BLSM				(1 << 7) /* Soft rest to the new Blanking SM */
#define EN_FORCE_GAIN_SOFT_OFF			(1 << 4) /* Enable forcing gan_soft_off from SPI */
#define GAIN_SOFT_OFF				(1 << 3) /* gain_soft_off forced value */
#define GAIN_SOFT_ON				(1 << 2) /* gain_soft_on forced value */
#define EN_FORCE_GAIN_SOFT_ON			(1 << 1) /* Force the gain_soft_on from SPI */

/*
 *	REG_BLSM_STAT
 */
#define SOFT_OFF_DONE				(1 << 5) /* Blanking SoftOff Enable */
#define SOFT_ON_DONE				(1 << 4) /* Blanking SoftOn Done */
#define GAIN_SOFT_OFF_RB			(1 << 3) /* gain soft off readback */
#define GAIN_SOFT_ON_RB				(1 << 2) /* gain soft on readback */
#define SOFT_OFF_EN_RB				(1 << 1) /* Blanking SM soft Off read back */
#define SOFT_ON_EN_RB				(1 << 0) /* Blanking SM soft On read back */
#define SOFTBLANKRB(x)				(((x) & 0x3) << 6) /* Blanking State */

/*
 *	REG_PRBS
 */
#define PRBS_GOOD_Q				(1 << 7) /* Good data indicator imaginary channel */
#define PRBS_GOOD_I				(1 << 6) /* Good data indicator real channel */
#define PRBS_INV_Q				(1 << 4) /* Data Inversion imaginary channel */
#define PRBS_INV_I				(1 << 3) /* Data Inversion real channel */
#define PRBS_MODE				(1 << 2) /* Polynomial Select */
#define PRBS_RESET				(1 << 1) /* Reset Error Counters */
#define PRBS_EN					(1 << 0) /* Enable PRBS Checker */

/*
 *	REG_DACPLLT5
 */
#define VCO_VAR(x)				(((x) & 0xF) << 0) /* Varactor KVO setting */

/*
 *	REG_DACPLLTB
 */
#define VCO_BIAS_REF(x)				(((x) & 0x7) << 0) /* VCO Bias control */

/*
 *	REG_DACPLLTD
 */
#define VCO_CAL_REF_MON				(1 << 3) /* Sent control voltage to outside world */
#define VCO_CAL_REF_TCF(x)			(((x) & 0x7) << 0) /* TempCo for cal ref */

/*
 *	REG_DACPLLT17
 */
#define VCO_VAR_REF_TCF(x)			(((x) & 0x7) << 4) /* Varactor Reference TempCo */
#define VCO_VAR_OFF(x)				(((x) & 0xF) << 0) /* Varactor Offset */

/*
 *	REG_SPISTRENGTH
 */
#define SPIDRV(x)				(((x) & 0xF) << 0) /* Slew and drive strength for cmos interface */

/*
 *	REG_CLK_TEST
 */
#define DUTYCYCLEON				(1 << 0) /* Clock Duty Cycle Control On */

/*
 *	REG_ATEST_VOLTS
 */
#define ATEST_EN				(1 << 0) /* Enable Analog Test Mode */
#define ATEST_TOPVSEL(x)			(((x) & 0x3) << 5) /* Which source at analog top to use */
#define ATEST_DACSEL(x)				(((x) & 0x3) << 3) /* DAC from which to get voltage */
#define ATEST_VSEL(x)				(((x) & 0x3) << 1) /* DAC Voltage to Select */

/*
 *	REG_ASPI_CLKSRC
 */
#define EN_CLKDIV				(1 << 3) /* Enable the fdac/8 clock path to generate PD timing clock */
#define ASPI_OSC_RATE				(1 << 2) /* Aspi Oscillator Rate */
#define ASPI_CLK_SRC				(1 << 1) /* Choose Aspi Clock Source */
#define EN_ASPI_OSC				(1 << 0) /* Enable Aspi Oscillator clock */

/*
 *	REG_MASTER_PD
 */
#define SPI_PD_MASTER				(1 << 0)

/*
 *	REG_GENERIC_PD
 */
#define SPI_SYNC1_PD				(1 << 1)
#define SPI_SYNC2_PD				(1 << 0)

/*
 *	REG_CDR_OPERATING_MODE_REG_0
 */
#define SPI_ENHALFRATE				(1 << 5)
#define SPI_DIVISION_RATE(x)			(((x) & 0x3) << 1)

/*
 *	REG_EQ_CONFIG_PHY_0_1
 */
#define SPI_EQ_CONFIG1(x)			(((x) & 0xF) << 4)
#define SPI_EQ_CONFIG0(x)			(((x) & 0xF) << 0)

/*
 *	REG_EQ_CONFIG_PHY_2_3
 */
#define SPI_EQ_CONFIG3(x)			(((x) & 0xF) << 4)
#define SPI_EQ_CONFIG2(x)			(((x) & 0xF) << 0)

/*
 *	REG_EQ_CONFIG_PHY_4_5
 */
#define SPI_EQ_CONFIG5(x)			(((x) & 0xF) << 4)
#define SPI_EQ_CONFIG4(x)			(((x) & 0xF) << 0)

/*
 *	REG_EQ_CONFIG_PHY_6_7
 */
#define SPI_EQ_CONFIG7(x)			(((x) & 0xF) << 4)
#define SPI_EQ_CONFIG6(x)			(((x) & 0xF) << 0)

/*
 *	REG_EQ_BIAS_REG
 */
#define SPI_EQ_EXTRA_SPI_LSBITS(x)		(((x) & 0x3) << 6)
#define SPI_EQ_BIASPTAT(x)			(((x) & 0x7) << 3)
#define SPI_EQ_BIASPLY(x)			(((x) & 0x7) << 0)

/*
 *	REG_SYNTH_ENABLE_CNTRL
 */
#define SPI_RECAL_SYNTH				(1 << 2)
#define SPI_ENABLE_SYNTH			(1 << 0)

/*
 *	REG_PLL_STATUS
 */
#define SPI_CP_CAL_VALID_RB			(1 << 3)
#define SPI_PLL_LOCK_RB				(1 << 0)

/*
 *	REG_REF_CLK_DIVIDER_LDO
 */
#define SPI_CDR_OVERSAMP(x)			(((x) & 0x3) << 0)

/*
 *	REG_TERM_BLK1_CTRLREG0
 */
#define SPI_I_TUNE_R_CAL_TERMBLK1		(1 << 0)

/*
 *	REG_TERM_BLK2_CTRLREG0
 */
#define SPI_I_TUNE_R_CAL_TERMBLK2		(1 << 0)

/*
 *	REG_GENERAL_JRX_CTRL_0
 */
#define CHECKSUM_MODE				(1 << 6) /* Checksum mode */
#define LINK_MODE				(1 << 3) /* Link mode */
#define SEL_REG_MAP_1				(1 << 2) /* Link register map selection */
#define LINK_EN(x)				(((x) & 0x3) << 0) /* Link enable */

/*
 *	REG_GENERAL_JRX_CTRL_1
 */
#define SUBCLASSV_LOCAL(x)			(((x) & 0x7) << 0) /* JESD204B subclass */

/*
 *	REG_DYN_LINK_LATENCY_0
 */
#define DYN_LINK_LATENCY_0(x)			(((x) & 0x1F) << 0) /* Dynamic link latency: Link 0 */

/*
 *	REG_DYN_LINK_LATENCY_1
 */
#define DYN_LINK_LATENCY_1(x)			(((x) & 0x1F) << 0) /* Dynamic link latency: Link 1 */

/*
 *	REG_LMFC_DELAY_0
 */
#define LMFC_DELAY_0(x)				(((x) & 0x1F) << 0) /* LMFC delay: Link 0 */

/*
 *	REG_LMFC_DELAY_1
 */
#define LMFC_DELAY_1(x)				(((x) & 0x1F) << 0) /* LMFC delay: Link 1 */

/*
 *	REG_LMFC_VAR_0
 */
#define LMFC_VAR_0(x)				(((x) & 0x1F) << 0) /* Location in RX LMFC where JESD words are read out from buffer */

/*
 *	REG_LMFC_VAR_1
 */
#define LMFC_VAR_1(x)				(((x) & 0x1F) << 0) /* Location in RX LMFC where JESD words are read out from buffer */

/*
 *	REG_XBAR_LN_0_1
 */
#define SRC_LANE1(x)				(((x) & 0x7) << 3) /* Logic Lane 1 source */
#define SRC_LANE0(x)				(((x) & 0x7) << 0) /* Logic Lane 0 source */

/*
 *	REG_XBAR_LN_2_3
 */
#define SRC_LANE3(x)				(((x) & 0x7) << 3) /* Logic Lane 3 source */
#define SRC_LANE2(x)				(((x) & 0x7) << 0) /* Logic Lane 2 source */

/*
 *	REG_XBAR_LN_4_5
 */
#define SRC_LANE5(x)				(((x) & 0x7) << 3) /* Logic Lane 5 source */
#define SRC_LANE4(x)				(((x) & 0x7) << 0) /* Logic Lane 4 source */

/*
 *	REG_XBAR_LN_6_7
 */
#define SRC_LANE7(x)				(((x) & 0x7) << 3) /* Logic Lane 7 source */
#define SRC_LANE6(x)				(((x) & 0x7) << 0) /* Logic Lane 6 source */

/*
 *	REG_FIFO_STATUS_REG_2
 */
#define DRDL_FIFO_EMPTY				(1 << 1) /* Deterministic latency (DRDL) FIFO is between JESD204B receiver and DAC2 and DAC3 */
#define DRDL_FIFO_FULL				(1 << 0) /* DRDL FIFO is between JESD204B receiver and DAC2 and DAC3 */

/*
 *	REG_SYNCB_GEN_0
 */
#define EOMF_MASK_1				(1 << 3) /* EOMF_MASK_1 */
#define EOMF_MASK_0				(1 << 2) /* EOMF_MASK_0 */
#define EOF_MASK_1				(1 << 1) /* Mask EOF from QBD_1 */
#define EOF_MASK_0				(1 << 0) /* Mask EOF from QBD_0 */

/*
 *	REG_SYNCB_GEN_1
 */
#define SYNCB_ERR_DUR(x)			(((x) & 0xF) << 4) /* Duration of SYNCOUT low for the purpose of error reporting */
#define SYNCB_SYNCREQ_DUR(x)			(((x) & 0xF) << 0) /* Duration of SYNCOUT low for purpose of synchronization request */

/*
 *	REG_PHY_PRBS_TEST_CTRL
 */
#define PHY_TEST_START				(1 << 1) /* PHY PRBS test start */
#define PHY_TEST_RESET				(1 << 0) /* PHY PRBS test reset */
#define PHY_SRC_ERR_CNT(x)			(((x) & 0x7) << 4) /* PHY error count source */
#define PHY_PRBS_PAT_SEL(x)			(((x) & 0x3) << 2) /* PHY PRBS pattern select */

/*
 *	REG_SHORT_TPL_TEST_0
 */
#define SHORT_TPL_TEST_RESET			(1 << 1) /* Short transport layer test reset */
#define SHORT_TPL_TEST_EN			(1 << 0) /* Short transport layer test enable */
#define SHORT_TPL_SP_SEL(x)			(((x) & 0x3) << 4) /* Short transport layer sample select */
#define SHORT_TPL_M_SEL(x)			(((x) & 0x3) << 2) /* Short transport layer test DAC select */

/*
 *	REG_SHORT_TPL_TEST_3
 */
#define SHORT_TPL_FAIL				(1 << 0) /* Short transport layer test fail */

/*
 *	REG_BID_REG
 */
#define ADJCNT_RD(x)				(((x) & 0xF) << 4)
#define BID_RD(x)				(((x) & 0xF) << 0)

/*
 *	REG_LID0_REG
 */
#define ADJDIR_RD				(1 << 6)
#define PHADJ_RD				(1 << 5)
#define LID0_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_SCR_L_REG
 */
#define SCR_RD					(1 << 7)
#define L_RD(x)					(((x) & 0x1F) << 0)

/*
 *	REG_K_REG
 */
#define K_RD(x)					(((x) & 0x1F) << 0)

/*
 *	REG_CS_N_REG
 */
#define CS_RD(x)				(((x) & 0x3) << 6)
#define N_RD(x)					(((x) & 0x1F) << 0)

/*
 *	REG_NP_REG
 */
#define SUBCLASSV_RD(x)				(((x) & 0x7) << 5)
#define NP_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_S_REG
 */
#define JESDV_RD(x)				(((x) & 0x7) << 5)
#define S_RD(x)					(((x) & 0x1F) << 0)

/*
 *	REG_HD_CF_REG
 */
#define HD_RD					(1 << 7)
#define CF_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_LID1_REG
 */
#define LID1_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_LID2_REG
 */
#define LID2_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_LID3_REG
 */
#define LID3_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_LID4_REG
 */
#define LID4_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_LID5_REG
 */
#define LID5_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_LID6_REG
 */
#define LID6_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_LID7_REG
 */
#define LID7_RD(x)				(((x) & 0x1F) << 0)

/*
 *	REG_ILS_BID
 */
#define ADJCNT(x)				(((x) & 0xF) << 4)
#define BID(x)					(((x) & 0xF) << 0)

/*
 *	REG_ILS_LID0
 */
#define ADJDIR					(1 << 6)
#define PHADJ					(1 << 5)
#define LID0(x)					(((x) & 0x1F) << 0)

/*
 *	REG_ILS_SCR_L
 */
#define SCR					(1 << 7)
#define L(x)					(((x) & 0x1F) << 0)

/*
 *	REG_ILS_K
 */
#define K(x)					(((x) & 0x1F) << 0)

/*
 *	REG_ILS_CS_N
 */
#define CS(x)					(((x) & 0x3) << 6)
#define N(x)					(((x) & 0x1F) << 0)

/*
 *	REG_ILS_NP
 */
#define SUBCLASSV(x)				(((x) & 0x7) << 5)
#define NP(x)					(((x) & 0x1F) << 0)

/*
 *	REG_ILS_S
 */
#define JESDV(x)				(((x) & 0x7) << 5)
#define S(x)					(((x) & 0x1F) << 0)

/*
 *	REG_ILS_HD_CF
 */
#define HD					(1 << 7)
#define CF(x)					(((x) & 0x1F) << 0)

/*
 *	REG_ERRCNTRMON
 */
#define LANESEL(x)				(((x) & 0x7) << 4)
#define CNTRSEL(x)				(((x) & 0x3) << 0)

/*
 *	REG_BADDISPARITY
 */
#define RST_IRQ_DIS				(1 << 7)
#define DIS_ERR_CNTR_DIS			(1 << 6)
#define RST_ERR_CNTR_DIS			(1 << 5)
#define LANE_ADDR_DIS(x)			(((x) & 0x7) << 0)

/*
 *	REG_NITDISPARITY
 */
#define RST_IRQ_NIT				(1 << 7)
#define DIS_ERR_CNTR_NIT			(1 << 6)
#define RST_ERR_CNTR_NIT			(1 << 5)
#define LANE_ADDR_NIT(x)			(((x) & 0x7) << 0)

/*
 *	REG_UNEXPECTEDKCHAR
 */
#define RST_IRQ_K				(1 << 7)
#define DIS_ERR_CNTR_K				(1 << 6)
#define RST_ERR_CNTR_K				(1 << 5)
#define LANE_ADDR_K(x)				(((x) & 0x7) << 0)

/*
 *	REG_CTRLREG2
 */
#define ILAS_MODE				(1 << 7)
#define REPDATATEST				(1 << 5)
#define QUETESTERR				(1 << 4)
#define AUTO_ECNTR_RST				(1 << 3)

/*
 *	REG_IRQVECTOR
 */
#define BADDIS_FLAG_OR_MASK			(1 << 7)
#define NITD_FLAG_OR_MASK			(1 << 6)
#define UEKC_FLAG_OR_MASK			(1 << 5)
#define INITIALLANESYNC_FLAG_OR_MASK		(1 << 3)
#define BADCHECKSUM_FLAG_OR_MASK		(1 << 2)
#define CODEGRPSYNC_FLAG_OR_MASK		(1 << 0)

/*
 *	REG_SYNCASSERTIONMASK
 */
#define BAD_DIS_S				(1 << 7)
#define NIT_DIS_S				(1 << 6)
#define UNEX_K_S				(1 << 5)
#define CMM_FLAG_OR_MASK			(1 << 4)
#define CMM_ENABLE				(1 << 3)


#define AD9144_MAX_DAC_RATE			2000000000UL
#define AD9144_CHIP_ID				0x44

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct {
	/* Device Settings */
	uint8_t		jesd_xbar_lane0_sel;
	uint8_t		jesd_xbar_lane1_sel;
	uint8_t		jesd_xbar_lane2_sel;
	uint8_t		jesd_xbar_lane3_sel;
	uint8_t		active_converters;
	uint32_t	stpl_samples[4][4];
	uint32_t	lane_rate_kbps;
	uint32_t	prbs_type;
} ad9144_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad9144_setup(spi_device *dev,
		ad9144_init_param init_param);
int32_t ad9144_spi_read(spi_device *dev,
		uint16_t reg_addr,
		uint8_t *reg_data);
int32_t ad9144_short_pattern_test(spi_device *dev,
		ad9144_init_param init_param);
int32_t ad9144_status(spi_device *dev);
int32_t ad9144_datapath_prbs_test(spi_device *dev,
	       ad9144_init_param init_param);
#endif
