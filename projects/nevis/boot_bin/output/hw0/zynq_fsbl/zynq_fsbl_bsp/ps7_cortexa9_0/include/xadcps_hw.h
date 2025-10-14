/******************************************************************************
* Copyright (C) 2011 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xadcps_hw.h
* @addtogroup xadcps_v2_6
* @{
*
* This header file contains identifiers and basic driver functions (or
* macros) that can be used to access the XADC device through the Device
* Config Interface of the Zynq.
*
*
* Refer to the device specification for more information about this driver.
*
* @note	 None.
*
*
* <pre>
*
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- -----  -------- -----------------------------------------------------
* 1.00a bss    12/22/11 First release based on the XPS/AXI xadc driver
* 1.03a bss    11/01/13 Modified macros to use correct Register offsets
*			CR#749687
* 2.6   aad    11/02/20 Fix MISRAC Mandatory and Advisory errors.
*
* </pre>
*
*****************************************************************************/
#ifndef XADCPS_HW_H /* Prevent circular inclusions */
#define XADCPS_HW_H /* by using protection macros  */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files ********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"

/************************** Constant Definitions ****************************/


/**@name Register offsets of XADC in the Device Config
 *
 * The following constants provide access to each of the registers of the
 * XADC device.
 * @{
 */

#define XADCPS_CFG_OFFSET	 0x00U /**< Configuration Register */
#define XADCPS_INT_STS_OFFSET	 0x04U /**< Interrupt Status Register */
#define XADCPS_INT_MASK_OFFSET	 0x08U /**< Interrupt Mask Register */
#define XADCPS_MSTS_OFFSET	 0x0CU /**< Misc status register */
#define XADCPS_CMDFIFO_OFFSET	 0x10U /**< Command FIFO Register */
#define XADCPS_RDFIFO_OFFSET	 0x14U /**< Read FIFO Register */
#define XADCPS_MCTL_OFFSET	 0x18U /**< Misc control register */

/* @} */





/** @name XADC Config Register Bit definitions
  * @{
 */
#define XADCPS_CFG_ENABLE_MASK	 0x80000000U /**< Enable access from PS mask */
#define XADCPS_CFG_CFIFOTH_MASK  0x00F00000U /**< Command FIFO Threshold mask */
#define XADCPS_CFG_DFIFOTH_MASK  0x000F0000U /**< Data FIFO Threshold mask */
#define XADCPS_CFG_WEDGE_MASK	 0x00002000U /**< Write Edge Mask */
#define XADCPS_CFG_REDGE_MASK	 0x00001000U /**< Read Edge Mask */
#define XADCPS_CFG_TCKRATE_MASK  0x00000300U /**< Clock freq control */
#define XADCPS_CFG_IGAP_MASK	 0x0000001FU /**< Idle Gap between
						* successive commands */
/* @} */


/** @name XADC Interrupt Status/Mask Register Bit definitions
  *
  * The definitions are same for the Interrupt Status Register and
  * Interrupt Mask Register. They are defined only once.
  * @{
 */
#define XADCPS_INTX_ALL_MASK   	   0x000003FFU /**< Alarm Signals Mask  */
#define XADCPS_INTX_CFIFO_LTH_MASK 0x00000200U /**< CMD FIFO less than threshold */
#define XADCPS_INTX_DFIFO_GTH_MASK 0x00000100U /**< Data FIFO greater than threshold */
#define XADCPS_INTX_OT_MASK	   0x00000080U /**< Over temperature Alarm Status */
#define XADCPS_INTX_ALM_ALL_MASK   0x0000007FU /**< Alarm Signals Mask  */
#define XADCPS_INTX_ALM6_MASK	   0x00000040U /**< Alarm 6 Mask  */
#define XADCPS_INTX_ALM5_MASK	   0x00000020U /**< Alarm 5 Mask  */
#define XADCPS_INTX_ALM4_MASK	   0x00000010U /**< Alarm 4 Mask  */
#define XADCPS_INTX_ALM3_MASK	   0x00000008U /**< Alarm 3 Mask  */
#define XADCPS_INTX_ALM2_MASK	   0x00000004U /**< Alarm 2 Mask  */
#define XADCPS_INTX_ALM1_MASK	   0x00000002U /**< Alarm 1 Mask  */
#define XADCPS_INTX_ALM0_MASK	   0x00000001U /**< Alarm 0 Mask  */

/* @} */


/** @name XADC Miscellaneous Register Bit definitions
  * @{
 */
#define XADCPS_MSTS_CFIFO_LVL_MASK  0x000F0000U /**< Command FIFO Level mask */
#define XADCPS_MSTS_DFIFO_LVL_MASK  0x0000F000U /**< Data FIFO Level Mask  */
#define XADCPS_MSTS_CFIFOF_MASK     0x00000800U /**< Command FIFO Full Mask  */
#define XADCPS_MSTS_CFIFOE_MASK     0x00000400U /**< Command FIFO Empty Mask  */
#define XADCPS_MSTS_DFIFOF_MASK     0x00000200U /**< Data FIFO Full Mask  */
#define XADCPS_MSTS_DFIFOE_MASK     0x00000100U /**< Data FIFO Empty Mask  */
#define XADCPS_MSTS_OT_MASK	    0x00000080U /**< Over Temperature Mask */
#define XADCPS_MSTS_ALM_MASK	    0x0000007FU /**< Alarms Mask  */
/* @} */


/** @name XADC Miscellaneous Control Register Bit definitions
  * @{
 */
#define XADCPS_MCTL_RESET_MASK      0x00000010U /**< Reset XADC */
#define XADCPS_MCTL_FLUSH_MASK      0x00000001U /**< Flush the FIFOs */
/* @} */


/**@name Internal Register offsets of the XADC
 *
 * The following constants provide access to each of the internal registers of
 * the XADC device.
 * @{
 */

/*
 * XADC Internal Channel Registers
 */
#define XADCPS_TEMP_OFFSET		  0x00U /**< On-chip Temperature Reg */
#define XADCPS_VCCINT_OFFSET		  0x01U /**< On-chip VCCINT Data Reg */
#define XADCPS_VCCAUX_OFFSET		  0x02U /**< On-chip VCCAUX Data Reg */
#define XADCPS_VPVN_OFFSET		  0x03U /**< ADC out of VP/VN	   */
#define XADCPS_VREFP_OFFSET		  0x04U /**< On-chip VREFP Data Reg */
#define XADCPS_VREFN_OFFSET		  0x05U /**< On-chip VREFN Data Reg */
#define XADCPS_VBRAM_OFFSET		  0x06U /**< On-chip VBRAM , 7 Series */
#define XADCPS_ADC_A_SUPPLY_CALIB_OFFSET  0x08U /**< ADC A Supply Offset Reg */
#define XADCPS_ADC_A_OFFSET_CALIB_OFFSET  0x09U /**< ADC A Offset Data Reg */
#define XADCPS_ADC_A_GAINERR_CALIB_OFFSET 0x0AU /**< ADC A Gain Error Reg  */
#define XADCPS_VCCPINT_OFFSET		  0x0DU /**< On-chip VCCPINT Reg, Zynq */
#define XADCPS_VCCPAUX_OFFSET		  0x0EU /**< On-chip VCCPAUX Reg, Zynq */
#define XADCPS_VCCPDRO_OFFSET		  0x0FU /**< On-chip VCCPDRO Reg, Zynq */

/*
 * XADC External Channel Registers
 */
#define XADCPS_AUX00_OFFSET	0x10U /**< ADC out of VAUXP0/VAUXN0 */
#define XADCPS_AUX01_OFFSET	0x11U /**< ADC out of VAUXP1/VAUXN1 */
#define XADCPS_AUX02_OFFSET	0x12U /**< ADC out of VAUXP2/VAUXN2 */
#define XADCPS_AUX03_OFFSET	0x13U /**< ADC out of VAUXP3/VAUXN3 */
#define XADCPS_AUX04_OFFSET	0x14U /**< ADC out of VAUXP4/VAUXN4 */
#define XADCPS_AUX05_OFFSET	0x15U /**< ADC out of VAUXP5/VAUXN5 */
#define XADCPS_AUX06_OFFSET	0x16U /**< ADC out of VAUXP6/VAUXN6 */
#define XADCPS_AUX07_OFFSET	0x17U /**< ADC out of VAUXP7/VAUXN7 */
#define XADCPS_AUX08_OFFSET	0x18U /**< ADC out of VAUXP8/VAUXN8 */
#define XADCPS_AUX09_OFFSET	0x19U /**< ADC out of VAUXP9/VAUXN9 */
#define XADCPS_AUX10_OFFSET	0x1AU /**< ADC out of VAUXP10/VAUXN10 */
#define XADCPS_AUX11_OFFSET	0x1BU /**< ADC out of VAUXP11/VAUXN11 */
#define XADCPS_AUX12_OFFSET	0x1CU /**< ADC out of VAUXP12/VAUXN12 */
#define XADCPS_AUX13_OFFSET	0x1DU /**< ADC out of VAUXP13/VAUXN13 */
#define XADCPS_AUX14_OFFSET	0x1EU /**< ADC out of VAUXP14/VAUXN14 */
#define XADCPS_AUX15_OFFSET	0x1FU /**< ADC out of VAUXP15/VAUXN15 */

/*
 * XADC Registers for Maximum/Minimum data captured for the
 * on chip Temperature/VCCINT/VCCAUX data.
 */
#define XADCPS_MAX_TEMP_OFFSET		0x20U /**< Max Temperature Reg */
#define XADCPS_MAX_VCCINT_OFFSET	0x21U /**< Max VCCINT Register */
#define XADCPS_MAX_VCCAUX_OFFSET	0x22U /**< Max VCCAUX Register */
#define XADCPS_MAX_VCCBRAM_OFFSET	0x23U /**< Max BRAM Register, 7 series */
#define XADCPS_MIN_TEMP_OFFSET		0x24U /**< Min Temperature Reg */
#define XADCPS_MIN_VCCINT_OFFSET	0x25U /**< Min VCCINT Register */
#define XADCPS_MIN_VCCAUX_OFFSET	0x26U /**< Min VCCAUX Register */
#define XADCPS_MIN_VCCBRAM_OFFSET	0x27U /**< Min BRAM Register, 7 series */
#define XADCPS_MAX_VCCPINT_OFFSET	0x28U /**< Max VCCPINT Register, Zynq */
#define XADCPS_MAX_VCCPAUX_OFFSET	0x29U /**< Max VCCPAUX Register, Zynq */
#define XADCPS_MAX_VCCPDRO_OFFSET	0x2AU /**< Max VCCPDRO Register, Zynq */
#define XADCPS_MIN_VCCPINT_OFFSET	0x2CU /**< Min VCCPINT Register, Zynq */
#define XADCPS_MIN_VCCPAUX_OFFSET	0x2DU /**< Min VCCPAUX Register, Zynq */
#define XADCPS_MIN_VCCPDRO_OFFSET	0x2EU /**< Min VCCPDRO Register,Zynq */
 /* Undefined 0x2F to 0x3E */
#define XADCPS_FLAG_OFFSET		0x3FU /**< Flag Register */

/*
 * XADC Configuration Registers
 */
#define XADCPS_CFR0_OFFSET	0x40U	/**< Configuration Register 0 */
#define XADCPS_CFR1_OFFSET	0x41U	/**< Configuration Register 1 */
#define XADCPS_CFR2_OFFSET	0x42U	/**< Configuration Register 2 */

/* Test Registers 0x43 to 0x47 */

/*
 * XADC Sequence Registers
 */
#define XADCPS_SEQ00_OFFSET	0x48U /**< Seq Reg 00 Adc Channel Selection */
#define XADCPS_SEQ01_OFFSET	0x49U /**< Seq Reg 01 Adc Channel Selection */
#define XADCPS_SEQ02_OFFSET	0x4AU /**< Seq Reg 02 Adc Average Enable */
#define XADCPS_SEQ03_OFFSET	0x4BU /**< Seq Reg 03 Adc Average Enable */
#define XADCPS_SEQ04_OFFSET	0x4CU /**< Seq Reg 04 Adc Input Mode Select */
#define XADCPS_SEQ05_OFFSET	0x4DU /**< Seq Reg 05 Adc Input Mode Select */
#define XADCPS_SEQ06_OFFSET	0x4EU /**< Seq Reg 06 Adc Acquisition Select */
#define XADCPS_SEQ07_OFFSET	0x4FU /**< Seq Reg 07 Adc Acquisition Select */

/*
 * XADC Alarm Threshold/Limit Registers (ATR)
 */
#define XADCPS_ATR_TEMP_UPPER_OFFSET	0x50U /**< Temp Upper Alarm Register */
#define XADCPS_ATR_VCCINT_UPPER_OFFSET	0x51U /**< VCCINT Upper Alarm Reg */
#define XADCPS_ATR_VCCAUX_UPPER_OFFSET	0x52U /**< VCCAUX Upper Alarm Reg */
#define XADCPS_ATR_OT_UPPER_OFFSET	0x53U /**< Over Temp Upper Alarm Reg */
#define XADCPS_ATR_TEMP_LOWER_OFFSET	0x54U /**< Temp Lower Alarm Register */
#define XADCPS_ATR_VCCINT_LOWER_OFFSET	0x55U /**< VCCINT Lower Alarm Reg */
#define XADCPS_ATR_VCCAUX_LOWER_OFFSET	0x56U /**< VCCAUX Lower Alarm Reg */
#define XADCPS_ATR_OT_LOWER_OFFSET	0x57U /**< Over Temp Lower Alarm Reg */
#define XADCPS_ATR_VBRAM_UPPER_OFFSET	0x58U /**< VBRAM Upper Alarm, 7 series */
#define XADCPS_ATR_VCCPINT_UPPER_OFFSET	0x59U /**< VCCPINT Upper Alarm, Zynq */
#define XADCPS_ATR_VCCPAUX_UPPER_OFFSET	0x5AU /**< VCCPAUX Upper Alarm, Zynq */
#define XADCPS_ATR_VCCPDRO_UPPER_OFFSET	0x5BU /**< VCCPDRO Upper Alarm, Zynq */
#define XADCPS_ATR_VBRAM_LOWER_OFFSET	0x5CU /**< VRBAM Lower Alarm, 7 Series */
#define XADCPS_ATR_VCCPINT_LOWER_OFFSET	0x5DU /**< VCCPINT Lower Alarm, Zynq */
#define XADCPS_ATR_VCCPAUX_LOWER_OFFSET	0x5EU /**< VCCPAUX Lower Alarm, Zynq */
#define XADCPS_ATR_VCCPDRO_LOWER_OFFSET	0x5FU /**< VCCPDRO Lower Alarm, Zynq */

/* Undefined 0x60 to 0x7F */

/*@}*/



/**
 * @name Configuration Register 0 (CFR0) mask(s)
 * @{
 */
#define XADCPS_CFR0_CAL_AVG_MASK	0x00008000U /**< Averaging enable Mask */
#define XADCPS_CFR0_AVG_VALID_MASK	0x00003000U /**< Averaging bit Mask */
#define XADCPS_CFR0_AVG1_MASK		0x00000000U /**< No Averaging */
#define XADCPS_CFR0_AVG16_MASK		0x00001000U /**< Average 16 samples */
#define XADCPS_CFR0_AVG64_MASK	 	0x00002000U /**< Average 64 samples */
#define XADCPS_CFR0_AVG256_MASK 	0x00003000U /**< Average 256 samples */
#define XADCPS_CFR0_AVG_SHIFT	 	12U     /**< Averaging bits shift */
#define XADCPS_CFR0_MUX_MASK	 	0x00000800U /**< External Mask Enable */
#define XADCPS_CFR0_DU_MASK	 	0x00000400U /**< Bipolar/Unipolar mode */
#define XADCPS_CFR0_EC_MASK	 	0x00000200U /**< Event driven/
						 *  Continuous mode selection
						 */
#define XADCPS_CFR0_ACQ_MASK	 	0x00000100U /**< Add acquisition by 6 ADCCLK */
#define XADCPS_CFR0_CHANNEL_MASK	0x0000001FU /**< Channel number bit Mask */

/*@}*/

/**
 * @name Configuration Register 1 (CFR1) mask(s)
 * @{
 */
#define XADCPS_CFR1_SEQ_VALID_MASK	  0x0000F000U /**< Sequence bit Mask */
#define XADCPS_CFR1_SEQ_SAFEMODE_MASK	  0x00000000U /**< Default Safe Mode */
#define XADCPS_CFR1_SEQ_ONEPASS_MASK	  0x00001000U /**< Onepass through Seq */
#define XADCPS_CFR1_SEQ_CONTINPASS_MASK	     0x00002000U /**< Continuous Cycling Seq */
#define XADCPS_CFR1_SEQ_SINGCHAN_MASK	     0x00003000U /**< Single channel - No Seq */
#define XADCPS_CFR1_SEQ_SIMUL_SAMPLING_MASK  0x00004000U /**< Simulataneous Sampling Mask */
#define XADCPS_CFR1_SEQ_INDEPENDENT_MASK  0x00008000U /**< Independent Mode */
#define XADCPS_CFR1_SEQ_SHIFT		  12U     /**< Sequence bit shift */
#define XADCPS_CFR1_ALM_VCCPDRO_MASK	  0x00000800U /**< Alm 6 - VCCPDRO, Zynq  */
#define XADCPS_CFR1_ALM_VCCPAUX_MASK	  0x00000400U /**< Alm 5 - VCCPAUX, Zynq */
#define XADCPS_CFR1_ALM_VCCPINT_MASK	  0x00000200U /**< Alm 4 - VCCPINT, Zynq */
#define XADCPS_CFR1_ALM_VBRAM_MASK	  0x00000100U /**< Alm 3 - VBRAM, 7 series */
#define XADCPS_CFR1_CAL_VALID_MASK	  0x000000F0U /**< Valid Calibration Mask */
#define XADCPS_CFR1_CAL_PS_GAIN_OFFSET_MASK  0x00000080U /**< Calibration 3 -Power
							Supply Gain/Offset
							Enable */
#define XADCPS_CFR1_CAL_PS_OFFSET_MASK	  0x00000040U /**< Calibration 2 -Power
							Supply Offset Enable */
#define XADCPS_CFR1_CAL_ADC_GAIN_OFFSET_MASK 0x00000020U /**< Calibration 1 -ADC Gain
							Offset Enable */
#define XADCPS_CFR1_CAL_ADC_OFFSET_MASK	 0x00000010U /**< Calibration 0 -ADC Offset
							Enable */
#define XADCPS_CFR1_CAL_DISABLE_MASK	0x00000000U /**< No Calibration */
#define XADCPS_CFR1_ALM_ALL_MASK	0x00000F0FU /**< Mask for all alarms */
#define XADCPS_CFR1_ALM_VCCAUX_MASK	0x00000008U /**< Alarm 2 - VCCAUX Enable */
#define XADCPS_CFR1_ALM_VCCINT_MASK	0x00000004U /**< Alarm 1 - VCCINT Enable */
#define XADCPS_CFR1_ALM_TEMP_MASK	0x00000002U /**< Alarm 0 - Temperature */
#define XADCPS_CFR1_OT_MASK		0x00000001U /**< Over Temperature Enable */

/*@}*/

/**
 * @name Configuration Register 2 (CFR2) mask(s)
 * @{
 */
#define XADCPS_CFR2_CD_VALID_MASK	0xFF00U  /**<Clock Divisor bit Mask   */
#define XADCPS_CFR2_CD_SHIFT		8U	/**<Num of shift on division */
#define XADCPS_CFR2_CD_MIN		8U	/**<Minimum value of divisor */
#define XADCPS_CFR2_CD_MAX		255U	/**<Maximum value of divisor */

#define XADCPS_CFR2_CD_MIN		8U	/**<Minimum value of divisor */
#define XADCPS_CFR2_PD_MASK		0x0030U	/**<Power Down Mask */
#define XADCPS_CFR2_PD_XADC_MASK	0x0030U	/**<Power Down XADC Mask */
#define XADCPS_CFR2_PD_ADC1_MASK	0x0020U	/**<Power Down ADC1 Mask */
#define XADCPS_CFR2_PD_SHIFT		4U	/**<Power Down Shift */
/*@}*/

/**
 * @name Sequence Register (SEQ) Bit Definitions
 * @{
 */
#define XADCPS_SEQ_CH_CALIB	0x00000001U /**< ADC Calibration Channel */
#define XADCPS_SEQ_CH_VCCPINT	0x00000020U /**< VCCPINT, Zynq Only */
#define XADCPS_SEQ_CH_VCCPAUX	0x00000040U /**< VCCPAUX, Zynq Only */
#define XADCPS_SEQ_CH_VCCPDRO	0x00000080U /**< VCCPDRO, Zynq Only */
#define XADCPS_SEQ_CH_TEMP	0x00000100U /**< On Chip Temperature Channel */
#define XADCPS_SEQ_CH_VCCINT	0x00000200U /**< VCCINT Channel */
#define XADCPS_SEQ_CH_VCCAUX	0x00000400U /**< VCCAUX Channel */
#define XADCPS_SEQ_CH_VPVN	0x00000800U /**< VP/VN analog inputs Channel */
#define XADCPS_SEQ_CH_VREFP	0x00001000U /**< VREFP Channel */
#define XADCPS_SEQ_CH_VREFN	0x00002000U /**< VREFN Channel */
#define XADCPS_SEQ_CH_VBRAM	0x00004000U /**< VBRAM Channel, 7 series */
#define XADCPS_SEQ_CH_AUX00	0x00010000U /**< 1st Aux Channel */
#define XADCPS_SEQ_CH_AUX01	0x00020000U /**< 2nd Aux Channel */
#define XADCPS_SEQ_CH_AUX02	0x00040000U /**< 3rd Aux Channel */
#define XADCPS_SEQ_CH_AUX03	0x00080000U /**< 4th Aux Channel */
#define XADCPS_SEQ_CH_AUX04	0x00100000U /**< 5th Aux Channel */
#define XADCPS_SEQ_CH_AUX05	0x00200000U /**< 6th Aux Channel */
#define XADCPS_SEQ_CH_AUX06	0x00400000U /**< 7th Aux Channel */
#define XADCPS_SEQ_CH_AUX07	0x00800000U /**< 8th Aux Channel */
#define XADCPS_SEQ_CH_AUX08	0x01000000U /**< 9th Aux Channel */
#define XADCPS_SEQ_CH_AUX09	0x02000000U /**< 10th Aux Channel */
#define XADCPS_SEQ_CH_AUX10	0x04000000U /**< 11th Aux Channel */
#define XADCPS_SEQ_CH_AUX11	0x08000000U /**< 12th Aux Channel */
#define XADCPS_SEQ_CH_AUX12	0x10000000U /**< 13th Aux Channel */
#define XADCPS_SEQ_CH_AUX13	0x20000000U /**< 14th Aux Channel */
#define XADCPS_SEQ_CH_AUX14	0x40000000U /**< 15th Aux Channel */
#define XADCPS_SEQ_CH_AUX15	0x80000000U /**< 16th Aux Channel */

#define XADCPS_SEQ00_CH_VALID_MASK  0x7FE1U /**< Mask for the valid channels */
#define XADCPS_SEQ01_CH_VALID_MASK  0xFFFFU /**< Mask for the valid channels */

#define XADCPS_SEQ02_CH_VALID_MASK  0x7FE0U /**< Mask for the valid channels */
#define XADCPS_SEQ03_CH_VALID_MASK  0xFFFFU /**< Mask for the valid channels */

#define XADCPS_SEQ04_CH_VALID_MASK  0x0800U /**< Mask for the valid channels */
#define XADCPS_SEQ05_CH_VALID_MASK  0xFFFFU /**< Mask for the valid channels */

#define XADCPS_SEQ06_CH_VALID_MASK  0x0800U /**< Mask for the valid channels */
#define XADCPS_SEQ07_CH_VALID_MASK  0xFFFFU /**< Mask for the valid channels */


#define XADCPS_SEQ_CH_AUX_SHIFT	16U /**< Shift for the Aux Channel */

/*@}*/

/**
 * @name OT Upper Alarm Threshold Register Bit Definitions
 * @{
 */

#define XADCPS_ATR_OT_UPPER_ENB_MASK	0x000FU /**< Mask for OT enable */
#define XADCPS_ATR_OT_UPPER_VAL_MASK	0xFFF0U /**< Mask for OT value */
#define XADCPS_ATR_OT_UPPER_VAL_SHIFT	4U      /**< Shift for OT value */
#define XADCPS_ATR_OT_UPPER_ENB_VAL	0x0003U /**< Value for OT enable */
#define XADCPS_ATR_OT_UPPER_VAL_MAX	0x0FFFU /**< Max OT value */

/*@}*/


/**
 * @name JTAG DRP Bit Definitions
 * @{
 */
#define XADCPS_JTAG_DATA_MASK		0x0000FFFFU /**< Mask for the Data */
#define XADCPS_JTAG_ADDR_MASK		0x03FF0000U /**< Mask for the Addr */
#define XADCPS_JTAG_ADDR_SHIFT		16U	   /**< Shift for the Addr */
#define XADCPS_JTAG_CMD_MASK		0x3C000000U /**< Mask for the Cmd */
#define XADCPS_JTAG_CMD_WRITE_MASK	0x08000000U /**< Mask for CMD Write */
#define XADCPS_JTAG_CMD_READ_MASK	0x04000000U /**< Mask for CMD Read */
#define XADCPS_JTAG_CMD_SHIFT		26U	   /**< Shift for the Cmd */

/*@}*/

/** @name Unlock Register Definitions
  * @{
 */
 #define XADCPS_UNLK_OFFSET	 0x034U /**< Unlock Register */
 #define XADCPS_UNLK_VALUE	 0x757BDF0DU /**< Unlock Value */

 /* @} */


/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/*****************************************************************************/
/**
*
* Read a register of the XADC device. This macro provides register
* access to all registers using the register offsets defined above.
*
* @param	BaseAddress contains the base address of the device.
* @param	RegOffset is the offset of the register to read.
*
* @return	The contents of the register.
*
* @note		C-style Signature:
*		u32 XAdcPs_ReadReg(u32 BaseAddress, u32 RegOffset);
*
******************************************************************************/
#define XAdcPs_ReadReg(BaseAddress, RegOffset) \
			(Xil_In32((BaseAddress) + (RegOffset)))

/*****************************************************************************/
/**
*
* Write a register of the XADC device. This macro provides
* register access to all registers using the register offsets defined above.
*
* @param	BaseAddress contains the base address of the device.
* @param	RegOffset is the offset of the register to write.
* @param	Data is the value to write to the register.
*
* @return	None.
*
* @note 	C-style Signature:
*		void XAdcPs_WriteReg(u32 BaseAddress,
*					u32 RegOffset,u32 Data)
*
******************************************************************************/
#define XAdcPs_WriteReg(BaseAddress, RegOffset, Data) \
		(Xil_Out32((BaseAddress) + (RegOffset), (Data)))

/************************** Function Prototypes ******************************/


/*****************************************************************************/
/**
*
* Formats the data to be written to the the XADC registers.
*
* @param	RegOffset is the offset of the Register
* @param	Data is the data to be written to the Register if it is
*		a write.
* @param	ReadWrite specifies whether it is a Read or a Write.
*		Use 0 for Read, 1 for Write.
*
* @return	None.
*
* @note 	C-style Signature:
*		void XAdcPs_FormatWriteData(u32 RegOffset,
*					     u16 Data, int ReadWrite)
*
******************************************************************************/
#define XAdcPs_FormatWriteData(RegOffset, Data, ReadWrite) 	    \
    ((ReadWrite ? XADCPS_JTAG_CMD_WRITE_MASK : XADCPS_JTAG_CMD_READ_MASK ) | \
     ((RegOffset << XADCPS_JTAG_ADDR_SHIFT) & XADCPS_JTAG_ADDR_MASK) | 	     \
     (Data & XADCPS_JTAG_DATA_MASK))



#ifdef __cplusplus
}
#endif

#endif  /* End of protection macro. */
/** @} */
